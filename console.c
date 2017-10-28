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
#include"calculator.h" 
#include"memorylist.h"
#include"tasklist.h"
#include"open.h"
#include"player.h"
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
	struct Sheet *consoleSheet;
	unsigned char *consoleBuffer;
	consoleSheet=allocSheet();
	consoleBuffer=(unsigned char *)allocMem_4k(200*68,"Console UI");//�����ڴ�ռ� 
	setBufInSheet(consoleSheet,consoleBuffer,200,68,-1);
	makeWindow(consoleSheet,200,68,"Console");
	putStrOnSht(consoleSheet,16+0*8,28+0*16,BLACK,"Welcome to YangXL OS!");
	slideSheet(consoleSheet,180,72);
	updownSheet(consoleSheet,task->winID+1);
	
	//�������Ϣ 
	char curInput[128];
	int curPosX=0,length=0;//��� 
	unsigned char data;
	struct Command command;//���� 
	char str[128];
	int flag=0,f1=0;
	int t=0;
	while (1)
	{
		flag=0; 
		if (window.focus!=task->winID)//���㲻�ڣ�ȡ����� 
		{
			if (f1==0)
			{
				boxfillOnSht(consoleSheet,16+8*curPosX,44,8,15,WHITE);
				refreshSubInSheet(consoleSheet,16+8*curPosX,44,8,15); 
				f1=1;
			}
			continue;
		}else if (timerCur->flag==TIMER_ALLOCED)//���»�ý��㣬������� 
		{
			initTimer(timerCur,&bufferTime,1);
			setTimer(timerCur,50);
		}
		f1=0;
		io_cli();
		//����������¼� 
		if (getBuffer(&task->bufAll.key,&data))
		{
			//���� 
			io_sti();
			flag=1;
			switch (data)
			{
				case 0x0e://�˸�� 
					if (curPosX>0)
					{
						curPosX--;
						length--; 
						for (int j=curPosX;j<=length;j++)
							curInput[j]=curInput[j+1];
						curInput[length] = '\0';
					} 
					break;
				case 0x4b:
					if (curPosX>0)
						curPosX--;
					break;
				case 0x4d:
					if (curPosX<length)
						curPosX++;
					break;
				case 0x1c://�س��� 
					analyseCommand(curInput,&command);
					curInput[0]='\0';
					curPosX=0;
					length=0;
					switch (command.commandType) 
					{
						case Calculator:
						{
							//����calculator����
							struct Task *calculatorTask; 
							calculatorTask=allocTask();		
							initTask(calculatorTask,(int)&calculatorTask_Main,"Calculator",11);
							createWindow(calculatorTask,"Calculator");
							runTask(calculatorTask);
							//sprintf (str,"%d",calculatorTask->winID);
							//putStrAndBackOnSht(consoleSheet,16,28,BLACK,WHITE,str,22);
							break;
						}
						case MemoryList:
						{
							struct Task *memoryListTask; 
							memoryListTask=allocTask();		
							initTask(memoryListTask,(int)&memoryListTask_Main,"Memory List",1);
							createWindow(memoryListTask,"Memory List");
							runTask(memoryListTask);
							break;
						}
						case TaskList:
						{
							struct Task *taskListTask; 
							taskListTask=allocTask();		
							initTask(taskListTask,(int)&taskListTask_Main,"Task List",1);
							createWindow(taskListTask,"Task List");
							runTask(taskListTask);
							break;
						}
						case Open:
						{
							openFile (command.par[0]);
							break;
						}
						case Play:
						{
							struct Task *playTask; 
							playTask=allocTask();
							initTask(playTask,(int)&playTask_Main,"Player",11);
							sprintf(playTask->par[0],command.par[0]);
							playTask->parCount++;
							createWindow(playTask,"Music Player");
							runTask(playTask);
							break;
						}
					}
					break;
				default:
					if (data<0x80 && keyboard.keyTable[data]>0 && curPosX<=20)//��ĸ������ 
					{
						curInput[curPosX++] = keyboard.keyTable[data+keyboard.isShift*0x80];
						if (curPosX>length)
						{
							curInput[curPosX] = '\0';
							length=curPosX;
						}
					}
					break;
			}
			sprintf (str,"%s",curInput);
			putStrAndBackOnSht(consoleSheet,16,44,BLACK,WHITE,str,22);
		}
		if (getBuffer(&task->bufAll.mouse,&data))
		{
			io_sti();
			flag=2;
			switch(data)
			{
				case 0:
					slideSheet(consoleSheet,mdec.x,mdec.y);
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
				boxfillOnSht(consoleSheet,16+8*curPosX,44,8,15,WHITE);
				refreshSubInSheet(consoleSheet,16+8*curPosX,44,8,15);   
				setTimer(timerCur,50);
				break;
			case 1:
				initTimer(timerCur,&bufferTime,0);
				boxfillOnSht(consoleSheet,16+8*curPosX,44,8,15,BLACK);
				refreshSubInSheet(consoleSheet,16+8*curPosX,44,8,15);
				setTimer(timerCur,50);
				break; 
			}
		} 
		if (flag==0)
			io_sti();
	}
}
