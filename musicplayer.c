#include"nasmfunc.h"
#include"memory.h"
#include"sheet.h"
#include"graphic.h"
#include"buffer.h"
#include"timer.h" 
#include"mtask.h"
#include"keyboard.h"
#include"mouse.h"
#include"musicplayer.h" 
#include<stdio.h>

void musicPlayerTask_Main(struct Task *task)
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

	//��ʾ����
	struct Sheet *textViewerSheet;
	unsigned char *textViewerBuffer;
	textViewerSheet=allocSheet();
	textViewerBuffer=(unsigned char *)allocMem_4k(200*68,"Music Player UI");//�����ڴ�ռ� 
	setBufInSheet(textViewerSheet,textViewerBuffer,200,68,-1);
	char s[60];
	sprintf (s,"Music Player: %s",task->par[0]);
	makeWindow(textViewerSheet,200,68,"Music Player");
	slideSheet(textViewerSheet,180,72);
	updownSheet(textViewerSheet,task->winID+1);
	
	//�������Ϣ 
	char curInput[128];
	int curPosX=0,length=0;//��� 
	unsigned char data;
	//struct Command command;//���� 
	char str[128];
	int flag=0,f1=0;
	int t=0;
	while (1);
}
