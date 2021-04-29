#include "sdio_sdcard.h"
#include "string.h"
#include <dfs_fs.h>
#include "drv_spi.h"
#include "spi_msd.h"
#include <dfs_posix.h> /* ����Ҫʹ���ļ�����ʱ����Ҫ�������ͷ�ļ� */
SD_HandleTypeDef        SDCARD_Handler;     //SD�����
HAL_SD_CardInfoTypeDef  SDCardInfo;         //SD����Ϣ�ṹ��
DMA_HandleTypeDef SDTxDMAHandler, SDRxDMAHandler;   //SD��DMA���ͺͽ��վ��

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__align(4) u8 SDIO_DATA_BUFFER[512];

//�õ�����Ϣ
//cardinfo:����Ϣ�洢��
//����ֵ:����״̬
u8 SD_GetCardInfo(HAL_SD_CardInfoTypeDef *cardinfo)
{
    u8 sta;
    sta = HAL_SD_GetCardInfo(&SDCARD_Handler, cardinfo);
    return sta;
}

//�ж�SD���Ƿ���Դ���(��д)����
//����ֵ:SD_TRANSFER_OK ������ɣ����Լ�����һ�δ���
//		 SD_TRANSFER_BUSY SD����æ�������Խ�����һ�δ���
u8 SD_GetCardState(void)
{
    return((HAL_SD_GetCardState(&SDCARD_Handler) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
u8 SD_ReadDisk(u8* buf, u32 sector, u32 cnt)
{
    u8 sta = HAL_OK;
    u32 timeout = SD_TIMEOUT;
    long long lsector = sector;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    sta = HAL_SD_ReadBlocks(&SDCARD_Handler, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT); //���sector�Ķ�����

    //�ȴ�SD������
    while(SD_GetCardState() != SD_TRANSFER_OK)
    {
        if(timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    INTX_ENABLE();//�������ж�
    return sta;
}

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
u8 SD_WriteDisk(u8 *buf, u32 sector, u32 cnt)
{
    u8 sta = HAL_OK;
    u32 timeout = SD_TIMEOUT;
    long long lsector = sector;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    sta = HAL_SD_WriteBlocks(&SDCARD_Handler, (uint8_t*)buf, lsector, cnt, SD_TIMEOUT); //���sector��д����

    //�ȴ�SD��д��
    while(SD_GetCardState() != SD_TRANSFER_OK)
    {
        if(timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    INTX_ENABLE();//�������ж�
    return sta;
}

#define SDCARD_MOUNTPOINT "/sdcard"

/**
 * @brief: ����sd��
 * @version: 1.0
 * @Date: 2021-04-24 17:07:41
 * @param {void} *parameter
 */
void sd_mount(void *parameter)
{
    int ret;
    ret = mkdir(SDCARD_MOUNTPOINT, 0x777);

    if(rt_device_find("sd0") != RT_NULL)
    {
        if (dfs_mount("sd0", SDCARD_MOUNTPOINT, "elm", 0, 0) == RT_EOK)
        {
            LOG_I("sd card mount success.\r\n");
        }
        else
        {
            LOG_W("sd card mount failed!\r\n");
        }
    }

    rt_thread_mdelay(500);
}

int stm32_sdcard_mount(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("sd_mount", sd_mount, RT_NULL,
                           1024, RT_THREAD_PRIORITY_MAX - 2, 20);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    else
    {
        LOG_E("create sd_mount thread err!");
    }

    return RT_EOK;
}
INIT_APP_EXPORT(stm32_sdcard_mount);
