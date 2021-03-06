#include "led.h"
#include "delay.h"
#include "key.h"
#include "tpad.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "flash.h"
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "mmc_sd.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"
#include "string.h"
#include "usmart.h"
#include "fattester.h"
#include "piclib.h"
#include "vs10xx.h"
#include "mp3player.h"
#include "audiosel.h"
#include "recorder.h"
//********************************************//
//File：UOC Receive&Play  Version 1.0
//Auther: FZUXMUBMW
//Last Edited Date:2015-01 11:48
//********************************************//
// u8 j;
u8 testCharArr[48]=
{
	0x52, 0x49, 0x46, 0x46, 0x08, 0x08, 0x00, 0x10, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
	0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x02, 0x08, 0x00, 0x10, 0xCE, 0x39, 0x57, 0x39
};
u8 Silent[32]=
{
	0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00
};
extern  unsigned char WAV_Data_Test1[34368];
u32 Wav_Data_Len = 0;
 int main(void)
 {	 
// 	Wav_Data_Len = strlen(WAV_Data_Test1);
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(256000);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始�
	TPAD_Init();		//初始化触摸按键
	LCD_Init();	
	KEY_Init();	 	
	Audiosel_Init();	//初始化音源选择
// 	usmart_dev.init(72);//usmart初始化	
 	mem_init(SRAMIN);	//初始化内部内存池	
 	VS_Init();	  		//初始化VS1053 
 	exfuns_init();					//为fatfs相关变量申请内存  
  	f_mount(0,fs[0]); 		 		//挂载SD卡 
 	f_mount(1,fs[1]); 				//挂载FLASH.
	POINT_COLOR=BLUE;      
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
	}
		Show_Str(60,50,200,16,"水下蓝光激光通信系统",16,0);				    	 
		Show_Str(80,70,200,16,"--解调接收端",16,0);			
		Show_Str(60,110,200,16,"2014-01-16",16,0);
		POINT_COLOR=RED;
// 		Show_Str(60,130,200,16,"KEY0:REC/PAUSE",16,0);
// 		POINT_COLOR=BLUE;
// 		Show_Str(60,150,200,16,"KEY2:STOP&SAVE",16,0);
// 		POINT_COLOR=RED;
// 		Show_Str(60,170,200,16,"KEY_UP:AGC+ KEY1:AGC-",16,0);
// 		POINT_COLOR=BLUE;
// 		Show_Str(60,190,200,16,"TPAD:Play The File",16,0);	
	
/*
	while(1)
	{
		Audiosel_Set(0);	//音频输入选择 MP3 通道
 		LED1=0; 	  
		Show_Str(60,210,200,16,"存储器测试...",16,0);
		VS_Ram_Test();	    
		Show_Str(60,210,200,16,"SIN TEST...",16,0); 	 	 
 		VS_Sine_Test();	   
		Show_Str(60,210,200,16,"<<WAV录音传输>>",16,0); 		 
		LED1=1;
		recoder_play();
// 		mp3_play();
	} 		

*/
	while(1)
	{
		Audiosel_Set(0);	//音频输入选择 MP3 通道
 		LED1=0; 	  
// 		Show_Str(60,210,200,16,"Memory Test...",16,0);
		VS_Ram_Test();	    
// 		Show_Str(60,210,200,16,"SIN TEST...",16,0); 	 	 
 		VS_Sine_Test();
		LED1=1;		
		USART_RX_BUF_Init(1);
// 		VS_Restart_Play();						//重启播放 
		VS_Set_All();							//设置音量等信息			 
		VS_SPI_SpeedHigh();	//高速
		VS_WR_Cmd(0x0b,0X2020);	  //设置音量	 
//		printf("mode:0x%x ;  \n",VS_RD_Reg(SPI_MODE));
		VS_Soft_Reset();  		//软复位 
	    VS_Reset_DecodeTime();	
		Play (testCharArr,48);//		
		while(1){			
			LED1=0;	
			//Play (Silent,32);//TEXT:2345  WAVE:123
			while(Text_EN>0){
				if(Receive_Over>0){ 				
// 				Audio_EN=0;
// 				Play_EN=0;
				USART_SendStr(USART_RX_BUF,(USART_RX_STA)&0X3FFF); 
				printf("\r\n\r\n");
				delay_ms(10);
				Text_EN=0;
				Receive_Over=0;
				}					
			}
			
			while(Play_EN>0)
			{
				if(Audio_EN>0){
					Audio_EN--;
					Play (USART_RX_BUF,14*1024);
				}
				LED1=!LED1;	
			}			
		}
	} 		  										    			    
}
