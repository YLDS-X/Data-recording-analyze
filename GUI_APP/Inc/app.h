/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-05 21:47:48
 * @LastEditTime: 2021-04-05 21:50:42
 */
#ifndef _APP_H
#define _APP_H

#include "lvgl.h"
#include "lv_ex_conf.h"
#include "main.h"


//���巵���¼�,���������Լ������һ���¼�,�������ֵֻҪ����littleVGL�Դ����¼�ֵ��ͻ����
//LV_EVENT_DELETE����littleVGL����������һ���¼�,��10��Ϊ����������
#define USER_EVENT_BACK		(LV_EVENT_DELETE+10)  

//��������
void gui_app_start(void);
void toast(const char * msg,uint16_t time);
lv_obj_t * general_win_create(lv_obj_t * parent,const char * title);

#endif

