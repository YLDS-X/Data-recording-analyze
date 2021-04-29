/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-06 22:30:35
 * @LastEditTime: 2021-04-24 17:05:16
 */
#include "calendar_win.h"
#include "app.h"
#include "rtc.h"
//����,��һ,�ܶ�,...,����
const char *const day_names[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
//һ��,����,...,ʮ����
const char *const month_names[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
static const char roller_string[] = 
{
	"00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n\
24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n\
48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59"
};
lv_calendar_date_t today;
static lv_obj_t *calendar1;
static lv_obj_t *date_roller[3]; //��������roller
static lv_obj_t *date_Hline_label[3];		//���ڵ�20 �� - lable
static lv_obj_t *enter_btn;		//ȷ�ϰ���
extern u8 Date[40];

/**
 * @brief: �¼��ص�����
 * @version: 1.0
 * @Date: 2021-04-24 17:05:08
 * @param {lv_obj_t} *obj
 * @param {lv_event_t} event
 */
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
	RTC_DateTypeDef RTC_DateStructure;
	if (obj == calendar1)
	{
		if (event == LV_EVENT_CLICKED)
		{
			lv_calendar_date_t *selected_date = lv_calendar_get_pressed_date(obj); //��ȡ�����µ�����
			if (selected_date && (selected_date->year != 0))
			{
				lv_calendar_set_today_date(obj, selected_date);						 //���û�ѡ�����������Ϊ���������,��TODAY����
				lv_calendar_set_showed_date(obj, selected_date);					 //��ת������������ڵĽ���
			}
		}
	}
	else if (obj == enter_btn)
	{
		if (event == LV_EVENT_RELEASED)
		{
			HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);    
			RTC_DateStructure.Year = lv_roller_get_selected(date_roller[0]) ;    
			RTC_DateStructure.Month = lv_roller_get_selected(date_roller[1]) + 1;
			RTC_DateStructure.Date = lv_roller_get_selected(date_roller[2]) + 1;
			HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure, RTC_FORMAT_BIN);
		}
	}
}

/**
 * @brief: �����������޸�����
 * @version: 1.0
 * @Date: 2021-04-23 10:10:53
 */
void calendar_win_create(lv_obj_t *parent)
{
	lv_obj_t *win = general_win_create(parent, "Calendar");
	uint8_t i;
	//��������
	calendar1 = lv_calendar_create(win, NULL);
	lv_obj_set_size(calendar1, 220, 220);					   //���ô�С
	lv_obj_align(calendar1, NULL, LV_ALIGN_IN_TOP_MID, 0, 40); //���ö��뷽ʽ
	lv_obj_set_event_cb(calendar1, event_handler);			   //�����¼��ص�����

	lv_calendar_set_today_date(calendar1, &today);																							   //����TODAY����
	lv_calendar_set_showed_date(calendar1, &today);																							   //��ת��TODAY�������ڵĽ���
	lv_calendar_set_day_names(calendar1, (const char **)day_names);																			   //����������Ϣ�еı���
	lv_calendar_set_month_names(calendar1, (const char **)month_names);																		   //�����·ݵı���

	//��������
	//��
	date_roller[0] = lv_roller_create(win, NULL);
	lv_roller_set_options(date_roller[0], roller_string, LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(date_roller[0], today.year - 2000, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(date_roller[0], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(date_roller[0], 1);					  //���ÿɼ�������
	lv_obj_align(date_roller[0], calendar1,LV_ALIGN_OUT_BOTTOM_LEFT, 50, 15); //���ö��뷽ʽ

	//��
	date_roller[1] = lv_roller_create(win, NULL);
	lv_roller_set_options(date_roller[1], "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(date_roller[1], today.month -1, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(date_roller[1], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(date_roller[1], 1);					  //���ÿɼ�������
	lv_obj_align(date_roller[1], calendar1,LV_ALIGN_OUT_BOTTOM_LEFT, 100, 15); //���ö��뷽ʽ

	//��
	date_roller[2] = lv_roller_create(win, NULL);
	lv_roller_set_options(date_roller[2], "01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(date_roller[2], today.day-1, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(date_roller[2], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(date_roller[2], 1);					  //���ÿɼ�������
	lv_obj_align(date_roller[2], calendar1, LV_ALIGN_OUT_BOTTOM_LEFT, 150, 15); //���ö��뷽ʽ

	for(i = 0; i < 3; i++)	//�������ڵ� ��20 �� - lable
	{
		date_Hline_label[i] = lv_label_create(win, NULL);		//�������ڵ� ��20 �� - lable
		lv_label_set_text(date_Hline_label[i], i ? "-" : "20");
		lv_obj_align(date_Hline_label[i], date_roller[i], LV_ALIGN_OUT_LEFT_MID, i ? -4 : 0, 0);
	}

	enter_btn = lv_btn_create(win, NULL);
	lv_obj_set_size(enter_btn,60, 40);		//���ð�ť��С
	lv_obj_align(enter_btn, win, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);		//���ð�ť����

	lv_obj_t *enter_btn_lable = lv_label_create(enter_btn, NULL);		//������ťlabel
	lv_label_set_text(enter_btn_lable, "Enter"); 
	lv_obj_align(enter_btn_lable, enter_btn, LV_ALIGN_CENTER, 0, 0);		//���ð�ť�ı�����
	lv_obj_set_event_cb(enter_btn, event_handler); //�����¼��ص�����
}
