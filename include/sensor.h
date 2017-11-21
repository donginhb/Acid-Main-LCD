/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ���ͷ�ļ���                                    
**                                                                             
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : sensor.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : 
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef SENSOR_H_     //��ֹ���ذ���
#define SENSOR_H_

/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************
*                      ��������                                                
******************************************************************************/
typedef enum
{
    sensor_state_buffing,
    sensor_state_seek_head,
    //sensor_state_wait_for_data,
    sensor_state_decoding
    
}sensor_state_t;

typedef struct
{
    float Sensor_Current;
    float Sensor_Temp;
    float Flue_Temp;
    float Ambient_Temp;
}sensor_data_t;

/******************************************************************************
*                      ������ú�                                              
******************************************************************************/


/******************************************************************************
*                      Ӳ�����ú�                                              
******************************************************************************/
    /******************USART6*****************************/
#define USART6_BAUND_RATE               115200
#define USART6_GPIO_PORT                'C'
#define USART6_TBUFF_SIZE               50        
#define USART6_RBUFF_SIZE               50  

#if(USART6_GPIO_PORT == 'C')
    #define USART6_GPIO_RCC_ID      RCC_AHB1Periph_GPIOC   
    
    #define USART6_TX_GPIO_PORT     GPIOC
    #define USART6_TX_PIN           GPIO_Pin_6
    #define USART6_TX_PINSOURCE     GPIO_PinSource6
    
    #define USART6_RX_GPIO_PORT     GPIOC    
    #define USART6_RX_PIN           GPIO_Pin_7
    #define USART6_RX_PINSOURCE     GPIO_PinSource7

    #define USART6_485_RCC_ID       RCC_AHB1Periph_GPIOC
    #define USART6_485_GPIO_PORT    GPIOC
    #define USART6_485_PIN          GPIO_Pin_5
    #define USART6_485_PINSOURCE    GPIO_PinSource5

#elif(USART6_GPIO_PORT == 'G')
    #define USART6_GPIO_RCC_ID      RCC_AHB1Periph_GPIOG   
    #define USART6_TX_GPIO_PORT     GPIOG
    #define USART6_TX_PIN           GPIO_Pin_14
    #define USART6_TX_PINSOURCE     GPIO_PinSource14
    #define USART6_RX_GPIO_PORT     GPIOG    
    #define USART6_RX_PIN           GPIO_Pin_9
    #define USART6_RX_PINSOURCE     GPIO_PinSource9

#else
    #error "USART6 �������ô���"
#endif



    /******************ģ��2*****************************/

    /******************ģ��3*****************************/

/******************************************************************************
*                      �ⲿȫ�ֱ���(ģ���ڶ��壬ģ����������                 
******************************************************************************/
#ifdef   SENSOR_MODULE
#define  SENSOR_EXT
#else
#define  SENSOR_EXT  extern
#endif


/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
void Usart6_Init(void);
void Sensor_Process(void);


/******************************************************************************
*                      �жϷ���������                                        
******************************************************************************/


#endif    //End of File

