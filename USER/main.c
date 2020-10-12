#include "stm32f4xx.h"
#include "sys.h"  
u8 Flag_Left,Flag_Right,Flag_Direction=0;   //����ң����صı���
u8 Flag_Stop=1,Flag_Show=1;                 //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ�ر�
int Encoder_A,Encoder_B,Encoder_C;          //���������������
long int Position_A,Position_B,Position_C,Rate_A,Rate_B,Rate_C; //PID������ر���                   
long int Motor_A,Motor_B,Motor_C;        //���PWM����
long int Target_A,Target_B,Target_C;     //���Ŀ��ֵ
int Voltage;                             //��ص�ѹ������صı���
float Show_Data_Mb;                      //ȫ����ʾ������������ʾ��Ҫ�鿴������                         
u8 delay_50,delay_flag;                          //��ʱ��ر���
u8 Run_Flag=0;  //����ң����ر���������״̬��־λ
u8 rxbuf[8],Urxbuf[8],CAN_ON_Flag=0,PS2_ON_Flag=0,Usart_ON_Flag=0,Usart_Flag,PID_Send,Flash_Send;  //CAN�ʹ��ڿ�����ر���
u8 Usart_Invalid_Count=0; //����ʧЧ����������һ��ʱ��δ�յ�����������ʧ�ܴ��ڿ���
u8 txbuf[8],txbuf2[8],Turn_Flag;             //CAN������ر���
float Pitch,Roll,Yaw,Move_X,Move_Y,Move_Z;   //����ǶȺ�XYZ��Ŀ���ٶ�
u32 PID_Parameter[10],Flash_Parameter[10];  //Flash�������
float	Position_KP=40,Position_KI=0,Position_KD=40;  //λ�ÿ���PID����
float Velocity_KP=10,Velocity_KI=2;	          //�ٶȿ���PID����
int RC_Velocity=45,RC_Position=3000;         //����ң�ص��ٶȺ�λ��ֵ
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY;     //
int Gryo_Z; //Z����ٶ�
int main(void)
{
	delay_init(168);                //=====��Ƶ168M
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//=====����ϵͳ�ж����ȼ�����2
	LED_Init();                     //=====LED��ʼ��
	KEY_Init();                     //=====������ʼ��
	if(MODE==0)Run_Flag=1;          //=====�����Ĺ����У�����ģʽѡ�񿪹�ȷ������λ��ģʽ�����ٶ�ģʽ
	else Run_Flag=0;                //=====�����Ĺ����У�����ģʽѡ�񿪹�ȷ������λ��ģʽ�����ٶ�ģʽ
  OLED_Init();                    //=====OLED��ʼ��
  uart_init(115200);              //=====��ʱ��ʼ��
	uart2_init(9600);               //=====����2��ʼ��
	uart3_init(115200);             //=====����3��ʼ��
	Encoder_Init_TIM2();            //=====��������ʼ��
	Encoder_Init_TIM3();            //=====��������ʼ��
	Encoder_Init_TIM4();            //=====��������ʼ��
	Adc_Init();                     //=====ģ�����ɼ���ʼ��
	delay_ms(500);                  //=====��ʱ�ȴ�ϵͳ�ȶ�
	IIC_Init();                     //=====IIC��ʼ��
  MPU6050_initialize();           //=====MPU6050��ʼ��	
  DMP_Init();                     //=====��ʼ��DMP 
	PS2_Init();									    //=====ps2�����˿ڳ�ʼ��
	PS2_SetInit();		 					    //=====ps2���ó�ʼ��,���á����̵�ģʽ������ѡ���Ƿ�����޸�
	MiniBalance_EXTI_Init();       				  //=====�ⲿ�жϳ�ʼ��
	CAN1_Mode_Init(1,7,6,3,CAN_Mode_Normal);//=====CAN��ʼ��  
	MiniBalance_PWM_Init(8400,1);   				//=====PWM��ʼ��
	delay_ms(500);
	while(1)
	{
		if(Flag_Show==0)           //ʹ��MiniBalance APP��OLED��ʾ��
		{
			APP_Show();	              
			oled_show();             //===��ʾ����
		}
//	CAN1_SEND();                //CAN����	      //�ݲ�ʹ��CAN����
//	PS2_Receive();             //PS2����        //��δʹ��PS2
		USART_TX();                //���ڷ���
		if(Usart_ON_Flag && Usart_Invalid_Count++ > 4)
		{
			memset(Urxbuf,0,8);
			Usart_ON_Flag=0;
		}
		delay_flag=1;	
		delay_50=0;
		while(delay_flag);	       //ͨ��MPU6050��INT�ж�ʵ�ֵ�50ms��׼��ʱ				
	}
}
