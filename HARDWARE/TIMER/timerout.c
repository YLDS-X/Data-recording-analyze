/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-04 17:30:36
 * @LastEditTime: 2021-04-24 17:10:20
 */
#include "timerout.h"
#include "lvgl.h"
#include "usmart.h"
#define LED1_PIN    GET_PIN(F, 10)
/* ��ʱ���Ŀ��ƿ� */
static struct rt_timer timer1;
static struct rt_timer timer2;

/**
 * @brief: ��ʱ�� 1 ��ʱ���� �ṩusmart 100msɨ��
 * @version: 1.0
 * @Date: 2021-04-24 17:09:56
 */
static void timeout1(void* parameter)
{
	usmart_dev.scan();	//ִ��usmartɨ��
}

/**
 * @brief: �ṩlvgl������ 1ms
 * @version: 1.0
 * @Date: 2021-04-24 17:09:46
 */
static void lv_tack(void* parameter)
{
	lv_tick_inc(1);//lvgl�� 1ms����
}

/**
 * @brief: �����ʱ����ʼ��
 * @version: 1.0
 * @Date: 2021-04-24 17:09:24
 */
int timer_static_sample(void)
{
	
    // rt_pin_mode(LED1_PIN,PIN_MODE_OUTPUT);
    /* ��ʼ����ʱ�� */
    rt_timer_init(&timer1, "timerout",  
                    timeout1, /* ��ʱʱ�ص��Ĵ����� */
                    RT_NULL, /* ��ʱ��������ڲ��� */
                    100, /* ��ʱ���ȣ��� OS Tick Ϊ��λ */
                    RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
	
    rt_timer_init(&timer2, "lv_tack",  
                    lv_tack, /* ��ʱʱ�ص��Ĵ����� */
                    RT_NULL, /* ��ʱ��������ڲ��� */
                    1, /* ��ʱ���ȣ��� OS Tick Ϊ��λ */
                    RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
	
    /* ������ʱ�� */
    rt_timer_start(&timer1);
	rt_timer_start(&timer2);
    return 0;
}

INIT_APP_EXPORT(timer_static_sample);



