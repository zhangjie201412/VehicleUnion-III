#include "includes.h"
#include "wdg_task.h"
#include "wdg.h"
#include "utils.h"
#include "transmit.h"

OS_TCB WdgTaskTCB;
CPU_STK WDG_TASK_STK[WDG_STK_SIZE];
OS_FLAG_GRP FLAG_TaskRunStatus;

void wdg_task(void *unused)
{
    OS_ERR err;

    unused = unused;
    iwdg_init(IWDG_Prescaler_256, 0xfff);

    OSFlagCreate(
            (OS_FLAG_GRP *)&FLAG_TaskRunStatus,
            (CPU_CHAR *)"FLAG TaskRunStatus",
            (OS_FLAGS)0,
            (OS_ERR *)&err
            );

    while(1) {
//		OSTimeDlyHMSM(0, 0, WDG_FEED_INTERFVAL,
//                0, OS_OPT_TIME_HMSM_STRICT, &err);
        OSFlagPend(
                (OS_FLAG_GRP *)&FLAG_TaskRunStatus,
                (OS_FLAGS)(FLAG_HEARTBEAT | FLAG_UPLOAD),
                (OS_TICK)0,
                (OS_OPT)OS_OPT_PEND_FLAG_SET_ALL + OS_OPT_PEND_FLAG_CONSUME,
                (CPU_TS *)0,
                (OS_ERR *)err
                );
//        logi("%s: feed!!", __func__);
        iwdg_feed();
    }
}
