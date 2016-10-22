#include "led_task.h"
#include "utils.h"
#include "cJSON.h"
#include "flash.h"
#include "sim800.h"
#include "utils.h"
#include "flexcan.h"

//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];

CanTxMsg rpmMsg =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00
};

void led0_task(void *p_arg)
{
    uint8_t i;
    int8_t ret;
    OS_ERR err;
    CanRxMsg *rxMsg;

    p_arg = p_arg;
    //sim800_setup();
    flexcan_init(CAN_500K);
    flexcan_filter(0x7e8, 0x7e8, 0x7ff, 0x7ff);
    //flexcan_filter(0x00, 0x00, 0x00, 0x00);
    while(1)
    {
        OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
        //logi("signal = %d", sim800_get_signal());
    }
}
