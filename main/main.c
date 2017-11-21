/******************************************************************************
*******************************************************************************
**                      ������                                              
**                                                                            
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : main.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   : ������������M4����
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  MAIN_MODULE
/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
#include "stm32f4xx.h"

//usbЭ��ջ����
#include "..\include\led.h"
#include "..\include\main.h"
#include "..\include\usart.h"
#include "..\fwlib\inc\stm32f4xx_gpio.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//user header


#include "main.h"

#include "dwin.h"
#include "sensor.h"
#include "battery.h"
#include "flash.h"
#include "stm324xg_eval_sdio_sd.h"
#include "ff.h"
#include "diskio.h"
#include "sd_card.h"

/******************************************************************************
*                      ���ú�                                                  
******************************************************************************/
    
    /******************ģ��1*****************************/

    /******************ģ��2*****************************/

    /******************ģ��3*****************************/


/******************************************************************************
*                      �ڲ�ȫ�ֱ�������                                        
******************************************************************************/
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;         //��ʱ����
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;     //��ʱ����





/******************************************************************************
*                      �ڲ���������                                            
******************************************************************************/
static void Init_Systick(void);

/******************************************************************************
*                      �ڲ���������                                                
******************************************************************************/
static void Init_Systick(void)
{
    while(SysTick_Config(SystemCoreClock /1000)!=0); //1ms��ʱ
}




/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/


/******************************************************************************
*                       main( )                                                
* Description : C������ں���                              
* Argument(s) : ��.                                        
* Return(s)   : ��.                                        
* Caller(s)   : .                                          
* Designed by : YZ                                         
* DATE        : 2017.7.9                                   
* Version     : V2.0                                       
* Note(s)     : .                                          
******************************************************************************/


//extern SD_CardInfo SDCardInfo;

int main(void)
{
    int i;
	//Flash_Init();
    #define Major_Version 2017
    #define Minor_Version 11
    #define Software_Version  10    
    //Dwin_Measurements .Version [0]=Major_Version;
    //Dwin_Measurements .Version [1]=Minor_Version;
    Dwin_Measurements .Version [0]=Software_Version;
    
    led.initialize();
    Init_Systick();
    SD_Init();
    for(i = 0;i < 1000000;i++);
    //sd_test();
    SD_Get_FreeCap();    
   	

    SD_Read_Default_Settings();  //����������׼��
    SD_Load_Settings();          //����������׼��
    
    //SD_Delete_Data()   ;
    Usart3_Init();     //����������
	usart1.initialize(115200);
    

	//usb��ʼ��
	USBH_Init(&USB_OTG_Core, 
					USB_OTG_HS_CORE_ID,
					&USB_Host,
					&USBH_MSC_cb, 
					&USR_cb);
	Usart6_Init();     //����������
    I2C1_Init ();      //��ؼ��


	usart1.initialize(115200);

//    usart1.printf("\x0c");                                   //����	
//    usart1.printf("\033[1;32;40m");                          //���������ն�Ϊ��ɫ
//    usart1.printf("\r\n\r\nhello! I am iCore3!\r\n\r\n\r\n");
	

	//usb�����û���غ�����usbh_usr.c�ļ��У��ļ���д��USBH_USR_MSC_Application()������
	//dwin_measurements .Uab =0;
    while(1)
    {
//		measurement .Uab++;
//        Dwin_Write_RAM((uint16_t)  APP_DWIN_RAM_ADDRESS_OF_MEASUREMENTS,   //����RAM��ַ
//                       (uint16_t *)&measurement,                 //����
//                       (uint8_t )  APP_DWIN_RAM_NUM_OF_MEASUREMENTS);      //����        
        Dwin_Process();
        Sensor_Process ();
        Battery_Process ();
        SD_Process();
        USBH_Process(&USB_OTG_Core, &USB_Host);
        //Flash_Process();
        led_shining();
	}


}

void led_shining(void)
{
    static uint32_t last_time=0;
    if(SysTick_ms -last_time <1000) LED1_ON ;
    else if(SysTick_ms -last_time <2000 )LED1_OFF ;
    else  last_time=SysTick_ms;        
}


void SysTick_Timing(void)
{
    SysTick_ms++;
    if(SysTick_ms_for_Usart3   !=0) SysTick_ms_for_Usart3--;
    if(SysTick_ms_for_Dwin     !=0) SysTick_ms_for_Dwin--;
    if(SysTick_ms_for_Battery  !=0) SysTick_ms_for_Battery--;
    if(SysTick_ms_for_Flash    !=0) SysTick_ms_for_Flash--;
    
    
    
    
}


















//End of File
