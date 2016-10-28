#ifndef __L206_H__
#define __L206_H__
#include "includes.h"
#include "bsp.h"

#define RCC_APBxPeriph_L206_USART_IO          RCC_APB2Periph_GPIOB
#define RCC_APBxPeriph_L206_USART             RCC_APB1Periph_USART3
#define L206_USART_TXD				        GPIO_Pin_10
#define L206_USART_RXD				        GPIO_Pin_11
#define L206_USART_IO                         GPIOB
#define L206_USART	                        USART3
#define L206_PinRemap                         DISABLE
#define L206_USARTAPB                         APB1
#define L206_USART_IRQHandler                 USART3_IRQHandler

typedef struct __L206_CMD {
    char *cmd;
    uint16_t delay;
} l206_cmd;

typedef void(*RecvFunc)(uint8_t *buf);

enum StatusType {
    STATE_UNINITED,
    STATE_POWERUP,
    STATE_INITED,
    STATE_SIGNAL,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_IDLE,
    STATE_CMDING,
    STATE_SENDING,
    STATE_CMD_BUSY,
    STATE_DATA_BUSY,
};

extern struct rb mRb;
//extern OS_SEM mWait;

//public:
bool l206_setup(bool reboot);
void l206_powerup(void);
void l206_powerdown(void);
bool l206_is_connected(void);
void l206_set_connected(bool connected);
void l206_send(uint8_t *buf, uint32_t len);
uint8_t l206_get_signal(void);

//private:
void l206_delay(uint8_t s);
void l206_delay_ms(uint16_t s);
bool l206_send_cmd(const char *cmd, const char *rsp);
bool l206_connect(const char *host, uint32_t port);
void l206_init(void);
void l206_write(uint8_t *buf, uint16_t size);
void l206_lock(void);
void l206_unlock(void);
bool l206_down(uint16_t sec);
void l206_up(void);
void l206_reset(void);
//void l206_register_recv(RecvFunc func);

#endif
