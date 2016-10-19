#include "stm32f10x.h"
#include "bsp.h"
#include "stdio.h"
#include "includes.h"
#include "start_task.h"
#include "delay.h"

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();

    BSP_Init();
    OSInit(&err);

    printf("\r\n\r\n++Now Start++\r\n\r\n");
    OS_CRITICAL_ENTER();
    OSTaskCreate((OS_TCB     *)&StartTaskTCB,
                 (CPU_CHAR   *)"Start Task",
                 (OS_TASK_PTR )start_task,
                 (void       *)0,
                 (OS_PRIO     )START_TASK_PRIO,
                 (CPU_STK    *)&START_TASK_STK[0],
                 (CPU_STK_SIZE)START_STK_SIZE/10,
                 (CPU_STK_SIZE)START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
    OS_CRITICAL_EXIT();
    OSStart(&err);

    while(1) {

    }
}
