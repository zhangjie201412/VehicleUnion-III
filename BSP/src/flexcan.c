#include "stm32f10x.h"
#include "stm32f10x_can.h"
#include "flexcan.h"
#include "ringbuffer.h"
#include "includes.h"
#include "utils.h"

#define FLEXCAN_QUEUE_SIZE      RX_PACKAGE_SIZE

static CanRxMsg g_rxMsg[RX_PACKAGE_SIZE];
static uint8_t w_off, r_off;
static CanRxMsg m_rxMsg;
static uint16_t filter_id = 0x00;
OS_MUTEX mFlexcanMutex;
OS_Q mFlexcanQueue;

void flexcan_nvic_init(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void flexcan_init(u8 velocity)
{
    OS_ERR err;
    CAN_InitTypeDef CAN_InitStructure;

    //init for can
    flexcan_can_enable();
    flexcan_nvic_init();
    flexcan_gpio_init();

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_11tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
    CAN_InitStructure.CAN_Prescaler = velocity;
    CAN_Init(CAN1, &CAN_InitStructure);
    w_off = 0;
    r_off = 0;
    OSMutexCreate(
            (OS_MUTEX *)&mFlexcanMutex,
            (CPU_CHAR *)"FLEXCAN_MUTEX",
            &err
            );
    OSQCreate(
            (OS_Q *)&mFlexcanQueue,
            (CPU_CHAR *)"FLEXCAN_QUEUE",
            (OS_MSG_QTY)FLEXCAN_QUEUE_SIZE,
            (OS_ERR *)&err
            );
    
    BSP_IntVectSet(BSP_INT_ID_CAN1_RX0, (CPU_FNCT_VOID)flexcan_recv);
    BSP_IntPrioSet(BSP_INT_ID_CAN1_RX0, 1);
    BSP_IntEn(BSP_INT_ID_CAN1_RX0);

    logi("%s: done", __func__);
}

void flexcan_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APBxPeriph_CAN_IO |
            RCC_APB2Periph_AFIO,
            ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,
            ENABLE);
    GPIO_InitStructure.GPIO_Pin = CAN_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_IO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CAN_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CAN_IO, &GPIO_InitStructure);

    if(CAN_PinRemap == ENABLE) {
        GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
    }
}

void flexcan_filter(u32 id1, u32 id2, u32 mid1, u32 mid2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    CAN_FilterInitStructure.CAN_FilterNumber=1;
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh=id1<<5;
    CAN_FilterInitStructure.CAN_FilterIdLow=id2<<5;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=mid1<<5;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=mid2<<5;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

void flexcan_can_enable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
}

void flexcan_send_frame(CanTxMsg *txMsg)
{
    u32 count = 0;
    u8 transmitMailBox;

    txMsg->IDE = CAN_ID_STD;
    transmitMailBox = CAN_Transmit(CAN1, txMsg);
    while(CAN_TransmitStatus(CAN1, transmitMailBox) != CANTXOK) {
        count ++;
        if(count > 1000000)
            break;
    }
}

int8_t flexcan_ioctl(uint8_t dir, CanTxMsg *txMsg, uint16_t rxId, uint8_t rxCount)
{
    CanRxMsg *rxMsg;
    OS_MSG_SIZE size;
    OS_ERR err;

    uint8_t i = 0, j = 0;
    int8_t ret = 0;
    uint8_t exception_count = 0;

    if(dir & DIR_INPUT) {
        flexcan_filter(rxId, rxId, rxId | 0xff, rxId | 0xff);
        filter_id = rxId;
    } else {
        flexcan_filter(0x00, 0x00, 0x00ff, 0x00ff);
        filter_id = 0x00;
    }
    if(dir & DIR_OUTPUT) {
        flexcan_send_frame(txMsg);
#ifdef FLEXCAN_DEBUG
        printf("->send %04x ", txMsg->StdId);
        for(i = 0; i < 8; i++) {
            printf("%02x ", txMsg->Data[i]);
        }
        printf("\r\n");
#endif
    }

#if 1
    if(dir & DIR_INPUT) {
        for(i = 0; i < rxCount; /*i++*/) {
            rxMsg = OSQPend(
                    (OS_Q *)&mFlexcanQueue,
                    (OS_TICK)(OS_CFG_TICK_RATE_HZ * FLEXCAN_TIMEOUT),
                    (OS_OPT)OS_OPT_PEND_BLOCKING,
                    (OS_MSG_SIZE *)&size,
                    (CPU_TS *)0,
                    (OS_ERR *)&err
                    );
            if(err != OS_ERR_TIMEOUT) {
                flexcan_lock();
#ifdef FLEXCAN_DEBUG
                printf("->recv %04x ", rxMsg->StdId);
                for(j = 0; j < 8; j++) {
                    printf("%02x ", rxMsg->Data[j]);
                }
                printf("\r\n");
#endif
                //check if the recv id is real rx id
                if(rxMsg->StdId == rxId) {
                    i ++;
                } else {
                    loge("exception!");
                    exception_count ++;
                    if(exception_count > 5) {
                        ret = -1;
                        flexcan_unlock();
                        break;
                    }
                    flexcan_unlock();
                    continue;
                }
                //write can msg
                g_rxMsg[w_off].StdId = rxMsg->StdId;
                g_rxMsg[w_off].DLC = rxMsg->DLC;
                for(j = 0; j < rxMsg->DLC; j++) {
                    g_rxMsg[w_off].Data[j] = rxMsg->Data[j];
                }
                w_off ++;
                if(w_off == RX_PACKAGE_SIZE) {
                    w_off = 0;
                }
                ret ++;
                flexcan_unlock();
            } else {
                loge("%s: timeout", __func__);
                break;
            }
        }
    }
#endif
    //filter none of can id
    flexcan_filter(0x00, 0x00, 0x00ff, 0x00ff);
    filter_id = 0x00;

    return ret;
}

uint8_t flexcan_count(void)
{
    if(w_off >= r_off)
        return w_off - r_off;
    else
        return RX_PACKAGE_SIZE - r_off + w_off;
}

CanRxMsg *flexcan_dump(void)
{
    CanRxMsg *msg = NULL;

    flexcan_lock();
    if(r_off == w_off) {
        flexcan_unlock();
        return NULL;
    }

    msg = &g_rxMsg[r_off ++];
    if(r_off == RX_PACKAGE_SIZE)
        r_off = 0;
    flexcan_unlock();

    return msg;
}

void flexcan_reset(void)
{
    flexcan_lock();
    w_off = 0;
    r_off = 0;
    flexcan_unlock();
}

void flexcan_lock(void)
{
    OS_ERR err;
    OSMutexPend(&mFlexcanMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
}

void flexcan_unlock(void)
{
    OS_ERR err;

    OSMutexPost(&mFlexcanMutex, OS_OPT_POST_NONE, &err);
}

void flexcan_rx_callack(void)
{
    if(0 == OSRunning)
        return;
    OSIntEnter();
    BSP_IntHandlerCAN1_RX0();
    OSIntExit();
}

void flexcan_recv(void)
{
    OS_ERR err;

    CAN_Receive(CAN1, CAN_FIFO0, &m_rxMsg);
    if(m_rxMsg.StdId == filter_id) {
        OSQPost(
                (OS_Q *)&mFlexcanQueue,
                (void *)&m_rxMsg,
                (OS_MSG_SIZE)sizeof(CanRxMsg),
                (OS_OPT)OS_OPT_POST_FIFO,
                (OS_ERR *)&err
               );
    } else {
        //drop
        //TODO:???
    }
}
