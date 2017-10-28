#include"nasmfunc.h"
#include"memory.h"
#include"sheet.h"
#include"graphic.h"
#include"buffer.h"
#include"timer.h" 
#include"mtask.h"
#include"keyboard.h"
#include"mouse.h" 
#include"console.h"
#include<stdio.h>
#include<string.h>
//���ַ������������� 
void analyseCommand(char *commandString,struct Command *command)
{
	command->commandType=-1;
	command->parCount=0;
	char com[MAXCOMMANDCOUNT][MAXCOMMANDLENGTH]=
	{
		"Calculator",
		"Open",
		"Play",
		"TaskList",
		"MemoryList",
		"Shutdown" 
	};
	char coms[MAXCOMMANDLENGTH];
	int i=0,j=0;
	//�������� 
	while (commandString[i]!='\0' && commandString[i]!=' ')
	{
		coms[i]=commandString[i];
		i++;
	}
	coms[i]='\0';
	if (commandString[i]==' ')
		i++;
	for (int i=0;i<MAXCOMMANDCOUNT;i++)
		if (strcmp(coms,com[i])==0)
			command->commandType=i;
	
	//�������� 
	while (commandString[i]!='\0')
		if (commandString[i]!=' ') 
			command->par[command->parCount][j++]=commandString[i++];
		else 
		{
			command->par[command->parCount++][j]='\0';
			j=0;
			i++;
		}
	command->par[command->parCount++][j]='\0';
}

//console���� 
void consoleTask_Main(struct Task *task)
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
	struct Sheet *sht_window;
	unsigned char *buf_window;
	sht_window=allocSheet();
	buf_window=(unsigned char *)allocMem_4k(200*68);//�����ڴ�ռ� 
	setBufInSheet(sht_window,buf_window,200,68,-1);
	makeWindow(sht_window,200,68,"Console");
	putStrOnSht(sht_window,16+0*8,28+0*16,BLACK,"Welcome to YangXL OS!");
	slideSheet(sht_window,280,72);
	setHeightSheet(sht_window,2);
	
	//�������Ϣ 
	char curInput[128];
	int curPos=0;//��� 
	unsigned char data;
	struct Command command;//���� 
	char str[128];
	int flag=0;
	while (1)
	{
		flag=0; 
		if (window.focus!=task->winID)//���㲻�ڣ�ȡ����� 
		{
			boxfillOnSht(sht_window,16+8*curPos,44,8,15,WHITE);
			refreshSubInSheet(sht_window,16+8*curPos,44,8,15); 
			continue;
		}else if (timerCur->flag==TIMER_ALLOCED)//���»�ý��㣬������� 
		{
			initTimer(timerCur,&bufferTime,1);
			setTimer(timerCur,50);
		}
		io_cli();
		//����������¼� 
		if (getBuffer(&task->bufAll.key,&data))
		{
			//���� 
			io_sti();
			flag=1;
			if (data<0x80 && keyboard.keyTable[data]>0 && curPos<=20)//��ĸ������ 
			{
				curInput[curPos++] = keyboard.keyTable[data+keyboard.isShift*0x80];
				curInput[curPos] = '\0';
			}else switch (data)
			{
				case 0x0e://�˸�� 
					if (curPos>0)
						curInput[--curPos] = '\0';
					break; 
				case 0x1c://�س��� 
					analyseCommand(curInput,&command);
					curInput[0]='\0';
					curPos=0;
					/*sprintf (str,"%d",command.commandType);
					for (int i=0;i<command.parCount;i++)
					{
						sprintf (str,"%s_%s",str,command.par[i]);
					}
					putStrAndBackOnSht(sht_window,16,28,BLACK,WHITE,str,22);*/
					break;
				default:
					break;
			}
			sprintf (str,"%s",curInput);
			putStrAndBackOnSht(sht_window,16,44,BLACK,WHITE,str,22);
		}
		if (getBuffer(&task->bufAll.mouse,&data))
		{
			io_sti();
			flag=2;
			switch(data)
			{
				case 2:
					slideSheet(sht_window,mdec.x,mdec.y);
					break;
			}
		}
		if (getBuffer(&bufferTime,&data))
		{
			io_sti();
			flag=3;
			switch (data)
			{
			case 0:
				initTimer(timerCur,&bufferTime,1);
				boxfillOnSht(sht_window,16+8*curPos,44,8,15,WHITE);
				refreshSubInSheet(sht_window,16+8*curPos,44,8,15);   
				setTimer(timerCur,50);
				break;
			case 1:
				initTimer(timerCur,&bufferTime,0);
				boxfillOnSht(sht_window,16+8*curPos,44,8,15,BLACK);
				refreshSubInSheet(sht_window,16+8*curPos,44,8,15);
				setTimer(timerCur,50);
				break; 
			}
		} 
		if (flag==0)
			io_sti();
	}
}
