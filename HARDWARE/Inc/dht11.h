/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-03 22:42:51
 * @LastEditTime: 2021-04-23 09:55:33
 */
#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"

//IO��������
#define DHT11_IO_IN()  {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=0<<(9*2);}	//PG9����ģʽ
#define DHT11_IO_OUT() {GPIOG->MODER&=~(3<<(9*2));GPIOG->MODER|=1<<(9*2);} 	//PG9���ģʽ
 
////IO��������											   
#define	DHT11_DQ_OUT    PGout(9)//���ݶ˿�	PG9
#define	DHT11_DQ_IN     PGin(9) //���ݶ˿�	PG9 


typedef struct
{
	rt_uint8_t sensor_pause; 
	rt_uint8_t sensor_period;
	rt_uint8_t temp_lower;
	rt_uint8_t temp_upper;
	rt_uint8_t humi_lower;
	rt_uint8_t humi_upper;	
}LOGGER_ConfigTypeDef;

typedef struct
{
	rt_uint8_t temperature;
	rt_uint8_t humidity;
}SENSOR_DataTypeDef;

extern u8 temperature_H;
extern u8 temperature_L;
extern u8 humidity_H;
extern u8 humidity_L;

void Amplitude_setting(u8 temp_up, u8 temp_low, u8 humi_up, u8 humi_low);
int DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11  
void Warning(void);
#endif
