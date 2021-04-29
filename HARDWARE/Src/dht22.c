#include "dht22.h"

// float temperature;
// float humidity;

void DHT22_Rst(void)
{
    DHT22_IO_OUT(); 	//���ģʽ
    DHT22_DQ_OUT = 0; 	//����
    rt_thread_mdelay(1);    	//Pull down Least 800us
    DHT22_DQ_OUT = 1; 	//����
    rt_hw_us_delay(30);     	//Pull up 20~40us
}

u8 DHT22_Check(void)
{
    u8 retry = 0;
    DHT22_IO_IN();      //����ģʽ

    while (DHT22_DQ_IN && retry < 100) //DHT22 Pull down 40~80us
    {
        retry++;
        rt_hw_us_delay(1);
    }

    if(retry >= 100)
    {
        return 1;
    }
    else
        retry = 0;

    while (!DHT22_DQ_IN && retry < 100) //DHT22 Pull up 40~80us
    {
        retry++;
        rt_hw_us_delay(1);
    }

    if(retry >= 100)
    {
        return 1;   //chack error

    }

    return 0;
}

u8 DHT22_Read_Bit(void)
{
    u8 retry = 0;

    while(DHT22_DQ_IN && retry < 100) //wait become Low level
    {
        retry++;
        rt_hw_us_delay(1);
    }

    retry = 0;

    while(!DHT22_DQ_IN && retry < 100) //wait become High level
    {
        retry++;
        rt_hw_us_delay(1);
    }

    rt_hw_us_delay(40);         //wait 40us

    if(DHT22_DQ_IN)
        return 1;
    else
        return 0;
}

u8 DHT22_Read_Byte(void)
{
    u8 i, dat;
    dat = 0;

    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT22_Read_Bit();
    }

    return dat;
}

u8 DHT22_Read_Data(float *temperature, float *humidity)
{
    u8 buf[5];
    u8 i;
    u8 sum;
    *humidity = 0;
    *temperature = 0;
    DHT22_Rst();

    if(DHT22_Check() == 0)
    {
        for(i = 0; i < 5; i++)
        {
            buf[i] = DHT22_Read_Byte();
        }

        sum = buf[0] + buf[1] + buf[2] + buf[3];

        if(sum == buf[4])
        {
            *humidity = (float)((buf[0] << 8) + buf[1]) / 10;
            *temperature = (float)((buf[2] << 8) + buf[3]) / 10;
        }
        else
        {
            *humidity = (float)((buf[0] << 8) + buf[1]) / 10;
            *temperature = (float)((buf[2] << 8) + buf[3]) / 10;
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ���
//����1:������
//����0:����
int DHT22_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();			//����GPIOGʱ��

    GPIO_Initure.Pin = GPIO_PIN_9;           	//PG9
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //�������
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);    //��ʼ��
    DHT22_Rst();
    return DHT22_Check();
}


//INIT_DEVICE_EXPORT(DHT22_Init);
