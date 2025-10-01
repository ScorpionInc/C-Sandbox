/* si_thread.h
 * Language: C
 * Created : 20251001
 * Purpose : Generalize some fo the common thread functions/structures for
 *           better cross-platform support.
*/

#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#define SI_PTHREAD (1)
#else
#define SI_PTHREAD (0)
#endif// Test for PThreads

#define SI_PTHREAD_SUCCESS (0)
#define SI_PTHREAD_ERROR   (-1)

#ifdef _WIN32

#include <handleapi.h> // CloseHandle()
#include <processthreadsapi.h> // CreateThread(), GetThreadId()
#include <sysinfoapi.h> // GetSystemInfo(), SYSTEM_INFO
#include <synchapi.h> // WaitForSingleObject(), WaitForSingleObjectEx()

#elif SI_PTHREAD

#ifndef _POSIX_C_SOURCE
// Define default minimum POSIX Feature version
#define _POSIX_C_SOURCE 200809L
#endif//_POSIX_C_SOURCE

#if defined(__linux__) || defined(__unix__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif//_GNU_SOURCE
#endif// Linux or Unix(FreeBSD)

#include <pthread.h> // pthread_create()
#include <signal.h> // pthread_kill()
#include <unistd.h> // sysconf, usleep

#if defined(__APPLE__)
int pthread_timedjoin(pthread_t thread, void** pp_result,
	struct timespec* p_timeout);
#else
// Linux or Unix(FreeBSD)
#define pthread_timedjoin(thread, pp_result, p_timeout) \
	pthread_timedjoin_np(thread, pp_result, p_timeout)
#endif

#else
#warning Unknown/Unsupported Operating System.
#endif // OS Test

#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint32_t

#ifdef _WIN32

#define si_thread_t HANDLE
#define si_thread_id_t DWORD
#define si_thread_func_t DWORD WINAPI
#define si_thread_func_return_t DWORD

#define si_thread_create(thread, func, p_arg) \
	(thread = CreateThread(NULL, 0, func, p_arg, 0, NULL))
#define si_thread_get_id(thread) (GetThreadId(thread))

#define si_thread_is_valid(thread) \
	(NULL != thread)
#define si_thread_is_running(thread) \
	(WAIT_TIMEOUT == WaitForSingleObject(thread, 0))

#define si_thread_kill(thread) \
	(SI_PTHREAD_SUCCESS != TerminateThread(thread, 0))
#define si_thread_free(thread) \
	((void)CloseHandle(thread); thread = INVALID_HANDLE_VALUE;)

#elif SI_PTHREAD

#define si_thread_t pthread_t
#define si_thread_id_t unsigned long
#define si_thread_func_t void*
#define si_thread_func_return_t void*

#define si_thread_create(thread, func, p_arg) \
	((void)pthread_create(thread, NULL, func, p_arg))
#define si_thread_get_id(thread) ((si_thread_id_t)thread)

#define si_thread_is_valid(thread) \
	(SI_PTHREAD_SUCCESS == pthread_kill(thread, 0))
// TODO This is not 100% accurate for detached pthreads.
// Unlike windows threads, pthreads, after they are created, start
//    immediately and remain "running" until they are dead/joined.
#define si_thread_is_running(thread) \
	(si_thread_is_valid(thread))

#define si_thread_kill(thread) \
	(SI_PTHREAD_SUCCESS == pthread_kill(thread, SIGKILL))
#define si_thread_free(thread) // NOP

#endif // OS specific defines


#ifndef SI_THREAD_H
#define SI_THREAD_H

size_t si_cpu_core_count();

si_thread_func_return_t si_thread_join(si_thread_t* const p_thread);
si_thread_func_return_t si_thread_timedjoin(si_thread_t* const p_thread,
	const uint32_t millisecs);

#endif//SI_THREAD_H