#include "lcd.h"
#include "delay.h"

u8 FontBuf[128];//字库缓存	
/******************************************************************************
      函数说明：向字库写入命令
      入口数据：dat  要写入的命令
      返回值：  无
******************************************************************************/
void ZK_command(u8 dat)
{
	u8 i; 
	for(i=0;i<8;i++ )
	{
		OLED_SCLK_Clr();  //字库时钟拉低 
		if(dat&0x80)
			OLED_SDIN_Set();
		else 
			OLED_SDIN_Clr();
		dat = dat<<1;
		OLED_SCLK_Set();  //字库时钟拉高
	}
 }

/******************************************************************************
      函数说明：从字库读取数据
      入口数据：无
      返回值：  ret_data 读取的数据
******************************************************************************/
u8  get_data_from_ROM(void)
{
	u8 i;
	u8 ret_data=0; //返回数据初始化
	OLED_SCLK_Set();//字库时钟拉高
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();  //字库时钟拉低  
		ret_data=ret_data<<1;
		if(ZK_OUT())
			{
				ret_data=ret_data+1;
			}
		else
			{
				ret_data=ret_data;
			}
		OLED_SCLK_Set(); //字库时钟拉高
	}
	return(ret_data);    //返回读出的一个字节
}
 
/******************************************************************************
      函数说明：读取N个数据
      入口数据：AddrHigh  写地址高字节
                AddrMid   写地址中字节
                AddrLow   写地址低字节
                *pBuff    读取的数据
                DataLen   读取数据的长度
      返回值：  无
******************************************************************************/
void get_n_bytes_data_from_ROM(u8 AddrHigh,u8 AddrMid,u8 AddrLow,u8 *pBuff,u8 DataLen)
{
 	u8 i;
	OLED_CS_Set();
	ZK_CS_Clr(); //字库片选
	OLED_SCLK_Clr();  //字库时钟拉低

	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);//写地址高字节
	ZK_command(AddrMid);//写地址中字节
	ZK_command(AddrLow);//写地址低字节
	for(i = 0; i < DataLen; i++ )
	{
	   *(pBuff+i) =get_data_from_ROM();//读一个字节数据
	}

	ZK_CS_Set();//取消字库片选
}

/******************************************************************************
      函数说明：显示汉字
      入口数据：x,y      写入的坐标
                zk_num   1:12*12,  2:15*16,  3:24*24,  4:32*32
                color    字体颜色
      返回值：  无
******************************************************************************/
void Display_GB2312(u16 x,u16 y,u8 zk_num,u16 color)
{
  u8 i,k;
	switch(zk_num)
	{
		case 1:
			 {
		 	   LCD_Address_Set(x,y,x+15,y+11);
			   for(i=0;i<24;i++)		 
	       {
					for(k=0;k<8;k++)
					{ 		     
						if((FontBuf[i]&(0x80>>k))!=0)
						{
							LCD_WR_DATA(color);
						} 
						else
						{
							LCD_WR_DATA(BACK_COLOR);
						}   
					}
	
	       }
			 }break;  // 12*12  

		case 2: 
			 {
		 	    LCD_Address_Set(x,y,x+15,y+15);
			    for(i=0;i<32;i++)		 
	       {
						for(k=0;k<8;k++)
						 { 		     
							if((FontBuf[i]&(0x80>>k))!=0)
							 {
								LCD_WR_DATA(color);
							 } 
							else
							 {
								LCD_WR_DATA(BACK_COLOR);
							 }   
						 }
	       }
		  }break;     // 15*16 

	    case 3:  
			 {
		 	    LCD_Address_Set(x,y,x+23,y+23);
			    for(i=0;i<72;i++)		 
						{
							for(k=0;k<8;k++)
							{ 		     
								if((FontBuf[i]&(0x80>>k))!=0)
							  {
									LCD_WR_DATA(color);
								} 
								else
								{
									LCD_WR_DATA(BACK_COLOR);
								}   
							}
						
						 }
		  }break;     // 24*24  

	    case 4:  
			 {
		 	    LCD_Address_Set(x,y,x+31,y+31);
			    for(i=0;i<128;i++)		 
	       {
						for(k=0;k<8;k++)
						{ 		     
							if((FontBuf[i]&(0x80>>k))!=0)
						  {
								LCD_WR_DATA(color);
							} 
							else
							{
								LCD_WR_DATA(BACK_COLOR);
							}   
						}
	       }
		  }break;    // 32*32  
	}
}

 
/******************************************************************************
      函数说明：显示汉字
      入口数据：zk_num    1:12*12,  2:15*16,  3:24*24,  4:32*32
                x,y       坐标
                text[]    要显示的汉字
                color     字体颜色
      返回值：  无
******************************************************************************/
void Display_GB2312_String(u16 x,u16 y,u8 zk_num,u8 text[],u16 color)
{
   
	u8 i= 0;
	u8 AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	
	u32 FontAddr=0; //字地址
	u32 BaseAdd=0; //字库基地址	
	u8 n,h,w,d,p;// 不同点阵字库的计算变量
	switch(zk_num)
		{  // n个数，h：字高度，w：字宽度， d：字间距，c：页大小
		  case 1 :  BaseAdd=0x00;    n=24;  h=12; w=12; d=12; p=2;  break;  // 12*12  
		  case 2 :  BaseAdd=0x2C9D0; n=32;  h=16; w=16; d=16; p=2;  break;   // 15*16  
	    case 3 :  BaseAdd=0x68190; n=72;  h=24; w=24; d=24; p=3;  break;   // 24*24  
	    case 4 :  BaseAdd=0xEDF00; n=128; h=32; w=32; d=32; p=4;  break;   // 32*32  
 		}

		while((text[i]>0x00))
		{
		    if(x>(LCD_W-d))
		      {  y=y+p; 
			     x=0; 
				 	 }//溢出换行
		
			if(((text[i]>=0xA1) &&(text[i]<=0xA9))&&(text[i+1]>=0xA1))
			{						
				//国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：//
				//Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址///
			 	FontAddr = (text[i]- 0xA1)*94; 
				FontAddr += (text[i+1]-0xA1);
				FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
				
				AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
				AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
				AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
				get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[]"
				Display_GB2312(x,y,zk_num,color);//显示一个汉字到OLED上/ 
	 		}
	
			else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))
		 	{
				//国标简体（GB2312） 字库IC中的地址由以下公式来计算：//
				//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址//
				FontAddr = (text[i]- 0xB0)*94; 
				FontAddr += (text[i+1]-0xA1)+846;
				FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
				
				AddrHigh = (FontAddr&0xff0000)>>16;  //地址的高8位,共24位//
				AddrMid = (FontAddr&0xff00)>>8;      //地址的中8位,共24位//
				AddrLow = FontAddr&0xff;	     //地址的低8位,共24位//
				get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );//取一个汉字的数据，存到"FontBuf[ ]"
				Display_GB2312(x,y,zk_num,color);//显示一个汉字到LCD上/
		 	}
			 x=x+d; //下一个字坐标
		 	 i+=2;  //下个字符
			}
} 
 
/******************************************************************************
      函数说明：显示ASCII码
      入口数据：x,y      写入的坐标
                zk_num   1:5*7   2:5*8   3:6*12,  4:8*16,  5:12*24,  6:16*32
                color    字体颜色
      返回值：  无
******************************************************************************/
void Display_Asc(u16 x,u16 y,u8 zk_num,u16 color)
{
    
    unsigned char i,k;
 
		
	switch(zk_num)
	{
	    case 1:   
			 {
		 	LCD_Address_Set(x,y,x+7,y+7);
			 for(i=0;i<7;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;    //	  5x7 ASCII

	 	case 2:   	
			 {
		 	LCD_Address_Set(x,y,x+7,y+7);
			 for(i=0;i<8;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;   //	  7x8 ASCII

	    case 3:   
			 {
		 	LCD_Address_Set(x,y,x+7,y+11);
			 for(i=0;i<12;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;  //  6x12 ASCII

	    case 4:  
	 {
		 	LCD_Address_Set(x,y,x+7,y+15);
			 for(i=0;i<16;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;     //  8x16 ASCII

	    case 5:  
	 {
		 	LCD_Address_Set(x,y,x+15,y+24);
			 for(i=0;i<48;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;    //  12x24 ASCII

	    case 6:  	 
	 {
		 	LCD_Address_Set(x,y,x+15,y+31);
			 for(i=0;i<64;i++)		 
	{
		for(k=0;k<8;k++)
		{ 		     
		 	if((FontBuf[i]&(0x80>>k))!=0)
		{
				LCD_WR_DATA(color);
			} 
			else
			{
				LCD_WR_DATA(BACK_COLOR);
			}   
		}
	
	 }
			 }break;   //  16x32 ASCII
			}
}

/******************************************************************************
      函数说明：显示ASCII码
      入口数据：x,y      写入的坐标
                zk_num   1:5*7   2:5*8   3:6*12,  4:8*16,  5:12*24,  6:16*32
                text[]   要显示的字符串
                color    字体颜色
      返回值：  无
******************************************************************************/
void Display_Asc_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 color)
{
	u8 i= 0;
	u8 AddrHigh,AddrMid,AddrLow ; //字高、中、低地址

	u32 FontAddr=0; //字地址
	u32 BaseAdd=0; //字库基地址	
  u8 n,h,w,d,c;// 不同点阵字库的计算变量
	switch(zk_num)
	{	// n个数，h：字高度，w：字宽度， d：字间距，c：页大小
		case 1:  BaseAdd=0x1DDF80; n=8;  h=7;  w=5;  d=6 ; c=1;  break;	 //	  5x7 ASCII
		case 2:  BaseAdd=0x1DE280; n=8;  h=8;  w=7;  d=8;  c=1;  break;	 //   7x8 ASCII
		case 3:  BaseAdd=0x1DBE00; n=12; h=12; w=6;  d=6;  c=2;  break;	 //  6x12 ASCII
	  case 4:  BaseAdd=0x1DD780; n=16; h=16; w=8;  d=8;  c=2;  break;	 //  8x16 ASCII	
	  case 5:  BaseAdd=0x1DFF00; n=48; h=24; w=12; d=12; c=3;  break;	 //  12x24 ASCII
	 	case 6:  BaseAdd=0x1E5A50; n=64; h=32; w=16; d=16; c=4;  break;	 //  16x32 ASCII

			}

	while((text[i]>0x00))
	{	
	   if(x>(LCD_W-d))   //溢出换行
	      {  y=y+c; 
		     x=0; 
			 	 }
	   if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
		    FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_Asc(x,y,zk_num,color);/*显示一个ascii到OLED上 */
  
		}
    i+=1;  //下个数据
	x+=d;//下一个字坐标 
	}	    
}



