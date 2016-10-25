#include <stdio.h>
#include "sim800.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "cJSON.h"
#include "utils.h"
#include "ringbuffer.h"
#include "includes.h"
#include "transmit.h"
#include "config.h"

#ifdef SERVER_IS_K
#define SERV_ADDR   "139.224.17.163"
#define SERV_PORT   8880
#elif defined SERVER_IS_VEHICLE_UNION
#define SERV_ADDR   "139.196.153.24"
#define SERV_PORT   9999
#endif

#define SIM800_RB_MAX_SIZE          256
#define SIM800_CONNECT_RETRY_TIMES  8

uint8_t mState;
uint8_t mRingBuffer[SIM800_RB_MAX_SIZE];
struct rb mRb;
OS_SEM mWait;
OS_MUTEX mMutex;
OS_MUTEX mSendMutex;
bool mIsConnected = FALSE;

#define SIM800_CMD_SIZE     8
sim800_cmd mCmds[SIM800_CMD_SIZE] =
{
    {"ATE0\r\n", 100},
    {"AT+CREG?\r\n", 100},
    {"AT+CIPMODE=0\r\n", 100},
    {"AT+CGATT?\r\n", 200},
    {"AT+CSTT=\"CMNET\"\r\n", 200},
    {"AT+CIICR\r\n", 3000},
    {"AT+CIFSR\r\n", 300},
    {"AT+CGATT=1\r\n", 300},
};


bool sim800_is_connected(void)
{
    return mIsConnected;
}

void sim800_set_connected(bool connected)
{
    mIsConnected = connected;
}

void sim800_delay(uint8_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, s, 0, OS_OPT_TIME_HMSM_STRICT, &err);
}

void sim800_delay_ms(uint16_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, 0, s, OS_OPT_TIME_HMSM_STRICT, &err);
}

void sim800_reset(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    logi("%s", __func__);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

#if 1
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
    sim800_delay(2);
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    sim800_delay_ms(200);
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
    sim800_delay(2);
#endif
#if 0
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    sim800_delay(2);
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
    sim800_delay_ms(500);
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    sim800_delay(2);
#endif
}

void sim800_powerup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    logi("%s", __func__);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    sim800_delay(4);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    sim800_delay(4);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void sim800_powerdown(void)
{
    sim800_powerup();
}

bool sim800_setup(bool reboot)
{
    bool ret;
    OS_ERR err;
    bool connect_done = FALSE;
    uint16_t retry;
    uint8_t i;

    retry = 0;
    if(!reboot) {
        //init for ucos
        OSSemCreate(
                (OS_SEM *) &mWait,
                (CPU_CHAR *)"SIM800_WAIT",
                (OS_SEM_CTR)0,
                (OS_ERR *)&err
                );
        OSMutexCreate(
                (OS_MUTEX *)&mMutex,
                (CPU_CHAR *)"SIM800_MUTEX",
                &err
                );
        OSMutexCreate(
                (OS_MUTEX *)&mSendMutex,
                (CPU_CHAR *)"SIM800_SEND_MUTEX",
                &err
                );
    }

    mIsConnected  = FALSE;
    while(!connect_done) {
        switch(mState) {
            case STATE_UNINITED:
                logi("%s: STATE_UNINITED", __func__);
                //                sim800_reset();
                sim800_powerup();
                sim800_delay(10);

                mState = STATE_POWERUP;
                if(sim800_send_cmd("AT\r\n", "AT") == TRUE) {
                    mState = STATE_INITED;
                    logi("powerup done");
                } else {
                    mState = STATE_UNINITED;
                    loge("powerup failed");
                }
                break;
            case STATE_INITED:
                logi("%s: STATE_INITED", __func__);
                for(i = 0; i < SIM800_CMD_SIZE; i++) {
                    sim800_write((uint8_t *)mCmds[i].cmd,
                            strlen(mCmds[i].cmd));
                    if(mCmds[i].delay >= 1000) {
                        sim800_delay(mCmds[i].delay / 1000);
                    } else {
                        sim800_delay_ms(mCmds[i].delay);
                    }
                }
                rb_clear(&mRb);
                mState = STATE_CONNECTING;
                break;
            case STATE_CONNECTING:
                logi("%s: STATE_CONNECTING", __func__);
                ret = sim800_connect(SERV_ADDR, SERV_PORT);
                if(ret ==TRUE) {
                    loge("connect success!");
                    mState = STATE_CONNECTED;
                } else {
                    loge("connect failed, retry!");
                    mState = STATE_UNINITED;
                    retry ++;
                }
                break;
            case STATE_CONNECTED:
                logi("%s: STATE_CONNECTED", __func__);
                connect_done = TRUE;
                mState = STATE_IDLE;
                mIsConnected  = TRUE;
            default:
                break;
        }
        if(retry > SIM800_CONNECT_RETRY_TIMES) {
            loge("%s: retry = %d", __func__, retry);
            break;
        }
    }
    if(!connect_done) {
        loge("%s: We cannot connect server, retried %d times",
                __func__, SIM800_CONNECT_RETRY_TIMES);
        return FALSE;
    }
    return TRUE;
}

void SIM800_USART_IRQHandler(void)
{
    if(0 == OSRunning)
        return;
    OSIntEnter();
    BSP_IntHandlerUSART3();
    OSIntExit();
}

void sim800_recv(void)
{
    static bool json_start;
    uint8_t data;
    static uint8_t buf[4];
    OS_ERR err;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        data =USART_ReceiveData(USART3);
        //        printf("%c", data);
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = data;

        switch(mState) {
            case STATE_UNINITED:
                break;
            case STATE_SIGNAL:
                sim800_lock();
                rb_put(&mRb, &data, 1);
                if(buf[2] == 'O' &&
                        buf[3] == 'K') {
                    sim800_up();
                }

                sim800_unlock();
                break;
            case STATE_POWERUP:
            case STATE_CONNECTING:
            case STATE_SENDING:
                sim800_lock();
                rb_put(&mRb, &data, 1);
                if(buf[0] == 'O' &&
                        buf[1] == 'K' &&
                        buf[2] == '\r' &&
                        buf[3] == '\n') {
                    sim800_up();
                }
                sim800_unlock();
                break;
            case STATE_IDLE:
                sim800_lock();
                if(data == '{') {
                    json_start = TRUE;
                    rb_clear(&mRb);
                    rb_put(&mRb, &data, 1);

                } else if(data == '}') {
                    rb_put(&mRb, &data, 1);
                    json_start = FALSE;
                    //TO FIX
                    OSTaskSemPost(&TransmitCallbackTaskTCB,
                            OS_OPT_POST_NONE, &err);
                    //                    logi("#####post task sem");
                } else {
                    if(json_start) {
                        rb_put(&mRb, &data, 1);
                    }
                }
                sim800_unlock();
            default:
                break;
        }
    }
}

bool sim800_send_cmd(const char *cmd, const char *rsp)
{
    uint16_t rspLen;
    uint8_t i, index = 0;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;

    rspLen = strlen(rsp);
    sim800_lock();
    rb_clear(&mRb);
    sim800_write((uint8_t *)cmd, strlen(cmd));
    ret = sim800_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            rx_buf[index ++] = recv;
        }
        for(i = 0; i < rspLen; i++) {
            if(rsp[i] != rx_buf[i]) {
                sim800_unlock();
                return FALSE;
            }
        }
        sim800_unlock();
        return TRUE;
    } else {
        sim800_unlock();
        return FALSE;
    }
}

bool sim800_connect(const char *host, uint32_t port)
{
    bool ret;
    uint8_t buf[128];
    uint8_t recv;
    uint8_t rx_buf[20];
    uint8_t index = 0;

    memset(buf, 0x00, 128);
    snprintf((char *)buf, 128, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n",
            host, port);
    logi("%s: %s", __func__, buf);
    sim800_lock();
    ret = sim800_send_cmd((const char *)buf, "\r\nOK");
    if(ret) {
        ret = sim800_down(10);
        if(ret == TRUE) {
            while(!rb_is_empty(&mRb)) {
                rb_get(&mRb, &recv, 1);
                //logi("RECV %02x", recv);
                rx_buf[index ++] = recv;
            }
            sim800_unlock();
            if(strstr((const char *)rx_buf, "CONNECT OK") != NULL) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            sim800_unlock();
            return FALSE;
        }
    } else {
        sim800_unlock();
        return FALSE;
    }
}

uint8_t sim800_get_signal(void)
{
    uint8_t signal;
    uint8_t index = 0, i;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;
    uint8_t *s, *p;
    uint8_t buf[4];

    sim800_lock();
    memset(rx_buf, 0x00, 20);
    mState = STATE_SIGNAL;
    rb_clear(&mRb);
    sim800_write("AT+CSQ\r\n", 8);
#if 1
    ret = sim800_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            rx_buf[index ++] = recv;
        }
        if(NULL != (s = strstr((const char *)rx_buf, "+CSQ:"))) {
            s = strstr((char *)(s), " ");
            s =s + 1;
            p = strstr((char *)(s), ",");
            if(NULL != s) {
                i = 0;
                while(s < p) {
                    buf[i++] = *(s++);
                }
                buf[i] = '\0';
            }
            signal = atoi(buf);
        }
    } else {
        signal = 0;
    }
#endif
    sim800_unlock();
    mState = STATE_IDLE;
    return signal;
}

void sim800_send(uint8_t *buf, uint32_t len)
{
    OS_ERR err;
    uint8_t endByte[1] = {0x1a};

    logi("%s: %s", __func__, buf);
    OSMutexPend(&mSendMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    mState = STATE_DATA_BUSY;
    sim800_write("AT+CIPSEND\r\n", 12);
    sim800_delay_ms(100);
    sim800_write(buf, len);
    mState = STATE_SENDING;
    sim800_write(endByte, 1);
    sim800_delay_ms(200);
    mState = STATE_IDLE;

    OSMutexPost(&mSendMutex, OS_OPT_POST_NONE, &err);
}


void sim800_init(void)
{
    u8 data;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (SIM800_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM800_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_SIM800_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_SIM800_USART_IO | RCC_APBxPeriph_SIM800_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (SIM800_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = SIM800_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM800_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SIM800_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM800_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(SIM800_USART,&USART_InitStructure);
    data = data;
    data = SIM800_USART->DR;
    data = SIM800_USART->SR;
    USART_ITConfig(SIM800_USART,USART_IT_RXNE,ENABLE);
    USART_Cmd(SIM800_USART,ENABLE);
    //init var
    rb_init(&mRb, &mRingBuffer[0], SIM800_RB_MAX_SIZE);
    mState = STATE_UNINITED;

    BSP_IntVectSet(BSP_INT_ID_USART3, (CPU_FNCT_VOID)sim800_recv);
    BSP_IntPrioSet(BSP_INT_ID_USART3, 1);
    BSP_IntEn(BSP_INT_ID_USART3);
    logi("%s", __func__);
}

void sim800_write(uint8_t *buf, uint16_t size)
{

    uint16_t i = 0;
    for(; i < size; i++) {
        USART_SendData(SIM800_USART, buf[i]);
        while(USART_GetFlagStatus(SIM800_USART, USART_FLAG_TXE) == RESET)
        {;}
    }
}

void sim800_lock(void)
{
    OS_ERR err;

    OSMutexPend(&mMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
}

void sim800_unlock(void)
{
    OS_ERR err;

    OSMutexPost(&mMutex, OS_OPT_POST_NONE, &err);
}

bool sim800_down(uint16_t sec)
{
    OS_ERR err;

    //logi("%s", __func__);
    OSSemPend(&mWait, sec * OS_CFG_TICK_RATE_HZ, OS_OPT_PEND_BLOCKING, 0, &err);
    if(err == OS_ERR_TIMEOUT) {
        logi("%s: timeout", __func__);
        return FALSE;
    } else {
        return TRUE;
    }
}

void sim800_up(void)
{
    OS_ERR err;
    //logi("%s", __func__);
    OSSemPost(&mWait, OS_OPT_POST_ALL, &err);
}

