#include "sys.h"
#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
		while((USART2->SR&0X40)==0);
		USART2->DR = (u8) ch;    	
  return ch;
}
#endif

int Usart_Receive;
/**************************ʵ�ֺ���**********************************************
*��    ��:		usart1����һ���ֽ�
*********************************************************************************/
void usart1_send(u8 data)
{
	USART1->DR = data;
	while((USART1->SR&0x40)==0);	
} 

void usart1_write(const u8* buf,u8 len)
{
	u8 i=0;
	for( ; i<len; ++i)
	{
		USART1->DR = buf[i];
		while((USART1->SR&0x40)==0);	
	}
}

//����1�жϷ������
//bound:������
void uart_init(u32 bound){
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	//Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}


/**************************************************************************
�������ܣ�����1�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
int USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //���յ�����
	{	       
		u8 temp;
		static u8 count,last_data,last_last_data,Usart_ON_Count;
		if(Usart_ON_Flag==0)
		{	
			if(++Usart_ON_Count>10)Usart_ON_Flag=1;
		}
		temp=USART1->DR;
		if(Usart_Flag==0)
		{	
			if(last_data==0xfe&&last_last_data==0xff) 
				Usart_Flag=1,count=0;	
		}
		if(Usart_Flag==1)
		{	
			Urxbuf[count]=temp;     
			count++;                
			if(count==8)
			{
				Usart_Flag=0;
				Usart_Invalid_Count=0;
			}
		}
		last_last_data=last_data;
		last_data=temp;
   }
	return 0;	
}
/**************************ʵ�ֺ���**********************************************
*��    ��:		usart2����һ���ֽ�
*********************************************************************************/
void usart2_send(u8 data)
{
	USART2->DR = data;
	while((USART2->SR&0x40)==0);	
}

//����2�жϷ������
//bound:������
void uart2_init(u32 bound){
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2 A3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2 A3

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������
	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/**************************************************************************
�������ܣ�����2�����ж�
��ڲ�������
����  ֵ����
**************************************************************************/
int USART2_IRQHandler(void)
{	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{	      
		static u8 Flag_PID,i,j,Receive[50];
		static float Data;
		Usart_Receive=USART2->DR;

		if(Usart_Receive==0x4B) Turn_Flag=1;  //����ת����ƽ���
		else if(Usart_Receive==0x49||Usart_Receive==0x4A) 	 Turn_Flag=0;	//������ƽ���
		
		if(Run_Flag==0)//�ٶȿ���ģʽ
		{			
			if(Turn_Flag==0)//�ٶȿ���ģʽ
			{
				if(Usart_Receive>=0x41&&Usart_Receive<=0x48)  
				{	
					Flag_Direction=Usart_Receive-0x40;
				}
				else	if(Usart_Receive<=8)   
				{			
					Flag_Direction=Usart_Receive;
				}	
				else  Flag_Direction=0;
			}
			else if(Turn_Flag==1)//���������ת����ƽ���
			{
				if(Usart_Receive==0x43) Flag_Left=0,Flag_Right=1;    
				else if(Usart_Receive==0x47) Flag_Left=1,Flag_Right=0;
				else Flag_Left=0,Flag_Right=0;
				if(Usart_Receive==0x41||Usart_Receive==0x45)Flag_Direction=Usart_Receive-0x40;
				else  Flag_Direction=0;
			}
		}	
		//��������APP���Խ���ͨѶ
		if(Usart_Receive==0x7B) Flag_PID=1;   //APP����ָ����ʼλ
		if(Usart_Receive==0x7D) Flag_PID=2;   //APP����ָ��ֹͣλ

		if(Flag_PID==1)  //�ɼ�����
		{
			Receive[i]=Usart_Receive;
			i++;
		}
		if(Flag_PID==2)  //��������
		{
			if(Receive[3]==0x50) 	 PID_Send=1;
			else  if(Receive[3]==0x57) 	 Flash_Send=1;
			else  if(Receive[1]!=0x23) 
			{								
				for(j=i;j>=4;j--)
				{
				  Data+=(Receive[j-1]-48)*pow(10,i-j);
				}
				switch(Receive[1])
				{
					case 0x30:  RC_Velocity=Data;break;
					case 0x31:  RC_Position=Data;break;
					case 0x32:  Position_KP=Data;break;
					case 0x33:  Position_KI=Data;break;
					case 0x34:  Position_KD=Data;break;
					case 0x35:  Velocity_KP=Data;break;
					case 0x36:  Velocity_KI=Data;break;
					case 0x37:  break; //Ԥ��
					case 0x38:  break; //Ԥ��
				}
			}				 
			Flag_PID=0;//��ر�־λ����
			i=0;
			j=0;
			Data=0;
			memset(Receive, 0, sizeof(u8)*50);//��������
		} 	 
	}
	return 0;	
}



/**************************************************************************
�������ܣ�����ɨ��
**************************************************************************/
u8 click_RC (void)
{
	static u8 flag_key=1;//�������ɿ���־
	u8 temp;
	if(flag_key&&Usart_Receive!=0x5A)
	{
		flag_key=0;
		if(Usart_Receive>=0x01&&Usart_Receive<=0x08)temp=Usart_Receive;
		else	if(Usart_Receive>=0x41&&Usart_Receive<=0x48)temp=Usart_Receive-0x40;	
		//	else 	temp=0;
		return temp;	// ��������
	}
	else if(Usart_Receive==0x5A)			flag_key=1;
	return 0;//�ް�������
}

u8 sum_check(const u8* buf, u8 len)
{
	u8 i=0, sum=0;
	for(; i<len; ++i)
		sum+=buf[i];
	return sum;
}
/**************************************************************************
�������ܣ�ͨ�����ڰ�����Ĵ��������ͳ�ȥ
**************************************************************************/

u8 imuBuf[13]= {0x55,0xAA,0x09,0x02};
void USART_TX_IMU(void)
{
	u16 Temp_GZ, Temp_yaw, Temp_accX, Temp_accY;
	Temp_accX = accel[0] + 32768;
	Temp_accY = accel[1] + 32768;
	Temp_GZ = gyro[2] + 32768;
	Temp_yaw = Yaw*100+ 32768;
	imuBuf[4] = Temp_accX;
	imuBuf[5] = Temp_accX >> 8;
	imuBuf[6] = Temp_accY;
	imuBuf[7] = Temp_accY >> 8;
	imuBuf[8] = Temp_GZ;
	imuBuf[9] = Temp_GZ >> 8;
	imuBuf[10] = Temp_yaw;
	imuBuf[11] = Temp_yaw >>8;
	imuBuf[12] = sum_check(imuBuf+2,10);
	usart3_write(imuBuf,13); //����3����imu����
//	usart1_write(imuBuf,13);
}

u8 encoderBuf[11] = {0x55,0xAA,0x07,0x01};
void USART_TX(void)
{
	encoderBuf[4] = Position_A;
	encoderBuf[5] =  Position_A>>8;
	encoderBuf[6] = Position_B;
	encoderBuf[7] =  Position_B>>8;
	encoderBuf[8] = Position_C;
	encoderBuf[9] =  Position_C>>8;
	encoderBuf[10] = sum_check(encoderBuf+2,8);
	usart3_write(encoderBuf,11); //����3���ͱ���������
//  usart1_write(encoderBuf,11);
	USART_TX_IMU();
	
/*
	u8 Direction_A,Direction_B,Direction_C;
	  u16 Temp_GZ;
		   if(Encoder_A>0) Direction_A=0;
	else if(Encoder_A<0) Direction_A=2;
	  else                 Direction_A=1;
			 if(Encoder_B>0) Direction_B=0;
	else if(Encoder_B<0) Direction_B=2;
	  else                 Direction_B=1;     
			 if(Encoder_C>0) Direction_C=0;
	else if(Encoder_C<0) Direction_C=2;
	  else                 Direction_C=1;
	Temp_GZ=Gryo_Z+32768;

	usart1_send(0xff);		
	usart1_send(0xfe);	
	usart1_send(abs(Encoder_A));		
	usart1_send(Direction_A);	
	usart1_send(abs(Encoder_B));		
	usart1_send(Direction_B);	
	usart1_send(abs(Encoder_C));		
	usart1_send(Direction_C);	
	usart1_send(Temp_GZ>>8);		
	usart1_send(Temp_GZ&0x00ff);	
	
	usart3_send(0xff);		
	usart3_send(0xfe);	
	usart3_send(abs(Encoder_A));		
	usart3_send(Direction_A);	
	usart3_send(abs(Encoder_B));		
	usart3_send(Direction_B);	
	usart3_send(abs(Encoder_C));		
	usart3_send(Direction_C);	
	usart3_send(Temp_GZ>>8);		
	usart3_send(Temp_GZ&0x00ff);	
*/
}


