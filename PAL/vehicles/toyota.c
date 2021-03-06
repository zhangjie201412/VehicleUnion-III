#include "toyota.h"
#include "pal.h"
#include "stdio.h"
#include "flexcan.h"
#include "vehicles.h"
#include "utils.h"

__IO uint8_t toyota_rx_buf[128];

VehiclesCtrlOps toyota_ctrl_ops;
VehiclesDataOps toyota_data_ops;

CanTxMsg toyota_lamp_on =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x06, 0x30, 0x15, 0x00, 0x40, 0x00, 0x00
};

CanTxMsg toyota_lamp_off =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x06, 0x30, 0x15, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_door_on =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0x00, 0x40, 0x00, 0x00
};

CanTxMsg toyota_door_off =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0x00, 0x80, 0x00, 0x00
};

CanTxMsg toyota_trunk_on =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x80, 0x00
};

CanTxMsg toyota_trunk_off =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x05, 0x30, 0x11, 0xff, 0x00, 0x00, 0x00
};

CanTxMsg toyota_sunroof_on =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0xad, 0x04, 0x30, 0x01, 0x01, 0x40, 0x00, 0x00
};

CanTxMsg toyota_sunroof_off =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0xad, 0x04, 0x30, 0x01, 0x01, 0x80, 0x00, 0x00
};

CanTxMsg toyota_window_on[4] =
{
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x90, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x91, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x92, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x93, 0x04, 0x30, 0x01, 0x00, 0x40, 0x00, 0x00
    },
};

CanTxMsg toyota_window_off[4] =
{
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x90, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x91, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x92, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x93, 0x04, 0x30, 0x01, 0x00, 0x80, 0x00, 0x00
    },
};

CanTxMsg toyota_findcar[2] =
{
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x04, 0x30, 0x06, 0xff, 0x10, 0x00, 0x00
    },
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x04, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg toyota_keepalive_normal =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_keepalive_door =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x90, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_continue_package =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x30, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_check_supported =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0xa8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_test =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

PidSupportItem toyotaSupportItems[PID_SIZE] =
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
    {ENG_DATA_FCLS, SUPPORTED},
    {ENG_DATA_KEYSTATUS, UNSUPPORTED},
    {ENG_DATA_HO2S1, SUPPORTED},
    {ENG_DATA_HO2S2, SUPPORTED},
    {ENG_DATA_MAP, SUPPORTED},
    {ENG_DATA_INJECTPULSE, UNSUPPORTED},
    {ENG_DATA_OILPRESSURE, SUPPORTED},
    {ENG_DATA_OILLEVELSTATUS, SUPPORTED},
    {ENG_DATA_AF, SUPPORTED},
    {ENG_DATA_IGTIMING, SUPPORTED},
    {ENG_DATA_MAF, SUPPORTED},
    {ENG_DATA_OILLIFE, UNSUPPORTED},
    {ENG_DATA_OILTEMP, SUPPORTED},
    {ENG_DATA_FUEL, SUPPORTED},
    {ENG_DATA_FUELLEVEL, SUPPORTED},
    {ENG_DATA_FUELTANK, UNSUPPORTED},
    {ENG_DATA_REALFUELCO, UNSUPPORTED},
    {AT_DATA_OILTEMP, SUPPORTED},
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
    {BCM_DATA_HAZARD, SUPPORTED},
    {BCM_DATA_FRONTFOG, SUPPORTED},
    {BCM_DATA_REARFOG, SUPPORTED},
    {BCM_DATA_LEFTTURN, SUPPORTED},
    {BCM_DATA_RIGHTTURN, SUPPORTED},
    {BCM_DATA_ODO, SUPPORTED},
};

StdDataStream toyotaStdDs[PID_SIZE] =
{
    {
        ENG_DATA_RPM, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x0a, 2,
    },
    {
        ENG_DATA_VS, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x0c, 1,
    },
    {
        ENG_DATA_ECT, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x09, 1,
    },
    //IAT
    {
        ENG_DATA_IAT, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x06, 1,
    },
    //APP
    {
        ENG_DATA_APP, 0X7e0, 8,
        {0x02, 0x21, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x05, 2,
    },
    //TP
    {
        ENG_DATA_TP, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x10, 1,
    },
    //ERT
    {
        ENG_DATA_ERT, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x0d, 1,
    },
    //LOAD
    {
        ENG_DATA_LOAD, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x01, 2,
    },
    //LTFT
    {
        ENG_DATA_LTFT, 0X7e0, 8,
        {0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //STFT
    {
        ENG_DATA_STFT, 0X7e0, 8,
        {0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 2, 1,
    },
    //MISFIRE1
    {
        ENG_DATA_MISFIRE1, 0X7e0, 8,
        {0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 5, 1,
    },
    //MISFIRE2
    {
        ENG_DATA_MISFIRE2, 0X7e0, 8,
        {0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 6, 1,
    },
    //MISFIRE3
    {
        ENG_DATA_MISFIRE3, 0X7e0, 8,
        {0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 7, 1,
    },
    //MISFIRE4
    {
        ENG_DATA_MISFIRE4, 0X7e0, 8,
        {0x02, 0x21, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 8, 1,
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
        ENG_DATA_FCLS, 0X7e0, 8,
        {0x02, 0x21, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0, 1,
    },
    //KEYSTATUS
    {
        ENG_DATA_KEYSTATUS, 0X7df, 8,
    },
    //HO2S1
    {
        ENG_DATA_HO2S1, 0X7e0, 8,
        {0x02, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x14, 1,
    },
    //HO2S2
    {
        ENG_DATA_HO2S2, 0X7e0, 8,
        {0x02, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x18, 1,
    },
    //MAP
    {
        ENG_DATA_MAP, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 5, 1,
    },
    //INJECTPULSE
    {
        ENG_DATA_INJECTPULSE, 0X7df, 8,
    },
    //OILPRESSURE
    {
        ENG_DATA_OILPRESSURE, 0X7c0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 4, 1,
    },
    //OILLEVELSTATUS
    {
        ENG_DATA_OILLEVELSTATUS, 0X7c0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 1, 1,
    },
    //AF
    {
        ENG_DATA_AF, 0X7e0, 8,
        {0x02, 0x21, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 2, 2,
    },
    //IGTIMING
    {
        ENG_DATA_IGTIMING, 0X7e0, 8,
        {0x02, 0x21, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 8, 2,
    },
    //MAF
    {
        ENG_DATA_MAF, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 3, 1,
    },
    //OILLIFE
    {
        ENG_DATA_OILLIFE, 0X7df, 8,
    },
    //OILTEMP
    {
        ENG_DATA_OILTEMP, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x31, 1,
    },
    //FUEL
    {
        ENG_DATA_FUEL, 0X7c0, 8,
        {0x02, 0x21, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 2, 1,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELLEVEL, 0X7c0, 8,
        {0x02, 0x21, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 1, 1,
    },
    //ENG_DATA_FUELLEVEL
    {
        ENG_DATA_FUELTANK, 0X7c0, 8,
    },
    //ENG_DATA_REALFUELCO
    {
        ENG_DATA_REALFUELCO, 0X7c0, 8,
    },
    //AT_DATA_OILTEMP
    {
        AT_DATA_OILTEMP, 0X7c0, 8,
        {0x02, 0x21, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 2, 2,
    },
    //ABS_DATA_OILLEVEL
    {
        ABS_DATA_OILLEVEL, 0X7b0, 8,
    },
    //BCM_DATA_CHARGESTATUS
    {
        BCM_DATA_CHARGESTATUS, 0X7b0, 8,
    },
    //BCM_DATA_BATTCURRENT
    {
        BCM_DATA_BATTCURRENT, 0X7b0, 8,
    },
    //BCM_DATA_BATTSTATUS
    {
        BCM_DATA_BATTSTATUS, 0X7b0, 8,
    },
    //BCM_DATA_BATTVOLT
    {
        BCM_DATA_BATTVOLT, 0X7e0, 8,
        {0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x1a, 2,
    },
    //BCM_DATA_DDA
    {
        BCM_DATA_DDA, 0X750, 8,
        {0x40, 0x02, 0x21, 0xA2, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x00, 1,
    },
    //BCM_DATA_PDA
    {
        BCM_DATA_PDA, 0X750, 8,
        {0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x00, 1,
    },
    //BCM_DATA_RRDA
    {
        BCM_DATA_RRDA, 0X750, 8,
        {0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x02, 1,
    },
    //BCM_DATA_LRDA
    {
        BCM_DATA_LRDA, 0X750, 8,
        {0x40, 0x02, 0x21, 0xA3, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x03, 1,
    },
    //BCM_DATA_SUNROOF
    {
        BCM_DATA_SUNROOF, 0X750, 8,
    },
    //BCM_DATA_PARKLAMP
    {
        BCM_DATA_PARKLAMP, 0X750, 8,
    },
    //BCM_DATA_HEADLAMP
    {
        BCM_DATA_HEADLAMP, 0X750, 8,
    },
    //BCM_DATA_HIGHBEAM
    {
        BCM_DATA_HIGHBEAM, 0X750, 8,
        {0x70, 0x02, 0x21, 0x01, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x01, 1,
    },
    //BCM_DATA_HAZARD
    {
        BCM_DATA_HAZARD, 0X7c0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 0x05, 1,
    },
    //BCM_DATA_FRONTFOG
    {
        BCM_DATA_FRONTFOG, 0X750, 8,
        {0x40, 0x02, 0x21, 0xA7, 0x00, 0x00, 0x00, 0x00},
        0x758, 0x02, 1,
    },
    //BCM_DATA_REARFOG
    {
        BCM_DATA_REARFOG, 0X7c0, 8,
        {0x02, 0x21, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 0x02, 1,
    },
    //BCM_DATA_LEFTTURN
    {
        BCM_DATA_LEFTTURN, 0X7c0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 0x04, 1,
    },
    //BCM_DATA_RIGHTTURN
    {
        BCM_DATA_RIGHTTURN, 0X7c0, 8,
        {0x02, 0x21, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7c8, 0x04, 1,
    },
    //BCM_DATA_ODO
    {
        BCM_DATA_ODO, 0X7e0, 8,
        {0x02, 0x21, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00},
        0x7e8, 0x00, 3,
    },
};

CanTxMsg toyota_fault_code[FAULT_CODE_MAX_SIZE] =
{
    //eng_code
    {
        0x7E0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0xb0, 0x0, 0x00, 0x00, 0x00, 0x00
    },
    //at_code
    {
        0x7E0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //abs_code
    {
        0x7B0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //srs_code
    {
        0x780, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //bcm_code
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x02, 0x13, 0x81, 0x00, 0x00, 0x00, 0x00
    },
    //ipc_code
    {
        0x7C0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //eps_code
    {
        0x7a1, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //ac_code
    {
        0x7c4, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x02, 0x13, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg toyota_clear_fault[FAULT_CODE_MAX_SIZE] =
{
    //ENG_CODE
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //AT_CODE
    {
        0x7e0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //ABS_CODE
    {
        0x7b0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //SRS_CODE
    {
        0x780, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //BCM_CODE
    {
        0x750, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //IPC_CODE
    {
        0x7c0, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //EPS_CODE
    {
        0x7a1, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //AC_CODE
    {
        0x7c4, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    //TPMS_CODE
    {
        0x000, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },
};

CanTxMsg toyotaEngineCmd =
{
    0x7df, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_eng_keepalive =
{
    0x7e0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x3e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_at_keepalive =
{
    0x7c0, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x02, 0x3e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

CanTxMsg toyota_bcm_keepalive =
{
    0x750, 0x18db33f1,
    CAN_ID_STD, CAN_RTR_DATA,
    8,
    0x40, 0x01, 0x3e, 0x01, 0x00, 0x00, 0x00, 0x00
};

__IO uint32_t toyota_code_val[FAULT_CODE_MAX_SIZE];
__IO uint8_t toyota_fault_data[100];

ToyotaSupportList mToyotaSupportList;
uint8_t toyota_support_buffer[1280];

void toyota_setup(Vehicles *vehicle)
{
    uint16_t i;

    //init toyota ops
    toyota_ctrl_ops.control_door = toyota_ctrl_door;
    toyota_ctrl_ops.control_window = toyota_ctrl_window;
    toyota_ctrl_ops.control_sunroof = toyota_ctrl_sunroof;
    toyota_ctrl_ops.control_light = toyota_ctrl_light;
    toyota_ctrl_ops.control_findcar = toyota_ctrl_findcar;
    toyota_ctrl_ops.control_trunk = toyota_ctrl_trunk;
    toyota_ctrl_ops.clear_fault_code = toyota_clear_fault_code;

    toyota_data_ops.transfer_data_stream = toyota_data_stream;
    toyota_data_ops.is_engine_on = toyota_engine_on;
    toyota_data_ops.check_fault_code = toyota_check_fault_code;
    toyota_data_ops.init = toyota_init;
    toyota_data_ops.exit = toyota_exit;
    toyota_data_ops.keepalive = toyota_keepalive;

    vehicle->ctrlOps = &toyota_ctrl_ops;
    vehicle->dataOps = &toyota_data_ops;

    mToyotaSupportList.support_done = FALSE;
    for(i = 0; i < SUPPORT_MAX_ITEMS; i++) {
        mToyotaSupportList.items[i].sub_id = 0;
    }
    for(i = 0; i < 1280; i++) {
        toyota_support_buffer[i] = 0x00;
    }
    vehicle->init = TRUE;
}

void toyota_init(uint8_t type)
{}

void toyota_exit(uint8_t type)
{}

void toyota_keepalive(uint8_t type)
{
    switch(type) {
        case TYPE_ENG:
            flexcan_send_frame(&toyota_eng_keepalive);
            xdelay_ms(100);
            break;
        case TYPE_AT:
            flexcan_send_frame(&toyota_at_keepalive);
            xdelay_ms(100);
            break;
        case TYPE_ABS:
            xdelay_ms(100);
            break;
        case TYPE_BCM:
            flexcan_send_frame(&toyota_bcm_keepalive);
            xdelay_ms(100);
            break;
        default:
            break;
    }
}

bool toyota_engine_on(void)
{
    CanRxMsg *rxMsg;
    int8_t ret = -1;
    bool on = FALSE;

    ret = flexcan_ioctl(DIR_BI, &toyotaEngineCmd,
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

void toyota_transfer(CanTxMsg *txMsg, uint8_t *len)
{
    uint8_t i, j;
    int8_t ret;
    uint8_t valid_len;
    uint8_t valid_index = 0;
    uint8_t data_type;
    uint8_t offset;
    uint8_t l_bytes = 0;
    uint8_t l_packages = 0;
    uint8_t sub_cmd = 0;

    uint16_t rxId;

    CanRxMsg *rxMsg;

    //clear flexcan rx buf
    flexcan_reset();

    rxId = txMsg->StdId + 0x08;
    logi("rx = %04x\r\n", rxId);
    ret = flexcan_ioctl(DIR_BI, txMsg, rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //check if the receive msg type is needed
        if(txMsg->Data[0] > 10) {
            data_type = 0x40 + txMsg->Data[2];
            sub_cmd = txMsg->Data[3];
        } else {
            data_type = 0x40 + txMsg->Data[1];
            sub_cmd = txMsg->Data[2];
        }
        for(i = 0; i < 8; i++) {
            toyota_rx_buf[i] = rxMsg->Data[i];
        }

        //find the valid index
        for(i = 0; i < 8; i++) {
            if(toyota_rx_buf[i] == data_type) {
                valid_index = i;
                break;
            }
        }

        if(sub_cmd != toyota_rx_buf[valid_index + 1]) {
            loge("ERROR: sub_cmd = %02x, valid_index = %d\r\n",
                    sub_cmd, valid_index);
        }

        //check if this recv package is a long package
        if(toyota_rx_buf[0] == 0x10) {
            l_bytes = toyota_rx_buf[1];
            l_packages = 0;
            l_packages = (l_bytes - 6) / 7;
            if((l_bytes - 6) % 7 > 0) {
                l_packages += 1;
            }
            for(i = 2; i < 8; i++) {
                toyota_rx_buf[i - 2] = rxMsg->Data[i];
            }
            //send continue package
            ret = flexcan_ioctl(DIR_BI, &toyota_continue_package,
                    rxId, l_packages);
            if(ret == l_packages) {
                for(i = 0;i < ret; i++) {
                    rxMsg = flexcan_dump();
                    for(j = 0; j < 7; j++) {
                        toyota_rx_buf[6 + i * 7 + j] = rxMsg->Data[j + 1];
                    }
                }
            } else {
                loge("error: ret = %d\r\n", ret);
            }
            *len = l_bytes;
        }
        //short package
        else {
            if(txMsg->Data[0] > 10) {
                data_type = 0x40 + txMsg->Data[2];
                sub_cmd = txMsg->Data[3];
            } else {
                data_type = 0x40 + txMsg->Data[1];
                sub_cmd = txMsg->Data[2];
            }
            //find the valid index
            for(i = 0; i < 8; i++) {
                if(toyota_rx_buf[i] == data_type) {
                    valid_index = i;
                    break;
                }
            }
            if(sub_cmd != toyota_rx_buf[valid_index + 1]) {
                loge("sub_cmd = %02x, valid_index = %d\r\n",
                        sub_cmd, valid_index);
            }
            //shift the valid data to head
            for(i = 0; i < 8; i++) {
                toyota_rx_buf[i] = toyota_rx_buf[valid_index + i];
            }
            *len = 8;
        }

        for(i = 0; i < *len; i++) {
            printf("%02x ", toyota_rx_buf[i]);
        }
        printf("\r\n");
    } else {
        loge("Error\r\n");
    }

}

void toyota_get_supported(void)
{
    uint8_t ret;
    uint8_t i;
    uint8_t first_byte;
    CanRxMsg *rxMsg;
    uint16_t index = 0;

    ret = flexcan_ioctl(DIR_BI, &toyota_check_supported,
            toyota_check_supported.StdId + 8, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //save bytes
        for(i = 4; i < 8; i++) {
            toyota_support_buffer[index ++] = rxMsg->Data[i];
        }
    } else {
        loge("failed to get support list\r\n");
        mToyotaSupportList.support_done = FALSE;
        return;
    }

    while(1) {
        ret = flexcan_ioctl(DIR_BI, &toyota_continue_package,
                toyota_continue_package.StdId + 8, 1);
        if(ret > 0) {
            rxMsg = flexcan_dump();
            first_byte = rxMsg->Data[0];
            if(first_byte == 0x10) {
                continue;
            } else if(first_byte >= 0x21) {
                //save bytes
                for(i = 1; i < 8; i++) {
                    toyota_support_buffer[index ++] = rxMsg->Data[i];
                }

                while(1) {
                    ret = flexcan_ioctl(DIR_INPUT, NULL,
                            toyota_check_supported.StdId + 8, 1);
                    if(ret > 0) {
                        rxMsg = flexcan_dump();
                        first_byte = rxMsg->Data[0];
                        if(first_byte == 0x10) {
                            for(i = 4; i < 8; i++) {
                                toyota_support_buffer[index ++] = rxMsg->Data[i];
                            }
                            //need to send continue package again
                            break;
                        } else {
                            for(i = 1; i < 8; i++) {
                                toyota_support_buffer[index ++] = rxMsg->Data[i];
                            }
                            //continue to recive 21 22 23 ...
                            continue;
                        }
                    } else {
                        toyota_map_support_list();
                        logi("%s: done\r\n", __func__);
                        return;
                    }
                }
            }
        } else {
            toyota_map_support_list();
            logi("%s: done\r\n", __func__);
            return;
        }
    }
}

void toyota_map_support_list(void)
{
    uint16_t i, j;
    uint8_t len;
    uint16_t index = 0;

//    logi("%s: start\r\n", __func__);
//    for(i = 0; i < 300; i++) {
//        printf("%02x ", toyota_support_buffer[i]);
//    }
    printf("\r\n");
    mToyotaSupportList.support_done = TRUE;
    for(i = 0; i < SUPPORT_MAX_ITEMS; i++) {
        mToyotaSupportList.items[i].sub_id = toyota_support_buffer[index++];
        len = toyota_support_buffer[index++];
        //printf("id = %02x\r\n", mToyotaSupportList.items[i].sub_id);
        //printf("len = %d\r\n", len);
        mToyotaSupportList.items[i].len = len;
        if(len > 0) {
            for(j = 0; j < len; j++) {
                mToyotaSupportList.items[i].support_raw_bytes[j] = toyota_support_buffer[index ++];
            }
            while(toyota_support_buffer[index] == 0x00) {
                index ++;
            }
        } else {
            break;
        }
    }
//    logi("%s: done\r\n", __func__);
#if 0
    for(i = 0; i < SUPPORT_MAX_ITEMS; i++) {
        len = mToyotaSupportList.items[i].len;
        if(len == 0)
            break;
        logi("sub id = %02x\tlen = %d\r\n", mToyotaSupportList.items[i].sub_id,
                mToyotaSupportList.items[i].len);
        for(j = 0; j < len; j++) {
            printf("%02x ", mToyotaSupportList.items[i].support_raw_bytes[j]);
        }
        printf("\r\n+++++++++++++++++\r\n");
    }
#endif
}

uint8_t get_real_offset(uint8_t id, uint8_t offset)
{
    uint8_t i, j;
    uint8_t off = 0;

    for(i = 0; i < SUPPORT_MAX_ITEMS; i++) {
        if(id == mToyotaSupportList.items[i].sub_id) {
            for(j = 0; j < offset; j++) {
                if(mToyotaSupportList.items[i].support_raw_bytes[j] == 0x00)
                    off ++;
            }
            break;
        }
    }

    return off;
}

uint8_t* toyota_data_stream(uint8_t pid, uint8_t *len)
{
    uint8_t i, j;
    int8_t ret;
    uint8_t valid_len;
    uint8_t valid_index = 0;
    uint8_t data_type;
    uint8_t offset;
    uint8_t l_bytes = 0;
    uint8_t l_packages = 0;
    uint8_t sub_cmd = 0;

    CanTxMsg txMsg;
    CanRxMsg *rxMsg;

    //check if this pid supported
    if(toyotaSupportItems[pid].support != SUPPORTED) {
        *len = UNSUPPORTED_LEN;
        return NULL;
    }

    if(mToyotaSupportList.support_done == FALSE) {
        toyota_get_supported();
    }

    //printf("pid: %s\r\n", getPidKey(pid));
    //clear flexcan rx buf
    //OSTimeDlyHMSM(0, 0, 0, 100);
    flexcan_reset();
    valid_len = toyotaStdDs[pid].valid_len;
    offset = toyotaStdDs[pid].offset;
    //logi("offset = %d\r\n", offset);
    //get real offset
    if(mToyotaSupportList.support_done == TRUE) {
        if(toyotaStdDs[pid].rxId == 0x7e8) {
            offset = offset - get_real_offset(toyotaStdDs[pid].data[2], offset);
            //logi("real offset = %d\r\n", offset);
        }
    }

    txMsg.StdId = toyotaStdDs[pid].txId;
    txMsg.IDE = CAN_ID_STD;
    txMsg.DLC = toyotaStdDs[pid].len;
    for(i = 0; i < txMsg.DLC; i++) {
        txMsg.Data[i] = toyotaStdDs[pid].data[i];
    }
    ret = flexcan_ioctl(DIR_BI, &txMsg, toyotaStdDs[pid].rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //check if the receive msg type is needed
        if(txMsg.Data[0] > 10) {
            data_type = 0x40 + txMsg.Data[2];
            sub_cmd = txMsg.Data[3];
        } else {
            data_type = 0x40 + txMsg.Data[1];
            sub_cmd = txMsg.Data[2];
        }
        for(i = 0; i < 8; i++) {
            toyota_rx_buf[i] = rxMsg->Data[i];
        }

        //find the valid index
        for(i = 0; i < 8; i++) {
            if(toyota_rx_buf[i] == data_type) {
                //skip 61 xx
                valid_index = i;
                break;
            }
        }

        if(sub_cmd != toyota_rx_buf[valid_index + 1]) {
            loge("ERROR: sub_cmd = %02x, valid_index = %d\r\n",
                    sub_cmd, valid_index);
            return NULL;
        }

        //check if this recv package is a long package
        if(toyota_rx_buf[0] == 0x10) {
            l_bytes = toyota_rx_buf[1];
            l_packages = 0;
            l_packages = (l_bytes - 6) / 7;
            if((l_bytes - 6) % 7 > 0) {
                l_packages += 1;
            }
            for(i = 2; i < 8; i++) {
                toyota_rx_buf[i - 2] = rxMsg->Data[i];
            }
            //send continue package
            ret = flexcan_ioctl(DIR_BI, &toyota_continue_package,
                    toyotaStdDs[pid].rxId, l_packages);
            if(ret == l_packages) {
                for(i = 0;i < ret; i++) {
                    rxMsg = flexcan_dump();
                    for(j = 0; j < 7; j++) {
                        toyota_rx_buf[6 + i * 7 + j] = rxMsg->Data[j + 1];
                    }
                }
            } else {
                loge("error: ret = %d\r\n", ret);
            }
        }
        //short package
        else {
            if(txMsg.Data[0] > 10) {
                data_type = 0x40 + txMsg.Data[2];
                sub_cmd = txMsg.Data[3];
            } else {
                data_type = 0x40 + txMsg.Data[1];
                sub_cmd = txMsg.Data[2];
            }
            //find the valid index
            for(i = 0; i < 8; i++) {
                if(toyota_rx_buf[i] == data_type) {
                    //skip 61 xx
                    valid_index = i;
                    break;
                }
            }
            if(sub_cmd != toyota_rx_buf[valid_index + 1]) {
                loge("sub_cmd = %02x, valid_index = %d\r\n",
                        sub_cmd, valid_index);
                return NULL;
            }
            //shift the valid data to head
            for(i = 0; i < 8; i++) {
                toyota_rx_buf[i] = toyota_rx_buf[valid_index + i];
            }
        }
        *len = valid_len;
        //logi("offset = %d, len = %d\r\n", offset, *len);
        //for(i = 0; i < 40; i++) {
        //    printf("%02x ", toyota_rx_buf[i]);
        //}
        //printf("\r\n");
        return toyota_rx_buf + offset + 2;
    } else {
        return NULL;
    }

}

CanTxMsg toyota_wakeup[2] =
{
    {
        0x020, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        4,
        0xe0, 0x00, 0x07, 0x0b
    },
    {
        0x0ba, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        4,
        0x00, 0x00, 0x00, 0xbe
    },
};
#if 0
CanTxMsg toyota_wakeup[8] =
{
    {
        0x440, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x00, 0x80, 0x00, 0x01, 0x0f, 0x0f, 0x0f
    },
    {
        0x44d, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x4d, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x442, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x42, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    {
        0x443, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x43, 0x00, 0x80, 0x20, 0x05, 0x00, 0x00, 0x00
    },
    {
        0x440, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x40, 0x01, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x44d, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x4d, 0x01, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00
    },
    {
        0x442, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x42, 0x01, 0x80, 0x00, 0x02, 0x00, 0x00, 0x00
    },
    {
        0x443, 0x18db33f1,
        CAN_ID_STD, CAN_RTR_DATA,
        8,
        0x43, 0x01, 0x80, 0x20, 0x05, 0x00, 0x00, 0x00
    },
};
#endif

void toyota_ctrl_window(uint8_t state)
{
    uint8_t i = 0;

    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }
    if(state) {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_on[i]);
            xdelay_ms(500);
        }
    } else {
        for(i = 0; i < 4; i++) {
            flexcan_send_frame(&toyota_window_off[i]);
            xdelay_ms(500);
        }
    }
}

void toyota_ctrl_door(uint8_t state)
{
    uint8_t i;

    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }

    if(state) {
        flexcan_send_frame(&toyota_door_on);
    } else {
        flexcan_send_frame(&toyota_door_off);
    }
}

void toyota_ctrl_light(uint8_t state)
{
    uint8_t i;

    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }
    if(state) {
        flexcan_send_frame(&toyota_lamp_on);
    } else {
        flexcan_send_frame(&toyota_lamp_off);
    }
}

void toyota_ctrl_sunroof(uint8_t state)
{
    uint8_t i;

    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }
    if(state) {
        flexcan_send_frame(&toyota_sunroof_on);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_on);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_on);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_on);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_on);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_on);
    } else {
        flexcan_send_frame(&toyota_sunroof_off);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_off);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_off);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_off);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_off);
        xdelay(1);
        flexcan_send_frame(&toyota_sunroof_off);
    }
}

void toyota_ctrl_trunk(uint8_t state)
{
    uint8_t i;

    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }
    if(state) {
        flexcan_send_frame(&toyota_trunk_on);
    } else {
        flexcan_send_frame(&toyota_trunk_off);
    }

}

void toyota_ctrl_findcar(uint8_t state)
{
    uint8_t i = 0;
    for(i = 0; i < 2; i++) {
        flexcan_send_frame(&toyota_wakeup[i]);
        xdelay_ms(200);
    }

    flexcan_send_frame(&toyota_findcar[0]);
    xdelay_ms(100);
    flexcan_send_frame(&toyota_lamp_on);
    xdelay(3);
    flexcan_send_frame(&toyota_findcar[1]);
    xdelay_ms(100);
    flexcan_send_frame(&toyota_lamp_off);
    xdelay(3);
}

void toyota_clear_fault_code(void)
{
    uint8_t i;

    for(i = 0; i < FAULT_CODE_MAX_SIZE; i++) {
        flexcan_send_frame(&toyota_clear_fault[i]);
        xdelay(200);
    }
}


uint32_t *toyota_check_fault_code(uint8_t id, uint8_t *len)
{
    uint8_t i, j;
    int8_t ret;
    uint8_t valid_len;
    uint8_t valid_index = 0;
    uint8_t data_type;
    uint8_t offset;
    uint8_t l_bytes = 0;
    uint8_t l_packages = 0;
    uint8_t sub_cmd = 0;
    uint16_t rxId;
    uint8_t hasSubCmd = FALSE;
    uint8_t shift = 3;

    CanRxMsg *rxMsg;

    memset(toyota_fault_data, 0x00, 100);
    memset(toyota_code_val, 0x00, FAULT_CODE_MAX_SIZE);

    rxId = toyota_fault_code[id].StdId + 8;
    if(toyota_fault_code[id].Data[0] > 0x10) {
        hasSubCmd = TRUE;
    }
    //printf("%s: txId = 0x%04x, rxId = 0x%04x\r\n", __func__,
    //        toyota_fault_code[id].StdId, rxId);
    ret = flexcan_ioctl(DIR_BI, &toyota_fault_code[id], rxId, 1);
    if(ret > 0) {
        rxMsg = flexcan_dump();
        //check if the receive msg type is needed
        //check if this recv package is a long package
        if(rxMsg->Data[0] == 0x10) {
            l_bytes = rxMsg->Data[1];
            l_packages = 0;
            l_packages = (l_bytes - 6) / 7;
            if((l_bytes - 6) % 7 > 0) {
                l_packages += 1;
            }
            for(i = 3; i < 8; i++) {
                toyota_fault_data[i - 3] = rxMsg->Data[i];
            }
            //send continue package
            ret = flexcan_ioctl(DIR_BI, &toyota_continue_package,
                    rxId, l_packages);
            if(ret == l_packages) {
                for(i = 0;i < ret; i++) {
                    rxMsg = flexcan_dump();
                    for(j = 0; j < 7; j++) {
                        toyota_fault_data[5 + i * 7 + j] = rxMsg->Data[j + 1];
                    }
                }
            } else {
                loge("error: ret = %d\r\n", ret);
                *len = 0;
                return NULL;
            }
        }
        //short package
        else {
            if(hasSubCmd) {
                shift = 3;
            } else {
                shift = 2;
            }
            //shift the valid data to head
            for(i = shift; i < 8; i++) {
                toyota_fault_data[i - shift] = rxMsg->Data[i];
            }
        }
        //parse fault code
        *len = toyota_fault_data[0];
        for(i = 0; i < *len; i++) {
            toyota_code_val[i] =
                (toyota_fault_data[i * 3 + 1] << 8) | (toyota_fault_data[i * 3 + 2]);
        }
        return toyota_code_val;
    } else {
        *len = 0;
        return NULL;
    }
}
