/**
 * This script is served by `harness.mjs` and runs in the browser.
 */
import { WASI, File, OpenFile, ConsoleStdout, PreopenDirectory } from 'https://cdn.jsdelivr.net/npm/@bjorn3/browser_wasi_shim@0.4.2/+esm'
import { polyfill } from 'https://cdn.jsdelivr.net/npm/wasm-imports-parser@1.0.4/polyfill.js/+esm';

/**
 * @param {{
 *   module: WebAssembly.Module,
 *   addToImports: (importObject: WebAssembly.Imports) => Promise<void>
 * }}
 */
export async function instantiate({ module, addToImports }) {
    const args = ["target.wasm"]
    const env = []
    const fds = [
        new OpenFile(new File([])), // stdin
        ConsoleStdout.lineBuffered((stdout) => {
            console.log(stdout);
        }),
        ConsoleStdout.lineBuffered((stderr) => {
            console.error(stderr);
        }),
    ];
    const wasi = new WASI(args, env, fds);

    const importObject = {
        wasi_snapshot_preview1: wasi.wasiImport,
    };
    await addToImports(importObject);
    const instance = await WebAssembly.instantiate(module, importObject);
    return { wasi, instance };
}

class WorkerInfoView {
    /**
     * The memory layout of a worker info placed in a shared array buffer.
     * All offsets are represented in Int32Array indices.
     */
    static Layout = {
        STATE: 0,
        TID: 1,
        START_ARG: 2,
        SIZE: 3,
    };

    /** @param {Int32Array} view - The memory view of the worker info */
    constructor(view) {
        this.view = view;
    }

    get state() {
        return this.view[WorkerInfoView.Layout.STATE];
    }

    setStateAndNotify(state) {
        this.view[WorkerInfoView.Layout.STATE] = state;
        Atomics.notify(this.view, WorkerInfoView.Layout.STATE);
    }

    async waitWhile(state) {
        return await Atomics.waitAsync(this.view, WorkerInfoView.Layout.STATE, state);
    }

    get tid() { return this.view[WorkerInfoView.Layout.TID]; }
    set tid(value) { this.view[WorkerInfoView.Layout.TID] = value; }

    get startArg() { return this.view[WorkerInfoView.Layout.START_ARG]; }
    set startArg(value) { this.view[WorkerInfoView.Layout.START_ARG] = value; }
}

const WorkerState = {
    NOT_STARTED: 0,
    READY: 1,
    STARTED: 2,
    FINISHED: 3,
    ERROR: 4,
};

class Threads {
    /**
     * @param {number} poolSize - The number of threads to pool
     * @param {WebAssembly.Module} module - The WebAssembly module to use
     * @param {WebAssembly.Memory} memory - The memory to use
     */
    static async create(poolSize, module, memory) {
        const workerScript = new Blob([`
            self.onmessage = async (event) => {
                const { selfFilePath } = event.data;
                const { startWorker } = await import(selfFilePath);
                await startWorker(event.data);
            }
        `], { type: 'text/javascript' });
        const workerScriptURL = URL.createObjectURL(workerScript);
        // Create a new SAB to communicate with the workers
        // Rationale: Some of the tests busy-wait on the main thread, and it
        // makes it impossible to use `postMessage` to communicate with workers
        // during the busy-wait as the event loop is blocked. Instead, we use a
        // shared array buffer and send notifications by `Atomics.notify`.
        const channel = new SharedArrayBuffer(poolSize * WorkerInfoView.Layout.SIZE * Int32Array.BYTES_PER_ELEMENT);

        const workers = [];
        for (let workerIndex = 0; workerIndex < poolSize; workerIndex++) {
            const worker = new Worker(workerScriptURL);
            const selfFilePath = import.meta.url;
            worker.postMessage({ selfFilePath, channel, workerIndex, module, memory });
            workers.push(worker);
        }

        // Wait until all workers are ready
        for (let workerIndex = 0; workerIndex < poolSize; workerIndex++) {
            const view = new Int32Array(channel, workerIndex * WorkerInfoView.Layout.SIZE * Int32Array.BYTES_PER_ELEMENT);
            const infoView = new WorkerInfoView(view);
            await (await infoView.waitWhile(WorkerState.NOT_STARTED)).value;
            const state = infoView.state;
            if (state !== WorkerState.READY) {
                throw new Error(`Worker ${workerIndex} is not ready: ${state}`);
            }
        }

        return new Threads(poolSize, workers, channel);
    }

    constructor(poolSize, workers, channel) {
        this.poolSize = poolSize;
        this.workers = workers;
        this.nextTid = 1;
        this.channel = channel;
    }

    findAvailableWorker() {
        for (let i = 0; i < this.workers.length; i++) {
            const view = new Int32Array(this.channel, i * WorkerInfoView.Layout.SIZE * Int32Array.BYTES_PER_ELEMENT);
            const infoView = new WorkerInfoView(view);
            const state = infoView.state;
            if (state === WorkerState.READY) {
                return i;
            }
        }
        throw new Error("No available worker");
    }

    spawnThread(startArg) {
        const tid = this.nextTid++;
        const index = this.findAvailableWorker();
        const view = new Int32Array(this.channel, index * WorkerInfoView.Layout.SIZE * Int32Array.BYTES_PER_ELEMENT);
        const infoView = new WorkerInfoView(view);
        infoView.tid = tid;
        infoView.startArg = startArg;
        infoView.setStateAndNotify(WorkerState.STARTED);
        return tid;
    }
}

export async function runWasmTest(wasmPath) {
    const response = await fetch(wasmPath);
    const wasmBytes = await response.arrayBuffer();

    // Polyfill WebAssembly if "Type Reflection JS API" is unavailable.
    // The feature is required to know the imported memory type.
    const WebAssembly = polyfill(globalThis.WebAssembly);

    const module = await WebAssembly.compile(wasmBytes);
    const imports = WebAssembly.Module.imports(module);

    const { wasi, instance } = await instantiate({
        module,
        addToImports: async (importObject) => {
            const memoryImport = imports.find(i => i.module === 'env' && i.name === 'memory');
            if (!memoryImport) {
                return;
            }

            // Add wasi-threads support if memory is imported
            const memoryType = memoryImport.type;
            const memory = new WebAssembly.Memory({
                initial: memoryType.minimum,
                maximum: memoryType.maximum,
                shared: memoryType.shared,
            });
            const threads = await Threads.create(8, module, memory);
            importObject.env = { memory };
            importObject.wasi = {
                "thread-spawn": (startArg) => {
                    return threads.spawnThread(startArg);
                }
            };
        },
    });

    const exitCode = wasi.start(instance);
    return exitCode === 0;
}

/**
 * @param {{
 *   channel: SharedArrayBuffer,
 *   workerIndex: number,
 *   module: WebAssembly.Module,
 *   memory: WebAssembly.Memory
 * }}
 */
export async function startWorker({ channel, workerIndex, module, memory }) {
    const view = new Int32Array(channel, workerIndex * WorkerInfoView.Layout.SIZE * Int32Array.BYTES_PER_ELEMENT);
    const infoView = new WorkerInfoView(view);
    // Mark the worker as ready
    infoView.setStateAndNotify(WorkerState.READY);
    // Wait until the main thread marks the worker as started
    await (await infoView.waitWhile(WorkerState.READY)).value;
    const tid = infoView.tid;
    const startArg = infoView.startArg;
    await startThread({ module, memory, tid, startArg });
    // Mark the worker as finished
    infoView.setStateAndNotify(WorkerState.FINISHED);
}

async function startThread({ module, memory, tid, startArg }) {
    const { instance, wasi } = await instantiate({
        module,
        addToImports(importObject) {
            importObject["env"] = { memory }
            importObject["wasi"] = {
                "thread-spawn": () => { throw new Error("Cannot spawn a new thread from a worker thread"); }
            };
        },
    });

    wasi.inst = instance;
    instance.exports.wasi_thread_start(tid, startArg);
}
