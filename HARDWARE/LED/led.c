#include "led.h"
#include "signal_led.h"

//�����źŵƶ�����
led_t *led0 = NULL;

//�����ڴ���������ӿ�
led_mem_opreation_t led_mem_opreation;

/*  �����źŵ�һ�������ڵ���˸ģʽ
 *  ��ʽΪ �������������������� ���������� ���Ȳ���
 *  ע�⣺  �����õ�λΪ���룬�ұ������ ��LED_TICK_TIME�� �꣬��Ϊ����������Ϊ������������ȡ������
 *          ������Ӣ�Ķ���Ϊ���������Ӣ�Ķ��Ž�β���ַ�����ֻ���������ּ����ţ������������ַ�����
 */
char *led_blink_mode_0 = "500,500,";          //1Hz��˸
char *led_blink_mode_1 = "50,50,";            //10Hz��˸
char *led_blink_mode_2 = "0,100,";            //����
char *led_blink_mode_3 = "100,0,";            //����
char *led_blink_mode_4 = "100,100,100,1000,"; //�ǹ̶�ʱ��
char *led_blink_mode_5 = "500,100,";
//���忪�ƺ���
void led0_switch_on(void)
{
    rt_pin_write(LED0_PIN, PIN_HIGH);
}

//����صƺ���
void led0_switch_off(void)
{
    rt_pin_write(LED0_PIN, PIN_LOW);
}

//��;�л�ģʽ����
void led_switch(void *param)
{
    rt_thread_mdelay(5000);
    led_set_mode(led0, LOOP_PERMANENT, led_blink_mode_1);
    rt_thread_mdelay(5000);
    led_set_mode(led0, 10, led_blink_mode_5);
}

void blink_over_callback(led_t *led_handler)
{
    led_set_mode(led_handler, LOOP_PERMANENT, led_blink_mode_4);
    led_start(led_handler);
}

static void led_run(void *parameter)
{
    while (1)
    {
        led_ticks();
        rt_thread_mdelay(LED_TICK_TIME);
    }
}

/**
 * @brief: led�źų�ʼ��
 * @version: 1.0
 * @Date: 2021-04-24 17:11:16
 */
int rt_led_timer_init(void)
{
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);	
    
	rt_pin_write(LED1_PIN, PIN_HIGH);
    /*�Զ����ڴ�����ӿ�
 *ע�⣺��Ҫ�����Զ����ڴ����������Ҫ�ڵ����κ�������ڽӿ�֮ǰ�����ã�
 *      �������ֲ������ϵĴ��󣡣���
 */
    led_mem_opreation.malloc_fn = (void *(*)(size_t))rt_malloc;
    led_mem_opreation.free_fn = rt_free;
    led_set_mem_operation(&led_mem_opreation);

    //��ʼ���źŵƶ���
    led0 = led_create(led0_switch_on, led0_switch_off);

    //�����źŵƹ���ģʽ��ѭ��ʮ��
    led_set_mode(led0, LOOP_PERMANENT, led_blink_mode_0);
    //�����źŵ���˸�����ص�����
    led_set_blink_over_callback(led0, blink_over_callback);

    //�����źŵ�
    led_start(led0);

    rt_thread_t tid = RT_NULL;
    tid = rt_thread_create("signal_led",
                           led_run,
                           RT_NULL,
                           512,
                           RT_THREAD_PRIORITY_MAX / 2,
                           100);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    /* �����߳�1 */
    tid = rt_thread_create("led_switch",
                           led_switch,
                           RT_NULL,
                           512,
                           RT_THREAD_PRIORITY_MAX / 2,
                           100);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}

INIT_APP_EXPORT(rt_led_timer_init);
