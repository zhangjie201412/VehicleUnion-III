#ifndef __WDG_TASK_H__
#define __WDG_TASK_H__

#include "includes.h"

#define WDG_FEED_INTERFVAL  8

#define WDG_TASK_PRIO		4u
#define WDG_STK_SIZE 		128u

extern OS_TCB WdgTaskTCB;
extern CPU_STK WDG_TASK_STK[WDG_STK_SIZE];

void wdg_task(void *unused);

#endif
