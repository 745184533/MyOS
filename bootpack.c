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
#include<stdio.h>
void HariMain()
{	
	//�жϳ�ʼ�� 
	init_gdtidt();
	init_pic();
	io_sti();
	io_out8(PIC0_IMR, 0xf9);//����PIC�ж�������ж�
	io_out8(PIC1_IMR, 0xef);//��������ж�
	
	//��ʼ�������� 
	unsigned char data;
	unsigned char keyb[32],mouseb[128];
	struct BufferAll allbuf;
	buffer_init(&allbuf.key,32,keyb);
	buffer_init(&allbuf.mouse,128,mouseb);
	
	//���->�����Ƶ�·->CPU
	//�ȳ�ʼ�������Ƶ�·�������ڼ��̿��Ƶ�·�
	//�ټ������
	struct Mouse_Dec mdec;
	init_keyboard();//�ȳ�ʼ�����̿��Ƶ�·
	enable_mouse(&mdec);//�ټ������
	
	//��ʼ����Ļ 
	struct BootInfo *binfo=(struct BootInfo *) BOOTADDR;//��Ļ����ͼ�񻺳�����ʼλ��
	init_palette();//�趨��ɫ�� 
	putstr_srn(binfo,0,20,LIGHTRED,"Welcome To MyOS");
	putstr_srn(binfo,0,50,LIGHTRED,"Check the memory now...");

	
	//��ʼ���ڴ�
	struct MemoryList *meml=(struct MemoryList *)MEMORYLISTADDR;
	int size=memtest(0x00400000,0xbfffffff); 
	mem_init(meml);
	mem_free(meml,0x00001000,0x0009e000);
	mem_free(meml,0x00400000,size-0x00400000);
	
	//��ʼ��ͼ���
	struct SheetControl *scl;
	scl=SCL_init(meml,binfo);
	//��ʼ������ͼ��
	struct Sheet *sht_back;
	unsigned char *back_buf;
	sht_back=sheet_alloc(scl);//����ͼ�� 
	back_buf=(unsigned char *)mem_alloc_4k(meml,sizeof (binfo->scrnx*binfo->scrny));//�����ڴ�ռ� 
	sheet_setbuf(sht_back,back_buf,binfo->scrnx,binfo->scrny,-1);//��ʼ��ͼ�㣬������͸��ɫ 
	init_screen_sht(sht_back);//��ͼ��
	sheet_slide(scl,sht_back,0,0);//�ƶ�ͼ��λ��
	sheet_updown(scl,sht_back,0);//Ų��ͼ��߶ȣ����ײ� 
	while (1);
	/*//��ʼ����Ļ 
	binfo=(struct BootInfo *) BOOTADDR;//��Ļ����ͼ�񻺳�����ʼλ��
	meml=(struct MemoryList *)MEMORYLISTADDR;
	meml��binfoָ����ͬ������ 
	���ǽ�������ɡ���  
	if(meml==binfo && binfo==scl) 
		while (1);
	else return;
	
	//�жϳ�ʼ�� 
	init_gdtidt();
	init_pic();
	io_sti();
	io_out8(PIC0_IMR, 0xf9);//����PIC�ж�������ж�
	io_out8(PIC1_IMR, 0xef);//��������ж�
	
	//���->�����Ƶ�·->CPU
	//�ȳ�ʼ�������Ƶ�·�������ڼ��̿��Ƶ�·�
	//�ټ������
	struct Mouse_Dec mdec;
	init_keyboard();//�ȳ�ʼ�����̿��Ƶ�·
	enable_mouse(&mdec);//�ټ������
	//��ʼ�������� 
	unsigned char data;
	unsigned char keyb[32],mouseb[128];
	buffer_init(&allbuf.key,32,keyb);
	buffer_init(&allbuf.mouse,128,mouseb);

	//��ʼ���ڴ�
	int size=memtest(0x00400000,0xbfffffff); 
	mem_init();
	mem_free(0x00001000,0x0009e000);
	mem_free(0x00400000,size-0x00400000);
		
	/*char sss[128];
	sprintf (sss,"%d,%d,%d,%d,%d",meml->freesize,meml->free[0].size,meml->free[1].size,sizeof (struct SheetControl),mem_alloc_4k(sizeof(struct SheetControl)));
	//binfo=(struct BootInfo *) 0x0ff0;
	Tputstr(0,0,7,sss);
	while (1);
	char str[60];
	int mx=100,my=100;
	
	init_palette();//�趨��ɫ�� 
	
	//��ʼ��ͼ�� 
	struct Sheet *sht_back,*sht_mouse;
	unsigned char *buf_back,buf_mouse[256]; 
	
	
	SCL_init();//��ʼ��ͼ��� 
	//����ͼ�� 
	sht_back=sheet_alloc();//���뱳��ͼ�� 
	buf_back=(unsigned char *)mem_alloc_4k(binfo->scrnx*binfo->scrny);//����ռ� 
	sheet_setbuf(sht_back,buf_back,binfo->scrnx,binfo->scrny,-1);//���ò��� 
	init_screen(sht_back);//������ 
	sheet_slide(sht_back,0,0);//�ƶ�����ͼ�� 
	sheet_updown(sht_back,0);//���õ�0�� 
	//���ͼ�� 
	sht_mouse=sheet_alloc();
	sheet_setbuf(sht_mouse,buf_back,16,16,99);
	init_mouse_cursor(sht_mouse);
	sheet_slide(sht_back,mx,my);
	sheet_updown(sht_mouse,1);
	

	
	
	sprintf (str,"Checking The Memory");
	boxfill(sht_back,0,0,30*8,16,LIGHTGRAY);
	putstr(sht_back,0,0,LIGHTRED,str);
	sheet_refresh();
	//�ڴ��� 
	sprintf (str,"Memory: %dM",size/1024/1024);
	boxfill(sht_back,0,0,30*8,16,LIGHTGRAY);
	putstr(sht_back,0,0,LIGHTRED,str);
	sheet_refresh();

	while(1)
	{
		io_cli();
		if (buffer_get(&(allbuf.key),&data))
		{
			io_sti();
			sprintf (str,"Key:%x",data);
			boxfill(0,1*16,30*8,16,LIGHTGRAY);
			putstr(0,16*1,LIGHTRED,str);
			
		}else if (buffer_get(&allbuf.mouse,&data))
		{
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
				
				boxfill(mx,my,16,16,LIGHTBLUE);
				
				mx+=mdec.dx;
				if (mx<0)
					mx=0;
				if (mx>binfo->scrnx-16)
					mx=binfo->scrnx-16;
				my+=mdec.dy;
				if (my<0)
					my=0;
				if (my>binfo->scrny-16)
					my=binfo->scrny-16;
				
				sprintf (str,"%s (%d,%d,%d,%d)",str,mx,my,mdec.dx,mdec.dy);
				boxfill(0,2*16,30*8,16,LIGHTGRAY);
				putstr(0,16*2,LIGHTRED,str);
				
				
				put_block(mx,my,16,16,mcursor);
				
			}
		}else io_stihlt();
	}*/
}
