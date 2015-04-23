#include "mp3player.h"
#include "vs10xx.h"	 
#include "delay.h"
#include "led.h"
#include "key.h"
#include "lcd.h"		 
#include "malloc.h"
#include "text.h"
#include "string.h"
#include "exfuns.h"
#include "fattester.h"	 
#include "ff.h"   
#include "flac.h"	
#include "usart.h"	
// #define BMW_TEST 1
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//MP3播放驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved								  						    								  
//////////////////////////////////////////////////////////////////////////////////

//显示曲目索引
//index:当前索引
//total:总文件数
void mp3_index_show(u16 index,u16 total)
{
	//显示当前曲目的索引,及总曲目数
	LCD_ShowxNum(60+0,230,index,3,16,0X80);		//索引
	LCD_ShowChar(60+24,230,'/',16,0);
	LCD_ShowxNum(60+32,230,total,3,16,0X80); 	//总曲目				  	  
}
//显示当前音量
void mp3_vol_show(u8 vol)
{			    
	LCD_ShowString(60+110,230,200,16,16,"VOL:");	  	  
	LCD_ShowxNum(60+142,230,vol,2,16,0X80); 	//显示音量	 
}
u16 f_kbps=0;//歌曲文件位率	
//显示播放时间,比特率 信息 
//lenth:歌曲总长度
void mp3_msg_show(u32 lenth)
{	
	static u16 playtime=0;//播放时间标记	     
 	u16 time=0;// 时间变量
	u16 temp=0;	  
	if(f_kbps==0xffff)//未更新过
	{
		playtime=0;
		f_kbps=VS_Get_HeadInfo();	   //获得比特率
	}	 	 
	time=VS_Get_DecodeTime(); //得到已解码时间
	if(playtime==0)playtime=time;
	else if((time!=playtime)&&(time!=0))//1s时间到,更新显示数据
	{
		playtime=time;//更新时间 	 				    
		temp=VS_Get_HeadInfo(); //获得比特率	   				 
		if(temp!=f_kbps)
		{
			f_kbps=temp;//更新KbpS	  				     
		}			 
		//显示播放时间			 
		LCD_ShowxNum(60,210,time/60,2,16,0X80);		//分钟
		LCD_ShowChar(60+16,210,':',16,0);
		LCD_ShowxNum(60+24,210,time%60,2,16,0X80);	//秒钟		
 		LCD_ShowChar(60+40,210,'/',16,0); 	    	 
		//显示总时间
		if(f_kbps)time=(lenth/f_kbps)/125;//得到秒钟数   (文件长度(字节)/(1000/8)/比特率=持续秒钟数    	  
		else time=0;//非法位率	  
 		LCD_ShowxNum(60+48,210,time/60,2,16,0X80);	//分钟
		LCD_ShowChar(60+64,210,':',16,0);
		LCD_ShowxNum(60+72,210,time%60,2,16,0X80);	//秒钟	  		    
		//显示位率			   
   		LCD_ShowxNum(60+110,210,f_kbps,3,16,0X80); 	//显示位率	 
		LCD_ShowString(60+134,210,200,16,16,"Kbps");	  	  
		LED0=!LED0;		//DS0翻转
	}   		 
}			  		 
//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 mp3_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize = _MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname = mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存;返回值:分配到的内存首地址.
	if(res==FR_OK && (tfileinfo.lfname != NULL))
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件;tdir: Pointer to the open directory object /* tfileinfo: Pointer to file information to return */
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//取高四位,看看是不是音乐文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}
//播放音乐
void mp3_play(void)
{
	u8 res;
 	DIR mp3dir;	 		//目录
	FILINFO mp3fileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totmp3num; 		//音乐文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值		  
 	u16 temp;
	u16 *mp3indextbl;	//音乐索引表 
 	while(f_opendir(&mp3dir,"0:/MUSIC"))//打开文件夹
 	{	    
		Show_Str(60,190,240,16,"MUSIC文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//清除显示	     
		delay_ms(200);				  
	} 									  
	totmp3num=mp3_get_tnum("0:/MUSIC"); //得到总有效文件数
  	while(totmp3num==NULL)//音乐文件总数为0		
 	{	    
		Show_Str(60,190,240,16,"没有音乐文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}										   
  	mp3fileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	mp3fileinfo.lfname=mymalloc(SRAMIN,mp3fileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,mp3fileinfo.lfsize);				//为带路径的文件名分配内存
	
 	mp3indextbl=mymalloc(SRAMIN,2*totmp3num);				//申请2*totmp3num个字节的内存,用于存放音乐文件索引
 	while(mp3fileinfo.lfname==NULL||pname==NULL||mp3indextbl==NULL)//内存分配出错
 	{	    
		Show_Str(60,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	VS_HD_Reset();	
#if !defined  BMW_TEST
    printf("mode:0x%x ;  \n",VS_RD_Reg(SPI_MODE));  //mode:0x4800
#endif		
	VS_Soft_Reset();
	vsset.mvol=220;						//默认设置音量为220.
	mp3_vol_show((vsset.mvol-100)/5);	//音量限制在:100~250,显示的时候,按照公式(vol-100)/5,显示,也就是0~30   
	//记录索引
    res=f_opendir(&mp3dir,"0:/MUSIC"); //打开目录
#if !defined  BMW_TEST
//When Reset Print then never print again.
    printf("open 0:/MUSIC; 0 mean success: %d ;  \n",res);	//open 0:/MUSIC; 0 mean success: 0 ;  
#endif	
	if(res==FR_OK) //Play the music always then.
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=mp3dir.index;								//记录当前index
	        res=f_readdir(&mp3dir,&mp3fileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||mp3fileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*mp3fileinfo.lfname ? mp3fileinfo.lfname : mp3fileinfo.fname);			 
			res=f_typetell(fn);	 
			if((res&0XF0)==0X40)//取高四位,看看是不是音乐文件	
			{
				mp3indextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
   	curindex=0;											//从0开始显示
   	res=f_opendir(&mp3dir,(const TCHAR*)"0:/MUSIC"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		#if !defined  BMW_TEST
		/*when mp3_play_Restart_; */
			printf("mp3_play_Restart_; \n");	
		#endif			
		dir_sdi(&mp3dir,mp3indextbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&mp3dir,&mp3fileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||mp3fileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
     	fn=(u8*)(*mp3fileinfo.lfname?mp3fileinfo.lfname:mp3fileinfo.fname);//fn 指向文件的指针	 
		strcpy((char*)pname,"0:/MUSIC/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
 		LCD_Fill(60,190,240,190+16,WHITE);				//清除之前的显示
		Show_Str(60,190,240-60,16,fn,16,0);				//显示歌曲名字 
		mp3_index_show(curindex+1,totmp3num);
		
			#if !defined  BMW_TEST
			/*播放第1个MP3-0:/MUSIC/刘德华 - 爱你一万年.mp3;*/
				printf("播放第%d个MP3-%s; \n",curindex+1,pname);	
			#endif			
		key=mp3_play_song(pname); 				 		//播放这个MP3    
		if(key==2)		//上一曲
		{
			#if !defined  BMW_TEST
			/**/
				printf("mp3_play_SYQ; \n");	
			#endif							
			if(curindex)curindex--;
			else curindex=totmp3num-1;
 		}else if(key<=1)//下一曲
		{
			#if !defined  BMW_TEST
			/*自然播放结束时切歌也会输出*/
				printf("mp3_play_XYQ; \n");	
			#endif				
			curindex++;		   	
			if(curindex>=totmp3num)curindex=0;//到末尾的时候,自动从头开始
 		}else break;	//产生了错误 	 
	} 											  
	myfree(SRAMIN,mp3fileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,mp3indextbl);			//释放内存	
#if !defined  BMW_TEST
/*正常播放时，由于一直循环，所以以下永远不执行*/
    printf("最后释放内存; \n");	
#endif		
}

//播放一曲指定的歌曲				     	   									    	 
//返回值:0,正常播放完成
//		 1,下一曲
//       2,上一曲
//       0XFF,出现错误了
u8 mp3_play_song(u8 *pname)
{	 
 	FIL* fmp3;
    u16 br;
	u8 res,rval;	  
	u8 *databuf;	   		   
	u16 i=0; 
	u8 key;  	    
			   
	rval=0;	    
	fmp3=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//申请内存
	databuf=(u8*)mymalloc(SRAMIN,4096);		//开辟4096字节的内存区域
	if(databuf==NULL||fmp3==NULL)rval=0XFF ;//内存申请失败.
#if !defined  BMW_TEST
/*mp3_play_song: 0:/MUSIC/bingyu_4_21M.wma; */
    printf("mp3_play_song: %s; \n",pname);	
#endif	
	if(rval==0)
	{	  
	  	VS_Restart_Play();  					//重启播放 
#if !defined  BMW_TEST
/*After VS_Restart_Play -> mode:0x800 ;*/
    printf("After VS_Restart_Play -> mode:0x%x ;  \n",VS_RD_Reg(SPI_MODE));
#endif			
		VS_Set_All();        					//设置音量等信息 			 
		VS_Reset_DecodeTime();					//复位解码时间 	  
		res=f_typetell(pname);	 	 			//得到文件后缀	 			  	 						 
		if(res==0x4c)//如果是flac,加载patch
		{	
			VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
			printf("if flac,load patch.  \n");
		}  				 		   		 						  
		res=f_open(fmp3,(const TCHAR*)pname,FA_READ);//打开文件	 
 		if(res==0)//打开成功.
		{ 
			u16 j=0;
			VS_SPI_SpeedHigh();	//高速	
			#if !defined  BMW_TEST
			/*After VS_SPI_SpeedHigh -> mode:0x800 ;*/
				printf("After VS_SPI_SpeedHigh -> mode:0x%x ;  \n",VS_RD_Reg(SPI_MODE));
			#endif				
			while(rval==0)
			{
				res=f_read(fmp3,databuf,4096,(UINT*)&br);//读出4096个字节   *br	 /* Pointer to number of bytes read */
				i=0;
				#if !defined  BMW_TEST
				/**/
					printf("%d: READ_4096_bYTE;  \n",j);
				#endif	
				j++;
// 				if(j>99) j=0;
				do//主播放循环
			    {  	
					if(VS_Send_MusicData(databuf+i)==0)//给VS10XX发送音频数据
					{
						i+=32;
					}else   
					{
						key=KEY_Scan(0);
						switch(key)
						{
							case KEY_RIGHT:
								rval=1;		//下一曲
								break;
							case KEY_LEFT:
								rval=2;		//上一曲
								break;
							case KEY_UP:	//音量增加
								if(vsset.mvol<250)
								{
									vsset.mvol+=5;
						 			VS_Set_Vol(vsset.mvol);	
								}else vsset.mvol=250;
								mp3_vol_show((vsset.mvol-100)/5);	//音量限制在:100~250,显示的时候,按照公式(vol-100)/5,显示,也就是0~30   
								break;
							case KEY_DOWN:	//音量减
								if(vsset.mvol>100)
								{
									vsset.mvol-=5;
						 			VS_Set_Vol(vsset.mvol);	
								}else vsset.mvol=100;
								mp3_vol_show((vsset.mvol-100)/5);	//音量限制在:100~250,显示的时候,按照公式(vol-100)/5,显示,也就是0~30   
								break;
						}
						mp3_msg_show(fmp3->fsize);//显示信息	    fmp3->fsize是文件长度大小，位宽32bits
					}	    	    
				}while(i<4096);//循环发送4096个字节 
				if(br!=4096||res!=0)
				{
					rval=0;
					#if !defined  BMW_TEST
					/* Read Finished ; */
						printf(" Read Finished ; \n");
					#endif				
					break;//读完了.		  
				} 							 
			}
			f_close(fmp3);
		}else 
		{
			rval=0XFF;//出现错误	
			#if !defined  BMW_TEST
			/**/
				printf("rval=0XFF;//出现错误; \n");
			#endif				
		}
	}						     	 
	myfree(SRAMIN,databuf);	  	 		  	    
	myfree(SRAMIN,fmp3);
#if !defined  BMW_TEST
/*Free SRAM: databuf & fmp3;*/
    printf("Free SRAM: databuf & fmp3; \n");
#endif		
	return rval;	  	 		  	    
}

		void Receive_Play(u8 *datbuf)
		{
			u8 key;
			static int i  = 0;
			do{
				if(VS_Send_MusicData(datbuf+i)==0)//给VS10XX发送音频数据
				{
					i+=32;
				}		
					key=KEY_Scan(0);
					switch(key)
					{
						case KEY_UP:	//音量增加
							if(vsset.mvol<250)
							{
								vsset.mvol+=5;
								VS_Set_Vol(vsset.mvol);	
							}else vsset.mvol=250;
							LED0 = !LED0;
							mp3_vol_show((vsset.mvol-100)/5);
		// 					recoder_show_agc(recagc);
		// 					VS_WR_Cmd(SPI_AICTRL1,1024*recagc);	//设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍					  
							break;
						case KEY_DOWN:	//音量减
							if(vsset.mvol>100)
							{
								vsset.mvol-=5;
								VS_Set_Vol(vsset.mvol);	
							}else vsset.mvol=100;
							LED1=!LED1;					
							mp3_vol_show((vsset.mvol-100)/5);
		// 					recoder_show_agc(recagc);
		// 					VS_WR_Cmd(SPI_AICTRL1,1024*recagc);	//设置增益,0,自动增益.1024相当于1倍,512相当于0.5倍					
							break;
							

					}
				}while(i<512);
				if(i>512) i = 0;

		}

		









