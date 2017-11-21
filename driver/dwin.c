/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ�                                              
**                                                                            
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : dwin.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   : USART3���ü�������ͨ��
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  DWIN_MODULE

/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
//#include <includes.h>
#include "stm32f4xx.h"

#include "main.h"
#include "dwin.h"
#include "battery.h"

/******************************************************************************
*                      ���ú�                                                  
******************************************************************************/
    
    /******************ģ��1*****************************/

    /******************ģ��2*****************************/

    /******************ģ��3*****************************/

/******************************************************************************
*                      �ڲ�ȫ�ֱ�������                                        
******************************************************************************/
#define USART3_TBUFF_SIZE               100        
#define USART3_RBUFF_SIZE               100  

static uint8_t Usart3_TBuff[USART3_TBUFF_SIZE];
static uint8_t Usart3_RBuff[USART3_RBUFF_SIZE];
static uint8_t Usart3_Tx_Counter;
//static uint8_t Usart3_Rx_Counter;
static uint8_t Usart3_Tx_Pointer;
static uint8_t Usart3_Rx_Pointer;
static uint8_t Usart3_ReceiveCompleted=0;


/******************************************************************************
*                      �ڲ���������                                            
******************************************************************************/
static void Init_Usart3_GPIO( void );
static void Init_Usart3_NVIC( void );
static void Init_Usart3     ( void );

static void Start_Usart3_Receive (void);
static void Start_Usart3_Transmit(uint16_t num);

static uint8_t Fill_Buff_for_WriteDwinREG8 (uint8_t REG_Address,uint8_t  data);
static uint8_t Fill_Buff_for_WriteDwinREG16(uint8_t REG_Address,uint16_t data);
static uint8_t Fill_Buff_for_WriteDwinRAM (uint16_t RAM_Address,uint16_t *p_data,uint8_t num);
static uint8_t Fill_Buff_for_WriteDwinBuff( uint8_t Buff_CH,    uint16_t *p_data,uint8_t num);
static uint8_t Fill_Buff_for_ReadDwinREG   (uint8_t REG_Address,uint8_t  *p_data,uint8_t num);
static uint8_t Fill_Buff_for_ReadDwinRAM  (uint16_t RAM_Address,uint16_t *p_data,uint8_t num);

static uint8_t Is_AutoAnswer_Right(uint8_t CMD);
static uint8_t Decode_DwinREG(uint8_t  * p_data);
static uint8_t Decode_DwinRAM(uint16_t * p_data);

static dwin_instruction_state_t  Write_Dwin_REG8 (uint8_t REG_Address,uint8_t  data);
static dwin_instruction_state_t  Write_Dwin_REG16(uint8_t REG_Address,uint16_t data);
static dwin_instruction_state_t  Write_Dwin_RAM (uint16_t RAM_Address,uint16_t *p_data,uint8_t num);
static dwin_instruction_state_t  Write_Dwin_Buff (uint8_t Buff_CH,    uint16_t *p_data,uint8_t num);
static dwin_instruction_state_t  Read_Dwin_REG   (uint8_t REG_Address,uint8_t  *p_data,uint8_t num);
static dwin_instruction_state_t  Read_Dwin_RAM  (uint16_t RAM_Address,uint16_t *p_data,uint8_t num);

static dwin_instruction_state_t Uptade_Measurements (void);
static dwin_instruction_state_t Get_Commands        (void);
static dwin_instruction_state_t Reset_Commands      (void);
static dwin_instruction_state_t Init_Settings       (void);
static dwin_instruction_state_t Get_Settings        (void);
static dwin_instruction_state_t Switch_Page         (uint16_t  PageNum);
static dwin_instruction_state_t Get_System_Time     (void);
static dwin_instruction_state_t Draw_Lines          (void);
static void Cal_Dwin_Display(void);
static void Post_Commands   (void);


static void Handle_NonComm_Commands(void);

/******************************************************************************
*                      �ڲ���������                                                
******************************************************************************/
//��ʼ����������
static void Init_Usart3_GPIO( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //ʱ��ʹ��
    RCC_AHB1PeriphClockCmd (USART3_GPIO_RCC_ID ,ENABLE );
    
    //��������    
    GPIO_InitStructure.GPIO_Pin     =USART3_TX_PIN ;		
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //���롢�����ģ�⡢����
    GPIO_InitStructure.GPIO_Speed   =GPIO_Speed_2MHz; //2\25\50\100
    GPIO_InitStructure.GPIO_OType   =GPIO_OType_PP ; //����\��©
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//����\����\����
    GPIO_Init(USART3_TX_GPIO_PORT ,&GPIO_InitStructure);        
    
    GPIO_InitStructure.GPIO_Pin     =USART3_RX_PIN ;
    GPIO_InitStructure.GPIO_Mode    =GPIO_Mode_AF;   //���롢�����ģ�⡢����
    GPIO_InitStructure.GPIO_PuPd    =GPIO_PuPd_UP ;//����\����\����
    GPIO_Init(USART3_RX_GPIO_PORT ,&GPIO_InitStructure);        

    //����ӳ��
    GPIO_PinAFConfig (USART3_TX_GPIO_PORT ,USART3_TX_PINSOURCE ,GPIO_AF_USART3 );
    GPIO_PinAFConfig (USART3_RX_GPIO_PORT ,USART3_RX_PINSOURCE ,GPIO_AF_USART3 );
}
//��ʼ�������ж����ȼ�
static void Init_Usart3_NVIC( void )
{
    NVIC_InitTypeDef 	NVIC_InitStructure;	
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	        
}

//��ʼ������
static void Init_Usart3( void )
{
    //�����ʼ��ʱ�õ��Ľṹ�����
	USART_InitTypeDef 			USART_InitStructure;
    //ʱ��ʹ��
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3,ENABLE ); 

    //���������⣬������������ȱʡ��8λ���ݣ�1λֹͣ����У�飬��Ӳ��������
    USART_InitStructure.USART_BaudRate=USART3_BAUND_RATE;					
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Parity =USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;	//ʹ�ܽ��պͷ���ģʽ
    
    USART_Init(USART3,&USART_InitStructure);

 	//�����ж�ʹ��
 	//USART_ITConfig(USART3,USART_IT_TC,ENABLE);			//��������ж�    
    //USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);         //�����ж�
    //USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);       //��������жϣ�ʹ��DMA�󽫱�����
    //USART_ITConfig(USART3,USART_IT_TXE,ENABLE);        //���ͻ�����жϣ�ʹ��DMA�󽫱�����

    //DMA����ʹ��
    //USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  //USART����DMAʹ�ܣ�ʹ��֮�󽫲��ٲ������ͻ�����ж�               
    //USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);  //USART����DMAʹ�ܣ�ʹ��֮�󽫲��ٲ�����������ж�    
    
    //ʹ�ܴ���
    USART_Cmd(USART3,ENABLE);    
}




//�������ڽ���
static void Start_Usart3_Receive(void)
{
    Usart3_Rx_Pointer =0;
    USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);       //�����ж�
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);       //��������жϣ�ʹ��DMA�󽫱�����    
}

//�������ڷ���
static void Start_Usart3_Transmit(uint16_t num)
{
    Usart3_ReceiveCompleted=0;
    
    Usart3_Tx_Counter =num;
    Usart3_Tx_Pointer =0;
    
    USART3->DR=Usart3_TBuff [Usart3_Tx_Pointer];  //���͵�һ���ֽ�
    USART_ITConfig(USART3,USART_IT_TC,ENABLE);	  //�����ֽ����ж��з���   
}

//����diwnд�Ĵ���ָ��1B
static uint8_t Fill_Buff_for_WriteDwinREG8(uint8_t REG_Address,uint8_t data)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=8;  //֡ͷ2B+����1B+����1B+��ַ1B+����1B+CRC 2B=8B 
    
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=frame_len -3;        //��֡�����ֽ�����������1B+��ַ1B+����+CRC 2B=5B
    //�����ַ������
    frame[3]=APP_DWIN_CMD_WRITE_REG;
    frame[4]=REG_Address ;     
    frame[5]=data;
        
    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [frame_len -2]=crc;    //CRC���ֽ���ǰ
    frame [frame_len -1]=crc>>8; //   
    return frame_len;
}
//����diwnд�Ĵ���ָ��2B
static uint8_t Fill_Buff_for_WriteDwinREG16(uint8_t REG_Address,uint16_t data)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=9;  //֡ͷ2B+����1B+����1B+��ַ1B+����2B+CRC 2B=9B 
    
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=frame_len -3;        //��֡�����ֽ�����������1B+��ַ1B+����+CRC 2B=5B
    //�����ַ������
    frame[3]=APP_DWIN_CMD_WRITE_REG;
    frame[4]=REG_Address ;     
    frame[5]=data>>8;    //big endian
    frame[6]=data;
        
    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [frame_len -2]=crc;    //CRC���ֽ���ǰ
    frame [frame_len -1]=crc>>8; //   
    return frame_len;
}



//����diwnд�ڴ�ָ��
static uint8_t Fill_Buff_for_WriteDwinRAM(uint16_t RAM_Address,uint16_t * p_data,uint8_t num)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;

    uint16_t    tmp;
    //uint8_t*    scr;
    uint8_t*    dst;
    uint8_t     i;
    
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=num*2+8;  //֡ͷ2B+����1B+����1B+��ַ2B+CRC 2B=8B 
    
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=frame_len -3;        //��֡�����ֽ��������������⻹�У�����1B+��ַ2B+CRC 2B=5B
    //����͵�ַ
    frame[3]=APP_DWIN_CMD_WRITE_RAM;
    frame[4]=RAM_Address >>8;     //Big Endian
    frame[5]=RAM_Address & 0x00FF;
        
    //���ݣ�frame[6]��frame[6+num*2-1]
    dst =(uint8_t *)&frame[6];
    for(i=0;i<num;i++)
    {
        tmp=*(p_data ++);  //ARM�����ֽ���ǰ
        *(dst++)=tmp>>8;   //���ķ��ͣ����ֽ���ǰ��big endian
        *(dst++)=tmp;
    }

    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [frame_len -2]=crc;    //CRC���ֽ���ǰ
    frame [frame_len -1]=crc>>8; //   
    return frame_len;
}
//����diwnд���߻���ָ��
static uint8_t Fill_Buff_for_WriteDwinBuff(uint8_t Buff_CH,uint16_t * p_data,uint8_t num)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;

    uint16_t    tmp;
    //uint8_t*    scr;
    uint8_t*    dst;
    uint8_t     i;
    
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=num*2+7;  //֡ͷ2B+����1B+����1B+ͨ��1B+CRC 2B=7B  
    
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=frame_len -3;        //��֡�����ֽ��������������⻹�У�����1B+��ַ2B+CRC 2B=5B
    //����͵�ַ
    frame[3]=APP_DWIN_CMD_WRITE_BUFF;
    frame[4]=Buff_CH ;

    //���ݣ�frame[5]��frame[5+num*2-1]
    dst =(uint8_t *)&frame[5];
    for(i=0;i<num;i++)
    {
        tmp=*(p_data ++);  //ARM�����ֽ���ǰ
        *(dst++)=tmp>>8;   //���ķ��ͣ����ֽ���ǰ��big endian
        *(dst++)=tmp;
    }

    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [frame_len -2]=crc;    //CRC���ֽ���ǰ
    frame [frame_len -1]=crc>>8; //   
    return frame_len;
}



//����diwn���Ĵ���ָ��
static uint8_t Fill_Buff_for_ReadDwinREG(uint8_t REG_Address,uint8_t  *p_data,uint8_t num)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=8;  //֡ͷ2B+����1B+����1B+��ַ1B+������1B+CRC 2B=8B 
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=5;        //��֡���ݺ����ֽ�����������1B+��ַ1B+������1B +CRC 2B=5B
    
    //�����ַ��������
    frame[3]=APP_DWIN_CMD_READ_REG;
    frame[4]=REG_Address ;
    frame[5]=num;
      
    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [6]=crc;                  //CRC���ֽ���ǰ
    frame [7]=crc>>8;
        
    return frame_len;
}



//����diwn���ڴ�ָ��
static uint8_t Fill_Buff_for_ReadDwinRAM(uint16_t RAM_Address,uint16_t * p_data,uint8_t num)
{
    uint8_t *   frame;      //���ڻ���֡
    uint8_t     frame_len;
    uint16_t    crc;
    
    frame=(uint8_t *)Usart3_TBuff;
    frame_len=9;  //֡ͷ2B+����1B+����1B+��ַ2B+������1B+CRC 2B=9B 
    //֡ͷ�����ݳ���
    frame[0]=APP_DWIN_FRAME_HEAD0 ;
    frame[1]=APP_DWIN_FRAME_HEAD1 ;
    frame[2]=6;        //��֡���ݺ����ֽ�����������1B+��ַ2B+������1B +CRC 2B=6B
    
    //�����ַ��������
    frame[3]=APP_DWIN_CMD_READ_RAM;
    frame[4]=RAM_Address >>8;       //Big Endian
    frame[5]=RAM_Address & 0x00FF;
    frame[6]=num;
    
    //����CRCУ��
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    frame [7]=crc;                  //CRC���ֽ���ǰ
    frame [8]=crc>>8;
  
    return frame_len;
}

//����dwin���Զ�Ӧ��
static uint8_t Is_AutoAnswer_Right(uint8_t CMD)
{
    uint8_t *   frame;      //���ڻ���֡
    //uint8_t     frame_len;
    //uint16_t    crc;        
    
    uint8_t     result;
    
    frame=(uint8_t *)Usart3_RBuff;    //���е���ָ��������յ�Ӧ��
    
    if(frame[0]==APP_DWIN_FRAME_HEAD0  && 
       frame[1]==APP_DWIN_FRAME_HEAD1  &&
       frame[2]==0x02                  &&
       frame[3]==CMD                   &&
       frame[4]==0xFF  )
    {   
        result=1;
    }
    else
    {
        result=0;
    }
    return result ;
}

//����Ĵ�������
static uint8_t Decode_DwinREG(uint8_t *p_data)
{
    uint8_t result;     //����״ֵ̬
    
    uint8_t *frame;
    uint8_t  frame_len;
    uint16_t crc;

    //uint16_t tmp;
    uint8_t* scr;
    uint8_t  i;
    uint8_t num;
    
    frame=(uint8_t *)&Usart3_RBuff[7];
    frame_len=frame[2]+3;  //֡ͷ2B+����1B+����1B+��ַ1B+������1B+CRC 2B=8B
    
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    
    if(frame[frame_len-2]==(crc & 0x00FF)         &&       //CRC��ȷ
       frame[frame_len-1]==((crc>>8) & 0x00FF)    &&       //CRC��ȷ
       frame[3]          ==APP_DWIN_CMD_READ_REG )         //������ȷ
    {
        num=frame [5];
        scr=(uint8_t *)&frame[6];
        for(i=0;i<num;i++)
        {
            *(p_data++)=*(scr++);
        }
        result=1;//���ݽ����ɹ�
    }
    else
    {
        result=0;
    }
    return result;
}



//�����ڴ����
static uint8_t Decode_DwinRAM(uint16_t * p_data)
{
    uint8_t result;     //����״ֵ̬
    
    uint8_t *frame;
    uint8_t  frame_len;
    uint16_t crc;

    uint16_t tmp;
    uint8_t* scr;
    uint8_t  i;
    uint8_t num;
    
    frame=(uint8_t *)&Usart3_RBuff[7];
    frame_len=frame[2]+3;  //֡ͷ2B+����1B+����1B+��ַ2B+������1B+CRC 2B=9B
    
    crc=Get_CRC16 (&frame[3],frame[2]-2);
    
    if(frame[frame_len-2]==(crc & 0x00FF)         &&       //CRC��ȷ
       frame[frame_len-1]==((crc>>8) & 0x00FF)    &&       //CRC��ȷ
       frame[3]          ==APP_DWIN_CMD_READ_RAM )         //������ȷ
    {
        scr=(uint8_t *)&frame[7];
        num=frame [6];
        for(i=0;i<num;i++)
        {
            tmp=(*(scr++))*256;   //big endian
            tmp+=*(scr++);
            *(p_data ++)=tmp;
        }
        result=1;//���ݽ����ɹ�
    }
    else
    {
        result=0;
    }
    return result;
}

//дdwin�Ĵ���1�ֽڣ�״̬����
static dwin_instruction_state_t  Write_Dwin_REG8 (uint8_t REG_Address,uint8_t  data)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_WriteDwinREG8(REG_Address,data); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_WRITE_RAM)==1)
            {
                Instruction_State=dwin_instruction_state_completed;
            }
            else
            {
                Instruction_State=dwin_instruction_state_err;
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;




}

//дdwin�Ĵ���2�ֽڣ�״̬����
static dwin_instruction_state_t  Write_Dwin_REG16 (uint8_t REG_Address,uint16_t  data)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_WriteDwinREG16(REG_Address,data); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_WRITE_RAM)==1)
            {
                Instruction_State=dwin_instruction_state_completed;
            }
            else
            {
                Instruction_State=dwin_instruction_state_err;
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;

}



//дdwin��������״̬����
static dwin_instruction_state_t Write_Dwin_RAM(uint16_t RAM_Address,uint16_t * p_data,uint8_t num)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_WriteDwinRAM(RAM_Address,p_data,num); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_WRITE_RAM)==1)
            {
                Instruction_State=dwin_instruction_state_completed;
            }
            else
            {
                Instruction_State=dwin_instruction_state_err;
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;
}



//дdwin�����߻��壨״̬����
static dwin_instruction_state_t Write_Dwin_Buff(uint8_t Buff_CH,uint16_t * p_data,uint8_t num)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_WriteDwinBuff(Buff_CH,p_data,num); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_WRITE_BUFF)==1)
            {
                Instruction_State=dwin_instruction_state_completed;
            }
            else
            {
                Instruction_State=dwin_instruction_state_err;
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;
}



//��dwin�Ĵ�����״̬����
static dwin_instruction_state_t  Read_Dwin_REG(uint8_t REG_Address,uint8_t  *p_data,uint8_t num)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_ReadDwinREG(REG_Address,p_data,num); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            Instruction_State=dwin_instruction_state_err;  //ȱʡֵ
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_READ_REG)==1)  //�Զ�Ӧ����֤
            {
                if(Decode_DwinREG(p_data)==1)     //���ݽ����ɹ�
                {
                    Instruction_State=dwin_instruction_state_completed;
                }
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;

}

//��dwin��������״̬����
static dwin_instruction_state_t Read_Dwin_RAM(uint16_t RAM_Address,uint16_t * p_data,uint8_t num)
{
    static dwin_instruction_state_t Instruction_State=dwin_instruction_state_idle;  //״̬��
    static uint8_t Retry_Times=0;

    uint8_t     frame_len;

    switch(Instruction_State)
    {
        
        case dwin_instruction_state_idle :    //ÿ��ͨ�Ŵ�idle��ʼ����idle����
            Retry_Times=0;
            Instruction_State=dwin_instruction_state_ready;
            break;
        
        case dwin_instruction_state_ready:
            frame_len=Fill_Buff_for_ReadDwinRAM(RAM_Address,p_data,num); //׼��ͨ��ָ��           
            Start_Usart3_Transmit(frame_len );  //��ʼͨ��      
            SysTick_ms_for_Usart3=100;          //��ʱ����100ms
            Instruction_State=dwin_instruction_state_communication;
            break;
        
        case dwin_instruction_state_communication :
            if(Usart3_ReceiveCompleted==1)   //���յ���������Ӧ
            {
                Instruction_State= dwin_instruction_state_decode;
            }
            else if(SysTick_ms_for_Usart3==0) //ͨ�ų�ʱ
            {
                Instruction_State= dwin_instruction_state_timeout;
            }
            break;
        
        case dwin_instruction_state_decode :
            Instruction_State=dwin_instruction_state_err;  //ȱʡֵ
            if(Is_AutoAnswer_Right(APP_DWIN_CMD_READ_RAM)==1)  //�Զ�Ӧ����֤
            {
                if(Decode_DwinRAM(p_data)==1)     //���ݽ����ɹ�
                {
                    Instruction_State=dwin_instruction_state_completed;
                }
            }
            break;
        
        case dwin_instruction_state_err:
        case dwin_instruction_state_timeout :
            if(Retry_Times <3)
            {
                Retry_Times++;
                Instruction_State=dwin_instruction_state_ready;  //׼����һ�γ���                 
            }
            else
            {
                Instruction_State=dwin_instruction_state_fail;
            }
            break;
        
        case dwin_instruction_state_fail:
        case dwin_instruction_state_completed :
            Instruction_State=dwin_instruction_state_idle; //֪ͨ���ú���������ͨ���ѽ�����
            break;
        
        default :
            Instruction_State=dwin_instruction_state_idle;
            break;
    }
    return Instruction_State ;
    
}



//�ϴ���ʾ����
static dwin_instruction_state_t  Uptade_Measurements(void)
{
    dwin_instruction_state_t State;
    State=Write_Dwin_RAM((uint16_t)  APP_DWIN_RAM_ADDRESS_OF_MEASUREMENTS,   //����RAM��ַ
                         (uint16_t *)&Dwin_Measurements,                 //����
                         (uint8_t )  APP_DWIN_RAM_NUM_OF_MEASUREMENTS);      //����      
    return State ;
}

//��ȡ�û�����
static dwin_instruction_state_t Get_Commands(void)
{
    dwin_instruction_state_t State;
    State=Read_Dwin_RAM( (uint16_t)  APP_DWIN_RAM_ADDRESS_OF_COMMANDS ,  //ͨ��ѡ��
                         (uint16_t *)&Dwin_Commands,                 //����
                         (uint8_t )  APP_DWIN_RAM_NUM_OF_COMMANDS );      //����      
    return State ;
}
//��λ�û�����
static dwin_instruction_state_t  Reset_Commands(void)
{
    dwin_instruction_state_t State;
    State=Write_Dwin_RAM((uint16_t)  APP_DWIN_RAM_ADDRESS_OF_COMMANDS,   //����RAM��ַ
                         (uint16_t *)&Dwin_Commands,                 //����
                         (uint8_t )  APP_DWIN_RAM_NUM_OF_COMMANDS);      //����      
    return State ;
}
//��ʼ��dwin������
static dwin_instruction_state_t Init_Settings(void)
{
    dwin_instruction_state_t State;
    State=Write_Dwin_RAM( (uint16_t)  APP_DWIN_RAM_ADDRESS_OF_SETTINGS ,  //ͨ��ѡ��
                          (uint16_t *)&Dwin_Settings,                 //����
                          (uint8_t )  APP_DWIN_RAM_NUM_OF_SETTINGS );      //����      
    return State ;
}
//��ȡ�û�����
static dwin_instruction_state_t Get_Settings(void)
{
    dwin_instruction_state_t State;
    State=Read_Dwin_RAM( (uint16_t)  APP_DWIN_RAM_ADDRESS_OF_SETTINGS ,  //ͨ��ѡ��
                         (uint16_t *)&Dwin_Settings,                 //����
                         (uint8_t )  APP_DWIN_RAM_NUM_OF_SETTINGS );      //����      
    return State ;
}



//�л���ָ��ҳ��
static dwin_instruction_state_t Switch_Page(uint16_t  PageNum)
{
//    #define Right_Page 15
//    #define Wrong_Page 24
//    #define STORED_PASSWORD   1111
    dwin_instruction_state_t State;
//    uint16_t PageNum;
//    if(Dwin_PostedCommands.Password ==STORED_PASSWORD)
//    {
//        Page=Right_Page ;
//    }
//    else
//    {
//        Page=Wrong_Page;
//    }
    State=Write_Dwin_REG16(0x03, PageNum);                                
    return State ;
}

//��ȡϵͳʱ��
static dwin_instruction_state_t Get_System_Time(void)
{
    dwin_instruction_state_t State;
    State=Read_Dwin_REG( (uint8_t)  0x20,  //RTC�Ĵ�����ַ
                         (uint8_t *)&Dwin_RTC,                 //����
                         (uint8_t)  7);      //����      
    if(State == dwin_instruction_state_completed )
    {
        Dwin_RTC .date [0]='2';
        Dwin_RTC .date [1]='0';
        Dwin_RTC .date [2]=(Dwin_RTC .YY >>4   )+0x30;
        Dwin_RTC .date [3]=(Dwin_RTC .YY & 0x0F)+0x30;
        Dwin_RTC .date [4]='-';
        Dwin_RTC .date [5]=(Dwin_RTC .MM >>4   )+0x30;
        Dwin_RTC .date [6]=(Dwin_RTC .MM & 0x0F)+0x30;
        Dwin_RTC .date [7]='-';
        Dwin_RTC .date [8]=(Dwin_RTC .DD >>4   )+0x30;;
        Dwin_RTC .date [9]=(Dwin_RTC .DD & 0x0F)+0x30;
        Dwin_RTC .date [10]=' ';
        Dwin_RTC .date [11]=(Dwin_RTC .hh >>4   )+0x30;
        Dwin_RTC .date [12]=(Dwin_RTC .hh & 0x0F)+0x30;
        Dwin_RTC .date [13]='-';
        Dwin_RTC .date [14]=(Dwin_RTC .mm >>4   )+0x30;
        Dwin_RTC .date [15]=(Dwin_RTC .mm & 0x0F)+0x30;
        Dwin_RTC .date [16]='-';
        Dwin_RTC .date [17]=(Dwin_RTC .ss >>4   )+0x30;
        Dwin_RTC .date [18]=(Dwin_RTC .ss & 0x0F)+0x30;
        Dwin_RTC .date [19]='\0';   
        
    }
    return State ;
}



//������������յ����û��������Posted�����󣬽�������ջ������㣩
static void Post_Commands(void)
{
    if(Dwin_Commands .Clear_Data ==1)
    {
        Dwin_PostedCommands .Clear_Data =1;
        Dwin_Commands .Clear_Data =0;
    }
        
    if(Dwin_Commands.Save_to_Card ==1)
    {
        Dwin_PostedCommands .Save_to_Card =1;
        Dwin_Commands .Save_to_Card =0;
    }
    if(Dwin_Commands.Save_to_Disk ==1)
    {
        Dwin_PostedCommands .Save_to_Disk =1;
        Dwin_Commands .Save_to_Disk =0;
    }
    if(Dwin_Commands.Password  !=0)
    {
        Dwin_PostedCommands .Password =Dwin_Commands.Password;
        Dwin_Commands.Password =0;
    }
    if(Dwin_Commands .Restore_Default_Settings ==1)
    {
        Dwin_PostedCommands .Restore_Default_Settings =1;
        Dwin_Commands.Restore_Default_Settings =0;    
    }
    if(Dwin_Commands .Save_Settings  ==1)
    {
        Dwin_PostedCommands .Save_Settings =1;
        Dwin_Commands.Save_Settings =0;    
    }
    
    if(Dwin_Commands .Delete_Data   ==1)
    {
        Dwin_PostedCommands .Delete_Data =1;
        Dwin_Commands.Delete_Data =0;    
    }
    
    
   
}

//���Ժ������ѷ���
void Set_Default_Settings(void)
{
    Dwin_Settings. Product_Num=0;
    Dwin_Settings. Operator_Num=0;
    
    Dwin_Settings. MMT_Offset=50;       //��λ��0.1��
    Dwin_Settings. Water_Content=100;    //��λ��ǧ��֮һ
	Dwin_Settings. Oxygen_Content=50;   //��λ��ǧ��֮һ

    Dwin_Settings. Sensor_Temp_Plus=0;
    Dwin_Settings. Sensor_Temp_Sub=0;

    Dwin_Settings. Flue_Temp_Plus=0;   
    Dwin_Settings. Flue_Temp_Sub=0;

    Dwin_Settings. Ambient_Temp_Plus=0;
    Dwin_Settings. Ambient_Temp_Sub=0;

    Dwin_Settings. Sensor_Current_Plus=0;    
    Dwin_Settings. Sensor_Current_Sub=0;

//    Dwin_Settings. SO3_A=370;      //0.0003704
//    Dwin_Settings. SO3_B=531;      //0.005309
//    Dwin_Settings. SO3_C=659;      //0.006593
//    Dwin_Settings. SO3_D=600;      //-0.006

//    Dwin_Settings. H2SO4_A=0;   
//    Dwin_Settings. H2SO4_B=0;
//    Dwin_Settings. H2SO4_C=0;   
//    Dwin_Settings. H2SO4_D=0;
}
static void Cal_Dwin_Display(void)
{
    //�����������Ǳ���ʾ
    Dwin_Measurements.Current_Icon=Dwin_Measurements .Sensor_Current;
    if(Dwin_Measurements.Current_Icon>100) Dwin_Measurements.Current_Icon=100;
    
    //�������¶��Ǳ���ʾ
    Dwin_Measurements.Sensor_TempIcon =Dwin_Measurements .Sensor_Temp ;
    if(Dwin_Measurements.Sensor_TempIcon<100) Dwin_Measurements.Sensor_TempIcon=100;
    if(Dwin_Measurements.Sensor_TempIcon>300) Dwin_Measurements.Sensor_TempIcon=300;
    
    //���״̬ͼ����ʾ
    if(Battery .A>0)    //���
        Dwin_Measurements .Battery_Icon =6;
    else if(Battery .C>90)
        Dwin_Measurements .Battery_Icon =5;
    else if(Battery .C>75)
        Dwin_Measurements .Battery_Icon =4;
    else if(Battery .C>60)
        Dwin_Measurements .Battery_Icon =3;
    else if(Battery .C>45)
        Dwin_Measurements .Battery_Icon =2;
    else if(Battery .C>30)
        Dwin_Measurements .Battery_Icon =1;
    else 
        Dwin_Measurements .Battery_Icon =0;
    
    //sd��������ʵ��sd��ģ���ת�ƣ�
    //Dwin_Measurements .Card_Space =1980;  

    //�����������ڴ�����ģ�����趨
}

//��������
static dwin_instruction_state_t Draw_Lines(void)
{
    dwin_instruction_state_t State;
    
    State=Write_Dwin_Buff(APP_DWIN_TRENDLINE_BUFF_CHS ,
                          Dwin_TrendLine_Buff ,                 //����
                          Dwin_TrendLine_Points_Num );      //����      

    return State ;
}                                                  

static void Handle_NonComm_Commands(void)
{
    
    if(Dwin_PostedCommands.Clear_Data ==1)
    {
        Dwin_Measurements .ADT =0;////////////
        Dwin_PostedCommands .Clear_Data =0;    
    }
    

    //ҳ���л����������
}



/******************************************************************************
*                      �ⲿ��������                                            
******************************************************************************/
//USART3ģ���ʼ��
void Usart3_Init(void)
{
    Init_Usart3_GPIO();
	Init_Usart3_NVIC();
    //Init_Usart3_DMA(); 	    
    Init_Usart3();
}

//������ͨ���ܹ��ܣ�״̬����
void Dwin_Process(void)
{
  
    static dwin_app_state_t Dwin_App_State= dwin_app_state_idle;
    static uint16_t Target_Page=0;
    
    switch(Dwin_App_State)
    {
        case dwin_app_state_idle:
            SysTick_ms_for_Dwin=4000;
            Dwin_App_State= dwin_app_state_delay0;
            break;
        case dwin_app_state_delay0:
            if(SysTick_ms_for_Dwin==0)   //�ȴ���ʱ��ɺ󣬽�����һ״̬
                Dwin_App_State= dwin_app_state_init_settings;  
            break;       
        case dwin_app_state_init_settings:
//            if(0)//flash���б���Ĳ���
//            {
//                ;//��ȡflash������dwin_settings
//            
//            }
//            else
//            {
//                Set_Default_Settings();  //���ó���������dwin_settings
//            }
//���ϲ�������������ʼ�������
             //Set_Default_Settings();
            //Dwin_Settings =Dwin_Default_Settings ;
        
            //��ʼ������������
            if(Init_Settings()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Dwin_App_State= dwin_app_state_get_settings;  
            }        
            break;            
        case dwin_app_state_get_settings:
            if(Get_Settings ()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Dwin_App_State= dwin_app_state_get_commands;  
            } 
            break;            
        case dwin_app_state_get_commands:
            if(Get_Commands ()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Post_Commands ();
                Dwin_App_State= dwin_app_state_reset_commands;  
            } 
            break;            
         
        case dwin_app_state_reset_commands:
            if(Reset_Commands ()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Dwin_App_State= dwin_app_state_handle_commands;  
            } 
            break;            
        case dwin_app_state_handle_commands :
            //Handle_NonComm_Commands(); //�����ڱ�ģ��ģ�����Ǩ�Ƶ�����ģ���첽����
            //�ָ���������
            if(Dwin_PostedCommands.Restore_Default_Settings ==1)
            {
                //Set_Default_Settings();
                Dwin_Settings =Dwin_Default_Settings ;
                Dwin_App_State =dwin_app_state_init_settings ;
                Dwin_PostedCommands .Restore_Default_Settings =0;    
            }            
            else
            {
                Dwin_App_State= dwin_app_state_switch_page;
            }
            
            break;
            
        case dwin_app_state_switch_page:    //�����ڲ�����
            //#define Right_Page 15
            //#define Wrong_Page 24
            //#define STORED_PASSWORD   1111
            if(Dwin_PostedCommands.Password ==DWIN_Sys_Settings_Password)  //����Ҫ�л���ҳ��
            {
                Dwin_Page_to_Switch  =DWIN_Sys_Settings_Page;    //ϵͳ����ҳ��
                Dwin_PostedCommands.Password =0;
            }
        
            if(Target_Page ==0)  //Target_Page ==0���ϴ��л���ɵı�־
            {
                Target_Page =Dwin_Page_to_Switch ; //��ȡ����
                Dwin_Page_to_Switch=0;             //�������            
            }
            
            if(Target_Page !=0)  //��Ҫ�л�,ִ���л�ҳ��״̬��
            {
                if(Switch_Page (Target_Page )==dwin_instruction_state_idle)
                {
                    Target_Page=0; 
                }            
            }
            
            if(Target_Page ==0)
            {
                Dwin_App_State= dwin_app_state_get_time;
            }
            
//            if(Dwin_PostedCommands.Password ==STORED_PASSWORD)
//            {
//                if(Switch_Page (15 )==dwin_instruction_state_idle)
//                {
//                    Dwin_PostedCommands.Password =0;                    
//                }
//            }
//            else 
//            {
//                Dwin_App_State= dwin_app_state_get_time;
//            }
            break;            
        case dwin_app_state_get_time:
            if(Get_System_Time ()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Dwin_App_State= dwin_app_state_cal_measurements;  
            } 
            break; 
        case dwin_app_state_cal_measurements:          
            Cal_Dwin_Display();
            Dwin_App_State= dwin_app_state_update_measurements;  
            break;
        case dwin_app_state_update_measurements: 
            if(Uptade_Measurements()==dwin_instruction_state_idle )  //����idle�������һ״̬
            {
                Dwin_App_State= dwin_app_state_draw_trendline;  
            }             
            break;            
        case dwin_app_state_draw_trendline:
            if(Draw_Lines()==dwin_instruction_state_idle )  //�������ݣ�ֱ����ɺ������һ״̬
            {
                Dwin_TrendLine_Points_Num=0;
                SysTick_ms_for_Dwin=200;
                Dwin_App_State= dwin_app_state_delay;  
            }              
            break;            
        case dwin_app_state_delay:      
            if(SysTick_ms_for_Dwin==0)   //�ȴ���ʱ��ɺ󣬽�����һ״̬
                Dwin_App_State= dwin_app_state_get_settings;  
            break;            
        default:
            Dwin_App_State=dwin_app_state_idle;
            break;
    
    }
    
}
void Dwin_AddPoint(uint16_t data1,uint16_t data2)
{
    Dwin_TrendLine_Buff [Dwin_TrendLine_Points_Num ++]=data1;
    Dwin_TrendLine_Buff [Dwin_TrendLine_Points_Num ++]=data2;
}
uint16_t Get_CRC16(__IO uint8_t *p_frame,uint8_t len_in_byte) 
{ 
    uint8_t i; 
    uint16_t crc = 0xFFFF; 
    while(len_in_byte--)  
    {   
        crc ^= *p_frame; 
        for(i=0; i<8; i++)  
        { 
            if(crc&0x0001) 
            { 
                crc >>= 1;  
				crc ^= 0xA001; 
            }  
            else 
            {
				crc >>= 1;
            } 
        }         
        p_frame++; 
    } 
    return(crc); 
} 








/**********************************************************************************************************
*                                       �жϴ�����
**********************************************************************************************************/
void USART3_IRQHandler(void)
{
    //�ڴ��ڿ����ж���
    if((USART3->SR)&USART_SR_IDLE)
    {
        USART3->DR ;		//��DR���ж�

        USART_ITConfig(USART3,USART_IT_IDLE,DISABLE);	
        USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);	
        //AppTaskSemPost_Dwin();
        Usart3_ReceiveCompleted=1;
    }
    //�ڴ��ڷ�������ж���
    if((USART3->SR)&USART_SR_TC)
    {
        USART3->SR =~USART_SR_TC ; //д�����־

        Usart3_Tx_Counter --;
        if(Usart3_Tx_Counter!=0)
        {
            Usart3_Tx_Pointer++;
            USART3->DR=Usart3_TBuff [Usart3_Tx_Pointer];
        }
        else
        {
            USART_ITConfig(USART3,USART_IT_TC,DISABLE);			//��������ж�   
            Start_Usart3_Receive();
        }
    }    
    

    //�ڴ��ڽ�������ж���
    if((USART3->SR)&USART_SR_RXNE)
    {
        //UART4->DR ;		//��DR���־
        //UART4->SR =(uint16_t)(~USART_SR_RXNE) ; **д�����־
        //BSP_LED_Toggle ();
        Usart3_RBuff [Usart3_Rx_Pointer ]=USART3->DR ;
        Usart3_Rx_Pointer++;
    } 
}



//End of File

