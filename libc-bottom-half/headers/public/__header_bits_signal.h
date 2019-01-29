#ifndef __wasilibc___header_bits_signal_h
#define __wasilibc___header_bits_signal_h

#include <__wasi_base.h>

#define SIGHUP    WASI_SIGHUP    
#define SIGINT    WASI_SIGINT    
#define SIGQUIT   WASI_SIGQUIT   
#define SIGILL    WASI_SIGILL    
#define SIGTRAP   WASI_SIGTRAP   
#define SIGABRT   WASI_SIGABRT   
#define SIGBUS    WASI_SIGBUS    
#define SIGFPE    WASI_SIGFPE    
#define SIGKILL   WASI_SIGKILL   
#define SIGUSR1   WASI_SIGUSR1   
#define SIGSEGV   WASI_SIGSEGV   
#define SIGUSR2   WASI_SIGUSR2   
#define SIGPIPE   WASI_SIGPIPE   
#define SIGALRM   WASI_SIGALRM   
#define SIGTERM   WASI_SIGTERM   
#define SIGCHLD   WASI_SIGCHLD   
#define SIGCONT   WASI_SIGCONT   
#define SIGSTOP   WASI_SIGSTOP   
#define SIGTSTP   WASI_SIGTSTP   
#define SIGTTIN   WASI_SIGTTIN   
#define SIGTTOU   WASI_SIGTTOU   
#define SIGURG    WASI_SIGURG    
#define SIGXCPU   WASI_SIGXCPU   
#define SIGXFSZ   WASI_SIGXFSZ   
#define SIGVTALRM WASI_SIGVTALRM 
#define SIGPROF   WASI_SIGPROF   
#define SIGWINCH  WASI_SIGWINCH  
#define SIGPOLL   WASI_SIGPOLL   
#define SIGPWR    WASI_SIGPWR    
#define SIGSYS    WASI_SIGSYS    

#define SIGIOT    SIGABRT
#define SIGIO     SIGPOLL
#define SIGUNUSED SIGSYS

#endif
