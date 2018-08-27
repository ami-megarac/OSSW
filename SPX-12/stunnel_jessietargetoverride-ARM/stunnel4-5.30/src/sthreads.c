/*
 *   stunnel       TLS offloading and load-balancing proxy
 *   Copyright (C) 1998-2016 Michal Trojnara <Michal.Trojnara@mirt.net>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, see <http://www.gnu.org/licenses>.
 *
 *   Linking stunnel statically or dynamically with other modules is making
 *   a combined work based on stunnel. Thus, the terms and conditions of
 *   the GNU General Public License cover the whole combination.
 *
 *   In addition, as a special exception, the copyright holder of stunnel
 *   gives you permission to combine stunnel with free software programs or
 *   libraries that are released under the GNU LGPL and with code included
 *   in the standard release of OpenSSL under the OpenSSL License (or
 *   modified versions of such code, with unchanged license). You may copy
 *   and distribute such a system following the terms of the GNU GPL for
 *   stunnel and the licenses of the other code concerned.
 *
 *   Note that people who make modified versions of stunnel are not obligated
 *   to grant this special exception for their modified versions; it is their
 *   choice whether to do so. The GNU General Public License gives permission
 *   to release a modified version without this exception; this exception
 *   also makes it possible to release a modified version which carries
 *   forward this exception.
 */

#ifdef USE_OS2
#define INCL_DOSPROCESS
#include <os2.h>
#endif

#include "common.h"
#include "prototypes.h"

int stunnel_locks[STUNNEL_LOCKS];

#if defined(USE_UCONTEXT) || defined(USE_FORK)
/* no need for critical sections */

#endif /* USE_UCONTEXT || USE_FORK */

#ifdef USE_UCONTEXT

#if defined(CPU_SPARC) && ( \
        defined(OS_SOLARIS2_0) || \
        defined(OS_SOLARIS2_1) || \
        defined(OS_SOLARIS2_2) || \
        defined(OS_SOLARIS2_3) || \
        defined(OS_SOLARIS2_4) || \
        defined(OS_SOLARIS2_5) || \
        defined(OS_SOLARIS2_6) || \
        defined(OS_SOLARIS2_7) || \
        defined(OS_SOLARIS2_8))
#define ARGC 2
#else
#define ARGC 1
#endif

/* first context on the ready list is the active context */
CONTEXT *ready_head=NULL, *ready_tail=NULL;         /* ready to execute */
CONTEXT *waiting_head=NULL, *waiting_tail=NULL;     /* waiting on poll() */

unsigned long stunnel_process_id(void) {
    return (unsigned long)getpid();
}

unsigned long stunnel_thread_id(void) {
    return ready_head ? ready_head->id : 0;
}

NOEXPORT CONTEXT *new_context(void) {
    static unsigned long next_id=1;
    CONTEXT *context;

    /* allocate and fill the CONTEXT structure */
    context=str_alloc_detached(sizeof(CONTEXT));
    context->id=next_id++;
    context->fds=NULL;
    context->ready=0;

    /* append to the tail of the ready queue */
    context->next=NULL;
    if(ready_tail)
        ready_tail->next=context;
    ready_tail=context;
    if(!ready_head)
        ready_head=context;

    return context;
}

int sthreads_init(void) {
    /* create the first (listening) context and put it in the running queue */
    if(!new_context()) {
        s_log(LOG_ERR, "Cannot create the listening context");
        return 1;
    }
    /* update tls for newly allocated ready_head */
    ui_tls=tls_alloc(NULL, ui_tls, "ui");
    /* no need to initialize ucontext_t structure here
       it will be initialied with swapcontext() call */
    return 0;
}

int create_client(SOCKET ls, SOCKET s, CLI *arg, void *(*cli)(void *)) {
    CONTEXT *context;

    (void)ls; /* this parameter is only used with USE_FORK */

    s_log(LOG_DEBUG, "Creating a new context");
    context=new_context();
    if(!context) {
        str_free(arg);
        if(s>=0)
            closesocket(s);
        return -1;
    }

    /* initialize context_t structure */
    if(getcontext(&context->context)<0) {
        str_free(context);
        str_free(arg);
        if(s>=0)
            closesocket(s);
        ioerror("getcontext");
        return -1;
    }
    context->context.uc_link=NULL; /* stunnel does not use uc_link */

    /* create stack */
    context->stack=str_alloc_detached(arg->opt->stack_size);
#if defined(__sgi) || ARGC==2 /* obsolete ss_sp semantics */
    context->context.uc_stack.ss_sp=context->stack+arg->opt->stack_size-8;
#else
    context->context.uc_stack.ss_sp=context->stack;
#endif
    context->context.uc_stack.ss_size=arg->opt->stack_size;
    context->context.uc_stack.ss_flags=0;

    makecontext(&context->context, (void(*)(void))cli, ARGC, arg);
    s_log(LOG_DEBUG, "New context created");
    return 0;
}

#endif /* USE_UCONTEXT */

#ifdef USE_FORK

int sthreads_init(void) {
    return 0;
}

unsigned long stunnel_process_id(void) {
    return (unsigned long)getpid();
}

unsigned long stunnel_thread_id(void) {
    return 0L;
}

NOEXPORT void null_handler(int sig) {
    (void)sig; /* squash the unused parameter warning */
    signal(SIGCHLD, null_handler);
}

int create_client(SOCKET ls, SOCKET s, CLI *arg, void *(*cli)(void *)) {
    switch(fork()) {
    case -1:    /* error */
        str_free(arg);
        if(s>=0)
            closesocket(s);
        return -1;
    case  0:    /* child */
        if(ls>=0)
            closesocket(ls);
        signal(SIGCHLD, null_handler);
        cli(arg);
        _exit(0);
    default:    /* parent */
        str_free(arg);
        if(s>=0)
            closesocket(s);
    }
    return 0;
}

#endif /* USE_FORK */

#ifdef USE_PTHREAD

struct CRYPTO_dynlock_value {
    pthread_rwlock_t rwlock;
};
static struct CRYPTO_dynlock_value *lock_cs;

NOEXPORT struct CRYPTO_dynlock_value *dyn_create_function(const char *file,
        int line) {
    struct CRYPTO_dynlock_value *value;

    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    value=str_alloc_detached(sizeof(struct CRYPTO_dynlock_value));
    pthread_rwlock_init(&value->rwlock, NULL);
    return value;
}

NOEXPORT void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *value,
        const char *file, int line) {
    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    if(mode&CRYPTO_LOCK) {
        /* either CRYPTO_READ or CRYPTO_WRITE (but not both) are needed */
        if(!(mode&CRYPTO_READ)==!(mode&CRYPTO_WRITE))
            fatal("Invalid locking mode");
        if(mode&CRYPTO_WRITE)
            pthread_rwlock_wrlock(&value->rwlock);
        else
            pthread_rwlock_rdlock(&value->rwlock);
    } else
        pthread_rwlock_unlock(&value->rwlock);
}

NOEXPORT void dyn_destroy_function(struct CRYPTO_dynlock_value *value,
        const char *file, int line) {
    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    pthread_rwlock_destroy(&value->rwlock);
    str_free(value);
}

NOEXPORT void locking_callback(int mode, int type, const char *file, int line) {
    dyn_lock_function(mode, lock_cs+type, file, line);
}

unsigned long stunnel_process_id(void) {
    return (unsigned long)getpid();
}

unsigned long stunnel_thread_id(void) {
#if defined(SYS_gettid) && defined(__linux__)
    return (unsigned long)syscall(SYS_gettid);
#else
    return (unsigned long)pthread_self();
#endif
}

#if OPENSSL_VERSION_NUMBER>=0x10000000L
NOEXPORT void threadid_func(CRYPTO_THREADID *tid) {
    CRYPTO_THREADID_set_numeric(tid, stunnel_thread_id());
}
#endif

int sthreads_init(void) {
    int i;

    /* initialize OpenSSL dynamic locks callbacks */
    CRYPTO_set_dynlock_create_callback(dyn_create_function);
    CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
    CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

    /* initialize stunnel critical sections */
    for(i=0; i<STUNNEL_LOCKS; i++)
        stunnel_locks[i]=CRYPTO_get_new_dynlockid();

    /* initialize OpenSSL locking callback */
    lock_cs=str_alloc_detached(
        (size_t)CRYPTO_num_locks()*sizeof(struct CRYPTO_dynlock_value));
    for(i=0; i<CRYPTO_num_locks(); i++)
        pthread_rwlock_init(&lock_cs[i].rwlock, NULL);
#if OPENSSL_VERSION_NUMBER>=0x10000000L
    CRYPTO_THREADID_set_callback(threadid_func);
#else
    CRYPTO_set_id_callback(stunnel_thread_id);
#endif
    CRYPTO_set_locking_callback(locking_callback);

    return 0;
}

int create_client(SOCKET ls, SOCKET s, CLI *arg, void *(*cli)(void *)) {
    pthread_t thread;
    pthread_attr_t pth_attr;
    int error;
#if defined(HAVE_PTHREAD_SIGMASK) && !defined(__APPLE__)
    /* disabled on OS X due to strange problems on Mac OS X 10.5
       it seems to restore signal mask somewhere (I couldn't find where)
       effectively blocking signals after first accepted connection */
    sigset_t new_set, old_set;
#endif /* HAVE_PTHREAD_SIGMASK && !__APPLE__*/

    (void)ls; /* this parameter is only used with USE_FORK */

#if defined(HAVE_PTHREAD_SIGMASK) && !defined(__APPLE__)
    /* the idea is that only the main thread handles all the signals with
     * posix threads;  signals are blocked for any other thread */
    sigfillset(&new_set);
    pthread_sigmask(SIG_SETMASK, &new_set, &old_set); /* block signals */
#endif /* HAVE_PTHREAD_SIGMASK && !__APPLE__*/
    pthread_attr_init(&pth_attr);
    pthread_attr_setdetachstate(&pth_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&pth_attr, arg->opt->stack_size);
    error=pthread_create(&thread, &pth_attr, cli, arg);
    pthread_attr_destroy(&pth_attr);
#if defined(HAVE_PTHREAD_SIGMASK) && !defined(__APPLE__)
    pthread_sigmask(SIG_SETMASK, &old_set, NULL); /* unblock signals */
#endif /* HAVE_PTHREAD_SIGMASK && !__APPLE__*/

    if(error) {
        errno=error;
        ioerror("pthread_create");
        str_free(arg);
        if(s>=0)
            closesocket(s);
        return -1;
    }
    return 0;
}

#endif /* USE_PTHREAD */

#ifdef USE_WIN32

/* Slim Reader/Writer (SRW) Lock would be better than CRITICAL_SECTION,
 * but it is unsupported on Windows XP (and earlier versions of Windows):
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa904937%28v=vs.85%29.aspx */

struct CRYPTO_dynlock_value {
    CRITICAL_SECTION mutex;
};
static struct CRYPTO_dynlock_value *lock_cs;

NOEXPORT struct CRYPTO_dynlock_value *dyn_create_function(const char *file,
        int line) {
    struct CRYPTO_dynlock_value *value;

    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    value=str_alloc_detached(sizeof(struct CRYPTO_dynlock_value));
    InitializeCriticalSection(&value->mutex);
    return value;
}

NOEXPORT void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *value,
        const char *file, int line) {
    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    if(mode&CRYPTO_LOCK)
        EnterCriticalSection(&value->mutex);
    else
        LeaveCriticalSection(&value->mutex);
}

NOEXPORT void dyn_destroy_function(struct CRYPTO_dynlock_value *value,
        const char *file, int line) {
    (void)file; /* squash the unused parameter warning */
    (void)line; /* squash the unused parameter warning */
    DeleteCriticalSection(&value->mutex);
    str_free(value);
}

NOEXPORT void locking_callback(int mode, int type, const char *file, int line) {
    dyn_lock_function(mode, lock_cs+type, file, line);
}

unsigned long stunnel_process_id(void) {
    return GetCurrentProcessId() & 0x00ffffff;
}

unsigned long stunnel_thread_id(void) {
    return GetCurrentThreadId() & 0x00ffffff;
}

int sthreads_init(void) {
    int i;

    /* initialize OpenSSL dynamic locks callbacks */
    CRYPTO_set_dynlock_create_callback(dyn_create_function);
    CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
    CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);

    /* initialize stunnel critical sections */
    for(i=0; i<STUNNEL_LOCKS; i++)
        stunnel_locks[i]=CRYPTO_get_new_dynlockid();

    /* initialize OpenSSL locking callback */
    lock_cs=str_alloc_detached(
        (size_t)CRYPTO_num_locks()*sizeof(struct CRYPTO_dynlock_value));
    for(i=0; i<CRYPTO_num_locks(); i++)
        InitializeCriticalSection(&lock_cs[i].mutex);
    CRYPTO_set_locking_callback(locking_callback);

    return 0;
}

int create_client(SOCKET ls, SOCKET s, CLI *arg, void *(*cli)(void *)) {
    (void)ls; /* this parameter is only used with USE_FORK */
    s_log(LOG_DEBUG, "Creating a new thread");
    if((long)_beginthread((void(*)(void *))cli,
            (unsigned)arg->opt->stack_size, arg)==-1) {
        ioerror("_beginthread");
        str_free(arg);
        if(s!=INVALID_SOCKET)
            closesocket(s);
        return -1;
    }
    s_log(LOG_DEBUG, "New thread created");
    return 0;
}

#endif /* USE_WIN32 */

#ifdef USE_OS2

int sthreads_init(void) {
    return 0;
}

unsigned long stunnel_process_id(void) {
    PTIB ptib=NULL;
    DosGetInfoBlocks(&ptib, NULL);
    return (unsigned long)ptib->tib_ordinal;
}

unsigned long stunnel_thread_id(void) {
    PPIB ppib=NULL;
    DosGetInfoBlocks(NULL, &ppib);
    return (unsigned long)ppib->pib_ulpid;
}

int create_client(SOCKET ls, SOCKET s, CLI *arg, void *(*cli)(void *)) {
    (void)ls; /* this parameter is only used with USE_FORK */
    s_log(LOG_DEBUG, "Creating a new thread");
    if((long)_beginthread((void(*)(void *))cli, NULL, arg->opt->stack_size, arg)==-1L) {
        ioerror("_beginthread");
        str_free(arg);
        if(s>=0)
            closesocket(s);
        return -1;
    }
    s_log(LOG_DEBUG, "New thread created");
    return 0;
}

#endif /* USE_OS2 */

#ifdef _WIN32_WCE

long _beginthread(void (*start_address)(void *),
        int stack_size, void *arglist) {
    DWORD thread_id;
    HANDLE handle;

    handle=CreateThread(NULL, stack_size,
        (LPTHREAD_START_ROUTINE)start_address, arglist,
        STACK_SIZE_PARAM_IS_A_RESERVATION, &thread_id);
    if(!handle)
        return -1L;
    CloseHandle(handle);
    return 0;
}

void _endthread(void) {
    ExitThread(0);
}

#endif /* _WIN32_WCE */

#ifdef DEBUG_STACK_SIZE

#define STACK_RESERVE (STACK_SIZE/8)
#define VERIFY_AREA ((STACK_SIZE-STACK_RESERVE)/sizeof(uint32_t))
#define TEST_VALUE 0xdeadbeef

/* some heuristic to determine the usage of client stack size */
void stack_info(int init) { /* 1-initialize, 0-display */
    uint32_t table[VERIFY_AREA];
    int i, num;
    static int min_num=VERIFY_AREA;

    if(init) {
        for(i=0; i<VERIFY_AREA; i++)
            table[i]=TEST_VALUE;
    } else {
        /* the stack is growing down */
        for(i=0; i<VERIFY_AREA; i++)
            if(table[i]!=TEST_VALUE)
                break;
        num=i;
        /* the stack is growing up */
        for(i=0; i<VERIFY_AREA; i++)
            if(table[VERIFY_AREA-i-1]!=TEST_VALUE)
                break;
        if(i>num) /* use the higher value */
            num=i;
        if(num<64) {
            s_log(LOG_NOTICE, "STACK_RESERVE is too high");
            return;
        }
        if(num<min_num)
            min_num=num;
        s_log(LOG_NOTICE,
            "stack_info: size=%d, current=%d (%d%%), maximum=%d (%d%%)",
            STACK_SIZE,
            (int)((VERIFY_AREA-num)*sizeof(uint32_t)),
            (int)((VERIFY_AREA-num)*sizeof(uint32_t)*100/STACK_SIZE),
            (int)((VERIFY_AREA-min_num)*sizeof(uint32_t)),
            (int)((VERIFY_AREA-min_num)*sizeof(uint32_t)*100/STACK_SIZE));
    }
}

#endif /* DEBUG_STACK_SIZE */

/* end of sthreads.c */
