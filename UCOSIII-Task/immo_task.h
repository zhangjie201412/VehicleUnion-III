#ifndef IMMO_TASK_H_
#define IMMO_TASK_H_


#include "includes.h"

//�������ȼ�
#define IMMO_TASK_PRIO		12u
//�����ջ��С	
#define IMMO_STK_SIZE 		256u
//������ƿ�
extern OS_TCB ImmoTaskTCB;
//�����ջ	
extern CPU_STK IMMO_TASK_STK[IMMO_STK_SIZE];

void immo_task(void *p_arg);

#endif
