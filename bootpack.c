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
	buffer_init(&allbuf.key,32,keyb);
	buffer_init(&allbuf.mouse,1024,mouseb);
	buffer_init(&timeBuffer,8,timeb);
	
	//��ʱ����ʼ��
	initPit();
	//�趨��ʱ�� 
	struct Timer *timer1,*timer2;//timer1Ϊ�����˸��ʱ��,timer2Ϊ10�뵹��ʱ��ʱ�� 
	timer1=allocTimer();
	initTimer(timer1,&timeBuffer,10);
	setTimer(timer1,50);
	timer2=allocTimer();
	initTimer(timer2,&timeBuffer,20);
	setTimer(timer2,500);
	

	
	//���->�����Ƶ�·->CPU
	//�ȳ�ʼ�������Ƶ�·�������ڼ��̿��Ƶ�·�
	//�ټ������
	struct Mouse_Dec mdec;
	int mx=100,my=100;
	init_keyboard();//�ȳ�ʼ�����̿��Ƶ�·
	enable_mouse(&mdec);//�ټ������
	
	//��ʼ����Ļ 
	struct BootInfo *binfo=(struct BootInfo *) BOOTADDR;//��Ļ����ͼ�񻺳�����ʼλ��
	init_palette();//�趨��ɫ�� 
	putstr_srn(binfo,0,20,LIGHTRED,"Welcome To MyOS");
	putstr_srn(binfo,0,50,LIGHTRED,"Check the memory now...");

	
	//��ʼ���ڴ�
	struct MemoryList *meml=(struct MemoryList *)MEMORYLISTADDR;
	int size;
	mem_init(meml);
	//size=memtest(0x00400000,0xbfffffff);//�ڴ���� 
	mem_free(meml,0x00001000,0x0009e000);
	mem_free(meml,0x00400000,size-0x00400000);

	//��ʼ��ͼ���
	struct SheetControl *scl;
	scl=SCL_init(meml,binfo);

	//��ʼ������ͼ��
	struct Sheet *sht_back;
	unsigned char *buf_back;
	sht_back=sheet_alloc(scl);//����ͼ�� 
	buf_back=(unsigned char *)mem_alloc_4k(meml,binfo->scrnx*binfo->scrny);//�����ڴ�ռ� 
	sheet_setbuf(sht_back,buf_back,binfo->scrnx,binfo->scrny,-1);//��ʼ��ͼ�㣬������͸��ɫ 
	init_screen_sht(sht_back);//��ͼ��
	sheet_slide(scl,sht_back,0,0);//�ƶ�ͼ��λ��
	sheet_updown(scl,sht_back,0);//Ų��ͼ��߶ȣ����ײ� 

	//��ʾ�ڴ��С
	char str[128];
	sprintf (str,"Memory: %dM",size/1024/1024);
	putstr_back_sht(scl,sht_back,0,0,LIGHTRED,LIGHTGRAY,str,13);
	
	//��ʾ����
	struct Sheet *sht_window;
	unsigned char *buf_window;
	sht_window=sheet_alloc(scl);
	buf_window=(unsigned char *)mem_alloc_4k(meml,160*68);//�����ڴ�ռ� 
	sheet_setbuf(sht_window,buf_window,160,68,-1);
	make_window(sht_window,160,68,"window");
	putstr_sht(sht_window,16,28,BLACK,"Welcome to My OS");
	sheet_slide(scl,sht_window,80,72);
	sheet_updown(scl,sht_window,1);
	
	//��ʼ�����ͼ��
	struct Sheet *sht_mouse;
	char mousebuf[256];
	sht_mouse=sheet_alloc(scl);
	sheet_setbuf(sht_mouse,mousebuf,16,16,99);
	init_mouse_cursor(sht_mouse);
	sheet_slide(scl, sht_mouse, mx, my);
	sheet_updown(scl,sht_mouse,2);
	sheet_refreshAll(scl);//����ͼ��׼�� 
	
	int count=0,second=0,p=0;
	int pst=0;//���״̬ 
	
	//�����ѭ�� 
	while(1)
	{
		count++;
		//һ����ʾʱ�� 
		if (timerctl.count>second*100+99)
		{
			second=timerctl.count/100; 
			sprintf (str,"%03d",second);
			putstr_back_sht(scl,sht_window,16,28,BLACK,LIGHTGRAY,str,16);
		} 
		//�������ж� 
		io_cli();
		if (buffer_get(&(allbuf.key),&data))
		{
			//���� 
			io_sti();
			sprintf (str,"Key:%x",data);
			putstr_back_sht(scl,sht_back,0,1*16,LIGHTRED,LIGHTBLUE,str,6);//��ʾ������Ϣ 
		}else if (buffer_get(&allbuf.mouse,&data))
		{
			//��� 
			io_sti();
			if (mouse_decode(&mdec,data))
			{
				sprintf (str,"Mouse:");
				if (mdec.lbtn)
					sprintf (str,"%sL",str);
				if (mdec.mbtn)
					sprintf (str,"%sM",str);				
				if (mdec.rbtn)
					sprintf (str,"%sR",str);
				
				mx+=mdec.dx;
				if (mx<0)
					mx=0;
				if (mx>binfo->scrnx-1)
					mx=binfo->scrnx-1;
				my+=mdec.dy;
				if (my<0)
					my=0;
				if (my>binfo->scrny-1)
					my=binfo->scrny-1;
				
				sprintf (str,"%s (%d,%d,%d,%d)",str,mx,my,mdec.dx,mdec.dy);
				putstr_back_sht(scl,sht_back,0,2*16,LIGHTRED,LIGHTBLUE,str,25);//��ʾ������Ϣ 
				sheet_slide(scl,sht_mouse,mx,my);
			}
		}else if (buffer_get(&timeBuffer,&data))
		{
			//��ʱ�� 
			io_sti();
			switch (data)//Ϊ�����ֲ�ͬ�Ķ�ʱ�� 
			{
			case 10://timer1���ع�� 
				initTimer(timer1,&timeBuffer,11);
				boxfill_sht(sht_back,0,3*16,8,15,LIGHTGRAY);
				sheet_refreshSheetSub(scl,sht_back,0,3*16,8,15);
				setTimer(timer1,50);
				break;
			case 11://timer1��ʾ��� 
				initTimer(timer1,&timeBuffer,10);
				boxfill_sht(sht_back,0,3*16,8,15,LIGHTBLUE);
				sheet_refreshSheetSub(scl,sht_back,0,3*16,8,15);
				setTimer(timer1,50);
				break; 
			case 20://timer2
				sprintf (str,"5S:%010d",count);
				putstr_back_sht(scl,sht_window,16,44,BLACK,LIGHTGRAY,str,16);
				break;
			}
		}
		else io_sti();
	}
}

