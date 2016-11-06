#ifndef IMMO_TASK_H_
#define IMMO_TASK_H_


#include "includes.h"

//任务优先级
#define IMMO_TASK_PRIO		12u
//任务堆栈大小	
#define IMMO_STK_SIZE 		256u
//任务控制块
extern OS_TCB ImmoTaskTCB;
//任务堆栈	
extern CPU_STK IMMO_TASK_STK[IMMO_STK_SIZE];

void immo_task(void *p_arg);

#endif
