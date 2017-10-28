#include"mtask.h"
#include"buffer.h"
#include"int.h"
#include"nasmfunc.h"
#include"dsctbl.h"  
#include"keyboard.h"
#include"mouse.h"
#include"memory.h"
#include"sheet.h"
#include"graphic.h"
#include"bootpack.h"
#include"sheet.h"
#include"timer.h" 
#include<stdio.h>



void HariMain()
{	
	//�жϳ�ʼ�� 
	init_gdtidt();
	init_pic();
	io_sti();
	io_out8(PIC0_IMR, 0xf8);//����PIC�ж�������ж�
	io_out8(PIC1_IMR, 0xef);//��������ж�
	
	//��ʼ����ꡢ���̡���ʱ�������� 
	unsigned char data;
	unsigned char keyb[32],mouseb[1024],timeb[8];
	struct Buffer timeBuffer;
	buffer_init(&allbuf.key,32,keyb,0);
	buffer_init(&allbuf.mouse,1024,mouseb,0);
	buffer_init(&timeBuffer,8,timeb,0);
	
	//��ʱ����ʼ��
	initPit();
	//�趨��ʱ�� 
	struct Timer *timer1;//timer1Ϊ�����˸��ʱ��,timer2Ϊ����ʱ��ʱ�� 
	timer1=allocTimer();
	initTimer(timer1,&timeBuffer,10);
	setTimer(timer1,50);
	
	//��ʼ����Ļ 
	struct BootInfo *binfo=(struct BootInfo *) BOOTADDR;//��Ļ����ͼ�񻺳�����ʼλ��
	init_palette();//�趨��ɫ�� 
	putstr_srn(binfo,0,20,LIGHTRED,"Welcome To my OS");
	putstr_srn(binfo,0,50,LIGHTRED,"Check the memory now...");
	
	//���->�����Ƶ�·->CPU
	//�ȳ�ʼ�������Ƶ�·�������ڼ��̿��Ƶ�·�
	//�ټ������
	struct Mouse_Dec mdec;
	mdec.x=100;
	mdec.y=100;
	init_keyboard();//�ȳ�ʼ�����̿��Ƶ�·
	enable_mouse(&mdec);//�ټ������
	


	
	//��ʼ���ڴ�
	//struct MemoryList *
	meml=(struct MemoryList *)MEMORYLISTADDR;
	int size;
	mem_init();
	//size=memtest(0x00400000,0xbfffffff);//�ڴ���� 
	mem_free(0x00001000,0x0009e000);
	mem_free(0x00400000,size-0x00400000);
	
	//��ʼ����������
	static char keyTable[0x80] =
	{
 		 0 , 0 ,'1','2','3','4','5','6','7','8','9','0','-','^', 0 , 0 ,
		'Q','W','E','R','T','Y','U','I','O','P','[',']', 0 , 0 ,'A','S',
		'D','F','G','H','J','K','L',';','\'','`', 0 ,'\\','Z','X','C','V',
		'B','N','M',',','.','/', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
 		 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'7','8','9','-','4','5','6','+','1',
		'2','3','0','.', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 		 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
 		 0 , 0 ,'_', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'\\', 0 , 0 
	};
	char curInput[128];
	int curPos = 0;

	//��ʼ��ͼ���
	scl=SCL_init(binfo);

	//��ʼ������ͼ��
	struct Sheet *sht_back;
	unsigned char *buf_back;
	sht_back=sheet_alloc();//����ͼ�� 
	buf_back=(unsigned char *)mem_alloc_4k(binfo->scrnx*binfo->scrny);//�����ڴ�ռ� 
	sheet_setbuf(sht_back,buf_back,binfo->scrnx,binfo->scrny,-1);//��ʼ��ͼ�㣬������͸��ɫ 
	init_screen_sht(sht_back);//��ͼ��
	sheet_slide(sht_back,0,0);//�ƶ�ͼ��λ��
	sheet_updown(sht_back,0);//Ų��ͼ��߶ȣ����ײ� 

	//��ʾ�ڴ��С
	char str[128];
	sprintf (str,"Memory: %dM",size/1024/1024);
	putstr_back_sht(sht_back,0,0,LIGHTRED,LIGHTGRAY,str,13);
	
	//��ʾ����
	struct Sheet *sht_window;
	unsigned char *buf_window;
	sht_window=sheet_alloc();
	buf_window=(unsigned char *)mem_alloc_4k(160*68);//�����ڴ�ռ� 
	sheet_setbuf(sht_window,buf_window,160,68,-1);
	make_window(sht_window,160,68,"window");
	putstr_sht(sht_window,16,28,BLACK,"Welcome to my OS");
	sheet_slide(sht_window,80,72);
	sheet_updown(sht_window,1);
	
	//��ʼ�����ͼ��
	struct Sheet *sht_mouse;
	char mousebuf[256];
	sht_mouse=sheet_alloc();
	sheet_setbuf(sht_mouse,mousebuf,16,16,99);
	init_mouse_cursor(sht_mouse);
	sheet_slide( sht_mouse, mdec.x, mdec.y);
	sheet_updown(sht_mouse,2);
	sheet_refreshAll();//����ͼ��׼�� 
	
	//��ʼ��������
	initTask();
	
	//�趨������
	struct Task *mainTask;
	mainTask=getMainTask();
	//�������󻺳��������� 
	allbuf.key.task=mainTask;
	allbuf.mouse.task=mainTask;
	//��������B
	struct Task *taskb;
	taskb=allocTask();
	taskb->tss.esp=mem_alloc_4k(64*1024)+64*1024-12;//ջ��ָ��Ϊ�ڴ��ĩβ��Ϊ���õ�ַ��������Χ��sht_backΪesp+8��esp+12 
	taskb->tss.eip=(int) &task_b_main;
	taskb->tss.es=1*8;
	taskb->tss.cs=2*8;
	taskb->tss.ss=1*8;
	taskb->tss.ds=1*8;
	taskb->tss.fs=1*8;
	taskb->tss.gs=1*8;
	*((int *)(taskb->tss.esp+4)) = (int)scl;//ѹ��ջ�����������Ĳ��� 
	*((int *)(taskb->tss.esp+8)) = (int)sht_back;
	runTask(taskb);
	
	
	int count=0;
	int flag;
	int pst=0;//���״̬ 
	
	//�����ѭ�� 
	while(1)
	{
		count++;
		flag=0;
		//�������ж� 
		io_cli();
		if (buffer_get(&(allbuf.key),&data))
		{
			//���� 
			io_sti();
			flag=1;
			sprintf (str,"Key:%x",data);
			putstr_back_sht(sht_back,0,1*16,LIGHTRED,LIGHTBLUE,str,6);//��ʾ������Ϣ 
			
			if (data<0x80 && keyTable[data]>0)
			{
				curInput[curPos++] = keyTable[data];
				curInput[curPos] = '\0';
				sprintf (str,"%s",curInput);
				putstr_back_sht(sht_window,16,44,BLACK,WHITE,str,16);
			}else if (data==0x0e)
			{
				if (curPos>0)
					curInput[--curPos] = '\0';
				sprintf (str,"%s",curInput);
				putstr_back_sht(sht_window,16,44,BLACK,WHITE,str,16);
			}
			
		}
		if (buffer_get(&allbuf.mouse,&data))
		{
			//��� 
			io_sti();
			flag=2;
			if (mouse_decode(&mdec,data))
			{
				sprintf (str,"Mouse:");
				if (mdec.lbtn)
					sprintf (str,"%sL",str);
				if (mdec.mbtn)  
					sprintf (str,"%sM",str);				
				if (mdec.rbtn)
					sprintf (str,"%sR",str);
				
				mdec.x+=mdec.dx;
				if (mdec.x<0)
					mdec.x=0;
				if (mdec.x>binfo->scrnx-1)
					mdec.x=binfo->scrnx-1;
				mdec.y+=mdec.dy;
				if (mdec.y<0)
					mdec.y=0;
				if (mdec.y>binfo->scrny-1)
					mdec.y=binfo->scrny-1;
				
				sprintf (str,"%s (%d,%d,%d,%d)",str,mdec.x,mdec.y,mdec.dx,mdec.dy);
				putstr_back_sht(sht_back,0,2*16,LIGHTRED,LIGHTBLUE,str,25);//��ʾ������Ϣ 
				sheet_slide(sht_mouse,mdec.x,mdec.y);
				
			}
		}
		if (buffer_get(&timeBuffer,&data))
		{
			//��ʱ�� 
			io_sti();
			flag=3;
			switch (data)//Ϊ�����ֲ�ͬ�Ķ�ʱ�� 
			{
			case 10://timer1��ʾ��� 
				initTimer(timer1,&timeBuffer,11);
				boxfill_sht(sht_window,16+8*curPos,44,8,15,BLACK);
				sheet_refreshSheetSub(sht_window,16+8*curPos,44,8,15);   
				setTimer(timer1,50);
				break;
			case 11://timer1���ع�� 
				initTimer(timer1,&timeBuffer,10);
				boxfill_sht(sht_window,16+8*curPos,44,8,15,WHITE);
				sheet_refreshSheetSub(sht_window,16+8*curPos,44,8,15);
				setTimer(timer1,50);
				break; 
			/*case 20://5����л�������B�����ߣ� 
				farjmp(0,4*8);
				setTimer(timer2,2);*/ 
			}
		}
		if (flag==0)
		{
			//û���¼������� 
			sleepTask(mainTask);
			io_sti();
		}
	}
}


//����B����Ҫ����������л� 
void task_b_main(struct SheetControl *scl,struct Sheet *sht_back)
{
	
	int count=1000000001,num=0;
	char str[128];
	while (1)
	{
		if (count>1000000000)
		{
			count=0;
			num++;
			sprintf (str,"Task B:%x(G)",num);
			putstr_back_sht(sht_back,0,3*16,LIGHTRED,LIGHTBLUE,str,30);//��ʾ����B
		}
		count++;
	} 
}
