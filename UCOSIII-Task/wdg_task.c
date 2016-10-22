#include "includes.h"
#include "wdg_task.h"
#include "wdg.h"
#include "utils.h"

OS_TCB WdgTaskTCB;
CPU_STK WDG_TASK_STK[WDG_STK_SIZE];

void wdg_task(void *unused)
{
    OS_ERR err;

    unused = unused;
    iwdg_init(IWDG_Prescaler_256, 0xfff);

    while(1) {
		OSTimeDlyHMSM(0, 0, WDG_FEED_INTERFVAL,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        iwdg_feed();
    }
}
