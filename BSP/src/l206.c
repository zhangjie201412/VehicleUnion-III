#include <stdio.h>
#include "l206.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "cJSON.h"
#include "utils.h"
#include "ringbuffer.h"
#include "includes.h"
#include "transmit.h"
#include "config.h"
#include "utils.h"

#ifdef SERVER_IS_K
#define SERV_ADDR   "139.224.17.163"
#define SERV_PORT   8880
#elif defined SERVER_IS_VEHICLE_UNION
#define SERV_ADDR   "139.196.153.24"
#define SERV_PORT   9999
#endif

#define L206_RB_MAX_SIZE          256
#define L206_CONNECT_RETRY_TIMES  4

uint8_t mState;
uint8_t mRingBuffer[L206_RB_MAX_SIZE];
struct rb mRb;
OS_SEM mWait;
OS_MUTEX mMutex;
OS_MUTEX mSendMutex;
bool mIsConnected = FALSE;

#define L206_CMD_SIZE     7
l206_cmd mCmds[L206_CMD_SIZE] =
{
    {"ATE0\r\n", 100},
    {"AT+CPIN?\r\n", 100},
    {"AT+CREG?\r\n", 100},
    {"AT+CGATT?\r\n", 200},
    {"AT+ZIPCFG=CMNET\r\n", 200},
    {"AT+ZIPCALL=1\r\n", 6000},
    {"AT+GTPOS=1\r\n", 3000},
    //{"AT+GTPOS=2\r\n", 3000},
};

uint32_t l206_lng = 0;
uint32_t l206_lat = 0;

bool l206_is_connected(void)
{
    return mIsConnected;
}

void l206_set_connected(bool connected)
{
    mIsConnected = connected;
}

void l206_delay(uint8_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, s, 0, OS_OPT_TIME_HMSM_STRICT, &err);
}

void l206_delay_ms(uint16_t s)
{
    OS_ERR err;
    OSTimeDlyHMSM(0, 0, 0, s, OS_OPT_TIME_HMSM_STRICT, &err);
}

void l206_reset(void)
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
    l206_delay(2);
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    l206_delay(1);
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
    l206_delay(2);
#endif
#if 0
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    l206_delay(2);
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
    l206_delay_ms(500);
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    l206_delay(2);
#endif
}

void l206_powerup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    logi("%s", __func__);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    l206_delay(4);
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    l206_delay(4);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void l206_powerdown(void)
{
    l206_powerup();
}

bool l206_setup(bool reboot)
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
                (CPU_CHAR *)"L206_WAIT",
                (OS_SEM_CTR)0,
                (OS_ERR *)&err
                );
        OSMutexCreate(
                (OS_MUTEX *)&mMutex,
                (CPU_CHAR *)"L206_MUTEX",
                &err
                );
        OSMutexCreate(
                (OS_MUTEX *)&mSendMutex,
                (CPU_CHAR *)"L206_SEND_MUTEX",
                &err
                );
    }

    mIsConnected  = FALSE;
    while(!connect_done) {
        switch(mState) {
            case STATE_UNINITED:
                logi("%s: STATE_UNINITED", __func__);
                l206_reset();
                l206_powerup();
                l206_delay(10);

                mState = STATE_POWERUP;
                if(l206_send_cmd("AT\r\n", "AT") == TRUE) {
                    mState = STATE_INITED;
                    logi("powerup done");
                } else {
                    mState = STATE_UNINITED;
                    loge("powerup failed");
                }
                break;
            case STATE_INITED:
                logi("%s: STATE_INITED", __func__);
                for(i = 0; i < L206_CMD_SIZE; i++) {
                    l206_write((uint8_t *)mCmds[i].cmd,
                            strlen(mCmds[i].cmd));
                    if(mCmds[i].delay >= 1000) {
                        l206_delay(mCmds[i].delay / 1000);
                    } else {
                        l206_delay_ms(mCmds[i].delay);
                    }
                }
                rb_clear(&mRb);
                mState = STATE_CONNECTING;
                break;
            case STATE_CONNECTING:
                //check signal
                logi("%s: signal = %d", __func__, l206_get_signal());
                mState = STATE_CONNECTING;
                logi("%s: STATE_CONNECTING", __func__);
                ret = l206_connect(SERV_ADDR, SERV_PORT);
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
        if(retry > L206_CONNECT_RETRY_TIMES) {
            loge("%s: retry = %d", __func__, retry);
            break;
        }
    }
    if(!connect_done) {
        loge("%s: We cannot connect server, retried %d times",
                __func__, L206_CONNECT_RETRY_TIMES);
        return FALSE;
    }

    return TRUE;
}

void L206_USART_IRQHandler(void)
{
    if(0 == OSRunning)
        return;
    OSIntEnter();
    BSP_IntHandlerUSART3();
    OSIntExit();
}

void l206_recv(void)
{
    static bool json_start = FALSE;
    uint8_t data;
    static uint8_t buf[4];
    OS_ERR err;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        data =USART_ReceiveData(USART3);
        //printf("%c", data);
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = data;

        switch(mState) {
            case STATE_UNINITED:
                break;
            case STATE_INITED:
                //printf("%c", data);
                break;
            case STATE_CMD_BUSY:
                break;
            case STATE_SIGNAL:
                l206_lock();
//                printf("%c", data);
                rb_put(&mRb, &data, 1);
                if(buf[2] == '\r' &&
                        buf[3] == '\n') {
                    l206_up();
                }

                l206_unlock();
                break;
            case STATE_POWERUP:
            case STATE_CONNECTING:
            case STATE_SENDING:
                l206_lock();
                rb_put(&mRb, &data, 1);
                if(buf[0] == 'O' &&
                        buf[1] == 'K' &&
                        buf[2] == '\r' &&
                        buf[3] == '\n') {
                    l206_up();
                }
                l206_unlock();
                break;
            case STATE_IDLE:
                l206_lock();
                if(data == '{' && json_start == FALSE) {
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
                l206_unlock();
                break;
            default:
                break;
        }
    }
}

bool l206_send_cmd(const char *cmd, const char *rsp)
{
    uint16_t rspLen;
    uint8_t i, index = 0;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;

    rspLen = strlen(rsp);
    l206_lock();
    rb_clear(&mRb);
    l206_write((uint8_t *)cmd, strlen(cmd));
    ret = l206_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            rx_buf[index ++] = recv;
            //printf("RECV = %02x, %c\r\n", recv, recv);
        }
        //logi("rx_buf = %s", rx_buf);
        for(i = 0; i < rspLen; i++) {
            if(rsp[i] != rx_buf[i]) {
                l206_unlock();
                return FALSE;
            }
        }
        l206_unlock();
        return TRUE;
    } else {
        l206_unlock();
        return FALSE;
    }
}

bool l206_connect(const char *host, uint32_t port)
{
    bool ret;
    uint8_t buf[128];
    uint8_t recv;
    uint8_t rx_buf[20];
    uint8_t index = 0;

    memset(buf, 0x00, 128);
    snprintf((char *)buf, 128, "AT+ZIPOPEN=2,0,%s,%d\r\n",
            host, port);
    logi("%s: %s", __func__, buf);
    l206_lock();
    rb_clear(&mRb);
    ret = l206_send_cmd((const char *)buf, "\r\nOK");
    if(ret) {
        //l206_unlock();
        //return TRUE;
#if 1
        ret = l206_down(10);
        if(ret == TRUE) {
            while(!rb_is_empty(&mRb)) {
                rb_get(&mRb, &recv, 1);
                //logi("RECV %02x %c", recv, recv);
                rx_buf[index ++] = recv;
            }
            l206_unlock();
            //logi("rx_buf = %s", rx_buf);
            if(strstr((const char *)rx_buf, "CONNECT OK") != NULL) {
                return TRUE;
            } else {
                return FALSE;
            }
        } else {
            l206_unlock();
            return FALSE;
        }
#endif
    } else {
        l206_unlock();
        return FALSE;
    }
}

void l206_connect_location_server(void)
{
    logi("%s: start", __func__);
    //l206_lock();
    mState = STATE_CMD_BUSY;    
    l206_write("AT+GTPOS=1\r\n", 12);
    l206_delay(1);
    mState = STATE_IDLE;
    //l206_unlock();
    logi("%s: done", __func__);
}

void l206_get_pos(void)
{
    uint8_t index = 0, i;
    uint8_t recv;
    uint8_t rx_buf[40];
    bool ret;
    uint8_t *s, *p;
    uint8_t buf[40];
    double lng, lat;
    char lng_buf[20];
    char lat_buf[20];

    l206_lock();
    memset(rx_buf, 0x00, 40);
    memset(buf, 0x00, 40);
    mState = STATE_SIGNAL;
    rb_clear(&mRb);
    l206_write("AT+GTPOS=2\r\n", 12);

    ret = l206_down(2);
#if 0
    if(ret == TRUE) {
    } else {
        loge("down failed");
    }
#endif
    while(!rb_is_empty(&mRb)) {
        rb_get(&mRb, &recv, 1);
        rx_buf[index ++] = recv;
    }
    if(NULL != (s = strstr((const char *)rx_buf, "+GTPOS:"))) {
        s = strstr((char *)(s), " ");
        s =s + 1;
        p = strstr((char *)(s), "\r\n");
        if(NULL != s) {
            i = 0;
            while(s < p) {
                buf[i++] = *(s++);
            }
            buf[i] = '\0';
        }
        logi("buf: %s", buf);
    } else {
        loge("cannot get position info, [%s]", rx_buf);
        mState = STATE_IDLE;
        l206_unlock();
        l206_lng = 0;
        l206_lat = 0;
        //need to reconnect position server
        l206_connect_location_server();
        return;
    }
    memset(lng_buf, 0x00, 20);
    memset(lat_buf, 0x00, 20);
    index = 0;
    i = 0;
    while(buf[i] != ',') {
        lng_buf[index ++] = buf[i++];
    }
    lng_buf[index] = '\0';
    i++;
    index = 0;
    while(buf[i] != '\0') {
        lat_buf[index ++] = buf[i++];
    }
    lat_buf[index] = '\0';
    logi("lng = %s, lat = %s", lng_buf, lat_buf);
    mState = STATE_IDLE;
    l206_unlock();
    lng = atof(lng_buf);
    lat = atof(lat_buf);
    //upload_location(lng * 1000000, lat * 1000000);
    l206_lng = lng * 100000;
    l206_lat = lat * 100000;
}

void l206_get_location(uint32_t *lng, uint32_t *lat)
{
    l206_get_pos();
    *lng = l206_lng;
    *lat = l206_lat;
}

uint8_t l206_get_signal(void)
{
    uint8_t signal;
    uint8_t index = 0, i;
    uint8_t recv;
    uint8_t rx_buf[20];
    bool ret;
    uint8_t *s, *p;
    uint8_t buf[4];

    l206_lock();
    memset(rx_buf, 0x00, 20);
    mState = STATE_SIGNAL;
    rb_clear(&mRb);
    l206_write("AT+CSQ\r\n", 8);
#if 1
    ret = l206_down(4);
    if(ret == TRUE) {
        while(!rb_is_empty(&mRb)) {
            rb_get(&mRb, &recv, 1);
            rx_buf[index ++] = recv;
            l206_delay_ms(1);
            //logi("recv = %02x %c", recv, recv);
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
    l206_unlock();
    mState = STATE_IDLE;
    return signal;
}

void l206_send(uint8_t *buf, uint32_t len)
{
    OS_ERR err;
    uint16_t index = 0, i;
    uint8_t *send_buf;

    logi("%s: %s", __func__, buf);
    OSMutexPend(&mSendMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    l206_lock();
    mState = STATE_SENDING;
    send_buf = (uint8_t *)malloc(13 + 2 * len);
    index = snprintf(send_buf, 1024, "AT+ZIPSEND=2,");
    for(i = 0; i < len; i++) {
        index += snprintf(send_buf + index, 1024, "%02x", buf[i]);
    }
    l206_write(send_buf, 13+2*len);
    if(l206_send_cmd("\r\n", "\r\nSEND OK") == TRUE) {
        //logi("send ok");
    } else {
        logi("send failed");
    }
    mState = STATE_IDLE;
    free(send_buf);
    l206_unlock();
    OSMutexPost(&mSendMutex, OS_OPT_POST_NONE, &err);
}


void l206_init(void)
{
    u8 data;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (L206_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_L206_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_L206_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_L206_USART_IO | RCC_APBxPeriph_L206_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (L206_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = L206_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(L206_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = L206_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(L206_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(L206_USART,&USART_InitStructure);
    data = data;
    data = L206_USART->DR;
    data = L206_USART->SR;
    USART_ITConfig(L206_USART,USART_IT_RXNE,ENABLE);
    USART_Cmd(L206_USART,ENABLE);
    //init var
    rb_init(&mRb, &mRingBuffer[0], L206_RB_MAX_SIZE);
    mState = STATE_UNINITED;

    BSP_IntVectSet(BSP_INT_ID_USART3, (CPU_FNCT_VOID)l206_recv);
    BSP_IntPrioSet(BSP_INT_ID_USART3, 1);
    BSP_IntEn(BSP_INT_ID_USART3);
}

void l206_write(uint8_t *buf, uint16_t size)
{

    uint16_t i = 0;
    for(; i < size; i++) {
        USART_SendData(L206_USART, buf[i]);
        while(USART_GetFlagStatus(L206_USART, USART_FLAG_TXE) == RESET)
        {;}
    }
}

void l206_lock(void)
{
    OS_ERR err;

    //logi("+++%s+++", __func__);
    OSMutexPend(&mMutex, 0, OS_OPT_PEND_BLOCKING, 0, &err);
}

void l206_unlock(void)
{
    OS_ERR err;

    //logi("---%s---", __func__);
    OSMutexPost(&mMutex, OS_OPT_POST_NONE, &err);
}

bool l206_down(uint16_t sec)
{
    OS_ERR err;

    //logi("%s", __func__);
    OSSemSet(&mWait, 0, &err);
    OSSemPend(&mWait, sec * OS_CFG_TICK_RATE_HZ, OS_OPT_PEND_BLOCKING, 0, &err);
    if(err == OS_ERR_TIMEOUT) {
        logi("%s: timeout", __func__);
        return FALSE;
    } else {
        return TRUE;
    }
}

void l206_up(void)
{
    OS_ERR err;
    //logi("%s", __func__);
    OSSemPost(&mWait, OS_OPT_POST_ALL, &err);
}

