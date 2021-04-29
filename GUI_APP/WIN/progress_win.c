#include "progress_win.h"
#include "app.h"
#include "rtc.h"
static lv_obj_t *arc1;
static lv_obj_t *progress_label;
static lv_task_t *task = NULL;
uint8_t progress_val;
lv_style_t arc1_style;
lv_style_t arc2_style;
lv_style_t arc3_style;
lv_obj_t *arc1;
lv_obj_t *arc2;
lv_obj_t *arc3;
extern u8 Time[40];
static lv_obj_t *time_roller[3]; //��������roller
static lv_obj_t *time_point_label[2];		//ʱ���: label
static lv_obj_t *enter_btn;		//ȷ�ϰ���
extern Clock_time RTC_time;
RTC_TimeTypeDef RTC_TimeStruct;
Clock_time RTC_time;
static const char roller_string[] = 
{
	"00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n\
24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n\
48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59"
};

/**
 * @brief: �¼��ص�����
 * @version: 1.0
 * @Date: 2021-04-24 17:06:40
 * @param {lv_obj_t} *obj
 * @param {lv_event_t} event
 */
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
	if (event == USER_EVENT_BACK) //���巵���¼�,������Ͻǵ�"< Back"��ť�ᴥ�����¼�
	{
		if (task) //ɾ��������
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
			RTC_Set_Time(lv_roller_get_selected(time_roller[0]), lv_roller_get_selected(time_roller[1]), lv_roller_get_selected(time_roller[2]), RTC_HOURFORMAT12_PM);  
		}
	}
}

/**
 * @brief: ���ý���ֵ
 * @version: 1.0
 * @Date: 2021-04-24 17:06:47
 * @param {uint8_t} val ��ΧΪ[0,100]
 */
static void progress_set_val(uint8_t val)
{
	lv_arc_set_angles(arc1, 0, (uint16_t)(15 * RTC_time.Hours));
	lv_arc_set_angles(arc2, 0, (uint16_t)(6 * RTC_time.Minutes));
	lv_arc_set_angles(arc3, 0, (uint16_t)(6 * RTC_time.Seconds));

	lv_label_set_text(progress_label, (char *)Time);
	lv_obj_realign(progress_label);
}

/**
 * @brief: ����ص�����
 * @version: 1.0
 * @Date: 2021-04-24 17:06:59
 * @param {lv_task_t} *t
 */
static void progress_task(lv_task_t *t)
{
	progress_set_val(progress_val);
}

/**
 * @brief: ��������
 * @version: 1.0
 * @Date: 2021-04-24 17:07:07
 * @param {lv_obj_t} *parent
 */
void progress_win_create(lv_obj_t *parent)
{
	lv_obj_t *win = general_win_create(parent, "Clock");
	lv_obj_set_event_cb(win, event_handler); //�����¼��ص�����
	uint8_t i;

	//1.������ʽ
	//ʱ-��-��-��
	lv_style_copy(&arc1_style, &lv_style_plain);
	arc1_style.line.color = LV_COLOR_RED; //���ε���ɫs
	arc1_style.line.width = 10;			  //���εĺ��
	arc1_style.line.rounded = 1;		  //ĩ��ΪԲ�� ,���Ϊ 0�Ļ� ,��Ϊֱ��

	//��-��-��-��
	lv_style_copy(&arc2_style, &lv_style_plain);
	arc2_style.line.color = LV_COLOR_BLUE; //���ε���ɫs
	arc2_style.line.width = 6;			   //���εĺ��
	arc2_style.line.rounded = 1;		   //ĩ��ΪԲ�� ,���Ϊ 0�Ļ� ,��Ϊֱ��

	//��-��-ϸ-��
	lv_style_copy(&arc3_style, &lv_style_plain);
	arc3_style.line.color = LV_COLOR_GREEN; //���ε���ɫs
	arc3_style.line.width = 4;				//���εĺ��
	arc3_style.line.rounded = 1;			//ĩ��ΪԲ�� ,���Ϊ 0�Ļ� ,��Ϊֱ��

	//2.����ʱ
	arc1 = lv_arc_create(win, NULL);						//�������ζ���
	lv_arc_set_style(arc1, LV_ARC_STYLE_MAIN, &arc1_style); //������ʽ
	lv_obj_set_size(arc1, 150, 150);						//���ô�С,���õĿ�Ⱥ͸߶ȱ�������,���ΰ뾶���ڿ�ȵ�һ��
															// lv_obj_set_pos(arc1, 0, 0);                             //��������
	lv_obj_align(arc1, NULL, LV_ALIGN_IN_TOP_MID, 0, 40);	//���ö��뷽ʽ

	//3.������
	arc2 = lv_arc_create(win, arc1);						//ֱ�Ӵ�arc1����
	lv_arc_set_style(arc2, LV_ARC_STYLE_MAIN, &arc2_style); //������ʽ
	lv_obj_set_size(arc2, 130, 130);						//���ô�С,���õĿ�Ⱥ͸߶ȱ�������,���ΰ뾶���ڿ�ȵ�һ��
	lv_obj_align(arc2, arc1, LV_ALIGN_CENTER, 0, 0);		//������arc1�Ķ��뷽ʽ

	//4.������
	arc3 = lv_arc_create(win, arc1);						//ֱ�Ӵ�arc1����
	lv_arc_set_style(arc3, LV_ARC_STYLE_MAIN, &arc3_style); //������ʽ
	lv_obj_set_size(arc3, 120, 120);						//���ô�С,���õĿ�Ⱥ͸߶ȱ�������,���ΰ뾶���ڿ�ȵ�һ��
	lv_obj_align(arc3, arc1, LV_ALIGN_CENTER, 0, 0);		//������arc1�Ķ��뷽ʽ

	progress_label = lv_label_create(win, NULL);
	lv_obj_align(progress_label, arc1, LV_ALIGN_CENTER, 0, 0);

	//��������
	//ʱ
	time_roller[0] = lv_roller_create(win, NULL);
	lv_roller_set_options(time_roller[0], "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(time_roller[0], RTC_time.Hours, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(time_roller[0], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(time_roller[0], 1);					  //���ÿɼ�������
	lv_obj_align(time_roller[0], win,LV_ALIGN_OUT_BOTTOM_MID, -50, -90); //���ö��뷽ʽ

	//��
	time_roller[1] = lv_roller_create(win, NULL);
	lv_roller_set_options(time_roller[1],roller_string, LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(time_roller[1], RTC_time.Minutes, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(time_roller[1], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(time_roller[1], 1);					  //���ÿɼ�������
	lv_obj_align(time_roller[1], win,LV_ALIGN_OUT_BOTTOM_MID, 0, -90); //���ö��뷽ʽ
	
	//��
	time_roller[2] = lv_roller_create(win, NULL);
	lv_roller_set_options(time_roller[2],roller_string, LV_ROLLER_MODE_INIFINITE);
	lv_roller_set_selected(time_roller[2], RTC_time.Seconds, LV_ANIM_OFF);	  //����Ĭ��ֵ
	lv_roller_set_fix_width(time_roller[2], 40);							  //���ù̶����
	lv_roller_set_visible_row_count(time_roller[2], 1);					  //���ÿɼ�������
	lv_obj_align(time_roller[2], win,LV_ALIGN_OUT_BOTTOM_MID, 50, -90); //���ö��뷽ʽ

	for(i = 0; i < 2; i++)	
	{
		time_point_label[i] = lv_label_create(win, NULL);		//����ʱ��point lable
		lv_label_set_text(time_point_label[i], ":");
		lv_obj_align(time_point_label[i], time_roller[i], LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	}

	enter_btn = lv_btn_create(win, NULL);
	lv_obj_set_size(enter_btn,60, 40);		//���ð�ť��С
	lv_obj_align(enter_btn, win, LV_ALIGN_OUT_BOTTOM_MID, 0, -40);		//���ð�ť����

	lv_obj_t *enter_btn_lable = lv_label_create(enter_btn, NULL);		//������ťlabel
	lv_label_set_text(enter_btn_lable, "Enter"); 
	lv_obj_align(enter_btn_lable, enter_btn, LV_ALIGN_CENTER, 0, 0);		//���ð�ť�ı�����
	lv_obj_set_event_cb(enter_btn, event_handler); //�����¼��ص�����

	//����Ĭ�Ͻ���
	progress_set_val(progress_val);
	//����һ��������ģ����ȵļ��ع���
	if (task == NULL)
		task = lv_task_create(progress_task, 200, LV_TASK_PRIO_MID, NULL);
}
