#include "gm.h"
#include "pal.h"
#include "flexcan.h"
#include "utils.h"

VehiclesCtrlOps gm_ctrl_ops;
VehiclesDataOps gm_data_ops;

CanTxMsg gm_exit_cmd =
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    2,
    0x01, 0x20
};

CanTxMsg gm_lamp_on =
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xae, 0x02, 0x00, 0x00, 0x02, 0x02, 0x00
};

CanTxMsg gm_lamp_off =
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x07, 0xAE, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00
};

CanTxMsg gm_door_on[5] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x04, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x02, 0x02, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_door_off[5] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_window_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x03, 0x03, 0x03, 0x03
    },
};

CanTxMsg gm_window_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x3B, 0x0F, 0x02, 0x02, 0x02, 0x02
    },
};

CanTxMsg gm_trunk_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x10, 0x10, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_trunk_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_sunroof_on[3] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x08, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_sunroof_off[3] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x4C, 0xFF, 0x04, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_findcar_on[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x07, 0xAE, 0x02, 0xF0, 0xF0, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_findcar_off[2] = {
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg gm_start_stream =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x03, 0xaa, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00
};

PidSupportItem gmSupportItems[PID_SIZE] =
{
    {ENG_DATA_RPM, SUPPORTED},
    {ENG_DATA_VS, SUPPORTED},
    {ENG_DATA_ECT, SUPPORTED},
    {ENG_DATA_IAT, UNSUPPORTED},
    {ENG_DATA_APP, SUPPORTED},
    {ENG_DATA_TP, SUPPORTED},
    {ENG_DATA_ERT, UNSUPPORTED},
    {ENG_DATA_LOAD, UNSUPPORTED},
    {ENG_DATA_LTFT, SUPPORTED},
    {ENG_DATA_STFT, SUPPORTED},
    {ENG_DATA_MISFIRE1, SUPPORTED},
    {ENG_DATA_MISFIRE2, SUPPORTED},
    {ENG_DATA_MISFIRE3, SUPPORTED},
    {ENG_DATA_MISFIRE4, SUPPORTED},
    {ENG_DATA_MISFIRE5, SUPPORTED},
    {ENG_DATA_MISFIRE6, SUPPORTED},
    {ENG_DATA_FCLS, UNSUPPORTED},
    {ENG_DATA_KEYSTATUS, SUPPORTED},
    {ENG_DATA_HO2S1, SUPPORTED},
    {ENG_DATA_HO2S2, SUPPORTED},
    {ENG_DATA_MAP, SUPPORTED},
    {ENG_DATA_INJECTPULSE, UNSUPPORTED},
    {ENG_DATA_OILPRESSURE, UNSUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, UNSUPPORTED},
    {ENG_DATA_AF, SUPPORTED},
    {ENG_DATA_IGTIMING, UNSUPPORTED},
    {ENG_DATA_MAF, SUPPORTED},
    {ENG_DATA_OILLIFE, SUPPORTED},
    {ENG_DATA_OILTEMP, SUPPORTED},
    {ENG_DATA_FUEL, SUPPORTED},
    {ENG_DATA_FUELLEVEL, SUPPORTED},
    {ENG_DATA_FUELTANK, SUPPORTED},
	{ENG_DATA_REALFUELCO, UNSUPPORTED},
    {AT_DATA_OILTEMP, SUPPORTED},
    {ABS_DATA_OILLEVEL, SUPPORTED},
    {BCM_DATA_CHARGESTATUS, SUPPORTED},
    {BCM_DATA_BATTCURRENT, UNSUPPORTED},
    {BCM_DATA_BATTSTATUS, UNSUPPORTED},
    {BCM_DATA_BATTVOLT, SUPPORTED},
    {BCM_DATA_DDA, SUPPORTED},
    {BCM_DATA_PDA, SUPPORTED},
    {BCM_DATA_RRDA, SUPPORTED},
    {BCM_DATA_LRDA, SUPPORTED},
    {BCM_DATA_SUNROOF, SUPPORTED},
    {BCM_DATA_PARKLAMP, UNSUPPORTED},
    {BCM_DATA_HEADLAMP, SUPPORTED},
    {BCM_DATA_HIGHBEAM, UNSUPPORTED},
    {BCM_DATA_HAZARD, SUPPORTED},
    {BCM_DATA_FRONTFOG, UNSUPPORTED},
    {BCM_DATA_REARFOG, UNSUPPORTED},
    {BCM_DATA_LEFTTURN, SUPPORTED},
    {BCM_DATA_RIGHTTURN, SUPPORTED},
    {BCM_DATA_ODO, SUPPORTED},
};

GmDataStream gmDataStream[PID_SIZE] =
{
    //ENG_DATA_RPM
    {
        //pid
        ENG_DATA_RPM,
        //txId
        {0x7e0, 0x7e0},
        //rxId
        {0x7e8, 0x5e8},
        //data
        {0x04, 0x2c, 0xfe, 0x00, 0x0c, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_VS
    {
        ENG_DATA_VS,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0d, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_ECT
    {
        ENG_DATA_ECT,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x05, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_IAT
    {
        ENG_DATA_IAT,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_APP
    {
        ENG_DATA_APP,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x13, 0x1f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_TP
    {
        ENG_DATA_TP,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0xb1, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_ERT
    {
        ENG_DATA_ERT,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x1f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_LOAD
    {
        ENG_DATA_LOAD,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x04, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_LTFT
    {
        ENG_DATA_LTFT,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x07, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_STFT
    {
        ENG_DATA_STFT,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x06, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE1
    {
        ENG_DATA_MISFIRE1,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x06, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE2
    {
        ENG_DATA_MISFIRE2,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x05, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE3
    {
        ENG_DATA_MISFIRE3,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x07, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE4
    {
        ENG_DATA_MISFIRE4,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0x08, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE5
    {
        ENG_DATA_MISFIRE5,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0xea, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MISFIRE6
    {
        ENG_DATA_MISFIRE6,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0xeb, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FCLS
    {
        ENG_DATA_FCLS,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x31, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_KEYSTATUS
    {
        ENG_DATA_KEYSTATUS,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x20, 0x7e, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_HO2S1
    {
        ENG_DATA_HO2S1,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x23, 0x20, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_HO2S2
    {
        ENG_DATA_HO2S2,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x23, 0x21, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MAP
    {
        ENG_DATA_MAP,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x20, 0x8a, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_INJECTPULSE
    {
        ENG_DATA_INJECTPULSE,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x12, 0xc3, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_OILPRESSURE
    {
        ENG_DATA_OILPRESSURE,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x27, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_OILLEVELSTATUS
    {
        ENG_DATA_OILLEVELSTATUS,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x27, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_AF
    {
        ENG_DATA_AF,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x44, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_IGTIMING
    {
        ENG_DATA_IGTIMING,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x0e, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_MAF
    {
        ENG_DATA_MAF,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x00, 0x10, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_OILLIFE
    {
        ENG_DATA_OILLIFE,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x02, 0x1a, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        3,
        1,
    },
    //ENG_DATA_OILTEMP
    {
        ENG_DATA_OILTEMP,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x11, 0x54, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FUEL
    {
        ENG_DATA_FUEL,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x19, 0x40, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELLEVEL,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x13, 0x2a, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_FUELTANK
    {
        ENG_DATA_FUELTANK,
        {0x7e0, 0x7e0},
        {0x7e8, 0x5e8},
        {0x04, 0x2c, 0xfe, 0x13, 0x1d, 0x00, 0x00, 0x00},
        TRUE,
        1,
        2,
    },
    //ENG_DATA_REALFUELCO
    {
        ENG_DATA_REALFUELCO,
    },
    //AT_DATA_OILTEMP
    {
        AT_DATA_OILTEMP,
        {0x7e2, 0x7e2},
        {0x7ea, 0x5ea},
        {0x04, 0x2c, 0xfe, 0x00, 0x2f, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //ABS_DATA_OILLEVEL
    {
        ABS_DATA_OILLEVEL,
        {0x243, 0x243},
        {0x643, 0x543},
        {0x04, 0x2c, 0xfe, 0x40, 0x75, 0x00, 0x00, 0x00},
        TRUE,
        1,
        1,
    },
    //BCM_DATA_CHARGESTATUS
    {
        BCM_DATA_CHARGESTATUS,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x90, 0xf8, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_BATTCURRENT
    {
        BCM_DATA_BATTCURRENT,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x90, 0xfb, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_BATTSTATUS
    {
        BCM_DATA_BATTSTATUS,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x02, 0x1a, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        3,
        2,
    },
    //BCM_DATA_BATTVOLT
    {
        BCM_DATA_BATTVOLT,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x90, 0xb1, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_DDA
    {
        BCM_DATA_DDA,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x85, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        1,
    },
    //BCM_DATA_PDA
    {
        BCM_DATA_PDA,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x86, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        1,
    },
    //BCM_DATA_RRDA
    {
        BCM_DATA_RRDA,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x88, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        1,
    },
    //BCM_DATA_LRDA
    {
        BCM_DATA_LRDA,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x87, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        1,
    },
    //BCM_DATA_SUNROOF
    {
        BCM_DATA_SUNROOF,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x90, 0xe0, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        1,
    },
    //BCM_DATA_PARKLAMP
    {
        BCM_DATA_PARKLAMP,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_HEADLAMP
    {
        BCM_DATA_HEADLAMP,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_HIGHBEAM
    {
        BCM_DATA_HIGHBEAM,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_HAZARD
    {
        BCM_DATA_HAZARD,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_FRONTFOG
    {
        BCM_DATA_FRONTFOG,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_REARFOG
    {
        BCM_DATA_REARFOG,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_LEFTTURN
    {
        BCM_DATA_LEFTTURN,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_RIGHTTURN
    {
        BCM_DATA_RIGHTTURN,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x03, 0x22, 0x80, 0x4c, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        2,
    },
    //BCM_DATA_ODO
    {
        BCM_DATA_ODO,
        {0x241, 0x241},
        {0x641, 0x641},
        {0x02, 0x1a, 0xdf, 0x00, 0x00, 0x00, 0x00, 0x00},
        FALSE,
        4,
        4,
    },
};

CanTxMsg gm_fault_code =
{
    0x000, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x03, 0xA9, 0x81, 0x1A, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg gm_clear_fault =
{
    0x000, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint16_t gm_code_list[FAULT_CODE_MAX_SIZE][2] =
{
    //eng_code
    {0x7e0, 0x5e8},
    //at_code
    {0x7e2, 0x5ea},
    //abs_code
    {0x243, 0x543},
    //srs_code
    {0x247, 0x547},
    //bcm_code
    {0x241, 0x541},
    //ipc_code
    {0x24c, 0x54c},
    //eps_code
    {0x242, 0x542},
    //ac_code
    {0x251, 0x551},
    //tpms
    {0x241, 0x541},
};

CanTxMsg gm_eng_init_code[2] =
{
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x01, 0x3e
    },
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        3,
        0x02, 0xaa, 0x00
    }
};

CanTxMsg gm_at_init_code[2] =
{
    {
        0x7e2, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x01, 0x3e
    },
    {
        0x7e2, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        3,
        0x02, 0xaa, 0x00
    }
};

CanTxMsg gm_abs_init_code[2] =
{
    {
        0x243, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x01, 0x3e
    },
    {
        0x243, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        3,
        0x02, 0xaa, 0x00
    }
};

CanTxMsg gm_bcm_init_code[2] =
{
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        2,
        0x01, 0x3e
    },
    {
        0x241, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        3,
        0x02, 0xaa, 0x00
    }
};

CanTxMsg gm_eng_exit_code =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    2,
    0x01, 0x20
};

CanTxMsg gm_at_exit_code =
{
    0x7e2, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    2,
    0x01, 0x20
};

CanTxMsg gm_abs_exit_code =
{
    0x243, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    2,
    0x01, 0x20
};

CanTxMsg gm_bcm_exit_code =
{
    0x241, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    2,
    0x01, 0x20
};

__IO uint8_t gm_rx_buf[8];
__IO uint32_t gm_code_val[FAULT_CODE_MAX_SIZE];

void gm_setup(Vehicles *vehicle)
{
    logi("-> %s\r\n", __func__);
    //init gm ops
    gm_ctrl_ops.control_door = gm_ctrl_door;
    gm_ctrl_ops.control_window = gm_ctrl_window;
    gm_ctrl_ops.control_sunroof = gm_ctrl_sunroof;
    gm_ctrl_ops.control_light = gm_ctrl_light;
    gm_ctrl_ops.control_findcar = gm_ctrl_findcar;
    gm_ctrl_ops.control_trunk = gm_ctrl_trunk;
    gm_ctrl_ops.clear_fault_code = gm_clear_fault_code;

    gm_data_ops.transfer_data_stream = gm_data_stream;
    gm_data_ops.is_engine_on = gm_engine_on;
    gm_data_ops.check_fault_code = gm_check_fault_code;
    gm_data_ops.init = gm_init;
    gm_data_ops.exit = gm_exit;
    gm_data_ops.keepalive = gm_keepalive;

    vehicle->ctrlOps = &gm_ctrl_ops;
    vehicle->dataOps = &gm_data_ops;
    vehicle->init = TRUE;
}

void gm_init(uint8_t type)
{
    uint8_t i;

    switch(type) {
        case TYPE_ENG:
            for(i = 0; i < 2; i ++) {
                flexcan_send_frame(&gm_eng_init_code[i]);
                xdelay_ms(100);
            }
            break;
        case TYPE_AT:
            for(i = 0; i < 2; i ++) {
                flexcan_send_frame(&gm_at_init_code[i]);
                xdelay_ms(100);
            }
            break;
        case TYPE_ABS:
            for(i = 0; i < 2; i ++) {
                flexcan_send_frame(&gm_abs_init_code[i]);
                xdelay_ms(100);
            }
            break;
        case TYPE_BCM:
            for(i = 0; i < 2; i ++) {
                flexcan_send_frame(&gm_bcm_init_code[i]);
                xdelay_ms(100);
            }
            break;
        default:
            break;
    }
}

void gm_exit(uint8_t type)
{

    switch(type) {
        case TYPE_ENG:
            flexcan_send_frame(&gm_eng_exit_code);
            xdelay_ms(100);
            break;
        case TYPE_AT:
            flexcan_send_frame(&gm_at_exit_code);
            xdelay_ms(100);
            break;
        case TYPE_ABS:
            flexcan_send_frame(&gm_abs_exit_code);
            xdelay_ms(100);
            break;
        case TYPE_BCM:
            flexcan_send_frame(&gm_bcm_exit_code);
            xdelay_ms(100);
            break;
        default:
            break;
    }
}

void gm_keepalive(uint8_t type)
{
    switch(type) {
        case TYPE_ENG:
            flexcan_send_frame(&gm_eng_init_code[0]);
            xdelay_ms(100);
            break;
        case TYPE_AT:
            flexcan_send_frame(&gm_at_init_code[0]);
            xdelay_ms(100);
            break;
        case TYPE_ABS:
            flexcan_send_frame(&gm_abs_init_code[0]);
            xdelay_ms(100);
            break;
        case TYPE_BCM:
            flexcan_send_frame(&gm_bcm_init_code[0]);
            xdelay_ms(100);
            break;
        default:
            break;
    }
}

bool gm_engine_on(void)
{
    CanRxMsg *rxMsg;
    int8_t ret = -1;
    uint8_t on = FALSE;
    CanTxMsg gm_engineAlive =
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x04, 0x2c, 0xfe, 0x00, 0x0c, 0x00, 0x00, 0x00,
    };

    ret = flexcan_ioctl(DIR_BI, &gm_engineAlive,
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

uint8_t fake1[1] = {24};
uint8_t fake2[1] = {25};
uint8_t fake3[1] = {25};
uint8_t fake4[1] = {27};

uint8_t* gm_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i, j;
    int8_t ret;
    uint8_t valid_len;
    uint8_t valid_index = 0;
    uint8_t data_type;
    uint8_t offset;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    //add tpms fake data
    if(pid == TPMS_DATA_LFTIREP) {
        *len = 1;
        return fake1;
    }
    if(pid == TPMS_DATA_RFTIREP) {
        *len = 1;
        return fake2;
    }
    if(pid == TPMS_DATA_LRTIREP) {
        *len = 1;
        return fake3;
    }
    if(pid == TPMS_DATA_RRTIREP) {
        *len = 1;
        return fake4;
    }

    //check if this pid supported
    if(gmSupportItems[pid].support != SUPPORTED) {
        *len = UNSUPPORTED_LEN;
        return NULL;
    }

    //clear flexcan rx buf
    xdelay_ms(400);
    //clear rx buf
    memset(gm_rx_buf, 0x00, 8);

    valid_len = gmDataStream[pid].valid_len;
    offset = gmDataStream[pid].offset;

    //send pid data
    txMsg.StdId = gmDataStream[pid].txId[0];
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = 8;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = gmDataStream[pid].data[i];
    }
    //get data type
    data_type = gmDataStream[pid].data[1] + 0x40;
    ret = flexcan_ioctl(DIR_BI, &txMsg, gmDataStream[pid].rxId[0], 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        if(rxMsg == NULL) {
            return NULL;
        }
        if(gmDataStream[pid].ds) {
            //check rx data type is ok?
            if(data_type == rxMsg->Data[1]) {
                //get valid pid callback
                //send start stream
                gm_start_stream.StdId = gmDataStream[pid].txId[1];
                ret = flexcan_ioctl(DIR_BI, &gm_start_stream,
                        gmDataStream[pid].rxId[1], 1);
                if(ret > 0) {
                    rxMsg = flexcan_dump();
                    if(rxMsg == NULL) {
                        return NULL;
                    }
                    //get valid buf
                    for(i = 0; i < 8; i++) {
                        gm_rx_buf[i] = rxMsg->Data[i];
                    }
                }
            } else {
                loge("Error: not valid rx msg\r\n");
                return NULL;
            }
        } else {
            //get valid buf
            for(i = 0; i < 8; i++) {
                gm_rx_buf[i] = rxMsg->Data[i];
            }
        }
        *len = valid_len;
        return gm_rx_buf + offset;
    } else {
        return NULL;
    }
}

void gm_ctrl_window(uint8_t state)
{
    uint8_t i = 0;

    logi("-> %s\r\n", __func__);
    flexcan_send_frame(&gm_exit_cmd);
    xdelay_ms(500);
    if(state) {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_window_on[i]);
            xdelay(5);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_window_off[i]);
            xdelay(5);
        }
    }
}

void gm_ctrl_door(uint8_t state)
{
    uint8_t i = 0;
    logi("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame(&gm_door_on[i]);
        }
    } else {
        for(i = 0; i < 5; i++) {
            flexcan_send_frame(&gm_door_off[i]);
        }
    }
}

void gm_ctrl_light(uint8_t state)
{
    logi("-> %s\r\n", __func__);
    if(state) {
        flexcan_send_frame(&gm_lamp_on);
    } else {
        flexcan_send_frame(&gm_lamp_off);
    }
}

void gm_ctrl_sunroof(uint8_t state)
{
    uint8_t i = 0;
    logi("-> %s\r\n", __func__);
    if(state) {
        for(i = 0; i < 3; i ++) {
            flexcan_send_frame(&gm_sunroof_on[i]);
            xdelay(3);
        }
    } else {
        for(i = 0; i < 3; i++) {
            flexcan_send_frame(&gm_sunroof_off[i]);
            xdelay(3);
        }
    }
}

void gm_ctrl_trunk(uint8_t state)
{
    uint8_t i = 0;
    logi("-> %s\r\n", __func__);
    xdelay(1);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame(&gm_trunk_on[i]);
            xdelay_ms(500);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_trunk_off[i]);
            xdelay_ms(500);
        }
    }
}

void gm_ctrl_findcar(uint8_t state)
{
    uint8_t i = 0;
    logi("-> %s\r\n", __func__);
    xdelay(1);
    if(state) {
        for(i = 0; i < 2; i ++) {
            flexcan_send_frame(&gm_findcar_on[i]);
            xdelay_ms(200);
        }
    } else {
        for(i = 0; i < 2; i++) {
            flexcan_send_frame(&gm_findcar_off[i]);
            xdelay_ms(200);
        }
    }
}

void gm_clear_fault_code(void)
{
    uint8_t i = 0;
    logi("-> %s\r\n", __func__);
    
    for(i = 0; i < FAULT_CODE_MAX_SIZE; i++) {
        gm_clear_fault.StdId = gm_code_list[i][0];
        flexcan_send_frame(&gm_clear_fault);
        xdelay_ms(200);
    }
}

uint32_t *gm_check_fault_code(uint8_t id, uint8_t *len)
{
    int8_t ret;
    uint8_t length;
    CanRxMsg *rxMsg;
    uint16_t txId = gm_code_list[id][0];
    uint16_t rxId = gm_code_list[id][1];

    //clear buf
    memset(gm_code_val, 0x00, FAULT_CODE_MAX_SIZE);
    gm_fault_code.StdId = txId;
    ret = flexcan_ioctl(DIR_BI, &gm_fault_code, rxId, 1);
    if(ret == 1) {
        rxMsg = flexcan_dump();
        if(rxMsg == NULL) {
            *len = 0;
            return NULL;
        }
        //check the return bytes
        if(rxMsg->Data[0] == 0x81) {
            gm_code_val[0] = (rxMsg->Data[1] << 8) | rxMsg->Data[2];
            if(gm_code_val[0] == 0x00) {
                *len = 1;
                return gm_code_val;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    length = 1;
    for(;;) {
        ret = flexcan_ioctl(DIR_INPUT, NULL, rxId, 1);
        if(ret == 1) {
            rxMsg = flexcan_dump();
            if(rxMsg == NULL) {
                *len = length;
                break;
            }
            //check the return bytes
            if(rxMsg->Data[0] == 0x81) {
                length ++;
                gm_code_val[length - 1] = (rxMsg->Data[1] << 8) | rxMsg->Data[2];
                if(gm_code_val[length - 1] == 0x00) {
                    break;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
    *len = length;

    return gm_code_val;
}
