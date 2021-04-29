/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-02 23:43:58
 * @LastEditTime: 2021-04-08 17:10:19
 */
#include "usart.h"
#include "usmart.h"
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA = 0; //����״̬���

u8 aRxBuffer[RXBUFFERSIZE];		  //HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART3_Handler; //UART���

#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	USART3->DR = (u8) ch;      
	return ch;
}

void HAL_UART3_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3) //����Ǵ���3
	{
		if ((USART_RX_STA & 0x8000) == 0) //����δ���
		{
			if (USART_RX_STA & 0x4000) //���յ���0x0d
			{
				if (aRxBuffer[0] != 0x0a)
					USART_RX_STA = 0; //���մ���,���¿�ʼ
				else
					USART_RX_STA |= 0x8000; //���������
			}
			else //��û�յ�0X0D
			{
				if (aRxBuffer[0] == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0];
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; //�������ݴ���,���¿�ʼ����
				}
			}
		}
	}
}

//����3�жϷ������
void USART3_IRQHandler(void)
{
	u32 timeout = 0;

	HAL_UART_IRQHandler(&UART3_Handler); //����HAL���жϴ����ú���

	timeout = 0;
	while (HAL_UART_GetState(&UART3_Handler) != HAL_UART_STATE_READY) //�ȴ�����
	{
		timeout++; ////��ʱ����
		if (timeout > HAL_MAX_DELAY)
			break;
	}

	timeout = 0;
	while (HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) //һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
	{
		timeout++; //��ʱ����
		if (timeout > HAL_MAX_DELAY)
			break;
	}
	HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
}


//��ʼ��IO ����1
//bound:������
int uart_init(void)
{
	//UART ��ʼ������
	UART3_Handler.Instance = USART3;					//USART3
	UART3_Handler.Init.BaudRate = 115200;				//������
	UART3_Handler.Init.WordLength = UART_WORDLENGTH_8B; //�ֳ�Ϊ8λ���ݸ�ʽ
	UART3_Handler.Init.StopBits = UART_STOPBITS_1;		//һ��ֹͣλ
	UART3_Handler.Init.Parity = UART_PARITY_NONE;		//����żУ��λ
	UART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //��Ӳ������
	UART3_Handler.Init.Mode = UART_MODE_TX_RX;			//�շ�ģʽ
	HAL_UART_Init(&UART3_Handler);						//HAL_UART_Init()��ʹ��UART3

	HAL_UART_Receive_IT(&UART3_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE); //�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
    
	usmart_dev.init(84); 		    //��ʼ��USMART	
	
    return RT_EOK;
}

INIT_DEVICE_EXPORT(uart_init);
