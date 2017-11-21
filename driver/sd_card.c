/******************************************************************************
*******************************************************************************
**                     �弶֧�ְ�                                              
**                                                                            
**                     ֪�пƼ�   Copyright 2017;                              
*******************************************************************************
** Filename      : sd_card.c
** Version       : V1.00    
** Programmer(s) : YZ       
** Date          : 2017.7.2 
** Discription   :�ж��������ã������ļ��е��ж������Ѹ�д��ֻ�轫�弶�жϺ�
                  ��ָ����ӵ����ļ�������ж���������BSP_IntVectTbl���ɡ�
** Note(s)		 :         
*******************************************************************************
******************************************************************************/

#define  SD_CARD_MODULE
/******************************************************************************
*                      ͷ�ļ�                                                  
******************************************************************************/
//#include <includes.h>
#include "stm32f4xx.h"
#include "sd_card.h"

#include "stm324xg_eval_sdio_sd.h"
#include "dwin.h"
#include "sensor.h"
#include "ff.h"
#include "diskio.h"










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
static uint16_t String_to_uint16(uint8_t *str);
static void Num16_to_ASIC(uint16_t Num,unsigned char *buff,uint16_t Field,uint16_t Decimal);



/******************************************************************************
*                      �ڲ���������                                                
******************************************************************************/
static void Num16_to_ASIC(uint16_t Num,unsigned char *buff,uint16_t Field,uint16_t Decimal)
{
    uint16_t i;
    uint16_t counter;
    counter =Field;
    
    buff+=(Field-1);
    for(i=0;i<Decimal ;i++) //С������
    {
        *(buff--)=Num%10 +0x30;
        Num/=10;
        counter --;
    }
    if(Decimal !=0)
    {
        *(buff--)='.';
        counter--;
    }
    *(buff--)=Num%10 +0x30;
    Num/=10;
    counter --;
    while(Num!=0)
    {
        *(buff--)=Num%10 +0x30;
        Num/=10;
        counter--;
    }
    while(counter--)
    {
        *(buff--)=' ';
    }
}


static uint16_t String_to_uint16(uint8_t *str)
{
    uint16_t num=0;
    uint16_t tmp;
    
    tmp=*(str++)-0x30;  
    num+=tmp*10000;
    tmp=*(str++)-0x30;  
    num+=tmp*1000;
    tmp=*(str++)-0x30;  
    num+=tmp*100;
    tmp=*(str++)-0x30;  
    num+=tmp*10;
    tmp=*(str++)-0x30;  
    num+=tmp*1;
    
    return num;    
}


/******************************************************************************
*                      �ⲿ��������                                          
******************************************************************************/
//__align(4)    uint8_t R_Buff[512];
//unsigned char write_buffer[512];     //д�ļ�������
unsigned char read_buffer[128];        //���ļ�������
unsigned int counter;

//��ȡ��������
void SD_Read_Default_Settings(void)
{
    int i;
    FIL file;      //�ļ�����
    FATFS fatfs;   //�߼��������Ĺ�����
    uint16_t * p_data;
    
    static FRESULT res; 
    
    
    res = f_mount(1,&fatfs);    //����SD��
	res = f_open(&file,"1:/default_settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�
    res = f_lseek(&file,0);     //�ƶ�дָ�뵽�ļ���
    

    p_data=(uint16_t *)&Dwin_Default_Settings;
    for(i=0;i<APP_DWIN_RAM_NUM_OF_SETTINGS;i++)
    {
        res = f_read(&file,read_buffer,27,&counter);  
        *p_data=String_to_uint16(&read_buffer [20]);
        p_data++;
    }
    if(res){;}     //avoid warnings
    f_close(&file);//�ر�Դ�ļ�
}

void SD_Load_Settings(void)
{
    //int i;
    FIL file;                                                                //�ļ�����
    FATFS fatfs;                                                             //�߼��������Ĺ�����
    static FRESULT res;   

  	res = f_mount(1,&fatfs);                                            //����SD��
	res = f_open(&file,"1:/settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�	
	
    if(file.fsize >10)  //�в����ļ�
    {
        res = f_lseek(&file,0);
        res = f_read(&file,&Dwin_Settings,sizeof(Dwin_Settings),&counter); 
        f_close(&file); 
    }
    else    //�޲����ļ�
    {
        Dwin_Settings=Dwin_Default_Settings ;
        SD_Save_Settings();
    
    }
    
    if(res){;}     //avoid warnings
}


void SD_Save_Settings(void)
{
    //int i;
    FIL file;                                                                //�ļ�����
    FATFS fatfs;                                                             //�߼��������Ĺ�����
    static FRESULT res;   

  	res = f_mount(1,&fatfs);                                            //����SD��
	res = f_open(&file,"1:/settings.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�	
	res = f_lseek(&file,0);                                                  //�ƶ�дָ�뵽�ļ���
	res = f_write (&file,&Dwin_Settings,sizeof(Dwin_Settings),&counter);                          //���������е�����д��Դ�ļ�
	f_close(&file); 
    if(res){;}     //avoid warnings    //�ر�Դ�ļ�
}

void SD_Save_Data(void)
{
    //int i;
    FIL file;                           //�ļ�����
    FATFS fatfs;                        //�߼��������Ĺ�����
    static FRESULT res;   

  	res = f_mount(1,&fatfs);           //����SD��
	res = f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);    //��������0�ϵ�Դ�ļ�	
	res = f_lseek(&file,file.fsize);                                          //�ƶ�дָ�뵽�ļ���
    
	res = f_write (&file,&Dwin_Settings .Product_Num ,2,&counter);            //���������е�����д��Դ�ļ�
	res = f_write (&file,&Dwin_Settings .Operator_Num,2,&counter);            //���������е�����д��Դ�ļ�
    res = f_write (&file,&Dwin_RTC.date ,sizeof(Dwin_RTC.date),&counter);
    
	res = f_write (&file,&Dwin_Measurements .ADT ,2,&counter);                //���������е�����д��Դ�ļ�
	res = f_write (&file,&Dwin_Measurements .MMT ,2,&counter);                //���������е�����д��Դ�ļ�
	res = f_write (&file,&Dwin_Measurements .SO3_Concentration  ,2,&counter); //���������е�����д��Դ�ļ�
	res = f_write (&file,&Dwin_Measurements .H2SO4_Concentration ,2,&counter);//���������е�����д��Դ�ļ�
	
    f_close(&file);      //�ر�Դ�ļ�
    if(res){;}     //avoid warnings
}

void SD_Delete_Data(void)
{
    //int i;
    //FIL file;      //�ļ�����
    FATFS fatfs;     //�߼��������Ĺ�����
    //static FRESULT res;   

  	f_mount(1,&fatfs);                      //����SD��
	//f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�	
    f_unlink ("1:/data.txt");
}
FRESULT SD_Read_Record(uint8_t * buff,uint32_t index)
{
#define SD_RECORD_LEN (4+20+8)        //ʱ��20B��2������4B��4������ֵ8B
    int i;
    FIL file;      //�ļ�����
    FATFS fatfs;   //�߼��������Ĺ�����
    uint16_t * p_data;
    
    static FRESULT res; 
    
   	res = f_mount(1,&fatfs);    //����SD��
	//res = f_open(&file,"1:/data1.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�
    res = f_open(&file,"1:/data.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS );   //��������0�ϵ�Դ�ļ�
    res = f_lseek(&file,index*SD_RECORD_LEN);     //�ƶ�ָ��
    res = f_read(&file,read_buffer,SD_RECORD_LEN,&counter);  
    f_close(&file);//�ر�Դ�ļ�    
    if(counter==SD_RECORD_LEN)
    {
        p_data=(uint16_t*)read_buffer;    //ָ���¼��ĵ�һ���������ݣ���Ʒ��ţ�
        Num16_to_ASIC(*p_data ,buff,8,0); //д��ASIC buff,8λ���
        buff+=(8);
        *(buff++)=' ';  
        
        p_data++;   //ָ���¼��ڶ����������ݣ�����Ա��ţ�
        Num16_to_ASIC(*p_data ,buff,10,0); //д��ASIC buff,10λ���
        buff+=(10);
        *(buff++)=' ';  
    
        
        for(i=0;i<20;i++)      //RTC�ı�
        {
            *(buff++)=read_buffer [i+4];
        }
        *(buff-4)=':';      //ʱ����ļ�����޸�Ϊð��
        *(buff-7)=':';
        

        *(buff++)='A';
        *(buff++)='D';
        *(buff++)='T';
        *(buff++)=':';
        p_data+=11; //20�ַ��ۺ�10������ ��ָ��ADT���� (��֪Ϊʲôû����)
        Num16_to_ASIC(*p_data ,buff,5,1); //д��ASIC buff,5λ���
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='o';
        *(buff++)='C';
        *(buff++)=' ';  
        *(buff++)=' ';  
    
        *(buff++)='M';
        *(buff++)='M';
        *(buff++)='T';
        *(buff++)=':';
        p_data++;       //ָ��MMT����
        Num16_to_ASIC(*p_data ,buff,5,1); //д��ASIC buff,5λ���
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='o';
        *(buff++)='C';
        *(buff++)=' ';  
        *(buff++)=' ';  
    
        *(buff++)='S';
        *(buff++)='O';
        *(buff++)='3';
        *(buff++)=':';
        p_data++;       //ָ��SO3����
        Num16_to_ASIC(*p_data ,buff,5,1); //д��ASIC buff,5λ���
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='p';
        *(buff++)='p';
        *(buff++)='m';
        *(buff++)=' '; 
        *(buff++)=' '; 
    
        *(buff++)='H';
        *(buff++)='2';
        *(buff++)='S';
        *(buff++)='O';
        *(buff++)='4';
        *(buff++)=':';
        p_data++;       //ָ��H2SO4����
        Num16_to_ASIC(*p_data ,buff,5,1); //д��ASIC buff,5λ���
        buff+=(5);
        *(buff++)=' ';  
        *(buff++)='p';
        *(buff++)='p';
        *(buff++)='m';
        (*buff++) ='\r';
        (*buff++) ='\n';       //ÿ����¼��100�ֽ�        
        
        //res=FR_OK ;        
    }
    else
    {
        //res=!FR_OK ;
        res=FR_NO_FILE ; //��ʱ��һ������ֵ����
    }
    return res;
}


void SD_Process(void)
{
    if(Dwin_PostedCommands.Save_Settings==1 )
    {
        SD_Save_Settings();
        Dwin_PostedCommands.Save_Settings=0;
    }
    if(Dwin_PostedCommands.Save_to_Card==1 )
    {
        SD_Save_Data();
        Dwin_PostedCommands.Save_to_Card=0;
    }
    
    if(Dwin_PostedCommands .Delete_Data ==1)
    {
        SD_Delete_Data();
        Dwin_PostedCommands .Delete_Data =0;
    }
    
    
    

}

FRESULT SD_Get_FreeCap(void)
{
    FATFS fs;
    FATFS *pfs;
    DWORD fre_clust;
    DWORD free_bytes;
    FRESULT  res;
                                              
    pfs=&fs;
    f_mount(1,&fs ); // �����ļ�ϵͳ

    res = f_getfree("1:/", &fre_clust, &pfs);
    if (res) return res; // ���f_getfree����ִ�д��󣬷��ش���ֵ
    else // ����ɹ��������
    {
        //TOT_SIZE = (pfs->n_fatent - 2) * pfs->csize/2; //������ ��λbyte
        //FRE_SIZE = fre_clust * pfs->csize/2; // �������� ��λbyte
        free_bytes= fre_clust * pfs->csize/2;
        Dwin_Measurements .Card_Space =free_bytes/1000;
    }
    f_mount(1, 0); //ж���ļ�ϵͳ
    return FR_OK ;
}

void sd_test(void)
{
//    int i;
//    FIL file;                                                                //�ļ�����
//    FATFS fatfs;                                                             //�߼��������Ĺ�����
//    static FRESULT res;   
//    
//	res = f_mount(1,&fatfs);                                            //����SD��
//	res = f_open(&file,"1:/test33.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�	
//	res = f_lseek(&file,0);                                                  //�ƶ�дָ�뵽�ļ���
//	
//	for(i = 0;i < 512;i++){
//		write_buffer[i] = i % 256;
//	}
//    write_buffer[0]='a';
//    write_buffer[1]='s';
//    write_buffer[2]='\r';
//    write_buffer[3]='\n';
//    
//    
//	res = f_write(&file,write_buffer,4,&counter);                          //���������е�����д��Դ�ļ�
//	res = f_write(&file,write_buffer,4,&counter); 
//	f_close(&file);                                                          //�ر�Դ�ļ�
//	res = f_open(&file,"1:/test33.txt",FA_READ | FA_WRITE | FA_OPEN_ALWAYS);   //��������0�ϵ�Դ�ļ�	
//	res = f_lseek(&file,file.fsize);                                                  //�ƶ�дָ�뵽�ļ���


//	res = f_write(&file,write_buffer,4,&counter);                          //���������е�����д��Դ�ļ�
//	res = f_write(&file,write_buffer,4,&counter);                          //���������е�����д��Դ�ļ�
//	if(res != RES_OK || counter != 512)
//    {
//        ;	
//	}
//	
//	res = f_lseek(&file,0);	                                                 //�ƶ���ָ�뵽�ļ���
//	res = f_read(&file,read_buffer,512,&counter);
//	if(res != RES_OK || counter != 512)
//    {
//        ;
//	}
//	f_close(&file);                                                          //�ر�Դ�ļ�


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
