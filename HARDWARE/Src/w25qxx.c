#include "w25qxx.h"
#include "drv_spi.h"
#include "spi.h"
#include "stm32f4xx_hal_gpio.h"
#include "usart.h"
#include <dfs_fs.h>
#include <dfs_posix.h> /* ����Ҫʹ���ļ�����ʱ����Ҫ�������ͷ�ļ� */
#define W25Q_SPI_DEVICE_NAME     "W25Q128"

u16 W25QXX_TYPE = W25Q256;  //Ĭ����W25Q256

// 4KbytesΪһ��Sector
// 16������Ϊ1��Block
// W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector

//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{
    u8 temp;
    W25QXX_CS = 1;                           // SPI FLASH��ѡ��
    SPI1_Init();                             //��ʼ��SPI
    SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_2);  //����Ϊ42Mʱ��,����ģʽ
    W25QXX_TYPE = W25QXX_ReadID();           //��ȡFLASH ID.

    if (W25QXX_TYPE == W25Q256)              // SPI FLASHΪW25Q256
    {
        temp = W25QXX_ReadSR(3);  //��ȡ״̬�Ĵ���3���жϵ�ַģʽ

        if ((temp & 0X01) == 0)  //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
        {
            W25QXX_CS = 0;  //ѡ��
            SPI1_ReadWriteByte(
                W25X_Enable4ByteAddr);  //���ͽ���4�ֽڵ�ַģʽָ��
            W25QXX_CS = 1;              //ȡ��Ƭѡ
        }
    }
}

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
// BIT7  6   5   4   3   2   1   0
// SPR   RV  TB BP2 BP1 BP0 WEL BUSY
// SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
// TB,BP2,BP1,BP0:FLASH����д��������
// WEL:дʹ������
// BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
// BIT7  6   5   4   3   2   1   0
// SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
// BIT7      6    5    4   3   2   1   0
// HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
// regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
u8 W25QXX_ReadSR(u8 regno)
{
    u8 byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = W25X_ReadStatusReg1;  //��״̬�Ĵ���1ָ��
            break;

        case 2:
            command = W25X_ReadStatusReg2;  //��״̬�Ĵ���2ָ��
            break;

        case 3:
            command = W25X_ReadStatusReg3;  //��״̬�Ĵ���3ָ��
            break;

        default:
            command = W25X_ReadStatusReg1;
            break;
    }

    W25QXX_CS = 0;                    //ʹ������
    SPI1_ReadWriteByte(command);      //���Ͷ�ȡ״̬�Ĵ�������
    byte = SPI1_ReadWriteByte(0Xff);  //��ȡһ���ֽ�
    W25QXX_CS = 1;                    //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(u8 regno, u8 sr)
{
    u8 command = 0;

    switch (regno)
    {
        case 1:
            command = W25X_WriteStatusReg1;  //д״̬�Ĵ���1ָ��
            break;

        case 2:
            command = W25X_WriteStatusReg2;  //д״̬�Ĵ���2ָ��
            break;

        case 3:
            command = W25X_WriteStatusReg3;  //д״̬�Ĵ���3ָ��
            break;

        default:
            command = W25X_WriteStatusReg1;
            break;
    }

    W25QXX_CS = 0;                //ʹ������
    SPI1_ReadWriteByte(command);  //����дȡ״̬�Ĵ�������
    SPI1_ReadWriteByte(sr);       //д��һ���ֽ�
    W25QXX_CS = 1;                //ȡ��Ƭѡ
}
// W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
    W25QXX_CS = 0;                         //ʹ������
    SPI1_ReadWriteByte(W25X_WriteEnable);  //����дʹ��
    W25QXX_CS = 1;                         //ȡ��Ƭѡ
}
// W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
    W25QXX_CS = 0;                          //ʹ������
    SPI1_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��
    W25QXX_CS = 1;                          //ȡ��Ƭѡ
}

//��ȡоƬID
//����ֵ����:
// 0XEF13,��ʾоƬ�ͺ�ΪW25Q80
// 0XEF14,��ʾоƬ�ͺ�ΪW25Q16
// 0XEF15,��ʾоƬ�ͺ�ΪW25Q32
// 0XEF16,��ʾоƬ�ͺ�ΪW25Q64
// 0XEF17,��ʾоƬ�ͺ�ΪW25Q128
// 0XEF18,��ʾоƬ�ͺ�ΪW25Q256
u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;
    W25QXX_CS = 0;
    SPI1_ReadWriteByte(0x90);  //���Ͷ�ȡID����
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    Temp |= SPI1_ReadWriteByte(0xFF) << 8;
    Temp |= SPI1_ReadWriteByte(0xFF);
    W25QXX_CS = 1;
    return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
// pBuffer:���ݴ洢��
// ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
// NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    u16 i;
    W25QXX_CS = 0;                      //ʹ������
    SPI1_ReadWriteByte(W25X_ReadData);  //���Ͷ�ȡ����

    if (W25QXX_TYPE == W25Q256)  //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((u8)((ReadAddr) >> 24));
    }

    SPI1_ReadWriteByte((u8)((ReadAddr) >> 16));  //����24bit��ַ
    SPI1_ReadWriteByte((u8)((ReadAddr) >> 8));
    SPI1_ReadWriteByte((u8)ReadAddr);

    for (i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = SPI1_ReadWriteByte(0XFF);  //ѭ������
    }

    W25QXX_CS = 1;
}
// SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 i;
    W25QXX_Write_Enable();                 // SET WEL
    W25QXX_CS = 0;                         //ʹ������
    SPI1_ReadWriteByte(W25X_PageProgram);  //����дҳ����

    if (W25QXX_TYPE == W25Q256)  //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((u8)((WriteAddr) >> 24));
    }

    SPI1_ReadWriteByte((u8)((WriteAddr) >> 16));  //����24bit��ַ
    SPI1_ReadWriteByte((u8)((WriteAddr) >> 8));
    SPI1_ReadWriteByte((u8)WriteAddr);

    for (i = 0; i < NumByteToWrite; i++)
        SPI1_ReadWriteByte(pBuffer[i]);  //ѭ��д��

    W25QXX_CS = 1;                       //ȡ��Ƭѡ
    W25QXX_Wait_Busy();                  //�ȴ�д�����
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���65535)
// CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256;  //��ҳʣ����ֽ���

    if (NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite;  //������256���ֽ�

    while (1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);

        if (NumByteToWrite == pageremain)
            break;  //д�������
        else        // NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;  //��ȥ�Ѿ�д���˵��ֽ���

            if (NumByteToWrite > 256)
                pageremain = 256;  //һ�ο���д��256���ֽ�
            else
                pageremain = NumByteToWrite;  //����256���ֽ���
        }
    };
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
// pBuffer:���ݴ洢��
// WriteAddr:��ʼд��ĵ�ַ(24bit)
// NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8* W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr / 4096;  //������ַ
    secoff = WriteAddr % 4096;  //�������ڵ�ƫ��
    secremain = 4096 - secoff;  //����ʣ��ռ��С

    // printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if (NumByteToWrite <= secremain)
        secremain = NumByteToWrite;  //������4096���ֽ�

    while (1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);  //������������������

        for (i = 0; i < secremain; i++)                //У������
        {
            if (W25QXX_BUF[secoff + i] != 0XFF) break;  //��Ҫ����
        }

        if (i < secremain)  //��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);     //�����������

            for (i = 0; i < secremain; i++)  //����
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }

            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096,
                                 4096);  //д����������

        }
        else
            W25QXX_Write_NoCheck(
                pBuffer, WriteAddr,
                secremain);  //д�Ѿ������˵�,ֱ��д������ʣ������.

        if (NumByteToWrite == secremain)
            break;  //д�������
        else        //д��δ����
        {
            secpos++;    //������ַ��1
            secoff = 0;  //ƫ��λ��Ϊ0

            pBuffer += secremain;         //ָ��ƫ��
            WriteAddr += secremain;       //д��ַƫ��
            NumByteToWrite -= secremain;  //�ֽ����ݼ�

            if (NumByteToWrite > 4096)
                secremain = 4096;  //��һ����������д����
            else
                secremain = NumByteToWrite;  //��һ����������д����
        }
    };
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();  // SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS = 0;                       //ʹ������
    SPI1_ReadWriteByte(W25X_ChipErase);  //����Ƭ��������
    W25QXX_CS = 1;                       //ȡ��Ƭѡ
    W25QXX_Wait_Busy();                  //�ȴ�оƬ��������
}
//����һ������
// Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //����falsh�������,������
    // printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();  // SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS = 0;                         //ʹ������
    SPI1_ReadWriteByte(W25X_SectorErase);  //������������ָ��

    if (W25QXX_TYPE == W25Q256)  //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((u8)((Dst_Addr) >> 24));
    }

    SPI1_ReadWriteByte((u8)((Dst_Addr) >> 16));  //����24bit��ַ
    SPI1_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPI1_ReadWriteByte((u8)Dst_Addr);
    W25QXX_CS = 1;       //ȡ��Ƭѡ
    W25QXX_Wait_Busy();  //�ȴ��������
}
//�ȴ�����
void W25QXX_Wait_Busy(void)
{
    while ((W25QXX_ReadSR(1) & 0x01) == 0x01)
        ;  // �ȴ�BUSYλ���
}
//�������ģʽ
void W25QXX_PowerDown(void)
{
    W25QXX_CS = 0;                       //ʹ������
    SPI1_ReadWriteByte(W25X_PowerDown);  //���͵�������
    W25QXX_CS = 1;                       //ȡ��Ƭѡ
    rt_hw_us_delay(3);                   //�ȴ�TPD
}
//����
void W25QXX_WAKEUP(void)
{
    W25QXX_CS = 0;  //ʹ������
    SPI1_ReadWriteByte(
        W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB
    W25QXX_CS = 1;               //ȡ��Ƭѡ
    rt_hw_us_delay(3);           //�ȴ�TRES1
}

static void stat_sample(void)
{
    int ret;
    struct stat buf;
    ret = stat("/recvdata_p0.csv", &buf);

    if(ret == 0)
        rt_kprintf("/recvdata_p0.csv file size = %d\n", buf.st_size);
    else
        rt_kprintf("/recvdata_p0.csv file not fonud\n");
}
/* ������ msh �����б��� */
MSH_CMD_EXPORT(stat_sample, show text.txt stat sample);

static void readwrite_sample(void)
{
    int fd, size;
    char s[] = "RT-Thread Programmer!", buffer[80];

    rt_kprintf("Write string %s to test.txt.\n", s);

    /* �Դ����Ͷ�дģʽ�� /text.txt �ļ���������ļ��������򴴽����ļ� */
    fd = open("/text.txt", O_WRONLY | O_CREAT);

    if (fd >= 0)
    {
        write(fd, s, sizeof(s));
        close(fd);
        rt_kprintf("Write done.\n");
    }

    /* ��ֻ��ģʽ�� /text.txt �ļ� */
    fd = open("/text.txt", O_RDONLY);

    if (fd >= 0)
    {
        size = read(fd, buffer, sizeof(buffer));
        close(fd);
        rt_kprintf("Read from file test.txt : %s \n", buffer);

        if (size < 0)
            return ;
    }
}
/* ������ msh �����б��� */
MSH_CMD_EXPORT(readwrite_sample, readwrite sample);

static void spi_w25q_sample(int argc, char *argv[])
{
    struct rt_spi_device *spi_dev_w25q;
    char name[RT_NAME_MAX];
    rt_uint8_t w25x_read_id = 0x90;
    rt_uint8_t id[5] = {0};

    if (argc == 2)
    {
        rt_strncpy(name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(name, W25Q_SPI_DEVICE_NAME, RT_NAME_MAX);
    }

    /* ���� spi �豸��ȡ�豸��� */
    spi_dev_w25q = (struct rt_spi_device *)rt_device_find(name);

    if (!spi_dev_w25q)
    {
        rt_kprintf("spi sample run failed! can't find %s device!\n", name);
    }
    else
    {
        /* ��ʽ1��ʹ�� rt_spi_send_then_recv()���������ȡID */
        rt_spi_send_then_recv(spi_dev_w25q, &w25x_read_id, 1, id, 5);
        rt_kprintf("use rt_spi_send_then_recv() read w25q ID is:%x%x\n", id[3], id[4]);

        /* ��ʽ2��ʹ�� rt_spi_transfer_message()���������ȡID */
        struct rt_spi_message msg1, msg2;

        msg1.send_buf   = &w25x_read_id;
        msg1.recv_buf   = RT_NULL;
        msg1.length     = 1;
        msg1.cs_take    = 1;
        msg1.cs_release = 0;
        msg1.next       = &msg2;

        msg2.send_buf   = RT_NULL;
        msg2.recv_buf   = id;
        msg2.length     = 5;
        msg2.cs_take    = 0;
        msg2.cs_release = 1;
        msg2.next       = RT_NULL;

        rt_spi_transfer_message(spi_dev_w25q, &msg1);
        rt_kprintf("use rt_spi_transfer_message() read w25q ID is:%x%x\n", id[3], id[4]);

    }
}
/* ������ msh �����б��� */
MSH_CMD_EXPORT(spi_w25q_sample, spi w25q sample);

/**
 * @brief: ����flash��"/"
 * @version: 1.0
 * @Date: 2021-04-24 17:08:25
 */
int mnt_init(void)
{
    rt_thread_delay(RT_TICK_PER_SECOND);

    if (dfs_mount(W25Q_SPI_DEVICE_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    }
    else
    {
        if(dfs_mkfs("elm", W25Q_SPI_DEVICE_NAME) == 0)
        {
            if (dfs_mount(W25Q_SPI_DEVICE_NAME, "/w25q128", "elm", 0, 0) == 0)
            {
                rt_kprintf("file system initialization done!\n");
            }
            else
            {
                rt_kprintf("file system initialization failed!\n");
            }
        }
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);

/**
 * @brief: ע��spi_flash�豸
 * @version: 1.0
 * @Date: 2021-04-24 17:08:09
 */
static int rt_hw_spi_flash_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    rt_hw_spi_device_attach("spi1", "spi10", GPIOB, GPIO_PIN_14);
    W25QXX_Init();

    if (RT_NULL == rt_sfud_flash_probe("W25Q128", "spi10"))
    {
        return -RT_ERROR;
    };

    return RT_EOK;
}
/* �������Զ���ʼ�� */
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);
