#include "transmit.h"
#include "l206.h"
#include "utils.h"
#include "cJSON.h"
#include "ringbuffer.h"
#include "pal.h"
#include "vehicles.h"
#include "config.h"
#include "sys.h"
#include "wdg_task.h"

#define HEARTBEAT_INTERVAL              10
#define HEARTBEAT_RSP_TIMEOUT              8
#define LOGIN_RSP_TIMEOUT              10

#define HEARTBEAT_FAIL_TOLERENT         3

#define ENG_INTERVAL                    500
#define AT_INTERVAL                     600
#define ABS_INTERVAL                    400
#define BCM_INTERVAL                    300

OS_TCB TransmitCallbackTaskTCB;
CPU_STK TRANSMIT_CALLBACK_TASK_STK[TRANSMIT_CALLBACK_STK_SIZE];
//heartbeat thread
OS_TCB HeartbeatTaskTCB;
CPU_STK HEARTBEAT_TASK_STK[HEARTBEAT_STK_SIZE];
//upload thread
OS_TCB UploadTaskTCB;
CPU_STK UPLOAD_TASK_STK[UPLOAD_STK_SIZE];

OS_MUTEX mTransmitMutex;

uint8_t heartbeat_count = 0;
uint8_t heartbeat_fail_times = 0;

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
    {ENG_DATA_REALFUELCO, "eng_data_realfuelco", ENG_INTERVAL},
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
    int cmd_id, ctrl_val;
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
        //logi("%s: %s", __func__, buf);
        json = cJSON_Parse((const char *)buf);
        if(!json) {
            loge("[%s]", cJSON_GetErrorPtr());
        } else {
            msg_type = json_get_msg_type(json);
            //logi("msg_type = %d", msg_type);

            switch(msg_type) {
                case MSG_TYPE_HEARTBEAT_RSP:
                    heartbeat_rsp = json_get_heartbeat(json);
                    if(((heartbeat_count - 1) * 2 + 1) == heartbeat_rsp) {
                        //post heart pend
                        OSTaskSemPost(&HeartbeatTaskTCB,
                                OS_OPT_POST_NONE, &err);
                    } else {
                        loge("failed to get heartbeat response");
                    }
                    break;
                case MSG_TYPE_CTRL:
#ifdef SERVER_IS_K
                    item = cJSON_GetObjectItem(json, KEY_CMD_ID);
                    cmd_id = item->valueint;
#endif

                    for(i = 0; i < CONTROL_END; i++) {
                        tmp = strstr((char *)buf, ctrlTable[i].key);
                        if(tmp) {
                            item = cJSON_GetObjectItem(json,
                                    ctrlTable[i].key);
#ifdef SERVER_IS_K
                            ctrl_val = item->valueint;
                            ctrlMsg.id = i;
                            ctrlMsg.cmd_id = cmd_id;
                            ctrlMsg.value = ctrl_val;
#elif defined SERVER_IS_VEHICLE_UNION
                            ctrlMsg.id = i;
                            ctrlMsg.cmd_id = 0;
                            if(!strcmp(item->valuestring, "ON")) {
                                ctrlMsg.value = 1;
                            } else if(!strcmp(item->valuestring, "OFF")) {
                                ctrlMsg.value = 0;
                            }
#endif
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
                    item = cJSON_GetObjectItem(json, KEY_VEHICLE_TYPE);
                    if(!strcmp(item->valuestring, "toyota")) {
                        logi("##TOYOTA##");
                        vehicle_setup(VEHICLE_TOYOTA);
                        flexcan_set_engine_id(0x7e8);
                    } else if(!strcmp(item->valuestring, "gm")) {
                        logi("##GM##");
                        vehicle_setup(VEHICLE_GM);
                        flexcan_set_engine_id(0x7e8);
                    } else if(!strcmp(item->valuestring, "vag1")) {
                        logi("##VAG1##");
                        vehicle_setup(VEHICLE_VAG);
                    } else if(!strcmp(item->valuestring, "vag")) {
                        logi("##PASSAT##");
                        vehicle_setup(VEHICLE_PASSAT);
                    } else if(!strcmp(item->valuestring, "greatwall")) {
                        logi("##HAVAL##");
                        vehicle_setup(VEHICLE_HAVAL);
                    }
                    else {
                        logi("##EOBD##");
                        vehicle_setup(VEHICLE_EOBD);
                        flexcan_set_engine_id(0x7e8);
                    }
                    //post heart pend
                    OSTaskSemPost(&HeartbeatTaskTCB,
                            OS_OPT_POST_NONE, &err);
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
    static bool logined = FALSE;
    uint8_t login_retry_count = 0;

    while(1) {
        //set the wdg feed flag
        OSFlagPost(
                (OS_FLAG_GRP *)&FLAG_TaskRunStatus,
                (OS_FLAGS)FLAG_HEARTBEAT,
                (OS_OPT)OS_OPT_POST_FLAG_SET,
                (OS_ERR *)&err
                );
#ifdef SERVER_IS_K
        if(!is_connected()) {
            xdelay(2);
            continue;
        }
        if(!logined) {
            login();
            //wait for login rsp
            OSTaskSemPend(
                    OS_CFG_TICK_RATE_HZ * LOGIN_RSP_TIMEOUT,
                    OS_OPT_PEND_BLOCKING,
                    0,
                    &err
                    );
            if(err == OS_ERR_TIMEOUT) {
                loge("##wait for login timeout");
                loge("##retry##");
                if(login_retry_count ++ > 10) {
                    loge("login retry %d times, reboot system", login_retry_count);
                    SystemReset();
                }
                continue;
            } else {
                logi("get login rsp!");
                logined = TRUE;
                login_retry_count = 0;
            }
        }
#elif defined SERVER_IS_VEHICLE_UNION
        logined = TRUE;
#endif
        OSTimeDlyHMSM(0, 0, HEARTBEAT_INTERVAL,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        if(is_connected() && logined){
            heartbeat_count = (heartbeat_count == 100) ? 0 : heartbeat_count;
            transmit_lock();
            //get signal
            logi("signal = %d", l206_get_signal());
            heartbeat(heartbeat_count++);
            //wait for heartbeat rsp
            OSTaskSemPend(
                    OS_CFG_TICK_RATE_HZ * HEARTBEAT_RSP_TIMEOUT,
                    OS_OPT_PEND_BLOCKING,
                    0,
                    &err
                    );
            if(err == OS_ERR_TIMEOUT) {
                loge("##wait for heartbeat timeout");
                loge("##need to re-connect to server");
                if(++heartbeat_fail_times > HEARTBEAT_FAIL_TOLERENT) {
                    heartbeat_fail_times = 0;
                    //transmit_reconnect();
                    //temp solution
                    loge("##SYSTEM REBOOT##");
                    l206_powerdown();
                    SystemReset();
                }
            } else {
                //logi("get heart beat rsp!");
                heartbeat_fail_times = 0;
            }
            transmit_unlock();
        } else {
            loge("gprs is not connect to server");
        }
    }
}

void transmit_reconnect(void)
{
    l206_set_connected(FALSE);
    l206_powerdown();
    l206_setup(TRUE);
}

void upload_task(void *unused)
{
    OS_ERR err;
    uint8_t i;
    static uint16_t upload_timer = 0;

    while(1) {
        //set the wdg feed flag
        OSFlagPost(
                (OS_FLAG_GRP *)&FLAG_TaskRunStatus,
                (OS_FLAGS)FLAG_UPLOAD,
                (OS_OPT)OS_OPT_POST_FLAG_SET,
                (OS_ERR *)&err
                );
        OSTimeDlyHMSM(0, 0, 2,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        if(!is_connected())
            continue;

        for(i = 0; i < PID_SIZE; i++) {
            mUpdateList[i].spend_time += 2;
            if(mUpdateList[i].updated &&
                    (mUpdateList[i].spend_time >= pidList[i].interval)) {
                mUpdateList[i].pid = i;
                logi("upload %d", i);
                //upload the pid data
                transmit_lock();
                upload_item(&mUpdateList[i], pidList[i].key);
                transmit_unlock();
                mUpdateList[i].updated = FALSE;
                mUpdateList[i].spend_time = 0;
            }
        }
        upload_timer += 2;
        if(upload_timer >= 300) {
            upload_timer = 0;
            //upload location
            transmit_lock();
            upload_location();
            transmit_unlock();
        }
    }
}

void transmit_init(void)
{
    bool ret;
    OS_ERR err;

    OSMutexCreate(
            (OS_MUTEX *)&mTransmitMutex,
            (CPU_CHAR *)"TRANSMIT_MUTEX",
            &err
            );
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
    heartbeat_count = 0;
    ret = l206_setup(FALSE);
    if(!ret) {
        loge("##SYSTEM REBOOT##");
        l206_powerdown();
        //wait 30min to restart
        OSTimeDlyHMSM(0, 10, 0,
                0, OS_OPT_TIME_HMSM_STRICT, &err);
        SystemReset();
    }
}

void transmit_control_rsp(uint32_t cmd_id, uint8_t id)
{
    transmit_lock();
    control_rsp(cmd_id, id, ctrlTable[id].key);
    transmit_unlock();
}

void transmit_lock(void)
{
    OS_ERR err;

//    logi("+++%s+++", __func__);
    OSMutexPend(&mTransmitMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
}

void transmit_unlock(void)
{
    OS_ERR err;

//    logi("---%s---", __func__);
    OSMutexPost(&mTransmitMutex, OS_OPT_POST_NONE, &err);
}

