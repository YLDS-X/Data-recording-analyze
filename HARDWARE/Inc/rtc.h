/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-03 22:42:18
 * @LastEditTime: 2021-04-22 21:08:32
 */
#ifndef __RTC_H
#define __RTC_H
#include "main.h"
#include "app.h"
#include "calendar_win.h"
#include "progress_win.h"

extern RTC_HandleTypeDef RTC_Handler;  //RTC���


int RTC_Init(void);              		//RTC��ʼ��
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);      //RTCʱ������
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);	//RTC��������
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec); //��������ʱ��(����������,24Сʱ��)
void RTC_Set_WakeUp(u32 wksel,u16 cnt);             //�����Ի��Ѷ�ʱ������
void rtc_read(void);
#endif
