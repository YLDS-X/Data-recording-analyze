#include "main_win.h"
#include "app.h"
#include "calendar_win.h"
#include "chart_win.h"
#include "gauge_win.h"
#include "login_win.h"
#include "progress_win.h"
#include "cpu_usage.h"
#include "file_win.h"

//����ͼƬ
LV_IMG_DECLARE(logo_img);
LV_IMG_DECLARE(date_img);
LV_IMG_DECLARE(chart_img);
LV_IMG_DECLARE(gauge_img);
LV_IMG_DECLARE(progress_img);
LV_IMG_DECLARE(file_img);
//��������
#define TABLE_COL_CNT 3		 //��������
#define TABLE_ROW_CNT 4		 //���Ŀ��
#define SPLIT_LINE_WIDTH 200 //ˮƽ�ָ��ߵĿ��
const lv_point_t SPLIT_LINE_POINTS[2] = {{0, 0}, {SPLIT_LINE_WIDTH, 0}};
const char *const MBOX_MAP[] = {"Yes", "No", ""};
//������ÿһ�е�����
const char *const TABLE_CELL_VALUE[TABLE_ROW_CNT][TABLE_COL_CNT] = {
	{"Name", "Sex", "Age"}, //��һ��,��Ϊ������
	{"XYL", "Man", "20"},
	{"ZWH", "Man", "22"},
	{"CL", "Man", "20"}};

lv_obj_t *clock_time;
lv_obj_t *cpu_load;
lv_obj_t *main_win;
static lv_obj_t *quit_btn;
static lv_obj_t *quit_mbox = NULL;
static lv_obj_t *calendar_item_btn, *progress_item_btn, *chart_item_btn, *gauge_item_btn, *file_item_btn;
static lv_obj_t *header;
static lv_task_t *task = NULL;
extern u8 Time[40];
float load = 0;
//��������
void quit_mbox_create(lv_obj_t *parent);
static void header_create(void);

/**
 * @brief: ��ť�¼��ص�����
 * @version: 1.0
 * @Date: 2021-04-23 17:35:02
 * @param {lv_obj_t} *obj
 * @param {lv_event_t} event
 */
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
	uint16_t btn_id;
	if (obj == quit_btn)
	{
		if (event == LV_EVENT_RELEASED)
		{
			quit_mbox_create(lv_obj_get_parent(obj)); //�Ƿ��˳�?
		}
	}
	else if (obj == quit_mbox)
	{
		if (event == LV_EVENT_VALUE_CHANGED)
		{
			lv_obj_del(quit_mbox); //�ر��˳��Ի���
			quit_mbox = NULL;
			btn_id = *((uint16_t *)lv_event_get_data()); //��ȡ�˳��Ի����еİ�ťid
			if (btn_id == 0)							 //�����Yes��ť
			{
				lv_obj_del(main_win);			//ɾ��������
				login_win_create(lv_scr_act()); //���ص���¼����
			}
		}
	}
	else if (obj == calendar_item_btn)
	{
		//���б���ֹ���ʱ,��LV_EVENT_CLICKED�����������¼����LV_EVENT_RELEASED��һЩ,
		//��Ϊ�����б�ʱ,LV_EVENT_CLICKED�¼���������󴥷�,��LV_EVENT_RELEASED���󴥷�
		if (event == LV_EVENT_CLICKED)
			calendar_win_create(lv_scr_act()); //��ת��calendar����
	}
	else if (obj == progress_item_btn)
	{
		if (event == LV_EVENT_CLICKED)
			progress_win_create(lv_scr_act()); //��ת��progress����
	}
	else if (obj == chart_item_btn)
	{
		if (event == LV_EVENT_CLICKED)
			chart_win_create(lv_scr_act()); //��ת��chart����
	}
	else if (obj == gauge_item_btn)
	{
		if (event == LV_EVENT_CLICKED)
			gauge_win_create(lv_scr_act()); //��ת��gauge����
	}
	else if (obj == file_item_btn)
	{
		if (event == LV_EVENT_CLICKED)
			file_win_create(lv_scr_act()); //��ת��gauge����
	}
}

/**
 * @brief: �����˳�ȷ�ϵ���Ϣ�Ի���
 * @version: 1.0
 * @Date: 2021-04-23 17:34:52
 * @param {lv_obj_t} *parent
 */
void quit_mbox_create(lv_obj_t *parent)
{
	if (quit_mbox) //��Ҫ�ظ�����
		return;
	quit_mbox = lv_mbox_create(parent, NULL);
	lv_obj_set_size(quit_mbox, lv_obj_get_width(parent) * 0.7f, lv_obj_get_height(parent) / 2);
	lv_mbox_set_text(quit_mbox, "Tip\nAre you to quit?");
	lv_mbox_add_btns(quit_mbox, (const char **)MBOX_MAP);
	lv_obj_set_drag(quit_mbox, true); //���öԻ�����Ա���ק
	lv_obj_align(quit_mbox, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_event_cb(quit_mbox, event_handler);
}

/**
 * @brief: 500ms����ص�����:����ʱ���ռ����
 * @version: 1.0
 * @Date: 2021-04-23 17:34:33
 * @param {lv_task_t} *t
 */
static void progress_task(lv_task_t *t)
{
	char a[20];
	load = cpu_load_average();
	sprintf(a, "%.1f%%", load); 
	lv_label_set_text(clock_time, (char *)Time); //��ʾʱ��
	lv_label_set_text(cpu_load, (char *)a); //��ʾռ����
}

/**
 * @brief: ״̬����ʼ��:��ʾʱ���CPUռ����
 * @version: 1.0
 * @Date: 2021-04-23 17:34:08
 */
static void header_create(void)
{
	header = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
	lv_obj_set_width(header, lv_disp_get_hor_res(NULL));

	lv_obj_t *sym = lv_label_create(header, NULL);
	lv_label_set_text(sym, LV_SYMBOL_GPS LV_SYMBOL_WIFI LV_SYMBOL_BLUETOOTH LV_SYMBOL_VOLUME_MAX);
	lv_obj_align(sym, NULL, LV_ALIGN_IN_RIGHT_MID, -LV_DPI / 10, 0);

	clock_time = lv_label_create(header, NULL);
	lv_label_set_text(clock_time, (char *)Time); //��ʾʱ��
	lv_obj_align(clock_time, NULL, LV_ALIGN_IN_LEFT_MID, LV_DPI / 10, 0);

	cpu_load = lv_label_create(header, NULL);
	lv_label_set_text(cpu_load, 0); //��ʾCPUʹ����
	lv_obj_align(cpu_load, NULL, LV_ALIGN_CENTER, LV_DPI / 10 - 10, 0);

	lv_cont_set_fit2(header, LV_FIT_NONE, LV_FIT_TIGHT); /*Let the height set automatically*/
	lv_obj_set_pos(header, 0, 0);

	if (task == NULL)
		task = lv_task_create(progress_task, 500, LV_TASK_PRIO_HIGH, NULL);
}

/**
 * @brief: ��ҳѡ���ʼ��
 * @version: 1.0
 * @Date: 2021-04-23 17:33:18
 * @param {lv_obj_t} *parent
 */
void home_tab_init(lv_obj_t *parent)
{
	//�����б�
	lv_obj_t *list = lv_list_create(parent, NULL);
	lv_obj_set_size(list, lv_obj_get_width(parent) * 0.9f, lv_obj_get_height(parent) - 40);
	lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 15);
	lv_list_set_sb_mode(list, LV_SB_MODE_AUTO);

	//����б���1:����
	calendar_item_btn = lv_list_add_btn(list, &date_img, "#FFFFFF Calendar#\n#666666 This is the date#");
	lv_obj_t *item_label = lv_list_get_btn_label(calendar_item_btn);
	lv_label_set_recolor(item_label, true);
	lv_obj_set_event_cb(calendar_item_btn, event_handler); //�����¼��ص�����

	//����б���2:ʱ��
	progress_item_btn = lv_list_add_btn(list, &progress_img, "#FFFFFF Clock#\n#666666 This is the time#");
	item_label = lv_list_get_btn_label(progress_item_btn);
	lv_label_set_recolor(item_label, true);
	lv_obj_set_event_cb(progress_item_btn, event_handler); //�����¼��ص�����

	//����б���3:ͼ��
	chart_item_btn = lv_list_add_btn(list, &chart_img, "#FFFFFF Chart#\n#666666 This is the chart#");
	item_label = lv_list_get_btn_label(chart_item_btn);
	lv_label_set_recolor(item_label, true);
	lv_obj_set_event_cb(chart_item_btn, event_handler); //�����¼��ص�����

	//����б���4:�Ǳ�
	gauge_item_btn = lv_list_add_btn(list, &gauge_img, "#FFFFFF Gauge#\n#666666 This is the Gauge#");
	item_label = lv_list_get_btn_label(gauge_item_btn);
	lv_label_set_recolor(item_label, true);
	lv_obj_set_event_cb(gauge_item_btn, event_handler); //�����¼��ص�����

	//����б���5:�ļ�
	file_item_btn = lv_list_add_btn(list, &file_img, "#FFFFFF File#\n#666666 This is the File#");
	item_label = lv_list_get_btn_label(file_item_btn);
	lv_label_set_recolor(item_label, true);
	lv_obj_set_event_cb(file_item_btn, event_handler); //�����¼��ص�����
}

/**
 * @brief: ����ѡ���ʼ��
 * @version: 1.0
 * @Date: 2021-04-23 17:33:41
 * @param {lv_obj_t} *parent
 */
void setting_tab_init(lv_obj_t *parent)
{
	uint16_t parent_width = lv_obj_get_width(parent); //��ȡ������Ŀ��

	//����һ�����
	lv_obj_t *table = lv_table_create(parent, NULL);
	lv_table_set_col_cnt(table, TABLE_COL_CNT); //��������
	lv_table_set_row_cnt(table, TABLE_ROW_CNT); //��������
	uint8_t row, col;
	for (col = 0; col < TABLE_COL_CNT; col++)
	{
		lv_table_set_col_width(table, col, parent_width * (0.8f / TABLE_COL_CNT)); //�����еĿ��
		lv_table_set_cell_align(table, 0, col, LV_LABEL_ALIGN_CENTER);			   //�����е��ı����ж���
	}
	for (row = 0; row < TABLE_ROW_CNT; row++)
	{
		for (col = 0; col < TABLE_COL_CNT; col++)
			lv_table_set_cell_value(table, row, col, (const char *)TABLE_CELL_VALUE[row][col]); //����ÿһ����Ԫ����ı�����
	}
	lv_obj_align(table, NULL, LV_ALIGN_CENTER, 0, 10); //���ñ��Ķ��뷽ʽ

	//��ʾ����
	static lv_style_t title_style;
	lv_obj_t *label1 = lv_label_create(parent,NULL);
	lv_label_set_long_mode(label1,LV_LABEL_LONG_BREAK);//���ó��ı�ģʽ
	lv_obj_set_width(label1,239);//���ÿ��
	lv_label_set_align(label1,LV_LABEL_ALIGN_CENTER);//�����ı����ж���
	lv_label_set_body_draw(label1,true);//ʹ�ܱ����ػ�
	lv_obj_align(label1,table,LV_ALIGN_OUT_TOP_MID,0,-20);//���ö��뷽ʽ

	lv_style_copy(&title_style, lv_obj_get_style(label1));	
	title_style.text.font = &lv_font_roboto_22;													//�ı�����
	lv_label_set_style(label1, LV_LABEL_STYLE_MAIN, &title_style);	
	lv_label_set_text(label1,"#FFFF00 Data recording analyzer#");//�����ı�
	lv_label_set_recolor(label1,true);//ʹ���ı��ػ�ɫ

	//����һ���˳���ť
	quit_btn = lv_btn_create(parent, NULL);
	lv_obj_set_width(quit_btn, parent_width / 2);
	lv_obj_align(quit_btn, table, LV_ALIGN_OUT_BOTTOM_MID, 0, 30);
	lv_obj_t *quit_label = lv_label_create(quit_btn, NULL);
	lv_label_set_recolor(quit_label, true);
	lv_label_set_text(quit_label, "#FF0000 Quit#");
	lv_obj_set_event_cb(quit_btn, event_handler);
}

/**
 * @brief: ����������
 * @version: 1.0
 * @Date: 2021-04-23 17:33:58
 * @param {lv_obj_t} *parent
 */
void main_win_create(lv_obj_t *parent)
{
	//Ϊ�˼��ٸ�����,���������ֱ�Ӱ�tabview��Ϊmain_win������

	main_win = lv_tabview_create(parent, NULL);
	lv_tabview_set_btns_pos(main_win, LV_TABVIEW_BTNS_POS_BOTTOM); //ѡ���ťλ�ڵײ�
																   //���tab1ѡ�
	lv_obj_t *home_tab = lv_tabview_add_tab(main_win, LV_SYMBOL_HOME " Home");
	home_tab_init(home_tab);
	//���tab1ѡ�
	lv_obj_t *setting_tab = lv_tabview_add_tab(main_win, LV_SYMBOL_SETTINGS " Setting");
	setting_tab_init(setting_tab);
	header_create();
}
