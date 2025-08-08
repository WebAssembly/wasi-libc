#!/usr/bin/env node

/*
 * Run a WASI-compatible test program in the browser.
 *
 * This script behaves like `wasmtime` but runs given WASI-compatible test
 * program in the browser.
 *
 * Example:
 * $ ./harness.mjs check.wasm
 */

import { parseArgs } from 'node:util';
import { createServer } from 'node:http';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { readFileSync } from 'node:fs';
import url from "node:url";
import { chromium } from 'playwright';

const SKIP_TESTS = [
    // "poll_oneoff" can't be implemented in the browser
    "libc-test/functional/pthread_cond",
    // atomic.wait32 can't be executed on the main thread
    "libc-test/functional/pthread_mutex",
    "libc-test/functional/pthread_tsd",
    // Skip test that uses command-line arguments
    "misc/argv_two_args",
    // Skip test that uses environment variables
    "misc/external_env",
    // XFAIL: @bjorn3/browser_wasi_shim doesn't support symlinks for now
    "misc/fts",
];

/**
 * @param {{wasmPath: string, port: number}}
 * @returns {Promise<{server: import('node:http').Server, port: number}>}
 */
async function startServer({ wasmPath, port }) {
    const server = createServer((req, res) => {
        // Set required headers for SharedArrayBuffer
        res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
        res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');

        let filePath;
        const parsed = url.parse(req.url, true);
        const pathname = parsed.pathname;
        if (pathname === "/target.wasm") {
            // Serve the test target Wasm file
            filePath = wasmPath;
            res.setHeader('Content-Type', 'application/wasm');
        } else {
            // Serve other resources
            const __dirname = dirname(fileURLToPath(import.meta.url));
            filePath = join(__dirname, pathname);
            const contentTypes = {
                "mjs": "text/javascript",
                "js": "text/javascript",
                "html": "text/html",
            }
            res.setHeader('Content-Type', contentTypes[pathname.split('.').pop()] || 'text/plain');
        }

        try {
            const content = readFileSync(filePath);
            res.end(content);
        } catch (error) {
            res.statusCode = 404;
            res.end('Not found');
        }
    });

    return new Promise((resolve) => {
        server.listen(port, () => {
            const port = server.address().port;
            resolve({ server, port });
        });
    });
}

/** @param {number} port */
function buildUrl(port) {
    return `http://localhost:${port}/run-test.html`;
}

/** @param {import('playwright').Page} page */
/** @param {number} port */
/** @returns {Promise<{passed: boolean, error?: string}>} */
async function runTest(page, port) {
    const url = buildUrl(port);
    const onExit = new Promise((resolve) => {
        page.exposeFunction("exitTest", resolve);
    });
    await page.goto(url);
    return onExit;
}

async function main() {
    // Parse and interpret a subset of the wasmtime CLI options used by the tests
    const args = parseArgs({
        args: process.argv.slice(2),
        allowPositionals: true,
        options: {
            // MARK: wasmtime CLI options
            wasi: {
                type: "string",
                multiple: true,
            },
            dir: {
                type: "string",
                multiple: true,
            },
            // MARK: For debugging purposes
            headful: {
                type: "boolean",
                default: false,
            },
            port: {
                type: "string",
                default: "0",
            }
        }
    });

    const wasmPath = args.positionals[0];
    if (!wasmPath) {
        console.error('Test path not specified');
        return 1;
    }

    if (SKIP_TESTS.some(test => wasmPath.includes(test + "."))) {
        // Silently skip tests that are known to fail in the browser
        return 0;
    }

    if (args.values.dir && args.values.dir.length > 0) {
        // Silently skip tests that require preopened directories for now
        // as it adds more complexity to the harness and file system emulation
        // is not our primary testing target.
        return 0;
    }

    // Start a HTTP server to serve the test files
    const { server, port } = await startServer({ wasmPath, port: parseInt(args.values.port) });

    const browser = await chromium.launch();
    const page = await browser.newPage();

    try {
        if (args.values.headful) {
            // Run in headful mode to allow manual testing
            console.log(`Please visit ${buildUrl(port)}`);
            console.log('Press Ctrl+C to stop');
            await new Promise(resolve => process.on('SIGINT', resolve));
            return 0;
        }

        // Run in headless mode
        const result = await runTest(page, port);
        if (!result.passed) {
            console.error('Test failed:', result.error);
            console.error(`Hint: You can debug the test by running it in headful mode by passing --headful
$ ${process.argv.join(' ')} --headful`);
            return 1;
        }
        return 0;
    } catch (error) {
        console.error('Test failed:', error);
        return 1;
    } finally {
        await browser.close();
        server.close();
    }
}

process.exit(await main());
