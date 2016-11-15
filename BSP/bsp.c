#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "bsp.h"
#include "stdio.h"
#include "flash.h"
#include "l206.h"
#include "flexcan.h"

#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef�� d in stdio.h. */
FILE __stdout;
_sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{

    while((PC_USART->SR & 0x40) == 0);
    PC_USART->DR = (u8)ch;
    return ch;
}
#endif

/*******************************************************************************
 * Function Name  : RCC_Configuration
 * Description    : Configures the different system clocks.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/

////////////////////////////////////////////////////////
void RCC_Configuration(void)
{
    SystemInit();
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

//�رյ��Խӿڣ���GPIOʹ��
void UnableJTAG(void)
{

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
    AFIO->MAPR &= ~(7UL<<24); // clear used bit
    AFIO->MAPR |= (4UL<<24); // set used bits
}

void BSP_Init(void)
{
    /* System Clocks Configuration --72M*/
    RCC_Configuration();

    GPIO_Configuration();
    USART_Config(115200);
    /* NVIC configuration */
    NVIC_Configuration();
    flash_init();
    //sim800_init();
    l206_init();
    adc_config();
}
#define ADC1_DR_Address    ((u32)0x4001244C)	 
vu16 ADC_ConvertedValue;

void adc_config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    //����ADģ������˿�Ϊ���� 1·AD ����ͨ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Enable DMA clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);

    /* DMA channel1 configuration ----------------------------------------------*/
    //ʹ��DMA
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			            //DMAͨ��1�ĵ�ַ 
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	            //DMA���͵�ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					            //���ͷ���
    DMA_InitStructure.DMA_BufferSize = 1;								            //�����ڴ��С��1��16λ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				            //�����ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		//ADC1ת����������16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				//���͵�Ŀ�ĵ�ַ��16λ���
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									//ѭ��
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* ����DMA1ͨ��1��������ж� */
    //DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);
    //ʹ��DMAͨ��1
    DMA_Cmd(DMA1_Channel1, ENABLE); 
    //ADC����
    /* ADCת��ʱ�䣺 �� STM32F103xx��ǿ�Ͳ�Ʒ��ʱ��Ϊ56MHzʱΪ1��s(ʱ��Ϊ72MHzΪ1.17��s)
       ADC������Χ0-3.3V    */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                   //����ADC��ʱ��Ϊ72MHZ/6=12M 

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC1�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;		//ģ��ת��������ɨ��ģʽ����ͨ�������ǵ��Σ���ͨ����ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת������������ģʽ�����ǵ���ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;               //�涨��˳����й���ת����ADCͨ������Ŀ�������Ŀ��ȡֵ��Χ��1��16
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channels configuration [����ģʽͨ������]*/ 

    //ADC1 ����ͨ������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);	  //ͨ��11����ʱ�� 55.5����

    //ʹ��ADC1 DMA 
    ADC_DMACmd(ADC1, ENABLE);
    //ʹ��ADC1
    ADC_Cmd(ADC1, ENABLE);	

    // ��ʼ��ADC1У׼�Ĵ���
    ADC_ResetCalibration(ADC1);
    //���ADC1У׼�Ĵ�����ʼ���Ƿ����
    while(ADC_GetResetCalibrationStatus(ADC1));

    //��ʼУ׼ADC1
    ADC_StartCalibration(ADC1);
    //����Ƿ����У׼
    while(ADC_GetCalibrationStatus(ADC1));

    //ADC1ת������
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
}

uint16_t get_adc_value(void)
{
    return ADC_ConvertedValue;
}

/*******************************************************************************
 * Function Name  : GPIO_Configuration
 * Description    : PB5: LED1 (mini and V3)
 PD6��LED2 (only V3)
 PD3��LED3 (only V3)
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Configuration(void)
{

}


/*******************************************************************************
 * Function Name  : NVIC_Configuration
 * Description    : Configures Vector Table base location.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART_Config(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    if (PC_USARTAPB == APB1)
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_PC_USART_IO | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APBxPeriph_PC_USART,ENABLE);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APBxPeriph_PC_USART_IO | RCC_APBxPeriph_PC_USART | RCC_APB2Periph_AFIO,ENABLE);
    }
    if (PC_PinRemap == ENABLE)
    {
        GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    }
    GPIO_InitStructure.GPIO_Pin = PC_USART_TXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PC_USART_IO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = PC_USART_RXD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PC_USART_IO,&GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(PC_USART,&USART_InitStructure);
    //data = data;
    //data = PC_USART->DR;
    //data = PC_USART->SR;
    NVIC_Configuration();
    USART_ITConfig(PC_USART, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(PC_USART, USART_IT_TXE, ENABLE);
    USART_Cmd(PC_USART,ENABLE);
}

#ifdef  DEBUG
/*******************************************************************************
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 *******************************************************************************/
void assert_failed(u8* file, u32 line)
{
    /* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

