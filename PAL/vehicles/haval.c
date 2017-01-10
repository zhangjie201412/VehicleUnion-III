#include "haval.h"
#include "pal.h"
#include "flexcan.h"
#include "utils.h"

VehiclesCtrlOps haval_ctrl_ops;
VehiclesDataOps haval_data_ops;
uint8_t haval_rx_buf[8];

PidSupportItem havalSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, SUPPORTED},
    {ENG_DATA_APP, UNSUPPORTED},
    {ENG_DATA_TP, SUPPORTED},
    {ENG_DATA_ERT, UNSUPPORTED},
    {ENG_DATA_LOAD, SUPPORTED},
    {ENG_DATA_LTFT, UNSUPPORTED},
    {ENG_DATA_STFT, UNSUPPORTED},
    {ENG_DATA_MISFIRE1, UNSUPPORTED},
    {ENG_DATA_MISFIRE2, UNSUPPORTED},
    {ENG_DATA_MISFIRE3, UNSUPPORTED},
    {ENG_DATA_MISFIRE4, UNSUPPORTED},
    {ENG_DATA_MISFIRE5, UNSUPPORTED},
    {ENG_DATA_MISFIRE6, UNSUPPORTED},
    {ENG_DATA_FCLS, UNSUPPORTED},
    {ENG_DATA_KEYSTATUS, UNSUPPORTED},
    {ENG_DATA_HO2S1, UNSUPPORTED},
    {ENG_DATA_HO2S2, UNSUPPORTED},
    {ENG_DATA_MAP, SUPPORTED},
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
	{BCM_DATA_DDA, SUPPORTED},
	{BCM_DATA_PDA, SUPPORTED},
	{BCM_DATA_RRDA, SUPPORTED},
	{BCM_DATA_LRDA, SUPPORTED},
	{BCM_DATA_SUNROOF, UNSUPPORTED},
	{BCM_DATA_PARKLAMP, UNSUPPORTED},
	{BCM_DATA_HEADLAMP, UNSUPPORTED},
	{BCM_DATA_HIGHBEAM, SUPPORTED},
	{BCM_DATA_HAZARD, UNSUPPORTED},
	{BCM_DATA_FRONTFOG, UNSUPPORTED},
	{BCM_DATA_REARFOG, UNSUPPORTED},
	{BCM_DATA_LEFTTURN, UNSUPPORTED},
	{BCM_DATA_RIGHTTURN, UNSUPPORTED},
	{BCM_DATA_ODO, SUPPORTED},
};


StdDataStream havalStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0x7e0, 8,
        {0x03, 0x22, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
    },
    {
        ENG_DATA_VS, 0x7e0, 8,
        {0x03, 0x22, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
    },
    {
        ENG_DATA_ECT, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x2f, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 1,
    },
    //IAT
    {
        ENG_DATA_IAT, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 1,
    },
    //APP
    {
        ENG_DATA_APP, 0x7e0, 8,
    },
    //TP
    {
        ENG_DATA_TP, 0x7e0, 8,
        {0x03, 0x22, 0x01, 0x0b, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
    },
    //ERT
    {
        ENG_DATA_ERT, 0X7e0, 8,
    },
    //LOAD
    {
        ENG_DATA_LOAD, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 1,
    },
    //LTFT
    {
        ENG_DATA_LTFT, 0X7e0, 8,
    },
    //STFT
    {
        ENG_DATA_STFT, 0X7e0, 8,
    },
    //MISFIRE1
    {
        ENG_DATA_MISFIRE1, 0X7e0, 8,
    },
    //MISFIRE2
    {
        ENG_DATA_MISFIRE2, 0X7e0, 8,
    },
    //MISFIRE3
    {
        ENG_DATA_MISFIRE3, 0X7e0, 8,
    },
    //MISFIRE4
    {
        ENG_DATA_MISFIRE4, 0X7e0, 8,
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
        {0x03, 0x22, 0x01, 0x45, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
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
        ENG_DATA_MAF, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 1,
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
        ENG_DATA_REALFUELCO, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
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
        BCM_DATA_BATTVOLT, 0X7e0, 8,
        {0x03, 0x22, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 4, 2,
    },
    //BCM_DATA_DDA
    {
        BCM_DATA_DDA, 0X765, 8,
        {0x03, 0x22, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00},
        0x76d, 4, 1,
    },
    //BCM_DATA_PDA
    {
        BCM_DATA_PDA, 0X765, 8,
        {0x03, 0x22, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00},
        0x76d, 4, 1,
    },
    //BCM_DATA_RRDA
    {
        BCM_DATA_RRDA, 0X765, 8,
        {0x03, 0x22, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00},
        0x76d, 4, 1,
    },
    //BCM_DATA_LRDA
    {
        BCM_DATA_LRDA, 0X765, 8,
        {0x03, 0x22, 0x10, 0x07, 0x00, 0x00, 0x00, 0x00},
        0x76d, 4, 1,
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
        {0x03, 0x22, 0x10, 0x0b, 0x00, 0x00, 0x00, 0x00},
        0x76d, 4, 1,
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
        BCM_DATA_ODO, 0X721, 8,
        {0x03, 0x22, 0xd0, 0x05, 0x00, 0x00, 0x00, 0x00},
        0x729, 4, 2,
    },
};

CanTxMsg havalEngineCmd =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void haval_setup(Vehicles *vehicle)
{
    haval_ctrl_ops.control_window = haval_ctrl_window;
    haval_ctrl_ops.control_door = haval_ctrl_door;
    haval_ctrl_ops.control_sunroof = haval_ctrl_sunroof;
    haval_ctrl_ops.control_light = haval_ctrl_light;
    haval_ctrl_ops.control_trunk = haval_ctrl_trunk;
    haval_ctrl_ops.control_findcar = haval_ctrl_findcar;
    haval_ctrl_ops.clear_fault_code = haval_clear_fault_code;

    haval_data_ops.transfer_data_stream = haval_data_stream;
    haval_data_ops.is_engine_on = haval_engine_on;
    haval_data_ops.check_fault_code = haval_check_fault_code;
    haval_data_ops.init = haval_init;
    haval_data_ops.exit = haval_exit;
    haval_data_ops.keepalive = haval_keepalive;

    vehicle->ctrlOps = &haval_ctrl_ops;
    vehicle->dataOps = &haval_data_ops;
    vehicle->init = TRUE;
}

void haval_init(uint8_t type)
{}
void haval_exit(uint8_t type)
{}
void haval_keepalive(uint8_t type)
{}

void haval_ctrl_window(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void haval_ctrl_door(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void haval_ctrl_light(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void haval_ctrl_sunroof(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void haval_ctrl_trunk(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void haval_ctrl_findcar(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

bool haval_engine_on(void)
{
    CanRxMsg *rxMsg;
    int8_t ret = -1;
    bool on = FALSE;

    ret = flexcan_ioctl(DIR_BI, &havalEngineCmd,
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
}

uint8_t* haval_data_stream(uint8_t pid, uint8_t *len)
{
    int8_t ret, i;
    uint8_t valid_len;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    if(havalSupportItems[pid].support != SUPPORTED) {
        *len = UNSUPPORTED_LEN;
        return NULL;
    }

    xdelay(2);
    valid_len = havalStdDs[pid].valid_len;
    offset = havalStdDs[pid].offset;
    txMsg.StdId = havalStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = havalStdDs[pid].len;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = havalStdDs[pid].data[i];
    }
    ret = flexcan_ioctl(DIR_BI, &txMsg, havalStdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        for(i = 0; i < 8; i++) {
            haval_rx_buf[i] = rxMsg->Data[i];
        }
        *len = valid_len;
        return haval_rx_buf + offset;
    } else {
        return NULL;
    }
}

uint32_t *haval_check_fault_code(uint8_t id, uint8_t *len)
{
    return NULL;
}

void haval_clear_fault_code(void)
{
    logi("%s", __func__);
}

