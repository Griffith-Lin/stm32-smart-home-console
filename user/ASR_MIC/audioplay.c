#include "audioplay.h"



__audiodev audiodev;		//音乐播放控制器  
/*********************************************************************************************************
* 函 数 名 : Audio_Start
* 功能说明 : 开始播放音频
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 实际上就是启动dma传输
*********************************************************************************************************/ 
void Audio_Start(void)
{
	audiodev.status=3<<0;	//开始播放+非暂停
	I2S_Play_Start();
} 
/*********************************************************************************************************
* 函 数 名 : Audio_Stop
* 功能说明 : 关闭音频播放
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 实际上就是关闭dma传输
*********************************************************************************************************/ 
void Audio_Stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
/*********************************************************************************************************
* 函 数 名 : Audio_GetTnum
* 功能说明 : 得到指定路径下的文件数量
* 形    参 : path：路径名
* 返 回 值 : 总有效文件数量
* 备    注 : 无
*********************************************************************************************************/ 
unsigned short Audio_GetTnum(unsigned char *path)
{	  
	unsigned char 	res  = 0;
	unsigned short	rval = 0;
	unsigned char *fn   = NULL; 			 
 	DIR tdir;	 				//临时目录
	FILINFO tfileinfo;			//临时文件信息		
				   			     
    res = f_opendir(&tdir, (const TCHAR *)path); 				//打开目录
  	tfileinfo.fsize =FF_MAX_LFN*2+1;							//长文件名最大长度
	//tfileinfo.fname = mymalloc(SRAMIN, tfileinfo.fsize);		//为长文件缓存区分配内存
	if(res==FR_OK)
	{
		while(1)	//查询总的有效文件数
		{
	        res = f_readdir(&tdir, &tfileinfo);       				//读取目录下的一个文件
	        if(res!=FR_OK || tfileinfo.fname[0]==0) break;			//错误了/到末尾了,退出		  
     		fn = (unsigned char *)(*tfileinfo.fname?tfileinfo.fname:tfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)	//取高四位,看看是不是音乐文件	
			{
				rval++;				//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN, tfileinfo.fname);
	return rval;
}
/*********************************************************************************************************
* 函 数 名 : Audio_IndexShow
* 功能说明 : 显示曲目当前索引
* 形    参 : fn: 歌名，index：当前索引，total：总文件数量
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_IndexShow(unsigned char *fn, unsigned short index, unsigned short total)
{
	u8 buff[5] = {0};
	sprintf((char*)buff,"%d",index);
	//显示当前曲目的索引,及总曲目数
//	st7789vm_DisplayString(100,70,BLACK,WHITE,buff,24);
//	st7789vm_DisplayEng(100,100,BLACK,WHITE,'/',24);
//	sprintf((char*)buff,"%d",total);
//	st7789vm_DisplayString(100,130,BLACK,WHITE,buff,24);	 
//	printf("%s\r\n",buff);
//	LCD_Font_Dis_Str(100,60,24,RED,WHITE,buff);
}
/*********************************************************************************************************
* 函 数 名 : Audio_MsgShow
* 功能说明 : 显示播放时间
* 形    参 : totsec：音频文件总时间长度，cursec：当前播放时间
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_MsgShow(unsigned int totsec, unsigned int cursec)
{	
	static u16 playtime=0XFFFF;			//播放时间标记	  
	
	if(playtime != cursec)			//需要更新显示时间
	{
		playtime=cursec;
//		lv_bar_set_value(guider_ui.screen_barMusic, (double)cursec/(double)totsec*100.0, LV_ANIM_ON);
//		sprintf((char *)Usart1.RxBuff, "%02d:%02d / %02d:%02d", playtime/60,playtime%60, totsec/60, totsec%60);
//		//LCD_ShowFlashString(0, 180, Usart1.RxBuff, LCD_RED, LCD_WHITE);
	} 		
}
/*********************************************************************************************************
* 函 数 名 : Audio_MusicPlay
* 功能说明 : 历遍指定路径，并按顺序播放改路径下的wav音频文件
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Audio_MusicPlay(void)
{
 	DIR wavdir;	 							//目录
	FILINFO wavfileinfo;					//文件信息
	unsigned char  *fn 	= NULL;   		//长文件名
	unsigned char  *pname 	= NULL;			//带路径的文件名
	unsigned short *wavindextbl = NULL;	//音乐索引表
	unsigned char  key 	= 0;			//键值
	unsigned char  res 	= 0;
	unsigned short totwavnum 	= 0; 		//音乐文件总数
	unsigned short curindex 	= 0;		//图片当前索引
 	unsigned short temp 		= 0;
    
    char music_name[200]={0};
	
 	while(f_opendir(&wavdir, "0:/MUSIC"))	//打开音乐文件夹
 	{	    
		printf("MUSIC文件夹错误!\r\n");
		Delay_Ms(200);				  			  
	} 									  
	totwavnum = Audio_GetTnum("0:/MUSIC");  //得到总有效文件数
  	while(totwavnum == NULL)				//音乐文件总数为0		
 	{	    
		printf("没有音乐文件!\r\n");
		Delay_Ms(200);			  
	}										   
	wavfileinfo.fsize = FF_MAX_LFN * 2 + 1;									//长文件名最大长度
	//wavfileinfo.fname = mymalloc(SRAMIN, wavfileinfo.fsize);				//为长文件缓存区分配内存
 	pname = mymalloc(SRAMIN, wavfileinfo.fsize);							//为带路径的文件名分配内存
 	wavindextbl = mymalloc(SRAMIN, 2*totwavnum);							//申请2*totwavnum个字节的内存,用于存放音乐文件索引
 	while(pname==NULL || wavindextbl==NULL)									//内存分配出错
 	{	    
		printf("内存分配失败!\r\n");
		Delay_Ms(200);				  			  
	}  	 

    res = f_opendir(&wavdir, "0:/MUSIC"); //打开目录
	if(res == FR_OK)
	{
		curindex = 0;	//当前索引为0
		while(1)		//全部查询一遍
		{
			temp = wavdir.dptr;								//记录当前index
	        res = f_readdir(&wavdir, &wavfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK || wavfileinfo.fname[0]==0)	break;	//错误了/到末尾了,退出		  
     		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
			res = f_typetell(fn);	
			if((res&0XF0) == 0X40)				//取高四位,看看是不是音乐文件	
			{
				wavindextbl[curindex] = temp;	//记录索引
				curindex++;
			}	    
		} 
	}   
	curindex = 0;											//从0开始显示
	res = f_opendir(&wavdir, (const TCHAR *)"0:/MUSIC"); 	//打开目录
	while(res == FR_OK)	//打开成功
	{	
		dir_sdi(&wavdir, wavindextbl[curindex]);			//改变当前目录索引	   
		res = f_readdir(&wavdir,&wavfileinfo);       		//读取目录下的一个文件
		if(res!=FR_OK || wavfileinfo.fname[0]==0) break;	//错误了/到末尾了,退出
		fn = (unsigned char *)(*wavfileinfo.fname?wavfileinfo.fname:wavfileinfo.fname);			 
		strcpy((char*)pname, "0:/MUSIC/");					//复制路径(目录)
		strcat((char*)pname, (const char*)fn);  			//将文件名接在后面
		printf("%s\r\n", fn);								//显示歌曲名字 
        sprintf(music_name,"%s",fn);       
        LCD_Font_Dis(30,0,16,RED,WHITE,(uint8_t*)music_name);
		Audio_IndexShow(fn, curindex+1, totwavnum);	//显示歌曲名称、总曲数、当前曲数
		key = Audio_PlaySong(pname); 			 			//播放这个音频文件
		if(key == 1)				//上一曲
		{
			if(curindex)	curindex--;
			else 			curindex = totwavnum-1;
 		}else if(key==0 || key==2)	//下一曲
		{
			curindex++;		   	
			if(curindex >= totwavnum)	curindex=0;	//到末尾的时候,自动从头开始
				
 		}else		
		{	
			break;	//产生了错误 	 
		}
	} 		

	myfree(SRAMIN, wavfileinfo.fname);		//释放内存			    
	myfree(SRAMIN, pname);					//释放内存			    
	myfree(SRAMIN, wavindextbl);			//释放内存	 
} 
/*********************************************************************************************************
* 函 数 名 : Audio_PlaySong
* 功能说明 : 播放某个音频文件
* 形    参 : fname：文件路径+文件名
* 返 回 值 : 0：下一首（自然播放），1：上一首，2：下一首，其他：停止播放或者发生了错误
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Audio_PlaySong(unsigned char *fname)
{
	unsigned char  res = f_typetell(fname);  

	switch(res)
	{
		case T_WAV:
			res = Wav_PlaySong(fname);
			break;
		default:	//其他文件,自动跳转到下一曲
			printf("can't play:%s\r\n",fname);
			res = 0;
			break;
	}
	return res;
}



