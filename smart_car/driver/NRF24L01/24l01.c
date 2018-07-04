#include "24l01.h"
#include "bsp_spi.h" 
#include "stm32f10x.h"

const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

 //��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_APB2PeriphResetCmd(NRF24L01_CE_CLK|NRF24L01_CE_CLK|NRF24L01_CE_CLK,ENABLE);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = NRF24L01_CE_PIN|NRF24L01_CE_PIN;
        GPIO_Init(GPIOC,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = NRF24L01_IRQ_PIN;
        GPIO_Init(GPIOC,&GPIO_InitStructure);
        
        NRF24L01_CE=0;          //ʹ��24L01
        NRF24L01_CSN=1;         //SPIƬѡȡ��
}
//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ�� 
u8 NRF24L01_Check(void)
{
        u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
        u8 i;
        SPI_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��    
        NRF24L01_Write_Buf(WRITE+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.        
        NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
        for(i=0;i<5;i++)if(buf[i]!=0XA5)break;  
        if(i!=5)return 1;//���24L01����    
        return 0;       //��⵽24L01
}       
u8 NRF_Check(void)
{
        u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
        u8 buf1[5];
        u8 i;

        /*д�� 5 ���ֽڵĵ�ַ. */
        NRF24L01_Write_Buf(WRITE+TX_ADDR,buf,5);

        /*����д��ĵ�ַ */
        NRF24L01_Read_Buf(TX_ADDR,buf1,5);

        /*�Ƚ�*/
        for(i=0;i<5;i++)
        {
                if(buf1[i]!=0xC2)
                break;
        }

        if(i==5)
        return SUCCESS ; //MCU �� NRF �ɹ�����
        else
        return ERROR ; //MCU �� NRF ����������
        }



//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
        u8 status;      
        NRF24L01_CSN=0;                 //ʹ��SPI����
        status =SPI_SentByte(reg);//���ͼĴ����� 
        SPI_SentByte(value);      //д��Ĵ�����ֵ
        NRF24L01_CSN=1;                 //��ֹSPI����       
        return(status);          //����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 NRF24L01_Read_Reg(u8 reg)
{
        u8 reg_val;                     
        NRF24L01_CSN = 0;          //ʹ��SPI����    
        SPI_SentByte(reg);   //���ͼĴ�����
        reg_val=SPI_SentByte(0XFF);//��ȡ�Ĵ�������
        NRF24L01_CSN = 1;          //��ֹSPI����    
        return(reg_val);           //����״ֵ̬
}       
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
        u8 status,u8_ctr;       
        NRF24L01_CSN = 0;           //ʹ��SPI����
        status=SPI_SentByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬ 
        for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI_SentByte(0XFF);//��������
        NRF24L01_CSN=1;       //�ر�SPI����
        return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
        u8 status,u8_ctr;       
        NRF24L01_CSN = 0;          //ʹ��SPI����
        status = SPI_SentByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
        for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI_SentByte(*pBuf++); //д������ 
        NRF24L01_CSN = 1;       //�ر�SPI����
        return status;          //���ض�����״ֵ̬
}       
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
u8 NRF24L01_TxPacket(u8 *txbuf)
{
        u8 sta;
        SPI_SetSpeed(SPI_SPEED_8);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
        NRF24L01_CE=0;
        NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
        NRF24L01_CE=1;//��������    
        while(NRF24L01_IRQ!=0);//�ȴ��������
        sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ     
        NRF24L01_Write_Reg(WRITE+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
        if(sta&MAX_TX)//�ﵽ����ط�����
        {
                NRF24L01_Write_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
                return MAX_TX; 
        }
        if(sta&TX_OK)//�������
        {
                return TX_OK;
        }
        return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
        u8 sta; 
        SPI_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
        sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ     
        NRF24L01_Write_Reg(WRITE+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
        if(sta&RX_OK)//���յ�����
        {
                NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
                NRF24L01_Write_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
                return 0; 
        }       
        return 1;//û�յ��κ�����
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ���,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������
void RX_Mode(void)
{
        NRF24L01_CE=0;  
        NRF24L01_Write_Buf(WRITE+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
          
        NRF24L01_Write_Reg(WRITE+EN_AA,0x01);       //ʹ��ͨ��0���Զ�Ӧ��    
        NRF24L01_Write_Reg(WRITE+EN_RXADDR,0x01);   //ʹ��ͨ��0�Ľ��յ�ַ 
        NRF24L01_Write_Reg(WRITE+RF_CH,40);         //����RFͨ��Ƶ�� 
        NRF24L01_Write_Reg(WRITE+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 
        NRF24L01_Write_Reg(WRITE+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
        NRF24L01_Write_Reg(WRITE+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
        NRF24L01_CE = 1; //CEΪ��,�������ģʽ 
}       
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ���,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������       
//CEΪ�ߴ���10us,����������.     
void TX_Mode(void)
{       
        NRF24L01_CE=0;  40
        NRF24L01_Write_Buf(WRITE +TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
        NRF24L01_Write_Buf(WRITE +RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
                                 
        NRF24L01_Write_Reg(WRITE +EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
        NRF24L01_Write_Reg(WRITE +EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
        NRF24L01_Write_Reg(WRITE +SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
        NRF24L01_Write_Reg(WRITE +RF_CH,40);       //����RFͨ��Ϊ40
        NRF24L01_Write_Reg(WRITE +RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
        NRF24L01_Write_Reg(WRITE +CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
        NRF24L01_CE=1;//CEΪ��,10us����������
} 



