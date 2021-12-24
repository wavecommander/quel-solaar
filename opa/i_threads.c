#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "forge.h"
#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <mmsystem.h>
#include <process.h>

void *imagine_mutex_create()
{
	CRITICAL_SECTION *mutex;
	mutex = malloc(sizeof *mutex);
	InitializeCriticalSection(mutex);
	return mutex;
}

void imagine_mutex_destroy(void *mutex)
{
    DeleteCriticalSection(mutex);
    free(mutex);
}

void imagine_mutex_lock(void *mutex)
{
	EnterCriticalSection(mutex);
}

boolean imagine_mutex_lock_try(void *mutex)
{
	return TryEnterCriticalSection(mutex);
}

void imagine_mutex_unlock(void *mutex)
{
	LeaveCriticalSection(mutex);
}
#else
#include <pthread.h>
#include <unistd.h>

void *imagine_mutex_create()
{
    pthread_mutex_t *mutex;
    mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    return mutex;
}
void imagine_mutex_destroy(void *mutex)
{
    pthread_mutex_destroy(mutex);
    free(mutex);
}
void imagine_mutex_lock(void *mutex)
{
    pthread_mutex_lock(mutex);
}

boolean imagine_mutex_lock_try(void *mutex)
{
    return pthread_mutex_trylock(mutex);
}

void imagine_mutex_unlock(void *mutex)
{
    pthread_mutex_unlock(mutex);
}

#endif

typedef struct{
	void *mutex;
	char *file;
	uint line;
}IDebugLock;

IDebugLock *i_debug_locks = NULL;
uint i_debug_lock_count = 0;
uint i_debug_lock_allocated = 0;



void imagine_mutex_lock_debug(void *mutex, char *file, uint line)
{
	uint i;
	for(i = 0; i < i_debug_lock_count; i++)
	{
		if(i_debug_locks[i].mutex == mutex)
		{
			printf("Warning: Imagine: trying to lock already locked lock: file %s line %u\n", file, line);
			if(i_debug_locks[i].file != file || i_debug_locks[i].line != line)
				printf("Locked in: file %s line %u\n", i_debug_locks[i].file, i_debug_locks[i].line);
		}
	}
	imagine_mutex_lock(mutex);
	if(i_debug_lock_count == i_debug_lock_allocated)
	{
		i_debug_lock_allocated += 256;
		i_debug_locks = realloc(i_debug_locks, (sizeof *i_debug_locks) * i_debug_lock_allocated);  
	}
	i_debug_locks[i_debug_lock_count].file = file;
	i_debug_locks[i_debug_lock_count].line = line;
	i_debug_locks[i_debug_lock_count++].mutex = mutex;
}

boolean imagine_mutex_lock_try_debug(void *mutex, char *file, uint line)
{
	boolean output;
	output = imagine_mutex_lock_try(mutex);
	if(output)
	{
		if(i_debug_lock_count == i_debug_lock_allocated)
		{
			i_debug_lock_allocated += 256;
			i_debug_locks = realloc(i_debug_locks, (sizeof *i_debug_locks) * i_debug_lock_allocated);  
		}
		i_debug_locks[i_debug_lock_count].file = file;
		i_debug_locks[i_debug_lock_count].line = line;
		i_debug_locks[i_debug_lock_count++].mutex = mutex;
	}
	return output;
}

void imagine_mutex_unlock_debug(void *mutex, char *file, uint line)
{
	uint i;
	for(i = 0; i < i_debug_lock_count && i_debug_locks[i].mutex != mutex; i++);

	if(i < i_debug_lock_count)
	{
		i_debug_locks[i] = i_debug_locks[--i_debug_lock_count];

	}else
	{
		printf("Error: Imagine: trying to unlock lock that hassent been lockled: file %s line %u\n", file, line);	
		exit(0);
	}
	imagine_mutex_unlock(mutex);
}


void imagine_mutex_print_debug()
{
	uint i;
	printf("Printing list of all %u locks locked:\n", i_debug_lock_count);
	for(i = 0; i < i_debug_lock_count; i++)
		printf("Mutex locked at: file %s line %u\n", i_debug_locks[i].file, i_debug_locks[i].line);
}

/* Signals */
#ifdef _WIN32

void *imagine_signal_create()
{
//	HANDLE *p;
	CONDITION_VARIABLE *p;
	p = malloc(sizeof *p);
//	p[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	InitializeConditionVariable(p);
	return p;
}

void *magine_signal_destroy(void *signal)
{
//	CloseHandle(&((HANDLE *)signal));
	free(signal);
}

boolean imagine_signal_wait(void *signal, void *mutex)
{
	SleepConditionVariableCS (signal, mutex, INFINITE);
	return TRUE;
//	return WAIT_OBJECT_0 == WaitForSingleObject(&((HANDLE *)signal), INFINITE);
}

boolean imagine_signal_activate(void *signal)
{
	WakeAllConditionVariable(signal);
//	SetEvent(&((HANDLE *)signal));
}
#else
#endif

/* Treads */

typedef struct{
    void	(*func)(void *data);
    void	*data;
}ImagineThreadParams;

#ifdef _WIN32

DWORD WINAPI i_win32_thread(LPVOID param)
{
	ImagineThreadParams *thread_param;
	thread_param = (ImagineThreadParams *)param; 
	thread_param->func(thread_param->data);
	free(thread_param);
	return TRUE;
}

void imagine_thread(void (*func)(void *data), void *data)
{
	ImagineThreadParams *thread_param;
	thread_param = malloc(sizeof *thread_param);
	thread_param->func = func;
	thread_param->data = data;
	CreateThread(NULL, 0,  i_win32_thread, thread_param, 0, NULL); 
}

#else

void *i_thread_thread(void *param)
{
    ImagineThreadParams *thread_param;
    thread_param = (ImagineThreadParams *)param;
    thread_param->func(thread_param->data);
    free(thread_param);
    return NULL;
}

void imagine_thread(void (*func)(void *data), void *data)
{
    pthread_t thread_id;
    ImagineThreadParams *thread_param;
    thread_param = malloc(sizeof *thread_param);
    thread_param->func = func;
    thread_param->data = data;
    pthread_create(&thread_id, NULL, i_thread_thread, thread_param);
}

#endif

/* Execute */
#ifdef _WIN32

boolean imagine_execute(char *command)
{
    STARTUPINFO startup_info;
    PROCESS_INFORMATION process_info;
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&process_info, sizeof(process_info));
    return CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &startup_info, &process_info);
}

#else

boolean imagine_execute(char *command)
{
    uint id;
    id = fork();
    if(id == 0)
        return execl(command, NULL);
    return  FALSE;
}

#endif

//#define IMAGINE_THREAD_TEST

#ifdef IMAGINE_THREAD_TEST

typedef struct{
	void *mutex;
	void *condition;
	uint id;
}ImagineThreadTestParam;

void imagine_thread_test_func(void *data)
{
	ImagineThreadTestParam *params;
	uint i;
	params = (ImagineThreadTestParam *)data;
//	imagine_mutex_lock(params->mutex);
	while(TRUE)
	{
		printf("thread %u ->", params->id);
		for(i = 0; i < 10; i++)
			printf(" %u", i + 1);
		printf("\n", params->id);
		imagine_signal_activate(params->condition);
		imagine_signal_wait(params->condition, params->mutex);
	}
}

int main()
{
	ImagineThreadTestParam params[2];
	params[0].mutex = params[1].mutex = imagine_mutex_create();
	params[0].condition = params[1].condition = imagine_signal_create();
	params[0].id = 0;
	imagine_thread(imagine_thread_test_func, &params[0]);
	params[1].id = 1;	
	imagine_thread_test_func(&params[1]);
}

#endif