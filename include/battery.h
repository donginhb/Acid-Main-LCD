/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ���ͷ�ļ���                                    
**                                                                             
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : battery.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : ��
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef BATTERY_H_     //��ֹ���ذ���
#define BATTERY_H_

/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
#include "stm32f4xx.h"


/******************************************************************************
*                      ��������                                                
******************************************************************************/
typedef struct 
{
    volatile uint8_t    busy;    //���б�־  0)����   1)æµ
    uint8_t             r_w;              //��д��־  0)д     1)��
    uint8_t             id;               //�ӻ��豸ID��
    uint8_t             addr;             //Ҫ��д�ĵ�ַ
    volatile uint16_t   index;  //��ǰ���������ݳ���
    uint16_t            bufLen;          //Ҫ���ͻ���յ����ݳ���   
    uint8_t * volatile  pBuff; //�������׵�ַ    
    void (* volatile FunCallBack)(void);//�жϻص�����
}I2C_ParamTypeDef;

typedef enum
{
    TL2943_state_set_delay0,
    TL2943_state_delay0,
    TL2943_state_init,
    TL2943_state_delay1,
    TL2943_state_get_current,
    TL2943_state_delay2,
    TL2943_state_get_voltage,
    TL2943_state_delay3,
    TL2943_state_set_charge,
    TL2943_state_delay4,
    
    TL2943_state_get_charge,
    TL2943_state_delay5
}TL2943_state_t;

typedef struct
{
    uint16_t V;    //��λmV
    int16_t  A;    //��λmA
    uint16_t C;    //�ٷֱ�

    
}battery_t;
/******************************************************************************
*                      ������ú�                                              
******************************************************************************/


/******************************************************************************
*                      Ӳ�����ú�                                              
******************************************************************************/
    /******************ģ��1*****************************/
#define TL2943_ADC_MODE_AUTO    0xC0    //����
#define TL2943_ADC_MODE_SCAN    0x80    //���10s
#define TL2943_ADC_MODE_MANUAL  0x40    //�ֶ���������ɺ�����
#define TL2943_ADC_MODE_SLEEP   0x00    //����


#define TL2943_COULOMB_PRESCALER_1      0x00
#define TL2943_COULOMB_PRESCALER_4      0x08
#define TL2943_COULOMB_PRESCALER_16     0x10
#define TL2943_COULOMB_PRESCALER_64     0x18
#define TL2943_COULOMB_PRESCALER_256    0x20
#define TL2943_COULOMB_PRESCALER_1024   0x28
#define TL2943_COULOMB_PRESCALER_4096   0x30

#define TL2943_ALCC_MODE_ALERT              0x04
#define TL2943_ALCC_MODE_CHARGE_COMPLETE    0x02
#define TL2943_ALCC_MODE_DISABLE            0x00

#define TL2943_ANALOG_ON   0x00
#define TL2943_ANALOG_OFF  0x01

#define TL2943_REG_STATE                0x00
#define TL2943_REG_CONTROL              0x01
#define TL2943_REG_CHARGE               0x02
#define TL2943_REG_CHARGE_THRESHOLD_HI  0x04
#define TL2943_REG_CHARGE_THRESHOLD_LO  0x06

#define TL2943_REG_VOLTAGE              0x08
#define TL2943_REG_VOLTAGE_THRESHOLD_HI 0x0A
#define TL2943_REG_VOLTAGE_THRESHOLD_LO 0x0C

#define TL2943_REG_CURRENT              0x0E
#define TL2943_REG_CURRENT_THRESHOLD_HI 0x10
#define TL2943_REG_CURRENT_THRESHOLD_LO 0x12

#define TL2943_REG_TEMP                 0x14
#define TL2943_REG_TEMP_THRESHOLD_HI    0x16
#define TL2943_REG_TEMP_THRESHOLD_LO    0x17

#define TL2943_SENSOR_RESISTANCE        0.025  
#define TL2943_ADC_VOLTAGE_RANGE        23.6
#define TL2943_ADC_CURRENT_RANGE        (0.12/TL2943_SENSOR_RESISTANCE)
 
#define TL2943_ADC_PER_VOLT             (65535.0/23.6)

#define TL2943_ADC_PER_AMP              (65535.0/0.12*TL2943_SENSOR_RESISTANCE)
#define TL2943_ADC_PER_mAH

//ע��18650��������9.0Wh����2500mAh,���ڲ���7500mAh��
//    ������������25��ŷ����ŵ����Ƶ���2A��
//    ��ɻ���ϵ��4096��1LSB=0.68mAh,����������Ӧ���׼���11029��
//    ��������˥����������ʱ��Ϊ10000��

    /******************ģ��2*****************************/

    /******************ģ��3*****************************/

/******************************************************************************
*                      �ⲿȫ�ֱ���(ģ���ڶ��壬ģ����������                 
******************************************************************************/
#ifdef   BATTERY_MODULE
#define  BATTERY_EXT
#else
#define  BATTERY_EXT  extern
#endif

BATTERY_EXT I2C_ParamTypeDef I2C1_ParamStructure;
BATTERY_EXT uint8_t I2C1_Buff[10];
BATTERY_EXT battery_t Battery;

/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
void I2C1_Init(void);
void Battery_Process (void);

/******************************************************************************
*                      �жϷ���������                                        
******************************************************************************/
void I2C1_EV_IRQHandler(void);

#endif    //End of File

