#include "transmit.h"
#include "sim800.h"
#include "utils.h"
#include "cJSON.h"
#include "ringbuffer.h"
#include "pal.h"
#include "vehicles.h"

#define HEARTBEAT_INTERVAL      15

#define ENG_INTERVAL                    50
#define AT_INTERVAL                     60
#define ABS_INTERVAL                    40
#define BCM_INTERVAL                    30

OS_TCB TransmitCallbackTaskTCB;
CPU_STK TRANSMIT_CALLBACK_TASK_STK[TRANSMIT_CALLBACK_STK_SIZE];
//heartbeat thread
OS_TCB HeartbeatTaskTCB;
CPU_STK HEARTBEAT_TASK_STK[HEARTBEAT_STK_SIZE];
//upload thread
OS_TCB UploadTaskTCB;
CPU_STK UPLOAD_TASK_STK[UPLOAD_STK_SIZE];

uint8_t heartbeat_count = 0;

CtrlItem ctrlTable[CONTROL_END] = {
    {CONTROL_WINDOW, "bcm_fun_window"},
    {CONTROL_DOOR, "bcm_fun_door"},
    {CONTROL_LIGHT, "bcm_fun_lamp"},
    {CONTROL_SUNROOF, "bcm_fun_sunroof"},
    {CONTROL_TRUNK, "bcm_fun_trunk"},
    {CONTROL_FINDCAR, "bcm_fun_findcar"},
    {CONTROL_IMMOLOCK, "bcm_fun_immolock"},
};

PidItem pidList[PID_SIZE] =
{
    
    {ENG_DATA_RPM, "eng_data_rpm", ENG_INTERVAL},
    {ENG_DATA_VS, "eng_data_vs", ENG_INTERVAL},
    {ENG_DATA_ECT, "eng_data_ect", ENG_INTERVAL},
    {ENG_DATA_IAT, "eng_data_iat", ENG_INTERVAL},
    {ENG_DATA_APP, "eng_data_app", ENG_INTERVAL},
    {ENG_DATA_TP, "eng_data_tp", ENG_INTERVAL},
    {ENG_DATA_ERT, "eng_data_ert", ENG_INTERVAL},
    {ENG_DATA_LOAD, "eng_data_load", ENG_INTERVAL},
    {ENG_DATA_LTFT, "eng_data_ltft", ENG_INTERVAL},
    {ENG_DATA_STFT, "eng_data_stft", ENG_INTERVAL},
    {ENG_DATA_MISFIRE1, "eng_data_misfire1", ENG_INTERVAL},
    {ENG_DATA_MISFIRE2, "eng_data_misfire2", ENG_INTERVAL},
    {ENG_DATA_MISFIRE3, "eng_data_misfire3", ENG_INTERVAL},
    {ENG_DATA_MISFIRE4, "eng_data_misfire4", ENG_INTERVAL},
    {ENG_DATA_MISFIRE5, "eng_data_misfire5", ENG_INTERVAL},
    {ENG_DATA_MISFIRE6, "eng_data_misfire6", ENG_INTERVAL},
    {ENG_DATA_FCLS, "eng_data_fcls", ENG_INTERVAL},
    {ENG_DATA_KEYSTATUS, "eng_data_keystatus", ENG_INTERVAL},
    {ENG_DATA_HO2S1, "eng_data_ho2s1", ENG_INTERVAL},
    {ENG_DATA_HO2S2, "eng_data_ho2s2", ENG_INTERVAL},
    {ENG_DATA_MAP, "eng_data_map", ENG_INTERVAL},
    {ENG_DATA_INJECTPULSE, "eng_data_injectpulse", ENG_INTERVAL},
    {ENG_DATA_OILPRESSURE, "eng_data_oilpressure", ENG_INTERVAL},
    {ENG_DATA_OILLEVELSTATUS, "eng_data_oillevelstatus", ENG_INTERVAL},
    {ENG_DATA_AF, "eng_data_af", ENG_INTERVAL},
    {ENG_DATA_IGTIMING, "eng_data_igtiming", ENG_INTERVAL},
    {ENG_DATA_MAF, "eng_data_maf", ENG_INTERVAL},
    {ENG_DATA_OILLIFE, "eng_data_oillife", ENG_INTERVAL},
    {ENG_DATA_OILTEMP, "eng_data_oiltemp", ENG_INTERVAL},
    {ENG_DATA_FUEL, "eng_data_fuel", ENG_INTERVAL},
    {ENG_DATA_FUELLEVEL, "eng_data_fuellevel", ENG_INTERVAL},
    {ENG_DATA_FUELTANK, "eng_data_fueltank", ENG_INTERVAL},
    {AT_DATA_OILTEMP, "at_data_oiltemp", AT_INTERVAL},
    {ABS_DATA_OILLEVEL, "abs_data_oillevel", ABS_INTERVAL},
    {BCM_DATA_CHARGESTATUS, "bcm_data_chargestatus", BCM_INTERVAL},
    {BCM_DATA_BATTCURRENT, "bcm_data_battcurrent", BCM_INTERVAL},
    {BCM_DATA_BATTSTATUS, "bcm_data_battstatus", BCM_INTERVAL},
    {BCM_DATA_BATTVOLT, "bcm_data_battvolt", BCM_INTERVAL},
    {BCM_DATA_DDA, "bcm_data_dda", BCM_INTERVAL},
    {BCM_DATA_PDA, "bcm_data_pda", BCM_INTERVAL},
    {BCM_DATA_RRDA, "bcm_data_rrda", BCM_INTERVAL},
    {BCM_DATA_LRDA, "bcm_data_lrda", BCM_INTERVAL},
    {BCM_DATA_SUNROOF, "bcm_data_sunroof", BCM_INTERVAL},
    {BCM_DATA_PARKLAMP, "bcm_data_parklamp", BCM_INTERVAL},
    {BCM_DATA_HEADLAMP, "bcm_data_headlamp", BCM_INTERVAL},
    {BCM_DATA_HIGHBEAM, "bcm_data_highbeam", BCM_INTERVAL},
    {BCM_DATA_HAZARD, "bcm_data_hazard", BCM_INTERVAL},
    {BCM_DATA_FRONTFOG, "bcm_data_frontfog", BCM_INTERVAL},
    {BCM_DATA_REARFOG, "bcm_data_rearfog", BCM_INTERVAL},
    {BCM_DATA_LEFTTURN, "bcm_data_leftturn", BCM_INTERVAL},
    {BCM_DATA_RIGHTTURN, "bcm_data_rightturn", BCM_INTERVAL},
    {BCM_DATA_ODO, "bcm_data_odo", BCM_INTERVAL},
    {TPMS_DATA_LFTIREP, "tpms_data_lftirep", BCM_INTERVAL},
    {TPMS_DATA_RFTIREP, "tpms_data_rftirep", BCM_INTERVAL},
    {TPMS_DATA_LRTIREP, "tpms_data_lrtirep", BCM_INTERVAL},
    {TPMS_DATA_RRTIREP, "tpms_data_rrtirep", BCM_INTERVAL},
};

void transmit_callback_task(void *unused)
{
    uint8_t i;
    cJSON *json, *item;
    OS_ERR err;
    uint8_t buf[256];
    uint16_t index = 0;
    uint8_t recv;
    uint8_t msg_type;
    uint8_t heartbeat_rsp;
    CtrlMsg ctrlMsg;
    char *tmp;

    memset(&ctrlMsg, 0x00, sizeof(ctrlMsg));

    while(1) {
        OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, 0, &err);
        memset(buf, 0x00, 256);
        index = 0;
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            buf[index ++] = recv;
        }
        json = cJSON_Parse((const char *)buf);
        if(!json) {
            loge("[%s]", cJSON_GetErrorPtr());
        } else {
            msg_type = json_get_msg_type(json);
            logi("msg_type = %d", msg_type);

            switch(msg_type) {
                case MSG_TYPE_HEARTBEAT_RSP:
                    heartbeat_rsp = json_get_heartbeat(json);
                    if(((heartbeat_count - 1) * 2 + 1) == heartbeat_rsp) {
                        logi("heartbeat!!");
                    } else {
                        loge("failed to get heartbeat response");
                    }
                    break;
                case MSG_TYPE_CTRL:
                    for(i = 0; i < CONTROL_END; i++) {
                        tmp = strstr((char *)buf, ctrlTable[i].key);
                        if(tmp) {
                            item = cJSON_GetObjectItem(json,
                                    ctrlTable[i].key);
                            ctrlMsg.id = i;
                            ctrlMsg.cmd_id = 0;
                            if(!strcmp(item->valuestring, "ON")) {
                                ctrlMsg.value = 1;
                            } else if(!strcmp(item->valuestring, "OFF")) {
                                ctrlMsg.value = 0;
                            }
                            logi("ctrl value = %d", ctrlMsg.value);
                            //post msg to vehicle control thread
                            OSTaskQPost(
                                    (OS_TCB *)&ControlTaskTCB,
                                    (void *)&ctrlMsg,
                                    (OS_MSG_SIZE)sizeof(CtrlMsg),
                                    (OS_OPT)OS_OPT_POST_FIFO,
                                    (OS_ERR *)&err
                                    );
                        }
                    }
                    break;
                case MSG_TYPE_CLEAR_FAULT:
                    vehicle_clear_code();
                    break;
                case MSG_TYPE_VEHICLE_TYPE:
                    break;
                default:
                    break;
            }

            cJSON_Delete(json);
        }
    }
}

void heartbeat_task(void *unused)
{
    OS_ERR err;

    while(1) {
        OSTimeDlyHMSM(0, 0, HEARTBEAT_INTERVAL,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        heartbeat_count = (heartbeat_count == 100) ? 0 : heartbeat_count;
        if(is_connected()){
            heartbeat(heartbeat_count++);
        } else {
            loge("gprs is not connect to server");
        }
    }
}

void upload_task(void *unused)
{
    OS_ERR err;
    uint8_t i;

    while(1) {
        OSTimeDlyHMSM(0, 0, 1,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        if(!is_connected())
            continue;

        for(i = 0; i < PID_SIZE; i++) {
            mUpdateList[i].spend_time += 1;
            if(mUpdateList[i].updated &&
                    (mUpdateList[i].spend_time >= pidList[i].interval)) {
                mUpdateList[i].pid = i;
                logi("upload %d", i);
                //upload the pid data
                upload_item(&mUpdateList[i], pidList[i].key);
                mUpdateList[i].updated = FALSE;
                mUpdateList[i].spend_time = 0;
            }
        }
    }
}

void transmit_init(void)
{
    OS_ERR err;

    OSTaskCreate((OS_TCB 	* )&TransmitCallbackTaskTCB,
            (CPU_CHAR	* )"transmit callback task",
            (OS_TASK_PTR )transmit_callback_task,
            (void		* )0,
            (OS_PRIO	  )TRANSMIT_CALLBACK_TASK_PRIO,
            (CPU_STK   * )&TRANSMIT_CALLBACK_TASK_STK[0],
            (CPU_STK_SIZE)TRANSMIT_CALLBACK_STK_SIZE/10,
            (CPU_STK_SIZE)TRANSMIT_CALLBACK_STK_SIZE,
            (OS_MSG_QTY  )0,
            (OS_TICK	  )0,
            (void   	* )0,
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);
    //heartbeat thread
    OSTaskCreate((OS_TCB 	* )&HeartbeatTaskTCB,
            (CPU_CHAR	* )"Heartbeat task",
            (OS_TASK_PTR )heartbeat_task,
            (void		* )0,
            (OS_PRIO	  )HEARTBEAT_TASK_PRIO,
            (CPU_STK   * )&HEARTBEAT_TASK_STK[0],
            (CPU_STK_SIZE)HEARTBEAT_STK_SIZE/10,
            (CPU_STK_SIZE)HEARTBEAT_STK_SIZE,
            (OS_MSG_QTY  )0,
            (OS_TICK	  )0,
            (void   	* )0,
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);
    //upload thread
    OSTaskCreate((OS_TCB 	* )&UploadTaskTCB,
            (CPU_CHAR	* )"Upload task",
            (OS_TASK_PTR )upload_task,
            (void		* )0,
            (OS_PRIO	  )UPLOAD_TASK_PRIO,
            (CPU_STK   * )&UPLOAD_TASK_STK[0],
            (CPU_STK_SIZE)UPLOAD_STK_SIZE/10,
            (CPU_STK_SIZE)UPLOAD_STK_SIZE,
            (OS_MSG_QTY  )0,
            (OS_TICK	  )0,
            (void   	* )0,
            (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
            (OS_ERR 	* )&err);
    sim800_setup();
    heartbeat_count = 0;
}

void transmit_control_rsp(uint32_t cmd_id, uint8_t id)
{
    control_rsp(cmd_id, id, ctrlTable[id].key);
}
