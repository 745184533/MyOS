#include"nasmfunc.h"
#include"memory.h"
#include"sheet.h"
#include"graphic.h"
#include"buffer.h"
#include"timer.h" 
#include"mtask.h"
#include"keyboard.h"
#include"mouse.h"
#include"calculator.h" 
#include<stdio.h>
//����calculator 
void calculatorTask_Main(struct Task *task)
{
	//��ʼ�������� 
	char bufferArray[128];
	struct Buffer bufferTime;
	initBuffer(&bufferTime,128,bufferArray);

	//��ʼ����ʱ�� 
	struct Timer *timerCur;
	timerCur=allocTimer();
	initTimer(timerCur,&bufferTime,1);
	setTimer(timerCur,50);
	
	//��ʼ������̨ͼ��
	struct Sheet *consoleSheet;
	unsigned char *consoleBuffer;	
	consoleBuffer=(unsigned char *)allocMem_4k(512*310);
	consoleSheet=allocSheet();
	setBufInSheet(consoleSheet,consoleBuffer,512,310,-1);//û��͸��ɫ
	slideSheet(consoleSheet,202,8);
	makeWindow(consoleSheet,512,310,"Calculator");
	makeTextBox(consoleSheet,8,27,496,276,BLACK);
	setHeightSheet(consoleSheet,1);
	
	unsigned char data;
	int curPos=0;
	int flag=0;
	while (1)
	{
		flag=0;
		if (window.focus!=task->winID)//���㲻�ڣ�ȡ����� 
		{
			boxfillOnSht(consoleSheet,8+8*curPos,28,8,15,BLACK);
			refreshSubInSheet(consoleSheet,8+8*curPos,28,8,15); 
			continue;
		}else if (timerCur->flag==TIMER_ALLOCED)//���»�ý��㣬������� 
		{
			initTimer(timerCur,&bufferTime,1);
			setTimer(timerCur,50);
		}
		io_cli();
		if (getBuffer(&bufferTime,&data))
		{
			io_sti();
			flag=3;
			switch (data)
			{
			case 0:
				initTimer(timerCur,&bufferTime,1);
				boxfillOnSht(consoleSheet,8+8*curPos,28,8,15,WHITE);
				refreshSubInSheet(consoleSheet,8+8*curPos,28,8,15);   
				setTimer(timerCur,50);
				break;
			case 1:
				initTimer(timerCur,&bufferTime,0);
				boxfillOnSht(consoleSheet,8+8*curPos,28,8,15,BLACK);
				refreshSubInSheet(consoleSheet,8+8*curPos,28,8,15);
				setTimer(timerCur,50);
				break; 
			}
		} 
		if (flag==0)
		{
			io_sti();
		} 
	}
}
