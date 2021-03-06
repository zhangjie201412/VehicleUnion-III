#include "start_task.h"
#include "led_task.h"
#include "immo_task.h"
#include "wdg_task.h"
#include "vehicles.h"

//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];

//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()


//开始任务函数
void start_task(void *p_arg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    p_arg = p_arg;

    CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
    //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
    OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

    OS_CRITICAL_ENTER();	//进入临界区
#if 1
    OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
            (CPU_CHAR	* )"led0 task", 		
            (OS_TASK_PTR )led0_task, 			
            (void		* )0,					
            (OS_PRIO	  )LED0_TASK_PRIO,     
            (CPU_STK   * )&LED0_TASK_STK[0],	
            (CPU_STK_SIZE)LED0_STK_SIZE/10,	
            (CPU_STK_SIZE)LED0_STK_SIZE,		
            (OS_MSG_QTY  )0,					
            (OS_TICK	  )0,					
            (void   	* )0,					
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);				
#endif
    OSTaskCreate((OS_TCB 	* )&ImmoTaskTCB,		
            (CPU_CHAR	* )"immo task", 		
            (OS_TASK_PTR )immo_task, 			
            (void		* )0,					
            (OS_PRIO	  )IMMO_TASK_PRIO,     
            (CPU_STK   * )&IMMO_TASK_STK[0],	
            (CPU_STK_SIZE)IMMO_STK_SIZE/10,	
            (CPU_STK_SIZE)IMMO_STK_SIZE,		
            (OS_MSG_QTY  )0,					
            (OS_TICK	  )0,					
            (void   	* )0,					
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);				
    //watchdog thread
    OSTaskCreate((OS_TCB 	* )&WdgTaskTCB,		
            (CPU_CHAR	* )"wdg task", 		
            (OS_TASK_PTR )wdg_task,	
            (void		* )0,					
            (OS_PRIO	  )WDG_TASK_PRIO,     
            (CPU_STK   * )&WDG_TASK_STK[0],	
            (CPU_STK_SIZE)WDG_STK_SIZE/10,	
            (CPU_STK_SIZE)WDG_STK_SIZE,		
            (OS_MSG_QTY  )0,					
            (OS_TICK	  )0,					
            (void   	* )0,					
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);				
    OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			

    OS_CRITICAL_EXIT();	//进入临界区
}
