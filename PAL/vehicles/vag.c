#include "vag.h"
#include "pal.h"
#include "flexcan.h"
#include "utils.h"

VehiclesCtrlOps vag_ctrl_ops;
VehiclesDataOps vag_data_ops;
uint8_t vag_rx_buf[8];

PidSupportItem vagSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, SUPPORTED},
    {ENG_DATA_APP, SUPPORTED},
    {ENG_DATA_TP, SUPPORTED},
    {ENG_DATA_ERT, SUPPORTED},
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
    {ENG_DATA_HO2S1, SUPPORTED},
    {ENG_DATA_HO2S2, SUPPORTED},
    {ENG_DATA_MAP, SUPPORTED},
    {ENG_DATA_INJECTPULSE, UNSUPPORTED},
    {ENG_DATA_OILPRESSURE, UNSUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, UNSUPPORTED},
    {ENG_DATA_AF, SUPPORTED},
    {ENG_DATA_IGTIMING, SUPPORTED},
    {ENG_DATA_MAF, SUPPORTED},
    {ENG_DATA_OILLIFE, UNSUPPORTED},
    {ENG_DATA_OILTEMP, UNSUPPORTED},
    {ENG_DATA_FUEL, SUPPORTED},
	{ENG_DATA_FUELLEVEL, SUPPORTED},
	{ENG_DATA_FUELTANK, UNSUPPORTED},
	{ENG_DATA_REALFUELCO, UNSUPPORTED},
	{AT_DATA_OILTEMP, SUPPORTED},
	{ABS_DATA_OILLEVEL, UNSUPPORTED},
	{BCM_DATA_CHARGESTATUS, SUPPORTED},
	{BCM_DATA_BATTCURRENT, SUPPORTED},
	{BCM_DATA_BATTSTATUS, UNSUPPORTED},
	{BCM_DATA_BATTVOLT, SUPPORTED},
	{BCM_DATA_DDA, SUPPORTED},
	{BCM_DATA_PDA, SUPPORTED},
	{BCM_DATA_RRDA, SUPPORTED},
	{BCM_DATA_LRDA, SUPPORTED},
	{BCM_DATA_SUNROOF, SUPPORTED},
	{BCM_DATA_PARKLAMP, SUPPORTED},
	{BCM_DATA_HEADLAMP, UNSUPPORTED},
	{BCM_DATA_HIGHBEAM, UNSUPPORTED},
	{BCM_DATA_HAZARD, SUPPORTED},
	{BCM_DATA_FRONTFOG, SUPPORTED},
	{BCM_DATA_REARFOG, SUPPORTED},
	{BCM_DATA_LEFTTURN, UNSUPPORTED},
	{BCM_DATA_RIGHTTURN, UNSUPPORTED},
	{BCM_DATA_ODO, SUPPORTED},
};


CanTxMsg vag_door_off[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x03, 0x03, 0xff, 0x01
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_door_on =
{
    0x74a, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x04, 0x2f, 0x04, 0x03, 0x00, 0x55, 0x55, 0x55
};

CanTxMsg vag_lamp_on[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x03, 0x03, 0x0a, 0x00
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_lamp_off =
{
    0x74a, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x04, 0x2f, 0x04, 0x00, 0x00, 0x55, 0x55, 0x55
};

CanTxMsg vag_window_on_fore[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x02, 0x03, 0x0a, 0x00
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_window_on_back[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x06, 0x03, 0x0a, 0x00
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_window_off_fore[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x01, 0x03, 0x0a, 0x00
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_window_off_back[2] =
{
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x10, 0x08, 0x2f, 0x04, 0x05, 0x03, 0x0a, 0x00
    },
    {
        0x74a, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x21, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x55
    },
};

CanTxMsg vag_trunk_on[2] =
{
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x27, 0x00, 0x06, 0x31, 0xb8, 0x01, 0x07, 0x01
    },
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x18, 0x49
    },
};

CanTxMsg vag_alarm_on[2] =
{
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x27, 0x00, 0x06, 0x31, 0xb8, 0x01, 0x07, 0x03
    },
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x18, 0x8a
    },
};

CanTxMsg vag_flashlight_on[2] =
{
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x20, 0x00, 0x06, 0x31, 0xb8, 0x01, 0x07, 0x03
    },
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x11, 0x8e
    },
};

CanTxMsg vag_sunroof_on[2] =
{
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x27, 0x00, 0x06, 0x31, 0xb8, 0x01, 0x07, 0x03
    },
    {
        0x338, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x18, 0xb4
    },
};


StdDataStream vagStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0x7e0, 8,
        {0x03, 0x22, 0xf4, 0x0c, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    {
        ENG_DATA_VS, 0x7e0, 8,
        {0x03, 0x22, 0xf4, 0x0d, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    {
        ENG_DATA_ECT, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x05, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //IAT
    {
        ENG_DATA_IAT, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x0f, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //APP
    {
        ENG_DATA_APP, 0x7e0, 8,
        {0x03, 0x22, 0x10, 0x26, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //TP
    {
        ENG_DATA_TP, 0x7e0, 8,
        {0x03, 0x22, 0xf4, 0x43, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //ERT
    {
        ENG_DATA_ERT, 0X7e0, 8,
        {0x03, 0x22, 0x38, 0x87, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 4,
    },
    //LOAD
    {
        ENG_DATA_LOAD, 0X7e0, 8,
        {0x03, 0x22, 0x10, 0x1c, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //LTFT
    {
        ENG_DATA_LTFT, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x07, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //STFT
    {
        ENG_DATA_STFT, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x06, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //MISFIRE1
    {
        ENG_DATA_MISFIRE1, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x4f, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //MISFIRE2
    {
        ENG_DATA_MISFIRE2, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x52, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //MISFIRE3
    {
        ENG_DATA_MISFIRE3, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x50, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //MISFIRE4
    {
        ENG_DATA_MISFIRE4, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x51, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
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
        {0x03, 0x22, 0xf4, 0x14, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //HO2S2
    {
        ENG_DATA_HO2S2, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x15, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //MAP
    {
        ENG_DATA_MAP, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x0b, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
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
        {0x03, 0x22, 0x11, 0x19, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //IGTIMING
    {
        ENG_DATA_IGTIMING, 0X7e0, 8,
        {0x03, 0x22, 0xf4, 0x1e, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 1,
    },
    //MAF
    {
        ENG_DATA_MAF, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0xac, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
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
        {0x03, 0x22, 0x14, 0xb4, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELLEVEL, 0X7e0, 8,
        {0x03, 0x22, 0x14, 0xb4, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELTANK, 0X7c0, 8,
    },
    //ENG_DATA_REALFUELCO
    {
        ENG_DATA_REALFUELCO, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x3f, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 2,
    },
    //AT_DATA_OILTEMP
    {
        AT_DATA_OILTEMP, 0X7e1, 8,
        {0x03, 0x22, 0x21, 0x04, 0x55, 0x55, 0x55, 0x55},
        0x7e9, 3, 1,
    },
    //ABS_DATA_OILLEVEL
    {
        ABS_DATA_OILLEVEL, 0X7b0, 8,
    },
    //BCM_DATA_CHARGESTATUS
    {
        BCM_DATA_CHARGESTATUS, 0X710, 8,
        {0x03, 0x22, 0x2a, 0x0f, 0x55, 0x55, 0x55, 0x55},
        0x77a, 3, 1,
    },
    //BCM_DATA_BATTCURRENT
    {
        BCM_DATA_BATTCURRENT, 0X710, 8,
        {0x03, 0x22, 0x2a, 0x09, 0x55, 0x55, 0x55, 0x55},
        0x77a, 3, 4,
    },
    //BCM_DATA_BATTSTATUS
    {
        BCM_DATA_BATTSTATUS, 0X7b0, 8,
    },
    //BCM_DATA_BATTVOLT
    {
        BCM_DATA_BATTVOLT, 0X710, 8,
        {0x03, 0x22, 0x2a, 0x07, 0x55, 0x55, 0x55, 0x55},
        0x77a, 3, 2,
    },
    //BCM_DATA_DDA
    {
        BCM_DATA_DDA, 0X74a, 8,
        {0x03, 0x22, 0x19, 0x44, 0x55, 0x55, 0x55, 0x55},
        0x7b4, 3, 1,
    },
    //BCM_DATA_PDA
    {
        BCM_DATA_PDA, 0X74b, 8,
        {0x03, 0x22, 0x19, 0x48, 0x55, 0x55, 0x55, 0x55},
        0x7b5, 3, 1,
    },
    //BCM_DATA_RRDA
    {
        BCM_DATA_RRDA, 0X70e, 8,
        {0x03, 0x22, 0x19, 0x50, 0x55, 0x55, 0x55, 0x55},
        0x778, 3, 1,
    },
    //BCM_DATA_LRDA
    {
        BCM_DATA_LRDA, 0X70e, 8,
        {0x03, 0x22, 0x19, 0x4c, 0x55, 0x55, 0x55, 0x55},
        0x778, 3, 1,
    },
    //BCM_DATA_SUNROOF
    {
        BCM_DATA_SUNROOF, 0X70e, 8,
        {0x03, 0x22, 0x19, 0x81, 0x55, 0x55, 0x55, 0x55},
        0x778, 4, 1,
    },
    //BCM_DATA_PARKLAMP
    {
        BCM_DATA_PARKLAMP, 0X74a, 8,
        {0x03, 0x22, 0x19, 0x43, 0x55, 0x55, 0x55, 0x55},
        0x7b4, 3, 1,
    },
    //BCM_DATA_HEADLAMP
    {
        BCM_DATA_HEADLAMP, 0X750, 8,
    },
    //BCM_DATA_HIGHBEAM
    {
        BCM_DATA_HIGHBEAM, 0X750, 8,
    },
    //BCM_DATA_HAZARD
    {
        BCM_DATA_HAZARD, 0X70e, 8,
        {0x03, 0x22, 0x19, 0x13, 0x55, 0x55, 0x55, 0x55},
        0x778, 3, 1,
    },
    //BCM_DATA_FRONTFOG
    {
        BCM_DATA_FRONTFOG, 0X74a, 8,
        {0x03, 0x22, 0x19, 0x43, 0x55, 0x55, 0x55, 0x55},
        0x7b4, 5, 1,
    },
    //BCM_DATA_REARFOG
    {
        BCM_DATA_REARFOG, 0X74a, 8,
        {0x03, 0x22, 0x19, 0x43, 0x55, 0x55, 0x55, 0x55},
        0x7b4, 5, 1,
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
        BCM_DATA_ODO, 0X7e0, 8,
        {0x03, 0x22, 0x11, 0x60, 0x55, 0x55, 0x55, 0x55},
        0x7e8, 3, 4,
    },
};

CanTxMsg vagEngineCmd =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void vag_setup(Vehicles *vehicle)
{
    vag_ctrl_ops.control_window = vag_ctrl_window;
    vag_ctrl_ops.control_door = vag_ctrl_door;
    vag_ctrl_ops.control_sunroof = vag_ctrl_sunroof;
    vag_ctrl_ops.control_light = vag_ctrl_light;
    vag_ctrl_ops.control_trunk = vag_ctrl_trunk;
    vag_ctrl_ops.control_findcar = vag_ctrl_findcar;
    vag_ctrl_ops.clear_fault_code = vag_clear_fault_code;

    vag_data_ops.transfer_data_stream = vag_data_stream;
    vag_data_ops.is_engine_on = vag_engine_on;
    vag_data_ops.check_fault_code = vag_check_fault_code;

    vehicle->ctrlOps = &vag_ctrl_ops;
    vehicle->dataOps = &vag_data_ops;
    vehicle->init = TRUE;
}

void vag_ctrl_window(uint8_t state)
{
    uint8_t i;

    logi("%s: state = %d", __func__, state);
    if(state == 1){
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_window_on_fore[i]);
            xdelay_ms(100);
        }
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_window_on_back[i]);
            xdelay_ms(100);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_window_off_fore[i]);
            xdelay_ms(100);
        }
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_window_off_back[i]);
            xdelay_ms(100);
        }
    }
}

void vag_ctrl_door(uint8_t state)
{
    uint8_t i;

    logi("%s: state = %d", __func__, state);
    if(state == 1) {
        flexcan_send_frame(&vag_door_on);
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_door_off[i]);
            xdelay_ms(100);
        }
    }
}

void vag_ctrl_light(uint8_t state)
{
    logi("%s: state = %d", __func__, state);
}

void vag_ctrl_sunroof(uint8_t state)
{
    uint8_t i;

    logi("%s: state = %d", __func__, state);
    if(state == 1) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_sunroof_on[i]);
            xdelay_ms(100);
        } 
    }
}

void vag_ctrl_trunk(uint8_t state)
{
    uint8_t i;

    logi("%s: state = %d", __func__, state);
    if(state == 1) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_trunk_on[i]);
            xdelay_ms(100);
        } 
    }
}

void vag_ctrl_findcar(uint8_t state)
{
    uint8_t i;

    logi("%s: state = %d", __func__, state);
    if(state == 1) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_alarm_on[i]);
            xdelay_ms(100);
        } 
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&vag_flashlight_on[i]);
            xdelay_ms(100);
        } 
    }
}

bool vag_engine_on(void)
{
    CanRxMsg *rxMsg;
    int8_t ret = -1;
    bool on = FALSE;

    ret = flexcan_ioctl(DIR_BI, &vagEngineCmd,
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

uint8_t* vag_data_stream(uint8_t pid, uint8_t *len)
{
    int8_t ret, i;
    uint8_t valid_len;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    if(vagSupportItems[pid].support != SUPPORTED) {
        *len = UNSUPPORTED_LEN;
        return NULL;
    }

    xdelay(2);
    valid_len = vagStdDs[pid].valid_len;
    offset = vagStdDs[pid].offset;
    txMsg.StdId = vagStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = vagStdDs[pid].len;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = vagStdDs[pid].data[i];
    }
    ret = flexcan_ioctl(DIR_BI, &txMsg, vagStdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        for(i = 0; i < 8; i++) {
            vag_rx_buf[i] = rxMsg->Data[i];
        }
        *len = valid_len;
        return vag_rx_buf + offset;
    } else {
        return NULL;
    }
}

uint32_t *vag_check_fault_code(uint8_t id, uint8_t *len)
{
    return NULL;
}

void vag_clear_fault_code(void)
{
    logi("%s", __func__);
}
