#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮������������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
const u8 Wav_Data_head[64]={//128kbps	
	0x52, 0x49, 0x46, 0x46, 0x08, 0x08, 0x00, 0x10, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
	0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x02, 0x08, 0x00, 0x10, 0xCE, 0x39, 0x57, 0x39, 
	0xC9, 0x38, 0xEC, 0x39, 0xAC, 0x39, 0x42, 0x39, 0x6C, 0x39, 0x92, 0x39, 0xD4, 0x38, 0xBD, 0x37, 
};
unsigned char Mp3_Data_head[64] = {//32kbps
	0x49, 0x44, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x76, 0x54, 0x50, 0x45, 0x31, 0x00, 0x00, 
	0x00, 0x1A, 0x00, 0x00, 0x00, 0x35, 0x31, 0x56, 0x4F, 0x41, 0x2E, 0x43, 0x4F, 0x4D, 0xC3, 0xC0, 
	0xB9, 0xFA, 0xD6, 0xAE, 0xD2, 0xF4, 0xC3, 0xE2, 0xB7, 0xD1, 0xCF, 0xC2, 0xD4, 0xD8, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���  

u8 USART_RX_BUF[USART_REC_LEN];  //���ջ���,���USART_REC_LEN���ֽ�.
// ={	//һ��16��
// 	0x52, 0x49, 0x46, 0x46, 0x08, 0x08, 0x00, 0x10, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
// 	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
// 	0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x02, 0x08, 0x00, 0x10, 0xCE, 0x39, 0x57, 0x39, 
// 	0xC9, 0x38, 0xEC, 0x39, 0xAC, 0x39, 0x42, 0x39, 0x6C, 0x39, 0x92, 0x39, 0xD4, 0x38, 0xBD, 0x37, 
// 	0x03, 0x38, 0xA4, 0x38, 0xFC, 0x37, 0xDC, 0x37, 0x23, 0x38, 0x3A, 0x37, 0x85, 0x36, 0x78, 0x36, 
// 	0x44, 0x36, 0x52, 0x36, 0x36, 0x36, 0xDF, 0x35, 0xF8, 0x35, 0x30, 0x34, 0xA0, 0x32, 0x91, 0x31, 
// 	0x4A, 0x30, 0x81, 0x2F, 0xFF, 0x2D, 0x24, 0x2C, 0x64, 0x2B, 0xAB, 0x29, 0xD9, 0x28, 0xF2, 0x28, 
// 	0x1A, 0x28, 0x19, 0x28, 0xFC, 0x27, 0x58, 0x27, 0x93, 0x26, 0xE9, 0x25, 0x5E, 0x25, 0xBD, 0x24, 
// 	0x37, 0x24, 0x10, 0x24, 0x79, 0x24, 0x69, 0x24, 0xE6, 0x23, 0xBB, 0x24, 0x7C, 0x24, 0xD7, 0x23, 
// 	0x9D, 0x24, 0xBD, 0x25, 0xC6, 0x26, 0x43, 0x27, 0x4E, 0x28, 0xC4, 0x28, 0x23, 0x28, 0xB7, 0x28, 
// 	0x56, 0x29, 0xF0, 0x28, 0x82, 0x28, 0x70, 0x28, 0x41, 0x28, 0x2C, 0x28, 0x98, 0x27, 0x8E, 0x26, 
// 	0x63, 0x26, 0xB4, 0x26, 0xFC, 0x25, 0xEE, 0x25, 0x24, 0x26, 0xB1, 0x25, 0xD7, 0x24, 0xD5, 0x25, 
// 	0x5B, 0x25, 0xD4, 0x25, 0xE6, 0x25, 0x25, 0x26, 0xE7, 0x26, 0x54, 0x27, 0x00, 0x27, 0xDE, 0x26, 
// 	0x79, 0x27, 0x18, 0x27, 0x1E, 0x28, 0x7B, 0x29, 0xF7, 0x29, 0xDE, 0x2A, 0x07, 0x2C, 0xBD, 0x2B, 
// 	0x21, 0x2D, 0x08, 0x2E, 0xEE, 0x2E, 0x48, 0x2F, 0x2E, 0x30, 0x02, 0x32, 0x20, 0x32, 0xD7, 0x31, 
// 	0xF6, 0x32, 0x87, 0x33, 0xDD, 0x33, 0xC6, 0x34, 0x1B, 0x35, 0x9F, 0x34, 0x0B, 0x36, 0xDE, 0x35, 
// 	0xA4, 0x35, 0xB6, 0x35, 0xD0, 0x35, 0xF9, 0x35, 0x6C, 0x35, 0x64, 0x35, 0x00, 0x35, 0x89, 0x34, 
// 	0xF6, 0x35, 0x86, 0x35, 0x72, 0x34, 0x7C, 0x34, 0x93, 0x35, 0x47, 0x35, 0x5B, 0x35, 0xB9, 0x35, 
// 	0x73, 0x36, 0xA6, 0x36, 0x49, 0x37, 0x0E, 0x39, 0x19, 0x39, 0x73, 0x39, 0xE9, 0x39, 0x1E, 0x39, 
// 	0xD1, 0x39, 0xA6, 0x3A, 0xD1, 0x39, 0x27, 0x3A, 0x29, 0x3A, 0x1B, 0x3B, 0x9E, 0x3B, 0x6D, 0x3B, 
// 	0x09, 0x3C, 0x90, 0x3D, 0x9E, 0x3C, 0x57, 0x3C, 0x40, 0x3D, 0xB4, 0x3B, 0xB6, 0x3B, 0xFF, 0x3B, 
// 	0x20, 0x3A, 0xC2, 0x38, 0xE0, 0x37, 0x2E, 0x37, 0x6F, 0x36, 0xC2, 0x35, 0x88, 0x35, 0xCB, 0x34, 
// 	0x3A, 0x34, 0x82, 0x34, 0x72, 0x33, 0x06, 0x32, 0x1B, 0x32, 0x8C, 0x31, 0xFB, 0x2F, 0x09, 0x2F, 
// 	0x22, 0x2E, 0xFD, 0x2C, 0x18, 0x2D, 0x51, 0x2D, 0xCC, 0x2C, 0x26, 0x2E, 0x9E, 0x2E, 0x0E, 0x2D, 
// 	0x6A, 0x2D, 0xFB, 0x2D, 0xAC, 0x2D, 0xBF, 0x2D, 0x2B, 0x2E, 0x4D, 0x2E, 0x98, 0x2E, 0xC8, 0x2D, 
// 	0x22, 0x2D, 0x96, 0x2C, 0x1E, 0x2C, 0x6D, 0x2B, 0x76, 0x2B, 0x49, 0x2A, 0x48, 0x29, 0x0A, 0x29, 
// 	0x1E, 0x28, 0xBA, 0x26, 0xB7, 0x25, 0x43, 0x25, 0xC0, 0x23, 0x87, 0x22, 0x8C, 0x22, 0x65, 0x21, 
// 	0x47, 0x20, 0x0A, 0x20, 0x6C, 0x20, 0xA7, 0x1F, 0x79, 0x1F, 0x1C, 0x1F, 0x3C, 0x1D, 0x17, 0x1D, 
// 	0x91, 0x1D, 0xBD, 0x1C, 0x5E, 0x1D, 0xDA, 0x1D, 0x87, 0x1D, 0x9A, 0x1E, 0xAA, 0x1F, 0x98, 0x1F, 
// 	0xE0, 0x1F, 0xD9, 0x1F, 0xD4, 0x1F, 0x53, 0x20, 0x80, 0x20, 0xE5, 0x20, 0x1B, 0x21, 0xC3, 0x20, 
// 	0x4C, 0x21, 0x06, 0x21, 0xA8, 0x20, 0xD4, 0x20, 0x94, 0x21, 0x69, 0x21, 0x8F, 0x21, 0x43, 0x21, 
// 	0xE1, 0x20, 0x51, 0x21, 0x3A, 0x20, 0x52, 0x1F, 0xC0, 0x1F, 0xEC, 0x1E, 0x99, 0x1E, 0x5D, 0x1E, 
// 	0x67, 0x1E, 0xAC, 0x1E, 0xE5, 0x1F, 0x25, 0x20, 0x99, 0x20, 0x90, 0x22, 0x05, 0x22, 0x83, 0x22, 
// 	0xCE, 0x23, 0x12, 0x24, 0xD5, 0x24, 0xF9, 0x25, 0xC5, 0x25, 0xB0, 0x26, 0x3F, 0x27, 0x05, 0x27, 
// 	0xE3, 0x27, 0x69, 0x28, 0x98, 0x28, 0xE3, 0x29, 0x9A, 0x2A, 0x0B, 0x2B, 0xD3, 0x2C, 0x92, 0x2D, 
// 	0x84, 0x2E, 0x2A, 0x30, 0xB5, 0x30, 0xD2, 0x30, 0xC1, 0x31, 0xBC, 0x31, 0x65, 0x32, 0xED, 0x31, 
// 	0x99, 0x31, 0xAA, 0x31, 0xBD, 0x31, 0xB8, 0x30, 0x66, 0x30, 0x57, 0x30, 0x5B, 0x2F, 0x83, 0x2F, 
// 	0x4F, 0x30, 0x51, 0x30, 0xFC, 0x30, 0xEA, 0x30, 0x8E, 0x30, 0x30, 0x31, 0xA5, 0x31, 0x1C, 0x31, 
// 	0x1F, 0x32, 0x28, 0x33, 0x29, 0x33, 0xB8, 0x33, 0x0C, 0x35, 0x8E, 0x35, 0xAA, 0x35, 0x23, 0x37, 
// 	0xAC, 0x37, 0xEB, 0x36, 0x2B, 0x37, 0x10, 0x38, 0x13, 0x37, 0x8F, 0x37, 0x78, 0x37, 0x83, 0x36, 
// 	0x87, 0x36, 0xBF, 0x36, 0x71, 0x37, 0x2D, 0x37, 0xCE, 0x38, 0x1E, 0x38, 0x5A, 0x38, 0x32, 0x39, 
// 	0xD2, 0x38, 0x88, 0x38, 0x1C, 0x38, 0xAF, 0x36, 0x60, 0x35, 0x8A, 0x33, 0xC2, 0x30, 0x69, 0x2F, 
// 	0x18, 0x2E, 0xDF, 0x2C, 0x2F, 0x2C, 0x3A, 0x2B, 0x01, 0x2B, 0x85, 0x2B, 0xC4, 0x2B, 0xE1, 0x2B, 
// 	0xD7, 0x2C, 0x19, 0x2E, 0x99, 0x2D, 0x62, 0x2E, 0x30, 0x2F, 0x0A, 0x2E, 0xD5, 0x2C, 0xA7, 0x2D, 
// 	0x7D, 0x2C, 0xCB, 0x2B, 0xAF, 0x2B, 0x19, 0x2B, 0x47, 0x2A, 0xD1, 0x2A, 0xA4, 0x2A, 0x5F, 0x2A, 
// 	0x5B, 0x2B, 0xA7, 0x2B, 0xF0, 0x2B, 0x1C, 0x2C, 0x55, 0x2C, 0xCE, 0x2B, 0x49, 0x2B, 0xBA, 0x29, 
// 	0xFA, 0x28, 0x80, 0x27, 0x29, 0x26, 0x6E, 0x24, 0x19, 0x22, 0x73, 0x20, 0xD9, 0x1E, 0x99, 0x1D, 
// 	0x4D, 0x1C, 0x7A, 0x1B, 0x7B, 0x1B, 0x8A, 0x1A, 0xE2, 0x1A, 0xD1, 0x1B, 0x03, 0x1C, 0x5C, 0x1C, 
// 	0xE9, 0x1C, 0xA8, 0x1D, 0x54, 0x1E, 0x54, 0x1F, 0x99, 0x1F, 0xA4, 0x1F, 0xC1, 0x1F, 0xDA, 0x1F, 
// 	0x77, 0x1F, 0xDB, 0x20, 0x0C, 0x21, 0xEF, 0x20, 0x97, 0x20, 0xC7, 0x20, 0x4D, 0x20, 0x85, 0x20, 
// 	0xEF, 0x20, 0x35, 0x20, 0x70, 0x20, 0x9E, 0x20, 0xF4, 0x1F, 0xBD, 0x1F, 0x6F, 0x1E, 0x6F, 0x1E, 
// 	0x16, 0x1B, 0xFF, 0x18, 0x5F, 0x0E, 0xDD, 0x04, 0x70, 0xC7, 0x00, 0x80, 0x11, 0x82, 0x32, 0x52, 
// 	0xC8, 0x30, 0x20, 0x06, 0xFF, 0x7F, 0xF1, 0x66, 0x72, 0x54, 0x65, 0x10, 0xEF, 0x36, 0xFF, 0x7F, 
// 	0xC6, 0x45, 0x9A, 0xDD, 0xEC, 0xE4, 0xC5, 0x26, 0x9C, 0x65, 0x62, 0xD9, 0x00, 0x80, 0xAE, 0xFA, 
// 	0xFF, 0x7F, 0x0E, 0x79, 0x17, 0x11, 0x0C, 0xFC, 0x9A, 0x6A, 0xFF, 0x7F, 0xFF, 0x7F, 0xBC, 0x25, 
// 	0xC4, 0xDF, 0x00, 0x33, 0x5A, 0x64, 0x89, 0x39, 0x89, 0xF7, 0x95, 0xFA, 0xB2, 0x30, 0x7E, 0x62, 
// 	0x01, 0x78, 0xA0, 0x34, 0xC3, 0x10, 0x6C, 0x16, 0x81, 0x21, 0xBF, 0x38, 0x89, 0x1C, 0x3A, 0xE3, 
// 	0x04, 0xCC, 0x39, 0x08, 0xCF, 0x50, 0x92, 0x53, 0x31, 0x29, 0x39, 0x07, 0x3B, 0x1E, 0x55, 0x54, 
// 	0x36, 0x63, 0xBA, 0x2F, 0xEC, 0x11, 0x33, 0x3C, 0x8E, 0x64, 0x5F, 0x5C, 0xB9, 0x29, 0x58, 0x12, 
// 	0x28, 0x1A, 0x45, 0x43, 0x73, 0x4A, 0xA9, 0x2F, 0xFD, 0x17, 0x40, 0x1D, 0x78, 0x2E, 0xD8, 0x41, 
// 	0x16, 0x40, 0xBF, 0x29, 0xFF, 0x2A, 0xEF, 0x34, 0x95, 0x34, 0x58, 0x44, 0x1A, 0x3A, 0xF3, 0x27, 
// 	0x20, 0x31, 0xAC, 0x3F, 0xD0, 0x42, 0xE7, 0x34, 0x88, 0x28, 0xD3, 0x2A, 0xDA, 0x39, 0x82, 0x3B, 
// 	0x9B, 0x36, 0x31, 0x2F, 0x6C, 0x2C, 0xE0, 0x35, 0x9B, 0x3E, 0x9B, 0x34, 0xC0, 0x2E, 0x02, 0x2C, 
// 	0x70, 0x2E, 0xF0, 0x2F, 0x24, 0x2A, 0xF7, 0x2B, 0xEA, 0x2E, 0x2B, 0x2C, 0x16, 0x2C, 0xA4, 0x26, 
// 	0x88, 0x21, 0x01, 0x20, 0xA4, 0x24, 0xC7, 0x27, 0x03, 0x2A, 0x93, 0x2D, 0x64, 0x2A, 0xA2, 0x28, 
// 	0x41, 0x29, 0x4C, 0x28, 0x52, 0x28, 0xFF, 0x29, 0xD0, 0x29, 0x6F, 0x2E, 0x41, 0x2D, 0xB6, 0x23, 
// 	0xA9, 0x20, 0x92, 0x20, 0x0F, 0x1D, 0xCD, 0x1E, 0x99, 0x1A, 0x06, 0x14, 0x47, 0x16, 0x39, 0x17, 
// 	0x34, 0x12, 0xA6, 0x12, 0x55, 0x11, 0xDF, 0x13, 0x98, 0x1B, 0x10, 0x1E, 0xE2, 0x1A, 0x9C, 0x19, 
// 	0x5E, 0x17, 0xC6, 0x19, 0x7C, 0x1E, 0xCA, 0x1B, 0x94, 0x1B, 0x83, 0x1B, 0xED, 0x19, 0x5D, 0x1C, 
// 	0x2A, 0x1B, 0x92, 0x17, 0xB9, 0x1A, 0xF8, 0x1E, 0xE3, 0x23, 0x3D, 0x26, 0xEF, 0x24, 0x5C, 0x23, 
// 	0x89, 0x23, 0xAF, 0x24, 0x8F, 0x26, 0x11, 0x27, 0x3F, 0x25, 0x31, 0x23, 0xA7, 0x24, 0x1C, 0x22, 
// 	0x3D, 0x1E, 0xFC, 0x1B, 0x9D, 0x17, 0x39, 0x18, 0xBA, 0x1B, 0x6D, 0x1B, 0x50, 0x19, 0xE1, 0x17, 
// 	0x2D, 0x16, 0xAE, 0x17, 0x53, 0x1A, 0x29, 0x1A, 0x2F, 0x1A, 0x86, 0x1C, 0xEE, 0x1C, 0x0C, 0x1F, 
// 	0x0E, 0x1F, 0x68, 0x1D, 0xC4, 0x1C, 0xBA, 0x1E, 0xCE, 0x1F, 0x8C, 0x21, 0x93, 0x20, 0x0E, 0x1F, 
// 	0x26, 0x1F, 0x67, 0x1F, 0x0B, 0x22, 0x0A, 0x23, 0x8F, 0x24, 0x1D, 0x26, 0xED, 0x27, 0x08, 0x29, 
// 	0xB9, 0x28, 0x6D, 0x2A, 0xC4, 0x29, 0x28, 0x2B, 0xFF, 0x2E, 0x67, 0x2D, 0x8C, 0x2B, 0xDC, 0x29, 
// 	0xE1, 0x27, 0xD8, 0x26, 0xDA, 0x26, 0xAA, 0x26, 0x1E, 0x26, 0xA4, 0x25, 0x23, 0x25, 0xF6, 0x26, 
// 	0xBD, 0x26, 0xA3, 0x24, 0x8C, 0x27, 0xB0, 0x28, 0xA7, 0x29, 0xD5, 0x2C, 0x9B, 0x2E, 0x1E, 0x2E, 
// 	0x65, 0x30, 0x10, 0x32, 0x67, 0x31, 0xE1, 0x32, 0x0D, 0x34, 0x04, 0x34, 0xE1, 0x34, 0xA8, 0x34, 
// 	0x03, 0x34, 0x45, 0x35, 0x85, 0x34, 0x94, 0x34, 0x68, 0x37, 0xE8, 0x37, 0x55, 0x37, 0x2C, 0x39, 
// 	0x76, 0x38, 0xC5, 0x36, 0x11, 0x39, 0xB8, 0x38, 0xC2, 0x37, 0x5F, 0x38, 0x8D, 0x37, 0xCD, 0x35, 
// 	0xF3, 0x34, 0x49, 0x34, 0x4F, 0x33, 0x0B, 0x33, 0xB7, 0x30, 0x7A, 0x32, 0x4B, 0x31, 0xBB, 0x30, 
// 	0xE4, 0x32, 0x21, 0x31, 0xD1, 0x31, 0x0E, 0x33, 0xBC, 0x34, 0x9E, 0x35, 0x94, 0x36, 0x63, 0x35, 
// 	0x2E, 0x38, 0xA9, 0x38, 0x44, 0x37, 0xEC, 0x3A, 0x41, 0x3A, 0xE7, 0x39, 0x64, 0x3C, 0x86, 0x3B, 
// 	0x90, 0x3B, 0x8A, 0x3C, 0x1D, 0x3C, 0x80, 0x3C, 0x22, 0x3E, 0x03, 0x3E, 0x5F, 0x3D, 0x0D, 0x3D, 
// 	0xFE, 0x3C, 0x9C, 0x3C, 0xFF, 0x3C, 0xD7, 0x3A, 0x6F, 0x39, 0x40, 0x39, 0xB9, 0x36, 0x4F, 0x36, 
// 	0xF3, 0x34, 0x21, 0x32, 0x51, 0x32, 0x68, 0x31, 0xFD, 0x2E, 0x0F, 0x30, 0xE0, 0x2E, 0xF0, 0x2D, 
// 	0xB1, 0x2D, 0xFF, 0x2E, 0xDD, 0x2E, 0x97, 0x2E, 0x8A, 0x2F, 0xCE, 0x30, 0x19, 0x30, 0xD3, 0x30, 
// 	0x31, 0x32, 0x85, 0x32, 0xE7, 0x32, 0xB0, 0x33, 0xCA, 0x33, 0xE0, 0x32, 0x5B, 0x31, 0xDF, 0x32, 
// 	0xC0, 0x32, 0xE4, 0x31, 0x67, 0x31, 0xD4, 0x30, 0x50, 0x30, 0x08, 0x31, 0xB2, 0x30, 0xB5, 0x30, 
// 	0xB5, 0x30, 0x2B, 0x30, 0x8C, 0x30, 0xCF, 0x2D, 0x63, 0x2D, 0xB5, 0x2C, 0x21, 0x2B, 0xAC, 0x29, 
// 	0x33, 0x29, 0xA4, 0x28, 0xC7, 0x25, 0x15, 0x26, 0x30, 0x25, 0xAF, 0x26, 0x52, 0x26, 0x54, 0x26, 
// 	0x78, 0x28, 0x0E, 0x29, 0x82, 0x29, 0xA5, 0x2A, 0xB8, 0x2B, 0x2E, 0x2C, 0xFA, 0x2C, 0xEF, 0x2D, 
// 	0x7A, 0x2D, 0x31, 0x2E, 0x95, 0x2F, 0xA1, 0x31, 0x64, 0x31, 0x10, 0x32, 0x6E, 0x33, 0x5F, 0x32, 
// 	0x2F, 0x33, 0x5B, 0x33, 0xF0, 0x32, 0xC3, 0x32, 0x5C, 0x32, 0xDA, 0x31, 0x64, 0x32, 0x01, 0x32, 
// 	0x3D, 0x32, 0x5D, 0x33, 0x82, 0x32, 0x92, 0x33, 0xDA, 0x32, 0xD9, 0x31, 0xFE, 0x30, 0x2F, 0x31, 
// 	0x9F, 0x30, 0x15, 0x30, 0xE3, 0x2E, 0x56, 0x2E, 0xA6, 0x2D, 0x74, 0x2D, 0xFD, 0x2E, 0x51, 0x2E, 
// 	0x5B, 0x2F, 0x9A, 0x31, 0x3D, 0x33, 0x0F, 0x34, 0x16, 0x35, 0xE3, 0x35, 0x1E, 0x36, 0x85, 0x37, 
// 	0xFD, 0x38, 0xB3, 0x38, 0x40, 0x39, 0xB6, 0x39, 0x78, 0x37, 0xF2, 0x38, 0x02, 0x3A, 0xEE, 0x39, 
// 	0x99, 0x39, 0x77, 0x39, 0xC2, 0x3A, 0x0D, 0x3A, 0xF7, 0x37, 0x07, 0x39, 0x64, 0x38, 0xB3, 0x37, 
// 	0x40, 0x37, 0xF7, 0x35, 0x31, 0x35, 0x87, 0x34, 0xED, 0x32, 0x5F, 0x33, 0x11, 0x31, 0x05, 0x2F, 
// 	0xB3, 0x2F, 0xC0, 0x2D, 0xE9, 0x2D, 0xEA, 0x2C, 0x77, 0x2B, 0x1D, 0x2C, 0x25, 0x2B, 0x19, 0x2C, 
// 	0x67, 0x2C, 0xC2, 0x2C, 0x3C, 0x2C, 0x51, 0x2C, 0x56, 0x2D, 0xA5, 0x2D, 0x44, 0x2C, 0xF4, 0x2D, 
// 	0x39, 0x2E, 0x48, 0x2E, 0x3A, 0x2D, 0x04, 0x2D, 0xCF, 0x2B, 0x54, 0x2C, 0xF9, 0x2C, 0xF1, 0x2B, 
// 	0xDB, 0x2B, 0xA0, 0x29, 0x72, 0x29, 0xCF, 0x28, 0x35, 0x28, 0x24, 0x26, 0x36, 0x25, 0x10, 0x24, 
// 	0x5A, 0x23, 0x7B, 0x21, 0xCE, 0x1F, 0x13, 0x1F, 0x56, 0x1E, 0x73, 0x1D, 0x65, 0x1D, 0x53, 0x1B, 
// 	0xE7, 0x18, 0x95, 0x17, 0x56, 0x16, 0x3B, 0x15, 0xA4, 0x14, 0x4B, 0x14, 0x83, 0x14, 0xD3, 0x14, 
// 	0xF5, 0x15, 0xB2, 0x15, 0x63, 0x16, 0xAD, 0x18, 0xDA, 0x18, 0x88, 0x1A, 0xA0, 0x1C, 0x02, 0x1C, 
// 	0x21, 0x1B, 0xCC, 0x1B, 0x31, 0x1A, 0xF8, 0x19, 0x38, 0x1A, 0x79, 0x1A, 0xDC, 0x19, 0x0F, 0x1B, 
// 	0x3B, 0x1A, 0x52, 0x17, 0x20, 0x1A, 0x10, 0x1A, 0x77, 0x19, 0xC0, 0x1A, 0x23, 0x1A, 0x03, 0x17, 
// 	0x5C, 0x16, 0x0A, 0x15, 0xA1, 0x13, 0xCC, 0x13, 0x22, 0x11, 0x82, 0x11, 0x22, 0x11, 0x5B, 0x0F, 
// 	0x88, 0x0F, 0x1F, 0x0F, 0x90, 0x0F, 0x4E, 0x10, 0x2B, 0x12, 0xDB, 0x11, 0xE9, 0x11, 0x8F, 0x14, 
// 	0x61, 0x14, 0xA3, 0x15, 0xC8, 0x16, 0x38, 0x19, 0x58, 0x1A, 0x49, 0x1C, 0xB5, 0x1D, 0x12, 0x1F, 
// 	0x0B, 0x1F, 0x73, 0x1F, 0x73, 0x21, 0x42, 0x21, 0xAB, 0x23, 0x3C, 0x23, 0x92, 0x23, 0xDB, 0x24, 
// 	0x62, 0x24, 0xDB, 0x24, 0xCF, 0x24, 0xC7, 0x25, 0xD2, 0x25, 0xAC, 0x23, 0x49, 0x24, 0xE1, 0x22, 
// 	0x39, 0x21, 0xA8, 0x21, 0x4B, 0x21, 0xC4, 0x1F, 0x75, 0x21, 0xF6, 0x20, 0xD7, 0x20, 0x08, 0x21, 
// 	0x8B, 0x20, 0x6A, 0x20, 0x3D, 0x20, 0xA6, 0x21, 0x44, 0x22, 0x5F, 0x23, 0x13, 0x23, 0xEC, 0x22, 
// 	0x5F, 0x23, 0x36, 0x24, 0x22, 0x26, 0x66, 0x28, 0x5B, 0x2A, 0x93, 0x2B, 0x43, 0x2D, 0x6A, 0x2E, 
// 	0x15, 0x2F, 0x28, 0x2E, 0xA8, 0x2E, 0x58, 0x2F, 0xC2, 0x2F, 0x91, 0x2E, 0xA3, 0x2F, 0x90, 0x2E, 
// 	0x31, 0x2E, 0x87, 0x2E, 0xE9, 0x2F, 0xF6, 0x2F, 0x95, 0x30, 0x43, 0x30, 0xFA, 0x2D, 0xB3, 0x2D, 
// 	0x24, 0x2A, 0x36, 0x28, 0x8A, 0x27, 0x36, 0x26, 0x6B, 0x25, 0xD4, 0x24, 0x8E, 0x22, 0x1D, 0x23, 
// 	0xEB, 0x22, 0x77, 0x23, 0x2F, 0x25, 0x65, 0x23, 0xC0, 0x24, 0x49, 0x24, 0x58, 0x26, 0x3D, 0x25, 
// 	0x0C, 0x25, 0x72, 0x25, 0x21, 0x28, 0x3A, 0x27, 0xB3, 0x26, 0x07, 0x27, 0x21, 0x27, 0x1B, 0x28, 
// 	0xEF, 0x27, 0xED, 0x27, 0x76, 0x25, 0xC5, 0x24, 0x7F, 0x23, 0x48, 0x24, 0x07, 0x23, 0xC2, 0x22, 
// 	0x03, 0x24, 0x27, 0x24, 0xFE, 0x23, 0x1A, 0x26, 0x22, 0x23, 0x44, 0x22, 0x74, 0x20, 0x7C, 0x1E, 
// 	0x10, 0x1F, 0x34, 0x1C, 0x29, 0x1B, 0x97, 0x19, 0xAB, 0x17, 0xE9, 0x14, 0x9B, 0x15, 0x78, 0x13, 
// 	0x23, 0x13, 0xBF, 0x13, 0x8B, 0x12, 0xDB, 0x12, 0x94, 0x12, 0x09, 0x12, 0x52, 0x12, 0x95, 0x13, 
// 	0x2A, 0x14, 0xD9, 0x14, 0x59, 0x14, 0x8B, 0x14, 0x76, 0x15, 0xDE, 0x14, 0x77, 0x15, 0x6A, 0x17,
// 	
// 	0x52, 0x49, 0x46, 0x46, 0x08, 0x08, 0x00, 0x10, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20, 
// 	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00, 
// 	0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61, 0x02, 0x08, 0x00, 0x10, 0xCE, 0x39, 0x57, 0x39, 
// 	0xC9, 0x38, 0xEC, 0x39, 0xAC, 0x39, 0x42, 0x39, 0x6C, 0x39, 0x92, 0x39, 0xD4, 0x38, 0xBD, 0x37, 
// 	0x03, 0x38, 0xA4, 0x38, 0xFC, 0x37, 0xDC, 0x37, 0x23, 0x38, 0x3A, 0x37, 0x85, 0x36, 0x78, 0x36, 
// 	0x44, 0x36, 0x52, 0x36, 0x36, 0x36, 0xDF, 0x35, 0xF8, 0x35, 0x30, 0x34, 0xA0, 0x32, 0x91, 0x31, 
// 	0x4A, 0x30, 0x81, 0x2F, 0xFF, 0x2D, 0x24, 0x2C, 0x64, 0x2B, 0xAB, 0x29, 0xD9, 0x28, 0xF2, 0x28, 
// 	0x1A, 0x28, 0x19, 0x28, 0xFC, 0x27, 0x58, 0x27, 0x93, 0x26, 0xE9, 0x25, 0x5E, 0x25, 0xBD, 0x24, 
// 	0x37, 0x24, 0x10, 0x24, 0x79, 0x24, 0x69, 0x24, 0xE6, 0x23, 0xBB, 0x24, 0x7C, 0x24, 0xD7, 0x23, 
// 	0x9D, 0x24, 0xBD, 0x25, 0xC6, 0x26, 0x43, 0x27, 0x4E, 0x28, 0xC4, 0x28, 0x23, 0x28, 0xB7, 0x28, 
// 	0x56, 0x29, 0xF0, 0x28, 0x82, 0x28, 0x70, 0x28, 0x41, 0x28, 0x2C, 0x28, 0x98, 0x27, 0x8E, 0x26, 
// 	0x63, 0x26, 0xB4, 0x26, 0xFC, 0x25, 0xEE, 0x25, 0x24, 0x26, 0xB1, 0x25, 0xD7, 0x24, 0xD5, 0x25, 
// 	0x5B, 0x25, 0xD4, 0x25, 0xE6, 0x25, 0x25, 0x26, 0xE7, 0x26, 0x54, 0x27, 0x00, 0x27, 0xDE, 0x26, 
// 	0x79, 0x27, 0x18, 0x27, 0x1E, 0x28, 0x7B, 0x29, 0xF7, 0x29, 0xDE, 0x2A, 0x07, 0x2C, 0xBD, 0x2B, 
// 	0x21, 0x2D, 0x08, 0x2E, 0xEE, 0x2E, 0x48, 0x2F, 0x2E, 0x30, 0x02, 0x32, 0x20, 0x32, 0xD7, 0x31, 
// 	0xF6, 0x32, 0x87, 0x33, 0xDD, 0x33, 0xC6, 0x34, 0x1B, 0x35, 0x9F, 0x34, 0x0B, 0x36, 0xDE, 0x35, 
// 	0xA4, 0x35, 0xB6, 0x35, 0xD0, 0x35, 0xF9, 0x35, 0x6C, 0x35, 0x64, 0x35, 0x00, 0x35, 0x89, 0x34, 
// 	0xF6, 0x35, 0x86, 0x35, 0x72, 0x34, 0x7C, 0x34, 0x93, 0x35, 0x47, 0x35, 0x5B, 0x35, 0xB9, 0x35, 
// 	0x73, 0x36, 0xA6, 0x36, 0x49, 0x37, 0x0E, 0x39, 0x19, 0x39, 0x73, 0x39, 0xE9, 0x39, 0x1E, 0x39, 
// 	0xD1, 0x39, 0xA6, 0x3A, 0xD1, 0x39, 0x27, 0x3A, 0x29, 0x3A, 0x1B, 0x3B, 0x9E, 0x3B, 0x6D, 0x3B, 
// 	0x09, 0x3C, 0x90, 0x3D, 0x9E, 0x3C, 0x57, 0x3C, 0x40, 0x3D, 0xB4, 0x3B, 0xB6, 0x3B, 0xFF, 0x3B, 
// 	0x20, 0x3A, 0xC2, 0x38, 0xE0, 0x37, 0x2E, 0x37, 0x6F, 0x36, 0xC2, 0x35, 0x88, 0x35, 0xCB, 0x34, 
// 	0x3A, 0x34, 0x82, 0x34, 0x72, 0x33, 0x06, 0x32, 0x1B, 0x32, 0x8C, 0x31, 0xFB, 0x2F, 0x09, 0x2F, 
// 	0x22, 0x2E, 0xFD, 0x2C, 0x18, 0x2D, 0x51, 0x2D, 0xCC, 0x2C, 0x26, 0x2E, 0x9E, 0x2E, 0x0E, 0x2D, 
// 	0x6A, 0x2D, 0xFB, 0x2D, 0xAC, 0x2D, 0xBF, 0x2D, 0x2B, 0x2E, 0x4D, 0x2E, 0x98, 0x2E, 0xC8, 0x2D, 
// 	0x22, 0x2D, 0x96, 0x2C, 0x1E, 0x2C, 0x6D, 0x2B, 0x76, 0x2B, 0x49, 0x2A, 0x48, 0x29, 0x0A, 0x29, 
// 	0x1E, 0x28, 0xBA, 0x26, 0xB7, 0x25, 0x43, 0x25, 0xC0, 0x23, 0x87, 0x22, 0x8C, 0x22, 0x65, 0x21, 
// 	0x47, 0x20, 0x0A, 0x20, 0x6C, 0x20, 0xA7, 0x1F, 0x79, 0x1F, 0x1C, 0x1F, 0x3C, 0x1D, 0x17, 0x1D, 
// 	0x91, 0x1D, 0xBD, 0x1C, 0x5E, 0x1D, 0xDA, 0x1D, 0x87, 0x1D, 0x9A, 0x1E, 0xAA, 0x1F, 0x98, 0x1F, 
// 	0xE0, 0x1F, 0xD9, 0x1F, 0xD4, 0x1F, 0x53, 0x20, 0x80, 0x20, 0xE5, 0x20, 0x1B, 0x21, 0xC3, 0x20, 
// 	0x4C, 0x21, 0x06, 0x21, 0xA8, 0x20, 0xD4, 0x20, 0x94, 0x21, 0x69, 0x21, 0x8F, 0x21, 0x43, 0x21, 
// 	0xE1, 0x20, 0x51, 0x21, 0x3A, 0x20, 0x52, 0x1F, 0xC0, 0x1F, 0xEC, 0x1E, 0x99, 0x1E, 0x5D, 0x1E, 
// 	0x67, 0x1E, 0xAC, 0x1E, 0xE5, 0x1F, 0x25, 0x20, 0x99, 0x20, 0x90, 0x22, 0x05, 0x22, 0x83, 0x22, 
// 	0xCE, 0x23, 0x12, 0x24, 0xD5, 0x24, 0xF9, 0x25, 0xC5, 0x25, 0xB0, 0x26, 0x3F, 0x27, 0x05, 0x27, 
// 	0xE3, 0x27, 0x69, 0x28, 0x98, 0x28, 0xE3, 0x29, 0x9A, 0x2A, 0x0B, 0x2B, 0xD3, 0x2C, 0x92, 0x2D, 
// 	0x84, 0x2E, 0x2A, 0x30, 0xB5, 0x30, 0xD2, 0x30, 0xC1, 0x31, 0xBC, 0x31, 0x65, 0x32, 0xED, 0x31, 
// 	0x99, 0x31, 0xAA, 0x31, 0xBD, 0x31, 0xB8, 0x30, 0x66, 0x30, 0x57, 0x30, 0x5B, 0x2F, 0x83, 0x2F, 
// 	0x4F, 0x30, 0x51, 0x30, 0xFC, 0x30, 0xEA, 0x30, 0x8E, 0x30, 0x30, 0x31, 0xA5, 0x31, 0x1C, 0x31, 
// 	0x1F, 0x32, 0x28, 0x33, 0x29, 0x33, 0xB8, 0x33, 0x0C, 0x35, 0x8E, 0x35, 0xAA, 0x35, 0x23, 0x37, 
// 	0xAC, 0x37, 0xEB, 0x36, 0x2B, 0x37, 0x10, 0x38, 0x13, 0x37, 0x8F, 0x37, 0x78, 0x37, 0x83, 0x36, 
// 	0x87, 0x36, 0xBF, 0x36, 0x71, 0x37, 0x2D, 0x37, 0xCE, 0x38, 0x1E, 0x38, 0x5A, 0x38, 0x32, 0x39, 
// 	0xD2, 0x38, 0x88, 0x38, 0x1C, 0x38, 0xAF, 0x36, 0x60, 0x35, 0x8A, 0x33, 0xC2, 0x30, 0x69, 0x2F, 
// 	0x18, 0x2E, 0xDF, 0x2C, 0x2F, 0x2C, 0x3A, 0x2B, 0x01, 0x2B, 0x85, 0x2B, 0xC4, 0x2B, 0xE1, 0x2B, 
// 	0xD7, 0x2C, 0x19, 0x2E, 0x99, 0x2D, 0x62, 0x2E, 0x30, 0x2F, 0x0A, 0x2E, 0xD5, 0x2C, 0xA7, 0x2D, 
// 	0x7D, 0x2C, 0xCB, 0x2B, 0xAF, 0x2B, 0x19, 0x2B, 0x47, 0x2A, 0xD1, 0x2A, 0xA4, 0x2A, 0x5F, 0x2A, 
// 	0x5B, 0x2B, 0xA7, 0x2B, 0xF0, 0x2B, 0x1C, 0x2C, 0x55, 0x2C, 0xCE, 0x2B, 0x49, 0x2B, 0xBA, 0x29, 
// 	0xFA, 0x28, 0x80, 0x27, 0x29, 0x26, 0x6E, 0x24, 0x19, 0x22, 0x73, 0x20, 0xD9, 0x1E, 0x99, 0x1D, 
// 	0x4D, 0x1C, 0x7A, 0x1B, 0x7B, 0x1B, 0x8A, 0x1A, 0xE2, 0x1A, 0xD1, 0x1B, 0x03, 0x1C, 0x5C, 0x1C, 
// 	0xE9, 0x1C, 0xA8, 0x1D, 0x54, 0x1E, 0x54, 0x1F, 0x99, 0x1F, 0xA4, 0x1F, 0xC1, 0x1F, 0xDA, 0x1F, 
// 	0x77, 0x1F, 0xDB, 0x20, 0x0C, 0x21, 0xEF, 0x20, 0x97, 0x20, 0xC7, 0x20, 0x4D, 0x20, 0x85, 0x20, 
// 	0xEF, 0x20, 0x35, 0x20, 0x70, 0x20, 0x9E, 0x20, 0xF4, 0x1F, 0xBD, 0x1F, 0x6F, 0x1E, 0x6F, 0x1E, 
// 	0x16, 0x1B, 0xFF, 0x18, 0x5F, 0x0E, 0xDD, 0x04, 0x70, 0xC7, 0x00, 0x80, 0x11, 0x82, 0x32, 0x52, 
// 	0xC8, 0x30, 0x20, 0x06, 0xFF, 0x7F, 0xF1, 0x66, 0x72, 0x54, 0x65, 0x10, 0xEF, 0x36, 0xFF, 0x7F, 
// 	0xC6, 0x45, 0x9A, 0xDD, 0xEC, 0xE4, 0xC5, 0x26, 0x9C, 0x65, 0x62, 0xD9, 0x00, 0x80, 0xAE, 0xFA, 
// 	0xFF, 0x7F, 0x0E, 0x79, 0x17, 0x11, 0x0C, 0xFC, 0x9A, 0x6A, 0xFF, 0x7F, 0xFF, 0x7F, 0xBC, 0x25, 
// 	0xC4, 0xDF, 0x00, 0x33, 0x5A, 0x64, 0x89, 0x39, 0x89, 0xF7, 0x95, 0xFA, 0xB2, 0x30, 0x7E, 0x62, 
// 	0x01, 0x78, 0xA0, 0x34, 0xC3, 0x10, 0x6C, 0x16, 0x81, 0x21, 0xBF, 0x38, 0x89, 0x1C, 0x3A, 0xE3, 
// 	0x04, 0xCC, 0x39, 0x08, 0xCF, 0x50, 0x92, 0x53, 0x31, 0x29, 0x39, 0x07, 0x3B, 0x1E, 0x55, 0x54, 
// 	0x36, 0x63, 0xBA, 0x2F, 0xEC, 0x11, 0x33, 0x3C, 0x8E, 0x64, 0x5F, 0x5C, 0xB9, 0x29, 0x58, 0x12, 
// 	0x28, 0x1A, 0x45, 0x43, 0x73, 0x4A, 0xA9, 0x2F, 0xFD, 0x17, 0x40, 0x1D, 0x78, 0x2E, 0xD8, 0x41, 
// 	0x16, 0x40, 0xBF, 0x29, 0xFF, 0x2A, 0xEF, 0x34, 0x95, 0x34, 0x58, 0x44, 0x1A, 0x3A, 0xF3, 0x27, 
// 	0x20, 0x31, 0xAC, 0x3F, 0xD0, 0x42, 0xE7, 0x34, 0x88, 0x28, 0xD3, 0x2A, 0xDA, 0x39, 0x82, 0x3B, 
// 	0x9B, 0x36, 0x31, 0x2F, 0x6C, 0x2C, 0xE0, 0x35, 0x9B, 0x3E, 0x9B, 0x34, 0xC0, 0x2E, 0x02, 0x2C, 
// 	0x70, 0x2E, 0xF0, 0x2F, 0x24, 0x2A, 0xF7, 0x2B, 0xEA, 0x2E, 0x2B, 0x2C, 0x16, 0x2C, 0xA4, 0x26, 
// 	0x88, 0x21, 0x01, 0x20, 0xA4, 0x24, 0xC7, 0x27, 0x03, 0x2A, 0x93, 0x2D, 0x64, 0x2A, 0xA2, 0x28, 
// 	0x41, 0x29, 0x4C, 0x28, 0x52, 0x28, 0xFF, 0x29, 0xD0, 0x29, 0x6F, 0x2E, 0x41, 0x2D, 0xB6, 0x23, 
// 	0xA9, 0x20, 0x92, 0x20, 0x0F, 0x1D, 0xCD, 0x1E, 0x99, 0x1A, 0x06, 0x14, 0x47, 0x16, 0x39, 0x17, 
// 	0x34, 0x12, 0xA6, 0x12, 0x55, 0x11, 0xDF, 0x13, 0x98, 0x1B, 0x10, 0x1E, 0xE2, 0x1A, 0x9C, 0x19, 
// 	0x5E, 0x17, 0xC6, 0x19, 0x7C, 0x1E, 0xCA, 0x1B, 0x94, 0x1B, 0x83, 0x1B, 0xED, 0x19, 0x5D, 0x1C, 
// 	0x2A, 0x1B, 0x92, 0x17, 0xB9, 0x1A, 0xF8, 0x1E, 0xE3, 0x23, 0x3D, 0x26, 0xEF, 0x24, 0x5C, 0x23, 
// 	0x89, 0x23, 0xAF, 0x24, 0x8F, 0x26, 0x11, 0x27, 0x3F, 0x25, 0x31, 0x23, 0xA7, 0x24, 0x1C, 0x22, 
// 	0x3D, 0x1E, 0xFC, 0x1B, 0x9D, 0x17, 0x39, 0x18, 0xBA, 0x1B, 0x6D, 0x1B, 0x50, 0x19, 0xE1, 0x17, 
// 	0x2D, 0x16, 0xAE, 0x17, 0x53, 0x1A, 0x29, 0x1A, 0x2F, 0x1A, 0x86, 0x1C, 0xEE, 0x1C, 0x0C, 0x1F, 
// 	0x0E, 0x1F, 0x68, 0x1D, 0xC4, 0x1C, 0xBA, 0x1E, 0xCE, 0x1F, 0x8C, 0x21, 0x93, 0x20, 0x0E, 0x1F, 
// 	0x26, 0x1F, 0x67, 0x1F, 0x0B, 0x22, 0x0A, 0x23, 0x8F, 0x24, 0x1D, 0x26, 0xED, 0x27, 0x08, 0x29, 
// 	0xB9, 0x28, 0x6D, 0x2A, 0xC4, 0x29, 0x28, 0x2B, 0xFF, 0x2E, 0x67, 0x2D, 0x8C, 0x2B, 0xDC, 0x29, 
// 	0xE1, 0x27, 0xD8, 0x26, 0xDA, 0x26, 0xAA, 0x26, 0x1E, 0x26, 0xA4, 0x25, 0x23, 0x25, 0xF6, 0x26, 
// 	0xBD, 0x26, 0xA3, 0x24, 0x8C, 0x27, 0xB0, 0x28, 0xA7, 0x29, 0xD5, 0x2C, 0x9B, 0x2E, 0x1E, 0x2E, 
// 	0x65, 0x30, 0x10, 0x32, 0x67, 0x31, 0xE1, 0x32, 0x0D, 0x34, 0x04, 0x34, 0xE1, 0x34, 0xA8, 0x34, 
// 	0x03, 0x34, 0x45, 0x35, 0x85, 0x34, 0x94, 0x34, 0x68, 0x37, 0xE8, 0x37, 0x55, 0x37, 0x2C, 0x39, 
// 	0x76, 0x38, 0xC5, 0x36, 0x11, 0x39, 0xB8, 0x38, 0xC2, 0x37, 0x5F, 0x38, 0x8D, 0x37, 0xCD, 0x35, 
// 	0xF3, 0x34, 0x49, 0x34, 0x4F, 0x33, 0x0B, 0x33, 0xB7, 0x30, 0x7A, 0x32, 0x4B, 0x31, 0xBB, 0x30, 
// 	0xE4, 0x32, 0x21, 0x31, 0xD1, 0x31, 0x0E, 0x33, 0xBC, 0x34, 0x9E, 0x35, 0x94, 0x36, 0x63, 0x35, 
// 	0x2E, 0x38, 0xA9, 0x38, 0x44, 0x37, 0xEC, 0x3A, 0x41, 0x3A, 0xE7, 0x39, 0x64, 0x3C, 0x86, 0x3B, 
// 	0x90, 0x3B, 0x8A, 0x3C, 0x1D, 0x3C, 0x80, 0x3C, 0x22, 0x3E, 0x03, 0x3E, 0x5F, 0x3D, 0x0D, 0x3D, 
// 	0xFE, 0x3C, 0x9C, 0x3C, 0xFF, 0x3C, 0xD7, 0x3A, 0x6F, 0x39, 0x40, 0x39, 0xB9, 0x36, 0x4F, 0x36, 
// 	0xF3, 0x34, 0x21, 0x32, 0x51, 0x32, 0x68, 0x31, 0xFD, 0x2E, 0x0F, 0x30, 0xE0, 0x2E, 0xF0, 0x2D, 
// 	0xB1, 0x2D, 0xFF, 0x2E, 0xDD, 0x2E, 0x97, 0x2E, 0x8A, 0x2F, 0xCE, 0x30, 0x19, 0x30, 0xD3, 0x30, 
// 	0x31, 0x32, 0x85, 0x32, 0xE7, 0x32, 0xB0, 0x33, 0xCA, 0x33, 0xE0, 0x32, 0x5B, 0x31, 0xDF, 0x32, 
// 	0xC0, 0x32, 0xE4, 0x31, 0x67, 0x31, 0xD4, 0x30, 0x50, 0x30, 0x08, 0x31, 0xB2, 0x30, 0xB5, 0x30, 
// 	0xB5, 0x30, 0x2B, 0x30, 0x8C, 0x30, 0xCF, 0x2D, 0x63, 0x2D, 0xB5, 0x2C, 0x21, 0x2B, 0xAC, 0x29, 
// 	0x33, 0x29, 0xA4, 0x28, 0xC7, 0x25, 0x15, 0x26, 0x30, 0x25, 0xAF, 0x26, 0x52, 0x26, 0x54, 0x26, 
// 	0x78, 0x28, 0x0E, 0x29, 0x82, 0x29, 0xA5, 0x2A, 0xB8, 0x2B, 0x2E, 0x2C, 0xFA, 0x2C, 0xEF, 0x2D, 
// 	0x7A, 0x2D, 0x31, 0x2E, 0x95, 0x2F, 0xA1, 0x31, 0x64, 0x31, 0x10, 0x32, 0x6E, 0x33, 0x5F, 0x32, 
// 	0x2F, 0x33, 0x5B, 0x33, 0xF0, 0x32, 0xC3, 0x32, 0x5C, 0x32, 0xDA, 0x31, 0x64, 0x32, 0x01, 0x32, 
// 	0x3D, 0x32, 0x5D, 0x33, 0x82, 0x32, 0x92, 0x33, 0xDA, 0x32, 0xD9, 0x31, 0xFE, 0x30, 0x2F, 0x31, 
// 	0x9F, 0x30, 0x15, 0x30, 0xE3, 0x2E, 0x56, 0x2E, 0xA6, 0x2D, 0x74, 0x2D, 0xFD, 0x2E, 0x51, 0x2E, 
// 	0x5B, 0x2F, 0x9A, 0x31, 0x3D, 0x33, 0x0F, 0x34, 0x16, 0x35, 0xE3, 0x35, 0x1E, 0x36, 0x85, 0x37, 
// 	0xFD, 0x38, 0xB3, 0x38, 0x40, 0x39, 0xB6, 0x39, 0x78, 0x37, 0xF2, 0x38, 0x02, 0x3A, 0xEE, 0x39, 
// 	0x99, 0x39, 0x77, 0x39, 0xC2, 0x3A, 0x0D, 0x3A, 0xF7, 0x37, 0x07, 0x39, 0x64, 0x38, 0xB3, 0x37, 
// 	0x40, 0x37, 0xF7, 0x35, 0x31, 0x35, 0x87, 0x34, 0xED, 0x32, 0x5F, 0x33, 0x11, 0x31, 0x05, 0x2F, 
// 	0xB3, 0x2F, 0xC0, 0x2D, 0xE9, 0x2D, 0xEA, 0x2C, 0x77, 0x2B, 0x1D, 0x2C, 0x25, 0x2B, 0x19, 0x2C, 
// 	0x67, 0x2C, 0xC2, 0x2C, 0x3C, 0x2C, 0x51, 0x2C, 0x56, 0x2D, 0xA5, 0x2D, 0x44, 0x2C, 0xF4, 0x2D, 
// 	0x39, 0x2E, 0x48, 0x2E, 0x3A, 0x2D, 0x04, 0x2D, 0xCF, 0x2B, 0x54, 0x2C, 0xF9, 0x2C, 0xF1, 0x2B, 
// 	0xDB, 0x2B, 0xA0, 0x29, 0x72, 0x29, 0xCF, 0x28, 0x35, 0x28, 0x24, 0x26, 0x36, 0x25, 0x10, 0x24, 
// 	0x5A, 0x23, 0x7B, 0x21, 0xCE, 0x1F, 0x13, 0x1F, 0x56, 0x1E, 0x73, 0x1D, 0x65, 0x1D, 0x53, 0x1B, 
// 	0xE7, 0x18, 0x95, 0x17, 0x56, 0x16, 0x3B, 0x15, 0xA4, 0x14, 0x4B, 0x14, 0x83, 0x14, 0xD3, 0x14, 
// 	0xF5, 0x15, 0xB2, 0x15, 0x63, 0x16, 0xAD, 0x18, 0xDA, 0x18, 0x88, 0x1A, 0xA0, 0x1C, 0x02, 0x1C, 
// 	0x21, 0x1B, 0xCC, 0x1B, 0x31, 0x1A, 0xF8, 0x19, 0x38, 0x1A, 0x79, 0x1A, 0xDC, 0x19, 0x0F, 0x1B, 
// 	0x3B, 0x1A, 0x52, 0x17, 0x20, 0x1A, 0x10, 0x1A, 0x77, 0x19, 0xC0, 0x1A, 0x23, 0x1A, 0x03, 0x17, 
// 	0x5C, 0x16, 0x0A, 0x15, 0xA1, 0x13, 0xCC, 0x13, 0x22, 0x11, 0x82, 0x11, 0x22, 0x11, 0x5B, 0x0F, 
// 	0x88, 0x0F, 0x1F, 0x0F, 0x90, 0x0F, 0x4E, 0x10, 0x2B, 0x12, 0xDB, 0x11, 0xE9, 0x11, 0x8F, 0x14, 
// 	0x61, 0x14, 0xA3, 0x15, 0xC8, 0x16, 0x38, 0x19, 0x58, 0x1A, 0x49, 0x1C, 0xB5, 0x1D, 0x12, 0x1F, 
// 	0x0B, 0x1F, 0x73, 0x1F, 0x73, 0x21, 0x42, 0x21, 0xAB, 0x23, 0x3C, 0x23, 0x92, 0x23, 0xDB, 0x24, 
// 	0x62, 0x24, 0xDB, 0x24, 0xCF, 0x24, 0xC7, 0x25, 0xD2, 0x25, 0xAC, 0x23, 0x49, 0x24, 0xE1, 0x22, 
// 	0x39, 0x21, 0xA8, 0x21, 0x4B, 0x21, 0xC4, 0x1F, 0x75, 0x21, 0xF6, 0x20, 0xD7, 0x20, 0x08, 0x21, 
// 	0x8B, 0x20, 0x6A, 0x20, 0x3D, 0x20, 0xA6, 0x21, 0x44, 0x22, 0x5F, 0x23, 0x13, 0x23, 0xEC, 0x22, 
// 	0x5F, 0x23, 0x36, 0x24, 0x22, 0x26, 0x66, 0x28, 0x5B, 0x2A, 0x93, 0x2B, 0x43, 0x2D, 0x6A, 0x2E, 
// 	0x15, 0x2F, 0x28, 0x2E, 0xA8, 0x2E, 0x58, 0x2F, 0xC2, 0x2F, 0x91, 0x2E, 0xA3, 0x2F, 0x90, 0x2E, 
// 	0x31, 0x2E, 0x87, 0x2E, 0xE9, 0x2F, 0xF6, 0x2F, 0x95, 0x30, 0x43, 0x30, 0xFA, 0x2D, 0xB3, 0x2D, 
// 	0x24, 0x2A, 0x36, 0x28, 0x8A, 0x27, 0x36, 0x26, 0x6B, 0x25, 0xD4, 0x24, 0x8E, 0x22, 0x1D, 0x23, 
// 	0xEB, 0x22, 0x77, 0x23, 0x2F, 0x25, 0x65, 0x23, 0xC0, 0x24, 0x49, 0x24, 0x58, 0x26, 0x3D, 0x25, 
// 	0x0C, 0x25, 0x72, 0x25, 0x21, 0x28, 0x3A, 0x27, 0xB3, 0x26, 0x07, 0x27, 0x21, 0x27, 0x1B, 0x28, 
// 	0xEF, 0x27, 0xED, 0x27, 0x76, 0x25, 0xC5, 0x24, 0x7F, 0x23, 0x48, 0x24, 0x07, 0x23, 0xC2, 0x22, 
// 	0x03, 0x24, 0x27, 0x24, 0xFE, 0x23, 0x1A, 0x26, 0x22, 0x23, 0x44, 0x22, 0x74, 0x20, 0x7C, 0x1E, 
// 	0x10, 0x1F, 0x34, 0x1C, 0x29, 0x1B, 0x97, 0x19, 0xAB, 0x17, 0xE9, 0x14, 0x9B, 0x15, 0x78, 0x13, 
// 	0x23, 0x13, 0xBF, 0x13, 0x8B, 0x12, 0xDB, 0x12, 0x94, 0x12, 0x09, 0x12, 0x52, 0x12, 0x95, 0x13, 
// 	0x2A, 0x14, 0xD9, 0x14, 0x59, 0x14, 0x8B, 0x14, 0x76, 0x15, 0xDE, 0x14, 0x77, 0x15, 0x6A, 0x17};



// 	

//
void USART_RX_BUF_Init(u8 audio)
	{
		u16 k;
		switch(audio)
		{
			case 1:
			for(k=0;k<64;k++)
			{
				USART_RX_BUF[k]=Wav_Data_head[k];
			}		
				break;
			case 2:
			for(k=0;k<64;k++)
			{
				USART_RX_BUF[k]=Mp3_Data_head[k];
			}		
				break;
		}
// 		for(k=64;k<USART_REC_LEN;k++)
// 		{
// 			USART_RX_BUF[k]=0x7F;
// 		}		
    }
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	
	
  
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}


//Use USART_x Send Str
void USART_SendStr(char *SendStr,int len)
 {   
	 int t1;
	 for(t1=0;t1<len;t1++)
	 {
		 USART_SendData(USART1, SendStr[t1]);	//�򴮿�1��������
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);  //�ȴ����ͽ���
	 }	 
 }

u16 Audio_EN=1;
u8  Play_EN = 1;
u8  Text_EN = 0;
u8  Receive_Over = 0;
void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Audio_EN=1;
			Res=USART_ReceiveData(USART1);
			USART_RX_BUF[USART_RX_STA&0X3FFF] = Res;//(USART1->DR);	//��ȡ���յ�������
			USART_RX_STA++;//
			if(USART_RX_BUF[USART_RX_STA-1]==':' && USART_RX_BUF[USART_RX_STA-2]=='T' && USART_RX_BUF[USART_RX_STA-3] =='X' && USART_RX_BUF[USART_RX_STA-4] =='E' && USART_RX_BUF[USART_RX_STA-5] =='T' ){
				Text_EN =1;Play_EN=0;USART_RX_STA=0;
			}else
			if(USART_RX_BUF[USART_RX_STA-2]=='E' && USART_RX_BUF[USART_RX_STA-3] =='V' && USART_RX_BUF[USART_RX_STA-4] =='A' && USART_RX_BUF[USART_RX_STA-5] =='W' ){
				Play_EN = 1;
			}
			if(Res==0x0a && USART_RX_BUF[USART_RX_STA-2]==0x0d ){
				Receive_Over=1;
			}
			
			if(USART_RX_STA >= USART_REC_LEN) 
				USART_RX_STA=0;			
// 		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
// 		if((USART_RX_STA&0x8000)==0)//����δ���
// 			{
// 			if(USART_RX_STA&0x4000)//���յ���0x0d
// 				{
// 				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
// 				else USART_RX_STA|=0x8000;	//��������� 
// 				}
// 			else //��û�յ�0X0D
// 				{	
// 				if(Res==0x0d)USART_RX_STA|=0x4000;
// 				else
// 					{
// 					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
// 					}		 
// 				}
// 			}   		 
     } 
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
#endif	
