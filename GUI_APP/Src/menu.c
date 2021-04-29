/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-03 17:13:18
 * @LastEditTime: 2021-04-23 10:58:57
 */

#include "menu.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_apps\demo\demo.h"
#include "app.h"
static rt_device_t device;
static struct rt_device_graphic_info info;

/**
 * @brief: ������ɨ��
 * @version: 1.0
 * @Date: 2021-04-23 10:12:42
 * @param {void} *parameter
 */
static void touch_entry(void *parameter)
{
	u8 key;
	tp_dev.init();				    //��������ʼ�� 
	while(1)
	{
		key=KEY_Scan(0);
		tp_dev.scan(0);
		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);	//����
		    TP_Adjust();  		//��ĻУ׼ 
			TP_Save_Adjdata();	 
		}
		rt_thread_delay(10);
	}
}

/**
 * @brief: ��ʼ��GUI���ṩGUI����
 * @version: 1.0
 * @Date: 2021-04-23 10:12:58
 * @param {void} *parameter
 */
static void gui_entry(void *parameter)
{
	device = rt_device_find("lcd");
    RT_ASSERT(device != RT_NULL);

    if (rt_device_open(device, RT_DEVICE_OFLAG_RDWR) == RT_EOK)
    {
        rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info);
    }

	lv_init(); //lvglϵͳ��ʼ��
	lv_port_disp_init(); //lvgl��ʾ�ӿڳ�ʼ�� ,���� lv_init()�ĺ���
	lv_port_indev_init(); //lvgl����ӿڳ�ʼ�� ,���� lv_init()�ĺ���
	
	gui_app_start();
	while(1)
	{
		lv_task_handler();	
		rt_thread_delay(10);
	}
}

/**
 * @brief: ������ɨ���GUI�����̳߳�ʼ��
 * @version: 1.0
 * @Date: 2021-04-23 10:12:11
 */
int APP(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("touch",
                           touch_entry, 
						   RT_NULL,
                           512,
                           8, 
						   10);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    tid = rt_thread_create("GUI_APP",
                           gui_entry, 
						   RT_NULL,
                           16*1024,
                           9, 
						   20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}

INIT_APP_EXPORT(APP);

