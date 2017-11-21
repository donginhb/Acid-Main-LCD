/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ�                                              
**                                                                            
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : flash.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :�ж��������ã������ļ��е��ж������Ѹ�д��ֻ�轫�弶�жϺ�
                  ��ָ����ӵ����ļ�������ж���������BSP_IntVectTbl���ɡ�
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  FLASH_MODULE
/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
//#include <includes.h>
#include "stm32f4xx.h"

#include "flash.h"
#include "dwin.h"
#include "main.h"
//#include "os.h"

//#include "bsp.h"        //�弶��������
//#include "bsp_mcu.h"    //оƬ��صĺ궨��
//#include "bsp_cfg.h"    //�弶�����Ӳ������

//#include "bsp_led.h"
/******************************************************************************
*                      ���ú�                                                  
******************************************************************************/
    
    /******************ģ��1*****************************/

    /******************ģ��2*****************************/

    /******************ģ��3*****************************/


/******************************************************************************
*                      �ڲ�ȫ�ֱ�������                                        
******************************************************************************/


/******************************************************************************
*                      �ڲ���������                                            
******************************************************************************/
static void Save_Parameter_to_Flash(void);
static void Read_Parameter_from_Flash(void);
/******************************************************************************
*                      �ڲ���������                                                
******************************************************************************/
static void Read_Parameter_from_Flash(void)
{
	__IO FLASH_Status FLASHStatus;
	//ָ��pָ�����1kB flash���׵�ַ
	//512kB flash��ַΪ0x08000000-0x08080000
	//ÿ2kB��0x800��һҳ�����һҳ�ĵ�ַΪ0x08080000-0x800=0x0807F800
	//(0x08080000-0x800);
    uint16_t * p=(uint16_t *)0x080E0000;
	uint16_t * Settings_p=(uint16_t *)(&Dwin_Settings );
	uint16_t i;
	if(*p==0xBAAA)
	{
		p++;
		for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
		{
			*(Settings_p +i)=*(p+i);
		}
		
	}
    else
    {
        Set_Default_Settings();
        Save_Parameter_to_Flash();
    }

}
static void Save_Parameter_to_Flash(void)
{
	__IO FLASH_Status FLASHStatus;
	uint16_t  i;
	uint16_t * Settings_p=(uint16_t *)(&Dwin_Settings);
	
	
    
    FLASHStatus =FLASH_COMPLETE ;
	FLASH_Unlock ();
	FLASH_ClearFlag (FLASH_FLAG_OPERR |
                     FLASH_FLAG_EOP   |
                     FLASH_FLAG_PGAERR|
                     FLASH_FLAG_WRPERR|
                     FLASH_FLAG_PGPERR|
                     FLASH_FLAG_PGSERR );
    //FLASHStatus =FLASH_ErasePage (0x0807f800);
    FLASH_EraseSector(FLASH_Sector_8,VoltageRange_3); //���� 
    //FLASH_ProgramByte(addr, *ptr);//д�� 	
    
	for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
	{
		FLASHStatus =FLASH_ProgramHalfWord (0x080E0000+2+2*i,*(Settings_p+i));
	}
	FLASHStatus =FLASH_ProgramHalfWord (0x080E0000,0xBAAA);	
	FLASH_Lock ();
}








/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
void Flash_Init(void)
{

     Read_Parameter_from_Flash();

}


void Flash_Process(void)
{
    //uint8_t Settings_updated=0;
    dwin_settings_t *p;
    
    if(SysTick_ms_for_Flash==0)
    {
        p=(dwin_settings_t *)0x080E0002;
//        if( p->Ambient_Temp_Plus  != Dwin_Settings .Ambient_Temp_Plus   ||
//            p->Ambient_Temp_Sub   != Dwin_Settings .Ambient_Temp_Sub    ||
//            p->Flue_Temp_Plus     != Dwin_Settings .Flue_Temp_Plus      ||
//            p->Flue_Temp_Sub      != Dwin_Settings .Flue_Temp_Sub       ||
//            p->Sensor_Current_Plus!= Dwin_Settings .Sensor_Current_Plus ||
//            p->Sensor_Current_Sub != Dwin_Settings .Sensor_Current_Sub  ||
//            p->Sensor_Temp_Plus   != Dwin_Settings .Sensor_Temp_Plus    ||
//            p->Sensor_Temp_Sub    != Dwin_Settings .Sensor_Temp_Sub     ||
//            p->Oxygen_Content     != Dwin_Settings .Oxygen_Content      ||
//            p->MMT_Offset         != Dwin_Settings .MMT_Offset          ||
//            p->Water_Content      != Dwin_Settings .Water_Content       ||
//            p->SO3_A   != Dwin_Settings .SO3_A   ||
//            p->SO3_B   != Dwin_Settings .SO3_B   ||
//            p->SO3_C   != Dwin_Settings .SO3_C   ||
//            p->SO3_D   != Dwin_Settings .SO3_D   ||
//            p->H2SO4_A != Dwin_Settings .H2SO4_A ||
//            p->H2SO4_B != Dwin_Settings .H2SO4_B ||
//            p->H2SO4_C != Dwin_Settings .H2SO4_C ||
//            p->H2SO4_D != Dwin_Settings .H2SO4_D )
        {
            Save_Parameter_to_Flash();
            SysTick_ms_for_Flash=2000;
        }
        
    }



}
/******************************************************************************
*                       main( )
* Description : C������ں����������ʼ��uCOS��������ʼ����.
* Argument(s) : ��.
* Return(s)   : ��.
* Caller(s)   : .
* Designed by : YZ
* DATE        : 2017.7.9
* Version     : V2.0
* Note(s)     : .
******************************************************************************/

/******************************************************************************
*                      �жϴ�����                                            
******************************************************************************/





//End of File
