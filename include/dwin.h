/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ���ͷ�ļ���                                    
**                                                                             
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : dwin.h  
** Version       : V1.00      
** Programmer(s) : YZ         
** Date          : 2017.7.2   
** Discription   : 
** Note(s)		 :            
*******************************************************************************
******************************************************************************/
#ifndef _DWIN_H_     //��ֹ���ذ���
#define _DWIN_H_

/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
#include "stm32f4xx.h"

/******************************************************************************
*                      ��������                                                
******************************************************************************/
//Dwin����ʾ���ݣ����ڵ�ѹ�����������ʵȲ������ݵĴ洢��32����
typedef struct 
{
    uint16_t Battery_Icon;       //0-6
    uint16_t Current_Icon;       //0-100
    uint16_t Sensor_TempIcon;    //100-300
    uint16_t Flue_Temp;          //0-600
    
    uint16_t ADT;                //��¶�㣬(��λ0.1��)[0,3000]
    uint16_t Sensor_Current;     //0-1500
    uint16_t Sensor_Temp;        //0-300
    uint16_t Ambient_Temp;       //LM35,0-70
    
    uint16_t SO3_Concentration;
    uint16_t H2SO4_Concentration;
    uint16_t Card_Space;         //MB
    uint16_t MMT;

    uint16_t U_Disk_Connected;
    uint16_t Version[2];
    uint16_t reserved[1];   
    
}dwin_measurements_t;


//Dwin��ָ�����ݣ�������ͣ�����桢�Լ��ָ��Ĵ洢��16����
typedef struct 
{
	uint16_t Reserved0;
    uint16_t Save_to_Disk;
	uint16_t Clear_Data;
	uint16_t Save_to_Card;
	uint16_t Password;
    uint16_t Restore_Default_Settings;
    uint16_t Save_Settings;
    uint16_t Delete_Data;
    //uint16_t reserved;
}dwin_commands_t;

//Dwin���������ݣ������û����ò����Ĵ洢��32����
typedef struct 
{
	uint16_t Oxygen_Content;
    uint16_t Product_Num;
    uint16_t Operator_Num;
    uint16_t MMT_Offset;
    
    uint16_t Water_Content;
    uint16_t Sensor_Temp_Plus;
    uint16_t Sensor_Temp_Sub;
    uint16_t Flue_Temp_Plus;
    
    uint16_t Flue_Temp_Sub;
    uint16_t Ambient_Temp_Plus;
    uint16_t Ambient_Temp_Sub;
    uint16_t Sensor_Current_Plus;
    
    uint16_t Sensor_Current_Sub;
    uint16_t SO3_Plus;
    uint16_t SO3_Sub;
    uint16_t H2SO4_Plus;
    uint16_t H2SO4_Sub;
//    uint16_t SO3_A;           //�б�Ҫ�ĳ��з�����
//    uint16_t SO3_B;
//    uint16_t SO3_C;
//    uint16_t SO3_D;
//    uint16_t H2SO4_A;
//    uint16_t H2SO4_B;
//    uint16_t H2SO4_C;
//    uint16_t H2SO4_D;
    uint16_t reserved[7];
   
}dwin_settings_t;


typedef struct
{
    uint8_t YY;   //BCD��,���Ե�����
    uint8_t MM;   //BCD��,���Ե�����
    uint8_t DD;   //BCD��,���Ե�����
    uint8_t WW;   //BCD��,���Ե�����
    uint8_t hh;   //BCD��,���Ե�����
    uint8_t mm;   //BCD��,���Ե�����
    uint8_t ss;   //BCD��,���Ե�����
    uint8_t date[20];    //ת�룬�ַ�����yyyy_mm_dd_hh_mm_ss��
    
}dwin_rtc_t;

//������ͨ�Ŵ�������
typedef enum
{
    dwin_err_none=0,
    dwin_err_no_answer,
    dwin_err_header,
    dwin_err_crc,
    dwin_err_cmd,
    dwin_err_auto_answer,
    dwin_err_unknown,
    dwin_err_uncompeleted
}dwin_err_t;


//����״̬��
typedef enum      
{
    dwin_usart_state_idle,
    dwin_usart_state_communication,
    dwin_usart_state_completed,
    dwin_usart_state_decode,
    dwin_usart_state_timeout
    
}dwin_uart_state_t;

//����ָ��״̬
typedef enum      
{
    dwin_instruction_state_idle,          //����
    dwin_instruction_state_ready,
    dwin_instruction_state_communication, //Ӳ��ͨ��
    dwin_instruction_state_decode,        //����
    dwin_instruction_state_timeout,       //ͨ�ų�ʱ
    dwin_instruction_state_err,       //ͨ�Ŵ���
    dwin_instruction_state_fail,
    dwin_instruction_state_completed      //���
    
}dwin_instruction_state_t;

//������ͨ��״̬��
typedef enum
{
    dwin_app_state_idle,
    dwin_app_state_delay0,
    dwin_app_state_init_settings,
    dwin_app_state_get_settings,
    dwin_app_state_get_commands,
    dwin_app_state_reset_commands,
    dwin_app_state_handle_commands, //����ͨ�ţ��ҹ�Ϊ������ģ��������   
    dwin_app_state_switch_page,     //��Ҫͨ�ŵ������
    dwin_app_state_get_time,
    dwin_app_state_cal_measurements,
    dwin_app_state_update_measurements,
    dwin_app_state_draw_trendline,
    dwin_app_state_delay
    

    
}dwin_app_state_t;



/******************************************************************************
*                      ������ú�                                              
******************************************************************************/
#define APP_DWIN_FRAME_HEAD0        0X5A                //ͨ��֡ͷ��Ҳ������Ϊ�����ַ
#define APP_DWIN_FRAME_HEAD1        0XA5                //ͨ��֡ͷ��Ҳ������Ϊ�����ַ
#define APP_DWIN_CMD_WRITE_REG      0X80                //�������Ĵ���дָ��
#define APP_DWIN_CMD_READ_REG       0X81                //�������Ĵ�����ָ��
#define APP_DWIN_CMD_WRITE_RAM      0X82                //����������дָ��
#define APP_DWIN_CMD_READ_RAM       0X83                //������������ָ��
#define APP_DWIN_CMD_WRITE_BUFF     0X84                //���������߻���дָ��

#define APP_DWIN_RAM_ADDRESS_OF_MEASUREMENTS    0X0010  //���������ݷ�����ַ���ο���������ƣ�
#define APP_DWIN_RAM_ADDRESS_OF_COMMANDS        0X0000  //���������ݷ�����ַ���ο���������ƣ�
#define APP_DWIN_RAM_ADDRESS_OF_SETTINGS        0X0020  //���������ݷ�����ַ���ο���������ƣ�

#define APP_DWIN_RAM_ADDRESS_OF_TIPS            0X0100  //��������ʾ�ַ�����ַ

#define APP_DWIN_RAM_NUM_OF_MEASUREMENTS    (sizeof(dwin_measurements_t)/2) //������������
#define APP_DWIN_RAM_NUM_OF_COMMANDS        (sizeof(dwin_commands_t)/2)     //������������
#define APP_DWIN_RAM_NUM_OF_SETTINGS        (sizeof(dwin_settings_t)/2)     //������������
#define APP_DWIN_RAM_NUM_OF_TIPS             0x20                           //�ݶ���δʹ��
#define APP_DWIN_TRENDLINE_BUFF_CHS          0x03                           //0��1ͨ��


#define DWIN_Sys_Settings_Page        15      //ϵͳ����ҳ��
#define DWIN_Sys_Settings_Password    1111    //ϵͳ����ҳ������
#define DWIN_USB_Saving_Page          28      //U�̱���ҳ��
#define DWIN_USB_Save_Compeleted_Page 29      //U�̱������ҳ��
#define DWIN_USB_Save_Err_Page        30      //U�̱���ʧ��ҳ��


/******************************************************************************
*                      Ӳ�����ú�                                              
******************************************************************************/
    /******************USART1*****************************/

    /******************USART2*****************************/

    /******************USART3*****************************/
#define USART3_BAUND_RATE               115200
#define USART3_GPIO_PORT                'D'

#if(USART3_GPIO_PORT == 'B')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOB   
    #define USART3_TX_GPIO_PORT     GPIOB
    #define USART3_TX_PIN           GPIO_Pin_10
    #define USART3_TX_PINSOURCE     GPIO_PinSource10
    #define USART3_RX_GPIO_PORT     GPIOB    
    #define USART3_RX_PIN           GPIO_Pin_11
    #define USART3_RX_PINSOURCE     GPIO_PinSource11
#elif(USART3_GPIO_PORT == 'C')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOC  
    #define USART3_TX_GPIO_PORT     GPIOC
    #define USART3_TX_PIN           GPIO_Pin_10
    #define USART3_TX_PINSOURCE     GPIO_PinSource10
    #define USART3_RX_GPIO_PORT     GPIOC    
    #define USART3_RX_PIN           GPIO_Pin_11
    #define USART3_RX_PINSOURCE     GPIO_PinSource11
#elif(USART3_GPIO_PORT == 'D')
    #define USART3_GPIO_RCC_ID      RCC_AHB1Periph_GPIOD  
    #define USART3_TX_GPIO_PORT     GPIOD
    #define USART3_TX_PIN           GPIO_Pin_8
    #define USART3_TX_PINSOURCE     GPIO_PinSource8
    #define USART3_RX_GPIO_PORT     GPIOD    
    #define USART3_RX_PIN           GPIO_Pin_9
    #define USART3_RX_PINSOURCE     GPIO_PinSource9        
#else
    #error "USART3 �������ô���"
#endif

    /******************USART4*****************************/

    /******************USART5*****************************/

    /******************USART6*****************************/

/******************************************************************************
*                      �ⲿȫ�ֱ���(ģ���ڶ��壬ģ����������                 
******************************************************************************/
#ifdef   DWIN_MODULE
#define  DWIN_EXT
#else
#define  DWIN_EXT  extern
#endif


DWIN_EXT dwin_measurements_t    Dwin_Measurements;      //
DWIN_EXT dwin_commands_t        Dwin_Commands;          //��Dwin�����յ����ݣ�������λ���ش�
DWIN_EXT dwin_commands_t        Dwin_PostedCommands;  //������ģ�鷢�������
DWIN_EXT dwin_settings_t        Dwin_Settings;     //��ʼ������֮��ֻд
DWIN_EXT dwin_settings_t        Dwin_Default_Settings;     //�����������ϵ�ʱ��SD����ȡ
DWIN_EXT dwin_rtc_t             Dwin_RTC;
DWIN_EXT uint16_t               Dwin_Page_to_Switch;

//DWIN_EXT uint8_t                flag_dwin_commands_excuted; //����ִ����ϱ�־
//DWIN_EXT uint8_t                flag_dwin_commands_updated; //����ִ����ϱ�־



DWIN_EXT uint16_t   Dwin_TrendLine_Buff[100] ;
DWIN_EXT uint8_t    Dwin_TrendLine_Points_Num;
/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
void        Usart3_Init(void);
void        Dwin_Process(void); //�ṩ���������Ľӿ�
uint16_t    Get_CRC16(__IO uint8_t *p_frame,uint8_t len_in_byte);
void        Dwin_AddPoint(uint16_t data1,uint16_t data2);
//void        Set_Default_Settings(void);

/******************************************************************************
*                      �жϷ���������                                        
******************************************************************************/


#endif    //End of File


