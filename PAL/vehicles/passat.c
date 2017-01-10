#include "passat.h"
#include "pal.h"
#include "flexcan.h"
#include "utils.h"

#define PASSAT_ENG_ID               0x740

VehiclesCtrlOps passat_ctrl_ops;
VehiclesDataOps passat_data_ops;
uint8_t passat_rx_buf[100];

PidSupportItem passatSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, SUPPORTED},
    {ENG_DATA_APP, UNSUPPORTED},
    {ENG_DATA_TP, SUPPORTED},
    {ENG_DATA_ERT, UNSUPPORTED},
    {ENG_DATA_LOAD, SUPPORTED},
    {ENG_DATA_LTFT, SUPPORTED},
    {ENG_DATA_STFT, SUPPORTED},
    {ENG_DATA_MISFIRE1, SUPPORTED},
    {ENG_DATA_MISFIRE2, SUPPORTED},
    {ENG_DATA_MISFIRE3, SUPPORTED},
    {ENG_DATA_MISFIRE4, SUPPORTED},
    {ENG_DATA_MISFIRE5, UNSUPPORTED},
    {ENG_DATA_MISFIRE6, UNSUPPORTED},
    {ENG_DATA_FCLS, UNSUPPORTED},
    {ENG_DATA_KEYSTATUS, UNSUPPORTED},
    {ENG_DATA_HO2S1, UNSUPPORTED},
    {ENG_DATA_HO2S2, UNSUPPORTED},
    {ENG_DATA_MAP, UNSUPPORTED},
    {ENG_DATA_INJECTPULSE, UNSUPPORTED},
    {ENG_DATA_OILPRESSURE, UNSUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, UNSUPPORTED},
    {ENG_DATA_AF, UNSUPPORTED},
    {ENG_DATA_IGTIMING, UNSUPPORTED},
    {ENG_DATA_MAF, SUPPORTED},
    {ENG_DATA_OILLIFE, UNSUPPORTED},
    {ENG_DATA_OILTEMP, UNSUPPORTED},
    {ENG_DATA_FUEL, UNSUPPORTED},
    {ENG_DATA_FUELLEVEL, UNSUPPORTED},
    {ENG_DATA_FUELTANK, UNSUPPORTED},
    {ENG_DATA_REALFUELCO, SUPPORTED},
    {AT_DATA_OILTEMP, UNSUPPORTED},
    {ABS_DATA_OILLEVEL, UNSUPPORTED},
    {BCM_DATA_CHARGESTATUS, UNSUPPORTED},
    {BCM_DATA_BATTCURRENT, UNSUPPORTED},
    {BCM_DATA_BATTSTATUS, UNSUPPORTED},
    {BCM_DATA_BATTVOLT, SUPPORTED},
    {BCM_DATA_DDA, UNSUPPORTED},
    {BCM_DATA_PDA, UNSUPPORTED},
    {BCM_DATA_RRDA, UNSUPPORTED},
    {BCM_DATA_LRDA, UNSUPPORTED},
    {BCM_DATA_SUNROOF, UNSUPPORTED},
    {BCM_DATA_PARKLAMP, UNSUPPORTED},
    {BCM_DATA_HEADLAMP, UNSUPPORTED},
    {BCM_DATA_HIGHBEAM, UNSUPPORTED},
    {BCM_DATA_HAZARD, UNSUPPORTED},
    {BCM_DATA_FRONTFOG, UNSUPPORTED},
    {BCM_DATA_REARFOG, UNSUPPORTED},
    {BCM_DATA_LEFTTURN, UNSUPPORTED},
    {BCM_DATA_RIGHTTURN, UNSUPPORTED},
    {BCM_DATA_ODO, SUPPORTED},
};


StdDataStream passatStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0x740, 5,
        {0x19, 0x00, 0x02, 0x21, 0x01},
        0x300, 3, 2,
    },
    {
        ENG_DATA_VS, 0x740, 5,
        {0x1d, 0x00, 0x02, 0x21, 0x05},
        0x300, 0x0a, 2,
    },
    {
        ENG_DATA_ECT, 0X740, 5,
        {0x19, 0x00, 0x02, 0x21, 0x01},
        0x300, 7, 2,
    },
    //IAT
    {
        ENG_DATA_IAT, 0X740, 5,
        {0x12, 0x00, 0x02, 0x21, 0x04},
        0x300, 0x0e, 2,
    },
    //APP
    {
        ENG_DATA_APP, 0x7e0, 8,
    },
    //TP
    {
        ENG_DATA_TP, 0x740, 5,
        {0x1d, 0x00, 0x02, 0x21, 0x03},
        0x300, 0x0a, 2,
    },
    //ERT
    {
        ENG_DATA_ERT, 0X7e0, 8,
    },
    //LOAD
    {
        ENG_DATA_LOAD, 0X740, 5,
        {0x1b, 0x00, 0x02, 0x21, 0x02},
        0x300, 7, 2,
    },
    //LTFT
    {
        ENG_DATA_LTFT, 0X740, 5,
        {0x1b, 0x00, 0x02, 0x21, 0x74},
        0x300, 0x0a, 2,
    },
    //STFT
    {
        ENG_DATA_STFT, 0X740, 5,
        {0x1b, 0x00, 0x02, 0x21, 0x74},
        0x300, 0x07, 2,
    },
    //MISFIRE1
    {
        ENG_DATA_MISFIRE1, 0X740, 5,
        {0x12, 0x00, 0x02, 0x21, 0x0f},
        0x300, 0x03, 2,
    },
    //MISFIRE2
    {
        ENG_DATA_MISFIRE2, 0X740, 5,
        {0x12, 0x00, 0x02, 0x21, 0x0f},
        0x300, 0x07, 2,
    },
    //MISFIRE3
    {
        ENG_DATA_MISFIRE3, 0X740, 5,
        {0x12, 0x00, 0x02, 0x21, 0x0f},
        0x300, 0x0a, 2,
    },
    //MISFIRE4
    {
        ENG_DATA_MISFIRE4, 0X740, 5,
        {0x11, 0x00, 0x02, 0x21, 0x10},
        0x300, 0x03, 2,
    },
    //MISFIRE5
    {
        ENG_DATA_MISFIRE5, 0X7df, 8,
    },
    //MISFIRE6
    {
        ENG_DATA_MISFIRE6, 0X7df, 8,
    },
    //FCLS
    {
        ENG_DATA_FCLS, 0X7df, 8,
    },
    //KEYSTATUS
    {
        ENG_DATA_KEYSTATUS, 0X7df, 8,
    },
    //HO2S1
    {
        ENG_DATA_HO2S1, 0X7e0, 8,
    },
    //HO2S2
    {
        ENG_DATA_HO2S2, 0X7e0, 8,
    },
    //MAP
    {
        ENG_DATA_MAP, 0X7e0, 8,
    },
    //INJECTPULSE
    {
        ENG_DATA_INJECTPULSE, 0X7df, 8,
    },
    //OILPRESSURE
    {
        ENG_DATA_OILPRESSURE, 0X7df, 8,
    },
    //OILLEVELSTATUS
    {
        ENG_DATA_OILLEVELSTATUS, 0X7df, 8,
    },
    //AF
    {
        ENG_DATA_AF, 0X7e0, 8,
    },
    //IGTIMING
    {
        ENG_DATA_IGTIMING, 0X7e0, 8,
    },
    //MAF
    {
        ENG_DATA_MAF, 0X740, 5,
        {0x11, 0x00, 0x02, 0x21, 0x10},
        0x300, 0x0e, 2,
    },
    //OILLIFE
    {
        ENG_DATA_OILLIFE, 0X7df, 8,
    },
    //OILTEMP
    {
        ENG_DATA_OILTEMP, 0X7df, 8,
    },
    //FUEL
    {
        ENG_DATA_FUEL, 0X7e0, 8,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELLEVEL, 0X7e0, 8,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELTANK, 0X7c0, 8,
    },
    //ENG_DATA_REALFUELCO
    {
        ENG_DATA_REALFUELCO, 0X714, 8,
        {0x03, 0x22, 0x22, 0x98, 0x55, 0x55, 0x55, 0x55},
        0x77e, 4, 2,
    },
    //AT_DATA_OILTEMP
    {
        AT_DATA_OILTEMP, 0X7e1, 8,
    },
    //ABS_DATA_OILLEVEL
    {
        ABS_DATA_OILLEVEL, 0X7b0, 8,
    },
    //BCM_DATA_CHARGESTATUS
    {
        BCM_DATA_CHARGESTATUS, 0X710, 8,
    },
    //BCM_DATA_BATTCURRENT
    {
        BCM_DATA_BATTCURRENT, 0X710, 8,
    },
    //BCM_DATA_BATTSTATUS
    {
        BCM_DATA_BATTSTATUS, 0X7b0, 8,
    },
    //BCM_DATA_BATTVOLT
    {
        BCM_DATA_BATTVOLT, 0X740, 5,
        {0x12, 0x00, 0x02, 0x21, 0x04},
        0x300, 7, 2,
    },
    //BCM_DATA_DDA
    {
        BCM_DATA_DDA, 0X765, 8,
    },
    //BCM_DATA_PDA
    {
        BCM_DATA_PDA, 0X765, 8,
    },
    //BCM_DATA_RRDA
    {
        BCM_DATA_RRDA, 0X765, 8,
    },
    //BCM_DATA_LRDA
    {
        BCM_DATA_LRDA, 0X765, 8,
    },
    //BCM_DATA_SUNROOF
    {
        BCM_DATA_SUNROOF, 0X70e, 8,
    },
    //BCM_DATA_PARKLAMP
    {
        BCM_DATA_PARKLAMP, 0X74a, 8,
    },
    //BCM_DATA_HEADLAMP
    {
        BCM_DATA_HEADLAMP, 0X750, 8,
    },
    //BCM_DATA_HIGHBEAM
    {
        BCM_DATA_HIGHBEAM, 0X765, 8,
    },
    //BCM_DATA_HAZARD
    {
        BCM_DATA_HAZARD, 0X70e, 8,
    },
    //BCM_DATA_FRONTFOG
    {
        BCM_DATA_FRONTFOG, 0X74a, 8,
    },
    //BCM_DATA_REARFOG
    {
        BCM_DATA_REARFOG, 0X74a, 8,
    },
    //BCM_DATA_LEFTTURN
    {
        BCM_DATA_LEFTTURN, 0X7c0, 8,
    },
    //BCM_DATA_RIGHTTURN
    {
        BCM_DATA_RIGHTTURN, 0X7c0, 8,
    },
    //BCM_DATA_ODO
    {
        BCM_DATA_ODO, 0X714, 8,
        {0x03, 0x22, 0x22, 0x03, 0x55, 0x55, 0x55, 0x55},
        0x77e, 4, 2,
    },
};

CanTxMsg passatEngineCmd =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void passat_setup(Vehicles *vehicle)
{
    passat_ctrl_ops.control_window = passat_ctrl_window;
    passat_ctrl_ops.control_door = passat_ctrl_door;
    passat_ctrl_ops.control_sunroof = passat_ctrl_sunroof;
    passat_ctrl_ops.control_light = passat_ctrl_light;
    passat_ctrl_ops.control_trunk = passat_ctrl_trunk;
    passat_ctrl_ops.control_findcar = passat_ctrl_findcar;
    passat_ctrl_ops.clear_fault_code = passat_clear_fault_code;

    passat_data_ops.transfer_data_stream = passat_data_stream;
    passat_data_ops.is_engine_on = passat_engine_on;
    passat_data_ops.check_fault_code = passat_check_fault_code;
    passat_data_ops.init = passat_init;
    passat_data_ops.exit = passat_exit;
    passat_data_ops.keepalive = passat_keepalive;

    vehicle->ctrlOps = &passat_ctrl_ops;
    vehicle->dataOps = &passat_data_ops;
    vehicle->init = TRUE;
}

void passat_init(uint8_t type)
{
    CanTxMsg init[3] =
    {
        {
            0x200, 0x18db33f1,
            CAN_ID_STD, CAN_RTR_DATA,
            7,
            0x01, 0xc0, 0x00, 0x10, 0x00, 0x03, 0x01
        },
        {
            0x740, 0x18db33f1,
            CAN_ID_STD, CAN_RTR_DATA,
            6,
            0xa0, 0x0f, 0x8a, 0xff, 0x32, 0xff
        },
        {
            0x740, 0x18db33f1,
            CAN_ID_STD, CAN_RTR_DATA,
            6,
            0xa0, 0x0f, 0x8a, 0xff, 0x4a, 0xff
        },
    };
    uint8_t i;

    for(i = 0; i < 3; i++) {
        flexcan_send_frame(&init[i]);
        xdelay_ms(200);
    }
}
void passat_exit(uint8_t type)
{}
void passat_keepalive(uint8_t type)
{}

void passat_ctrl_window(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void passat_ctrl_door(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void passat_ctrl_light(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void passat_ctrl_sunroof(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void passat_ctrl_trunk(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void passat_ctrl_findcar(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

bool passat_engine_on(void)
{
    CanRxMsg *rxMsg;
    int8_t ret = -1;
    bool on = FALSE;

#if 0
    ret = flexcan_ioctl(DIR_BI, &passatEngineCmd,
            0x7e8, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //check if the receive msg type is needed
        //TODO: ???
        if(rxMsg == NULL) {
            on = FALSE;
        } else {
            on = TRUE;
        }
    } else {
        on = FALSE;
    }
    return on;
#endif
    return TRUE;
}

uint8_t* passat_data_stream(uint8_t pid, uint8_t *len)
{
    int8_t ret, i, j;
    uint8_t valid_len;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;
    uint8_t rx_index = 0x00;
    uint8_t l_bytes;
    uint8_t l_packages;
    uint8_t index = 0;
    CanTxMsg endPackage =
    {
        0x740, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        1,
        0xb1
    };

    if(passatSupportItems[pid].support != SUPPORTED) {
        *len = UNSUPPORTED_LEN;
        return NULL;
    }

    xdelay(2);
    valid_len = passatStdDs[pid].valid_len;
    offset = passatStdDs[pid].offset;
    txMsg.StdId = passatStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = passatStdDs[pid].len;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = passatStdDs[pid].data[i];
    }

    if(txMsg.StdId == PASSAT_ENG_ID) {
        ret = flexcan_ioctl(DIR_BI, &txMsg, passatStdDs[pid].rxId, 1);
        if(ret > 0) {
            rxMsg = flexcan_dump();
            if(rxMsg->DLC == 1) {
                rx_index = rxMsg->Data[0];
            } else {
                return NULL;
            }
            //get first package
            ret = flexcan_ioctl(DIR_INPUT, NULL, passatStdDs[pid].rxId, 1);
            if(ret > 0) {
                rxMsg = flexcan_dump();
                l_bytes = rxMsg->Data[2];
                l_packages = 1 + (l_bytes - 5) / 7;
                if((l_bytes - 5) % 7 != 0) {
                    l_packages ++;
                }
                index = 0;
                for(i = 3; i < 8; i++) {
                    passat_rx_buf[index ++] = rxMsg->Data[i];
                }

                ret = flexcan_ioctl(DIR_INPUT, NULL, passatStdDs[pid].rxId, l_packages);
                if(ret > 0) {
                    for(i = 0; i < ret; i++) {
                        rxMsg = flexcan_dump();
                        for(j = 1; j < rxMsg->DLC; j++) {
                            passat_rx_buf[index ++] = rxMsg->Data[j];
                        }
                    }
                }
                rx_index += l_packages;
                endPackage.Data[0] = rx_index;
                flexcan_send_frame(&endPackage);
            }

            *len = valid_len;
            return passat_rx_buf + offset;
        } else {
            return NULL;
        }

    } else {
        ret = flexcan_ioctl(DIR_BI, &txMsg, passatStdDs[pid].rxId, 1);
        if(ret > 0) {
            rxMsg = flexcan_dump();
            for(i = 0; i < 8; i++) {
                passat_rx_buf[i] = rxMsg->Data[i];
            }
            *len = valid_len;
            return passat_rx_buf + offset;
        } else {
            return NULL;
        }
    }

}

uint32_t *passat_check_fault_code(uint8_t id, uint8_t *len)
{
    return NULL;
}

void passat_clear_fault_code(void)
{
    logi("%s", __func__);
}

