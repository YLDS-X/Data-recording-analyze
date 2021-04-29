/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-06 22:30:35
 * @LastEditTime: 2021-04-24 13:27:58
 */
#include "gauge_win.h"
#include "app.h"
#include <stdio.h>
#include "dht11.h"
static lv_obj_t * gauge1;
static lv_obj_t * label1;
static int16_t speed_val = 0;
lv_color_t needle_colors[2];//ÿһ��ָ�����ɫ
static lv_task_t * task = NULL;
static lv_obj_t *humi[2]; //��������roller
static lv_obj_t *enter_btn;		//ȷ�ϰ���

extern u8 humidity[40];

/**
 * @brief: ��ť�¼��ص�����
 * @version: 1.0
 * @Date: 2021-04-23 17:49:33
 */
static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	if(event==USER_EVENT_BACK)//���巵���¼�,������Ͻǵ�"< Back"��ť�ᴥ�����¼�
	{
		if(task)//ɾ��������
		{
			lv_task_del(task);
			task = NULL;
		}
	}
	else if (obj == enter_btn)
	{
		if (event == LV_EVENT_RELEASED)
		{
			HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);    
			humidity_L = lv_roller_get_selected(humi[0]) ;    
			humidity_H = lv_roller_get_selected(humi[1]) ;
		}
	}
}


/**
 * @brief: 500ms����ص�����
 * @version: 1.0
 * @Date: 2021-04-23 17:49:19
 */
void task_cb(lv_task_t * task)
{
	char buff[40];
	speed_val = humidity[39];
	//����ָ�����ֵ
	lv_gauge_set_value(gauge1,0,speed_val);
	lv_gauge_set_value(gauge1,1,humidity_H);//����ָ��2ָ�����ֵ,������ָ��ؼ���ֵ��
	//�Ѵ��ٶ���ʾ�ڱ�ǩ��,Ȼ����ݲ�ͬ����ֵ��ʾ����ͬ����ɫ
	//��ɫ����ȫ,��ɫ������,��ɫ����Σ��  
	sprintf(buff,"#%s %d %%#",speed_val<60?"00FF00":(speed_val<90?"FFFF00":"FF0000"),speed_val); 
	lv_label_set_text(label1,buff);
}

/**
 * @brief: �����Ǳ�����ʾʪ�Ⱥ��޸�ʪ��������
 * @version: 1.0
 * @Date: 2021-04-23 10:10:07
 */
void gauge_win_create(lv_obj_t * parent)
{
	lv_obj_t * win;
	win = general_win_create(parent,"Humi");
	lv_obj_set_event_cb(win,event_handler);//�����¼��ص�����
	
	//�����Ǳ���
	gauge1 = lv_gauge_create(win, NULL);
	lv_obj_set_size(gauge1,200,200);//���ô�С
	lv_gauge_set_range(gauge1,0,100);//�����Ǳ��̵ķ�Χ
	needle_colors[0] = LV_COLOR_BLUE;
	needle_colors[1] = LV_COLOR_RED;
	lv_gauge_set_needle_count(gauge1,sizeof(needle_colors)/sizeof(needle_colors[0]),needle_colors);
	lv_gauge_set_value(gauge1,0,speed_val);//����ָ��1ָ�����ֵ,���ǰ�ָ��1�����ٶ�ָ��
	lv_gauge_set_value(gauge1,1,humidity_H);//����ָ��2ָ�����ֵ,������ָ��ؼ���ֵ��
	lv_gauge_set_critical_value(gauge1,humidity_H);//���ùؼ���ֵ��
	lv_gauge_set_scale(gauge1,240,31,6);//���ýǶ�,�̶��ߵ�����,��ֵ��ǩ������
	lv_obj_align(gauge1,NULL,LV_ALIGN_IN_TOP_MID,0,40);//���ö��뷽ʽ
	
	//����һ����ǩ,������ʾ�Ǳ�����ָ��1����ֵ
	label1 = lv_label_create(win,NULL);
	lv_label_set_long_mode(label1,LV_LABEL_LONG_BREAK);//���ó��ı�ģʽ
	lv_obj_set_width(label1,40);//���ÿ��
	lv_label_set_align(label1,LV_LABEL_ALIGN_CENTER);//�����ı����ж���
	lv_label_set_body_draw(label1,true);//ʹ�ܱ����ػ�
	lv_obj_align(label1,gauge1,LV_ALIGN_CENTER,0,80);//���ö��뷽ʽ
	lv_label_set_text(label1,"0 %");//�����ı�
	lv_label_set_recolor(label1,true);//ʹ���ı��ػ�ɫ
	
	// //ʪ������
	humi[0] = lv_roller_create(win, NULL);
	lv_roller_set_options(humi[0], "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(humi[0], humidity_L, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(humi[0], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(humi[0], 1);					  //���ÿɼ�������
	lv_obj_align(humi[0], label1, LV_ALIGN_OUT_LEFT_MID, -20, 0); //���ö��뷽ʽ

	//ʪ������
	humi[1] = lv_roller_create(win, NULL);
	lv_roller_set_options(humi[1],"00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n71\n71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n91\n92\n93\n94\n95\n96\n97\n98\n99", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(humi[1], humidity_H, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(humi[1], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(humi[1], 1);					  //���ÿɼ�������
	lv_obj_align(humi[1], label1,LV_ALIGN_OUT_RIGHT_MID, 20, 0); //���ö��뷽ʽ
	
	enter_btn = lv_btn_create(win, NULL);
	lv_obj_set_size(enter_btn,60, 40);		//���ð�ť��С
	lv_obj_align(enter_btn, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);		//���ð�ť����

	lv_obj_t *enter_btn_lable = lv_label_create(enter_btn, NULL);		//������ťlabel
	lv_label_set_text(enter_btn_lable, "Enter"); 
	lv_obj_align(enter_btn_lable, enter_btn, LV_ALIGN_CENTER, 0, 0);		//���ð�ť�ı�����
	lv_obj_set_event_cb(enter_btn, event_handler); //�����¼��ص�����
	
	//����һ��������ģ���ٶ�ָ��ı仯 
	if(task==NULL)
		task = lv_task_create(task_cb,500,LV_TASK_PRIO_MID,NULL);
}













