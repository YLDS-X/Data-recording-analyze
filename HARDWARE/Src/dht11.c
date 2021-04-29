/*
 * @Descripttion:
 * @version: 1.0
 * @Author: YLDS
 * @Date: 2021-04-03 22:42:56
 * @LastEditTime: 2021-04-25 20:15:22
 */
#include "dht11.h"
#include "app.h"
#include "calendar_win.h"
#include "led.h"
#include "rtc.h"
#include "progress_win.h"
#include <dfs_posix.h> /* ����Ҫʹ���ļ�����ʱ����Ҫ�������ͷ�ļ� */
u8 temperature[40];
u8 humidity[40];
#define WRITE_EVENT (0x01U << 0)          /* ����Ȥ���¼� */
#define RINGBUFFERSIZE (4069)             /* ringbuffer��������С */
#define THRESHOLD (RINGBUFFERSIZE / 2)    /* ringbuffer��������ֵ */
static rt_event_t recvdata_event;         /* �¼��� */
rt_mq_t sensor_mq;
u8 temperature_H = 40;
u8 temperature_L = 15;
u8 humidity_H = 90;
u8 humidity_L = 10;
int data_collection;
extern u8 Time[40];
extern u8 Date[40];
extern lv_calendar_date_t today;
extern Clock_time RTC_time;
rt_timer_t temp_humi;

//��λDHT11
void DHT11_Rst(void)
{
    DHT11_IO_OUT();       //����Ϊ���
    DHT11_DQ_OUT = 0;     //����DQ
    rt_thread_mdelay(20); //��������18ms
    DHT11_DQ_OUT = 1;     //DQ=1
    rt_hw_us_delay(30);   //��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_Check(void)
{
    u8 retry = 0;
    DHT11_IO_IN(); //����Ϊ���

    while (DHT11_DQ_IN && retry < 100) //DHT11������40~80us
    {
        retry++;
        rt_hw_us_delay(1);
    };

    if (retry >= 100)
        return 1;
    else
        retry = 0;

    while (!DHT11_DQ_IN && retry < 100) //DHT11���ͺ���ٴ�����40~80us
    {
        retry++;
        rt_hw_us_delay(1);
    };

    if (retry >= 100)
        return 1;

    return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_Read_Bit(void)
{
    u8 retry = 0;

    while (DHT11_DQ_IN && retry < 100) //�ȴ���Ϊ�͵�ƽ
    {
        retry++;
        rt_hw_us_delay(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) //�ȴ���ߵ�ƽ
    {
        retry++;
        rt_hw_us_delay(1);
    }

    rt_hw_us_delay(40); //�ȴ�40us

    if (DHT11_DQ_IN)
        return 1;
    else
        return 0;
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
u8 DHT11_Read_Byte(void)
{
    u8 i, dat;
    dat = 0;

    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }

    return dat;
}

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_Read_Data(u8 *temp, u8 *humi)
{
    u8 buf[5];
    u8 i;
    DHT11_Rst();

    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++) //��ȡ40λ����
        {
            buf[i] = DHT11_Read_Byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
        return 1;

    return 0;
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����
int DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE(); //����GPIOGʱ��

    GPIO_Initure.Pin = GPIO_PIN_9;           //PG9
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //�������
    GPIO_Initure.Pull = GPIO_PULLUP;         //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    //����
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);     //��ʼ��

    DHT11_Rst();
    return DHT11_Check();
}

//n����������p��λ��
void reverse(u8 R[], int from, int to)
{
    int i, temp;

    for (i = 0; i < (to - from + 1) / 2; i++)
    {
        temp = R[from + i];
        R[from + i] = R[to - i];
        R[to - i] = temp;
    }
}

void shift(u8 R[], int n, int p)
{
    reverse(R, 0, p - 1);
    reverse(R, p, n - 1);
    reverse(R, 0, n - 1);
}

/**
 * @brief: ������ʪ��������
 * @version: 1.0
 * @Date: 2021-04-23 10:08:18
 * @param {u8} temp_up
 * @param {u8} temp_low
 * @param {u8} humi_up
 * @param {u8} humi_low
 */
void Amplitude_setting(u8 temp_up, u8 temp_low, u8 humi_up, u8 humi_low)
{
    temperature_H = temp_up;
    temperature_L = temp_low;
    humidity_H = humi_up;
    humidity_L = humi_low;
}

/**
 * @brief: ��ʪ�ȱ���
 * @version: 1.0
 * @Date: 2021-04-23 10:08:02
 */
void Warning(void)
{
    if (temperature[39] > temperature_H || temperature[39] < temperature_L || humidity[39] > humidity_H || humidity[39] < humidity_L)
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
    else
        rt_pin_write(GET_PIN(F, 10), PIN_HIGH);
}

/**
 * @brief: ��ȡ��ʪ��
 * @version: 1.0
 * @Date: 2021-04-23 10:08:55
 * @param {void} *parameter
 */
static void read_temp_entry(void *parameter)
{
    static SENSOR_DataTypeDef value;
    DHT11_Init();
    data_collection = 1;

    while (1)
    {
        if(DHT11_Read_Data(& value.temperature, &value.humidity) && data_collection)
        {
            //��ȡ��ʪ��ֵ
            temperature[0] = value.temperature;
            humidity[0] = value.humidity;
            shift(temperature, 40, 1);
            shift(humidity, 40, 1);
            Warning();
            rtc_read();
            rt_mq_send(sensor_mq, &value, sizeof(value));
            rt_event_send(recvdata_event, WRITE_EVENT);
        }

        rt_thread_delay(500);
    }
}

/**
 * @brief: �����ļ��к��ļ��洢��ʪ������
 * @version: 1.0
 * @Date: 2021-04-23 10:09:11
 * @param {void} *parameter
 */
static void save_data_entry(void *parameter)
{
    char recvdatafile_p0;
    rt_uint32_t set;
    static SENSOR_DataTypeDef sensordata;
    static int writebuffer[1024];
    static char str_data[64];
    static struct rt_ringbuffer *recvdatabuf;           /* ringbuffer */
    rt_size_t size;
    char folder_name[64] = {0};
    char file_name[40] = {0};
    char name[104] = {0};
    recvdatabuf = rt_ringbuffer_create(RINGBUFFERSIZE); /* ringbuffer�Ĵ�С��4KB */
    RT_ASSERT(recvdatabuf);

    while (1)
    {
        /* ���ո���Ȥ���¼�WRITE_EVENT�������õȴ���ʽȥ���� */
        if (rt_event_recv(recvdata_event, WRITE_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &set) == RT_EOK)
        {
            if(rt_mq_recv(sensor_mq, &sensordata, sizeof(sensordata), RT_WAITING_FOREVER) == RT_EOK)
            {
                sprintf(str_data, "%d,%d,%d:%d:%d\n",  sensordata.temperature, sensordata.humidity, RTC_time.Hours, RTC_time.Minutes, RTC_time.Seconds);
                rt_ringbuffer_put(recvdatabuf, (rt_uint8_t *)str_data, strlen(str_data));      /* �����ݴ�ŵ�ringbuffer�� */

                if (RTC_time.Seconds == 59)
                {
                    /* ����ֵ��ֱ��д���� */
                    sprintf(folder_name, "/sdcard/%02d-%02d-%02d", today.year, today.month, today.day);
                    mkdir(folder_name, 0x777);
                    sprintf(file_name, "%02d-%02d", RTC_time.Hours, RTC_time.Minutes);
                    sprintf(name, "%s/%s.csv", folder_name, file_name);

                    recvdatafile_p0 = open(name, O_RDWR | O_CREAT);

                    if (recvdatafile_p0 != NULL)
                    {
                        while (rt_ringbuffer_data_len(recvdatabuf))
                        {
                            size = rt_ringbuffer_get(recvdatabuf, (rt_uint8_t *)writebuffer, THRESHOLD);
                            write(recvdatafile_p0, writebuffer, size);
                            rt_kprintf("write success!\n");
                        }

                        close(recvdatafile_p0);
                    }

                    rt_thread_delay(500);
                }
            }
        }
    }
}

/**
 * @brief: ��ʪ�ȶ�ȡ�ʹ����̳߳�ʼ��
 * @version: 1.0
 * @Date: 2021-04-23 10:09:34
 */
static int dht11_read(void)
{
    rt_thread_t DFS_thread, temp_humi;
    recvdata_event = rt_event_create("temp_evt0", RT_IPC_FLAG_FIFO);
    RT_ASSERT(recvdata_event);
    sensor_mq = rt_mq_create("SenMQ", sizeof(SENSOR_DataTypeDef), 16, RT_IPC_FLAG_FIFO);
    RT_ASSERT(sensor_mq);

    temp_humi = rt_thread_create("humi_th",
                                 read_temp_entry,
                                 RT_NULL,
                                 1024,
                                 7,
                                 20);

    rt_thread_delay(1000);

    if (temp_humi)
        rt_thread_startup(temp_humi);

    DFS_thread = rt_thread_create("DFSsave",
                                  save_data_entry,
                                  RT_NULL,
                                  2048,
                                  7 - 1,
                                  10);

    if (DFS_thread != RT_NULL)
    {
        rt_thread_startup(DFS_thread);
    }

    return RT_EOK;
}

INIT_APP_EXPORT(dht11_read);
