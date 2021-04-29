/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-06 22:30:35
 * @LastEditTime: 2021-04-25 19:40:59
 */
#include "chart_win.h"
#include "app.h"
#include "dht11.h"
#define POINT_COUNT 40	   //�����������е����ݵ����
#define SLIDER_MAX_VAL 100 //��������ֵ
static lv_coord_t series1_y[POINT_COUNT] = {0};
static lv_obj_t *chart1;
static lv_chart_series_t *series1;
static lv_chart_series_t *series2;
static lv_task_t *task = NULL;
static lv_obj_t * label1;
static lv_obj_t *temp[2]; //��������roller
static lv_obj_t *enter_btn;		//ȷ�ϰ���
extern u8 temperature[40];
extern u8 humidity[40];
u8 chart_flag;

/**
 * @brief: ����ص�����
 * @version: 1.0
 * @Date: 2021-04-25 19:40:50
 * @param {lv_task_t} *t
 */
static void Temp_task(lv_task_t *t)
{
	uint8_t i;
	// char buff1[40];
	char buff[40];
	int16_t speed_val;
	speed_val = temperature[39];
	if (chart_flag == 1)
	{
		for (i = 0; i < POINT_COUNT; i++)
		{
			series1->points[i] = (lv_coord_t)(temperature[i]); //��������ϵ��,�޸����ݵ��ֵ
			series2->points[i] = (lv_coord_t)(humidity[i]);	   //��������ϵ��,�޸����ݵ��ֵ
		}
		lv_chart_refresh(chart1); //����ͼ��
		//�Ѵ��ٶ���ʾ�ڱ�ǩ��,Ȼ����ݲ�ͬ����ֵ��ʾ����ͬ����ɫ
		//��ɫ����ȫ,��ɫ������,��ɫ����Σ��  
		sprintf(buff,"#%s %d C#",speed_val<30?"00FF00":(speed_val<40?"FFFF00":"FF0000"),speed_val); 
		lv_label_set_text(label1,buff);
	}
}

/**
 * @brief: �¼��ص�����
 * @version: 1.0
 * @Date: 2021-04-25 19:40:41
 * @param {lv_obj_t} *obj
 * @param {lv_event_t} event
 */
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
	if (obj == enter_btn)
	{
		if (event == LV_EVENT_RELEASED)
		{
			HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);    
			temperature_L = lv_roller_get_selected(temp[0]) ;
			temperature_H = lv_roller_get_selected(temp[1]) ;
		}
	}
}


/**
 * @brief: ����ͼ��������ʾ��ʪ�Ⱥ��޸��¶�������
 * @version: 1.0
 * @Date: 2021-04-23 10:11:27
 * @param {lv_obj_t} *parent
 */
void chart_win_create(lv_obj_t *parent)
{
	lv_obj_t *win;
	win = general_win_create(parent, "Temp_Humi");

	//����ͼ��
	chart1 = lv_chart_create(win, NULL);
	lv_obj_set_size(chart1, 180, 200);									 //���ô�С
	lv_obj_align(chart1, NULL, LV_ALIGN_IN_TOP_MID, 17, 40);			 //���ö��뷽ʽ
	lv_chart_set_type(chart1, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE); //����Ϊɢ������ߵ����
	lv_chart_set_series_opa(chart1, LV_OPA_80);							 //���������ߵ�͸����
	lv_chart_set_series_width(chart1, 4);								 //���������ߵĿ��
	lv_chart_set_series_darking(chart1, LV_OPA_80);						 //���������ߵĺ���ӰЧ��
	lv_chart_set_point_count(chart1, POINT_COUNT);						 //�������ݵ�ĸ���
	lv_chart_set_div_line_count(chart1, 4, 4);							 //����ˮƽ�ʹ�ֱ�ָ���
	lv_chart_set_range(chart1, 0, 100);									 //����y�����ֵ��Χ
	lv_chart_set_y_tick_length(chart1, 10, 3);
	lv_chart_set_y_tick_texts(chart1, "100\n80\n60\n40\n20\n0", 2, LV_CHART_AXIS_DRAW_LAST_TICK);
	lv_chart_set_x_tick_length(chart1, 10, 3);
	lv_chart_set_x_tick_texts(chart1, "0\n\n\n\n\n40", 2, LV_CHART_AXIS_DRAW_LAST_TICK);
	lv_chart_set_margin(chart1, 40);

	//��ͼ�������������
	series1 = lv_chart_add_series(chart1, LV_COLOR_RED);		   //ָ��Ϊ��ɫ
	series2 = lv_chart_add_series(chart1, LV_COLOR_BLUE);		   //ָ��Ϊ��ɫ
	lv_chart_set_points(chart1, series1, (lv_coord_t *)series1_y); //��ʼ�����ݵ��ֵ
	lv_chart_set_points(chart1, series2, (lv_coord_t *)series1_y); //��ʼ�����ݵ��ֵ	//��������

	//��ʾ�¶�
	label1 = lv_label_create(win,NULL);
	lv_label_set_long_mode(label1,LV_LABEL_LONG_BREAK);//���ó��ı�ģʽ
	lv_obj_set_width(label1,40);//���ÿ��
	lv_label_set_align(label1,LV_LABEL_ALIGN_CENTER);//�����ı����ж���
	lv_label_set_body_draw(label1,true);//ʹ�ܱ����ػ�
	lv_obj_align(label1,chart1,LV_ALIGN_OUT_BOTTOM_MID,-15,65);//���ö��뷽ʽ
	lv_label_set_text(label1,"0 ��C");//�����ı�
	lv_label_set_recolor(label1,true);//ʹ���ı��ػ�ɫ

	//�¶�����
	temp[0] = lv_roller_create(win, NULL);
	lv_roller_set_options(temp[0], "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(temp[0], temperature_L, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(temp[0], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(temp[0], 1);					  //���ÿɼ�������
	lv_obj_align(temp[0], label1,LV_ALIGN_OUT_LEFT_MID, -20, 0); //���ö��뷽ʽ

	//�¶�����
	temp[1] = lv_roller_create(win, NULL);
	lv_roller_set_options(temp[1], "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(temp[1], temperature_H, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(temp[1], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(temp[1], 1);					  //���ÿɼ�������
	lv_obj_align(temp[1], label1,LV_ALIGN_OUT_RIGHT_MID, 20, 0); //���ö��뷽ʽ
	
	enter_btn = lv_btn_create(win, NULL);
	lv_obj_set_size(enter_btn,60, 40);		//���ð�ť��С
	lv_obj_align(enter_btn, label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);		//���ð�ť����

	lv_obj_t *enter_btn_lable = lv_label_create(enter_btn, NULL);		//������ťlabel
	lv_label_set_text(enter_btn_lable, "Enter"); 
	lv_obj_align(enter_btn_lable, enter_btn, LV_ALIGN_CENTER, 0, 0);		//���ð�ť�ı�����
	lv_obj_set_event_cb(enter_btn, event_handler); //�����¼��ص�����
	
	chart_flag = 1;
	if (task == NULL)
		task = lv_task_create(Temp_task, 500, LV_TASK_PRIO_LOW, NULL);
}
