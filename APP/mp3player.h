#ifndef __MP3PLAYER_H
#define __MP3PLAYER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//MP3�������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved								  						    								  
//////////////////////////////////////////////////////////////////////////////////
	
						  
void mp3_play(void);
u16 mp3_get_tnum(u8 *path);
u8 mp3_play_song(u8 *pname);
void mp3_index_show(u16 index,u16 total);
void mp3_msg_show(u32 lenth);
void Receive_Play(u8 *datbuf);
//��ʾ��ǰ����
void mp3_vol_show(u8 vol);
#endif











