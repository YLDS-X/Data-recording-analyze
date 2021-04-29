#ifndef _USART_H
#define _USART_H
#include "main.h"
#include "stdio.h"
#include "sys.h"

#define USART_REC_LEN 200 //�����������ֽ��� 200
#define EN_USART1_RX 1    //ʹ�ܣ�1��/��ֹ��0������1����

extern u8 USART_RX_BUF[USART_REC_LEN];   //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u16 USART_RX_STA;                 //����״̬���
extern UART_HandleTypeDef UART1_Handler; //UART���

#define RXBUFFERSIZE 1             //�����С
extern u8 aRxBuffer[RXBUFFERSIZE]; //HAL��USART����Buffer

//����봮���жϽ��գ��벻Ҫע�����º궨��
int uart_init(void);
void HAL_UART3_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
