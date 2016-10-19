#include <stdio.h>
#include "sim800.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "cJSON.h"
#include "utils.h"
#include "ringbuffer.h"

#define SERV_ADDR   "139.196.153.24"
#define SERV_PORT   9999

#define SIM800_RB_MAX_SIZE          256
#define SIM800_CONNECT_RETRY_TIMES  8

uint8_t mState;
uint8_t mRingBuffer[SIM800_RB_MAX_SIZE];
struct rb mRb;

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

void sim800_powerup(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

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

void sim800_setup(void)
{
    bool connect_done = FALSE;
    uint16_t retry;
    uint8_t recv;
    uint8_t i;

    retry = 0;

    while(!connect_done) {
        switch(mState) {
            case STATE_UNINITED:
                logi("try to powerup sim800");
                sim800_powerup();
                sim800_delay(10);
                mState = STATE_POWERUP;
                break;
            default:
                break;
        }
    }

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
    uint8_t data;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        data =USART_ReceiveData(USART3);
        printf("%c", data);
    }
}

static void sim800_callback(void *unused)
{
}

bool sim800_send_cmd(const char *cmd, const char *rsp)
{
    return TRUE;
}

bool sim800_connect(const char *host, uint32_t port)
{
    return TRUE;
}

uint8_t sim800_get_signal(void)
{
    return 0;
}

void sim800_send(uint8_t *buf, uint32_t len)
{
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
}

void sim800_unlock(void)
{
}

bool sim800_down(uint16_t sec)
{
    return TRUE;
}

void sim800_up(void)
{
}

