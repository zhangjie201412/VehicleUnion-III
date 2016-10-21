#ifndef __START_TASK_H__
#define __START_TASK_H__

#include "includes.h"

#define START_TASK_PRIO     3u
#define START_STK_SIZE      512u
extern OS_TCB StartTaskTCB;
extern CPU_STK START_TASK_STK[START_STK_SIZE];

void start_task(void *parg);

#endif
