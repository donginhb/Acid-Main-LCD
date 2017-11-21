/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ�                                              
**                                                                            
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : sensor.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  SENSOR_MODULE
/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
#include "stm32f4xx.h"
#include "sensor.h"

#include "dwin.h"
#include "math.h"


/******************************************************************************
*                      ���ú�                                                  
******************************************************************************/
    
    /******************ģ��1*****************************/

    /******************ģ��2*****************************/

    /******************ģ��3*****************************/


/******************************************************************************
*                      �ڲ�ȫ�ֱ�������                                        
******************************************************************************/
static uint8_t Usart6_RBuff[USART6_RBUFF_SIZE];
static uint8_t Usart6_RBuff_RPointer;
static uint8_t Usart6_RBuff_WPointer;

/******************************************************************************
*                      �ڲ���������                                            
******************************************************************************/
static void Init_Usart6_GPIO( void );
static void Init_Usart6_NVIC( void );
static void Init_Usart6( void );
static void Start_Usart6_Receive(void) ;

static void Write_Usart6_RBuff(uint8_t data);
static uint8_t Read_Usart6_RBuff(void);

/******************************************************************************
*                      �ڲ���������                                                
******************************************************************************/
//��ʼ����������
static void Init_Usart6_GPIO( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //ʱ��ʹ��
    RCC_AHB1PeriphClockCmd (USART6_GPIO_RCC_ID ,ENABLE );
    RCC_AHB1PeriphClockCmd (USART6_485_RCC_ID ,ENABLE );
    
    //��������      
    GPIO_InitStructure.GPIO_Pin     =USART6_TX_PIN ;		
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //���롢�����ģ�⡢����
    GPIO_InitStructure.GPIO_Speed   =GPIO_Speed_2MHz; //2\25\50\100
    GPIO_InitStructure.GPIO_OType   =GPIO_OType_PP ; //����\��©
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//����\����\����
    GPIO_Init(USART6_TX_GPIO_PORT ,&GPIO_InitStructure);        
    
    GPIO_InitStructure.GPIO_Pin     =USART6_RX_PIN ;
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //���롢�����ģ�⡢����
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//����\����\����
    GPIO_Init(USART6_RX_GPIO_PORT ,&GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin     =USART6_485_PIN ;		
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_OUT;   //���롢�����ģ�⡢����
    GPIO_InitStructure.GPIO_Speed   =GPIO_Speed_2MHz; //2\25\50\100
    GPIO_InitStructure.GPIO_OType   =GPIO_OType_PP ; //����\��©
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_DOWN ;//����\����\����
    GPIO_Init(USART6_TX_GPIO_PORT ,&GPIO_InitStructure);        
    GPIO_ResetBits (USART6_485_GPIO_PORT,USART6_485_PIN );//����״̬
    
    //����ӳ��
    GPIO_PinAFConfig (USART6_TX_GPIO_PORT ,USART6_TX_PINSOURCE ,GPIO_AF_USART6 );
    GPIO_PinAFConfig (USART6_RX_GPIO_PORT ,USART6_RX_PINSOURCE ,GPIO_AF_USART6 );
}
//��ʼ�������ж����ȼ�
static void Init_Usart6_NVIC( void )
{
    NVIC_InitTypeDef 	NVIC_InitStructure;	
    
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	        
}
//��ʼ������
static void Init_Usart6( void )
{
    //�����ʼ��ʱ�õ��Ľṹ�����
	USART_InitTypeDef 			USART_InitStructure;
    //ʱ��ʹ��
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART6,ENABLE ); 

    //���������⣬������������ȱʡ��8λ���ݣ�1λֹͣ����У�飬��Ӳ��������
    USART_InitStructure.USART_BaudRate=USART6_BAUND_RATE;					//���ڲ������ã�9600,8,1,0
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode=USART_Mode_Rx;	//ʹ�ܽ���ģʽ
    
    USART_Init(USART6,&USART_InitStructure);

 	//�����ж�ʹ��
 	//USART_ITConfig(USART6,USART_IT_TC,ENABLE);			//��������ж�    
    //USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);         //�����ж�
    //USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);       //��������жϣ�ʹ��DMA�󽫱�����
    //USART_ITConfig(USART6,USART_IT_TXE,ENABLE);        //���ͻ�����жϣ�ʹ��DMA�󽫱�����

    //USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);  //USART����DMAʹ�ܣ�ʹ��֮�󽫲��ٲ������ͻ�����ж�               
    //USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);  //USART����DMAʹ�ܣ�ʹ��֮�󽫲��ٲ�����������ж�    
    
    //ʹ�ܴ���
    USART_Cmd(USART6,ENABLE);    
}
//�������ڽ���
static void Start_Usart6_Receive(void)
{
    Usart6_RBuff_WPointer =0;
    Usart6_RBuff_RPointer =0;
    //USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);         //�����ж�
    USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);       //��������жϣ�ʹ��DMA�󽫱�����    
}

//д���ڽ��ջ���
static void Write_Usart6_RBuff(uint8_t data)
{
    Usart6_RBuff[Usart6_RBuff_WPointer]=data;
    if(Usart6_RBuff_WPointer==USART6_RBUFF_SIZE -1)
        Usart6_RBuff_WPointer=0;
    else
        Usart6_RBuff_WPointer++;
}
//�����ڽ��ջ���
static uint8_t Read_Usart6_RBuff(void)
{
    uint8_t data;
    data=Usart6_RBuff[Usart6_RBuff_RPointer];
    if(Usart6_RBuff_RPointer==USART6_RBUFF_SIZE -1)
        Usart6_RBuff_RPointer=0;
    else
        Usart6_RBuff_RPointer++;
    return data;
}

/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
//USART6ģ���ʼ��
void Usart6_Init(void)
{
    Init_Usart6_GPIO();
	Init_Usart6_NVIC();
    //Init_Usart3_DMA(); 	    
    Init_Usart6();
    Start_Usart6_Receive();
}


//������ͨ���ܹ��ܺ���

float    gAverage_Current;
uint16_t gStable_Time;
void Sensor_Process(void)
{
    #define SENSOR_FRAME_LEN  ((uint8_t)19)
    #define SENSOR_FRAME_HEAD '$'
    #define CURRENT_THRESHOLD  50
    #define STABLE_THRESHOLD   5
    
    __align(4) static uint8_t frame[SENSOR_FRAME_LEN];  
    static uint8_t received_bytes=0;
    
    static float    Average_Current=0;
    //static float    Current=0;
    static uint16_t Stable_Time=0;
    static uint8_t ADT_is_Found=0;
    
    uint8_t i;
    uint16_t crc;
    sensor_data_t* pSensor;
    while(Usart6_RBuff_RPointer != Usart6_RBuff_WPointer) //����������
    {
        frame[received_bytes ++]=Read_Usart6_RBuff();
        if(received_bytes==SENSOR_FRAME_LEN)  //��һ֡����
        {
            if(frame[0]==SENSOR_FRAME_HEAD)   //��֡ͷ
            {
                crc=Get_CRC16 (frame,SENSOR_FRAME_LEN-2);
                if(frame[SENSOR_FRAME_LEN-2]==(crc&0x00FF) &&
                   frame[SENSOR_FRAME_LEN-1]==((crc>>8)&0x00FF) ) //CRC��ȷ
                {
                    //ȥ֡ͷ����
                    for(i=1;i<SENSOR_FRAME_LEN ;i++)
                    {
                        frame[i-1]=frame[i];
                    }
                    pSensor=(sensor_data_t*)frame;
                    
//                    if(Dwin_Measurements .Sensor_Current>CURRENT_THRESHOLD &&
//                        pSensor ->Sensor_Current< CURRENT_THRESHOLD)
//                    {
//                        Dwin_Measurements.ADT= pSensor ->Sensor_Temp*10;
//                    }
                    //����dwin��������ʾ
                    Dwin_Measurements .Sensor_Current=pSensor ->Sensor_Current;
                    Dwin_Measurements .Sensor_Temp =pSensor ->Sensor_Temp ;
                    Dwin_Measurements .Ambient_Temp =pSensor ->Ambient_Temp ;
                    //Dwin_Measurements .Ambient_Temp/=10;
                    Dwin_Measurements .Flue_Temp =pSensor ->Flue_Temp ;
                    
                    Dwin_Measurements .Sensor_Current+=(Dwin_Settings .Sensor_Current_Plus/10) ;
                    Dwin_Measurements .Sensor_Current-=(Dwin_Settings .Sensor_Current_Sub/10) ;
                    Dwin_Measurements .Sensor_Temp+=(Dwin_Settings .Sensor_Temp_Plus/10) ;
                    Dwin_Measurements .Sensor_Temp-=(Dwin_Settings .Sensor_Temp_Sub/10) ;
                    Dwin_Measurements .Ambient_Temp+=(Dwin_Settings .Ambient_Temp_Plus) ;
                    Dwin_Measurements .Ambient_Temp-=(Dwin_Settings .Ambient_Temp_Sub );
                    Dwin_Measurements .Flue_Temp+=(Dwin_Settings .Flue_Temp_Plus/10) ;
                    Dwin_Measurements .Flue_Temp-=(Dwin_Settings .Flue_Temp_Sub /10);
                    
                    //�޷�
                    if(Dwin_Measurements .Sensor_Current>32767)
                    {
                        Dwin_Measurements .Sensor_Current=0;
                    }
                    else if(Dwin_Measurements .Sensor_Current>1500)
                    {
                        Dwin_Measurements .Sensor_Current=1500;
                    }
                    //�޷�
                    if( Dwin_Measurements .Sensor_Temp>32767)
                    {
                        Dwin_Measurements .Sensor_Temp=0;
                    }    
                    else if(Dwin_Measurements .Sensor_Temp>600)
                    {
                        Dwin_Measurements .Sensor_Temp=600;
                    }
                    //�޷�
                    if( Dwin_Measurements .Flue_Temp>32767)
                    {
                        Dwin_Measurements .Flue_Temp=0;
                    }    
                    else if(Dwin_Measurements .Flue_Temp>600)
                    {
                        Dwin_Measurements .Flue_Temp=600;
                    }
                    //������ͼ
                    Dwin_AddPoint(Dwin_Measurements .Sensor_Current ,Dwin_Measurements .Sensor_Temp );
                    
                    //������¶��
                    if(ADT_is_Found ==0)
                    {
                        Average_Current *= 0.8;
                        Average_Current += Dwin_Measurements .Sensor_Current*0.2;
                        gAverage_Current=Average_Current;
                        if( Average_Current> CURRENT_THRESHOLD &&
                            Dwin_Measurements .Sensor_Current< Average_Current+ STABLE_THRESHOLD &&
                            Dwin_Measurements .Sensor_Current> Average_Current- STABLE_THRESHOLD )
                        {
                            Stable_Time++;
                            gStable_Time=Stable_Time;
                            if(Stable_Time >5)
                            {
                                ADT_is_Found =1;
                                Dwin_Measurements .ADT = Dwin_Measurements .Sensor_Temp*10;
                                //Dwin_Measurements .MMT = Dwin_Measurements .ADT +Dwin_Settings .MMT_Offset ;
                                Stable_Time=0;
                                Average_Current=0;
                            }
                        }
                        else
                        {
                            Stable_Time =0;
                        }                        
                    }
                    received_bytes=0;
                }
                else
                {
                    for(i=1;i<SENSOR_FRAME_LEN ;i++)
                    {
                        frame[i-1]=frame[i];
                    }
                    received_bytes --;                               
                }
            }
            else
            {
                for(i=1;i<SENSOR_FRAME_LEN ;i++)
                {
                    frame[i-1]=frame[i];
                }
                received_bytes --;
            }
        }
    }
    

    
    if(ADT_is_Found==1)
    {
        float t,x;
        float so3,h2so4;
        float a,b,c,d; //���ϵ��        
        //������������Ũ��
        t=Dwin_Measurements .ADT; //��λ0.1��
        t/=10;    //��λ1��
        t-=100;   //ƽ�Ƶ��������

        a=0.0003704;
        b=0.005309;
        c=0.006593;
        d=-0.006;
        
        so3=a*t*t*t+b*t*t+c*t-d;
        //so3�ֶ�����
        so3+=(Dwin_Settings .SO3_Plus/10);
        so3-=(Dwin_Settings .SO3_Sub/10) ;
        
        Dwin_Measurements .SO3_Concentration =so3*10;
        
        
        //��������Ũ��
        t=Dwin_Measurements .ADT; //��λ0.1��
        t/=10;    //��λ1��
        x=Dwin_Settings .Water_Content ;
        x/=10;
        
//һ����ϵ��        
//y1=0.00095784*x+0.036123;
//%������
//y2=0.18864*x+3.49; 
        a=0.00095784*x+0.036123;
        b=0.18864*x+3.49;
        
        //h2so4=(t-117)/22;
        h2so4=a*t-b;
        h2so4=pow(10,h2so4);
        //h2so4=0;
        h2so4 +=(Dwin_Settings .H2SO4_Plus/10) ;
        h2so4-=(Dwin_Settings .H2SO4_Sub /10);
        
        Dwin_Measurements.H2SO4_Concentration =h2so4*10;
        
        //add code here
        
        

        
        a=
        
        
        //����MMT
        Dwin_Measurements.MMT =Dwin_Measurements .ADT+Dwin_Settings .MMT_Offset ;
        
        
    }
    
    
    
    
    if(Dwin_PostedCommands.Clear_Data ==1)
    {
        Dwin_Measurements .ADT =0;
        Dwin_Measurements .MMT =0;
        Dwin_Measurements .SO3_Concentration =0;
        Dwin_Measurements .H2SO4_Concentration =0;
        ADT_is_Found=0;
        Dwin_PostedCommands .Clear_Data =0;    
    }
    
    
    
}



/******************************************************************************
*                     �жϴ�����
******************************************************************************/
void USART6_IRQHandler(void)
{
    uint8_t data;
    //�ڴ��ڽ�������ж���
    if((USART6->SR)&USART_SR_RXNE)
    {
        data=USART6->DR ; 
        Write_Usart6_RBuff (data);
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



//End of File
