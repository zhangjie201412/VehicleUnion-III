#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "rf_module.h"
#include "includes.h"

#define SET_LOW()       GPIO_ResetBits(GPIOC, GPIO_Pin_14)
#define SET_HIGH()       GPIO_SetBits(GPIOC, GPIO_Pin_14)

#define RF_REPEAT_COUNT     20

void rf_module_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //poweron immo module
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);
}

void rf_send_high(void)
{
    OS_ERR err;

    SET_HIGH();
    //delay_us(4000);
    OSTimeDly (20, OS_OPT_TIME_HMSM_STRICT, &err);
    SET_LOW();
    //delay_us(800);
    OSTimeDly (4, OS_OPT_TIME_HMSM_STRICT, &err);
}
void rf_send_low(void)
{
    OS_ERR err;
    SET_HIGH();
    //delay_us(800);
    OSTimeDly (4, OS_OPT_TIME_HMSM_STRICT, &err);
    SET_LOW();
    //delay_us(4000);
    OSTimeDly (20, OS_OPT_TIME_HMSM_STRICT, &err);
}

void rf_send_start(void)
{
    OS_ERR err;
    SET_HIGH();
    //delay_us(400);
    OSTimeDly (2, OS_OPT_TIME_HMSM_STRICT, &err);
    SET_LOW();
    //delay_us(12400);
    OSTimeDly (62, OS_OPT_TIME_HMSM_STRICT, &err);
}

void rf_send(int data)
{
    s8 i;
    int address = data;
    for(i = 7; i >= 0; i--) {
        if((address >> i) & 0x01) {
            rf_send_high();
        } else {
            rf_send_low();
        }
    }
}

void rf_lock(void)
{
    uint8_t i;
    for(i = 0; i < RF_REPEAT_COUNT; i++) {
        rf_send_start();
        rf_send(IMMO_LOCK);
    }
}

void rf_unlock(void)
{
    uint8_t i;
    for(i = 0; i < RF_REPEAT_COUNT; i++) {
        rf_send_start();
        rf_send(IMMO_UNLOCK);
    }
}
