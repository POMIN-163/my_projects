/********************************** (C) COPYRIGHT *******************************
* File Name          : IIC.C
* Author             : GJ
* Version            : V2.00
* Date               : 2018-7-10
* Description        : Interface of I2C
*******************************************************************************/
#include <intrins.h>
#include "DEBUG.H"
#include "CONFIG.H"

/*延时子程序*/
/*延时指定微秒时间*/          				 	
#define		DELAY5uS()		mDelayuS(5)    		/* 适当增减延时来改变IIC速度 */
#define		DELAY2uS()		mDelayuS(2)			
#define		DELAY1uS()		mDelayuS(1)			
#define 	IIC_READ		(0X01)
#define 	IIC_WRITE		(0X00)
/*100K DELAY5 增加_nop_();到20个*/
/*100K DELAY2 增加_nop_();到10个*/


/*******************************以下为IIC器件操作常用子函数*********************************************************************/
void I2c_Init( void )
{
	P1_MOD_OC &= ~(BIT4 + BIT5);
	P1_DIR_PU |= (BIT4 + BIT5);
	P1 |= BIT4 + BIT5;
}

static void I2c_SDA_Out( void )
{
	 P1_MOD_OC &= ~BIT5;
	 P1_DIR_PU |= BIT5;
}

static void I2c_SDA_In( void )
{
	 P1_MOD_OC |= BIT5;
	 P1_DIR_PU |= BIT5;
}
/*******************************************************************************
* Function Name  : IIC_Start
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************************************************************/
void IIC_Start( void )
{
	IIC_SDA=1;
	I2c_SDA_Out(); 	  
	IIC_SCL=1;
	mDelayuS(5);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	DELAY2uS();
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

/*******************************************************************************
* Function Name  : IIC_Stop
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IIC_Stop( void )
{
	IIC_SDA = 0;
	I2c_SDA_Out();
	IIC_SCL=1;
	DELAY2uS();
	IIC_SDA=1;//发送I2C总线结束信号  
}

/*******************************************************************************
* Function Name  : Ack_I2c
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IIC_Ack( void )
{
	IIC_SCL=0;
	I2c_SDA_Out();
	IIC_SDA=0;
	DELAY1uS();
	IIC_SCL=1;
	DELAY2uS();	
	IIC_SCL=0;
	I2c_SDA_In();
	IIC_SDA=1;	
}

/*******************************************************************************
* Function Name  : Nack_I2c
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IIC_Nack(void)
{
	IIC_SCL=0;
	I2c_SDA_Out();
	IIC_SDA=1;
	DELAY1uS();
	IIC_SCL=1;
	DELAY2uS();
	IIC_SCL=0;
	I2c_SDA_In();
}			
/*******************************************************************************
* Function Name  : Ack_I2c
* Description    :等待应答信号到来
				  返回值：1，接收应答失败
					      0，接收应答成功
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 IIC_Wait_Ack(void)
{
	UINT8 ucErrTime=0;
	IIC_SDA=1;	   
	IIC_SCL=1;
	I2c_SDA_In();
	DELAY2uS();
	while(IIC_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
		mDelayuS(1);
	}	
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
/*******************************************************************************
* Function Name  : IIC_Send_Byte
* Description    :
* Input          : UINT8 c
* Output         : None
* Return         : None
*******************************************************************************/
void  IIC_Send_Byte(UINT8 txd)
{
    UINT8 t;   
    IIC_SCL=0;//拉低时钟开始数据传输
	I2c_SDA_Out();
	DELAY2uS();
	for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
		DELAY1uS();
		IIC_SCL=1; 
		DELAY1uS();
		if( t == 7 )
		{
			I2c_SDA_In();
		}
		IIC_SCL=0;	
		DELAY1uS();
    }
}

/*******************************************************************************
* Function Name  : IIC_Read_Byte
* Description    :
* Input          : None
* Output         : None
* Return         : UINT8
*******************************************************************************/
UINT8  IIC_Read_Byte(UINT8 ack)
{
	UINT8 i,receive=0;
	DELAY2uS();
	I2c_SDA_In();
	for(i=0;i<8;i++ )
	{ 
		IIC_SCL=0; 	    	   
		DELAY2uS();
		IIC_SCL=1;	 
		receive<<=1;
		if(IIC_SDA)
			receive++;   
	}
	if (!ack)
		IIC_Nack();//发送nACK
	else 
		IIC_Ack(); //发送ACK   
 	return receive;
}
