#include"memory.h"
#include"nasmfunc.h" 
#include"sheet.h"
#include<stdio.h>
struct SheetControl* SCL_init(struct MemoryList *meml,struct BootInfo *binfo)
{
	int i;
	struct SheetControl *scl;
	scl=(struct SheetControl *)mem_alloc_4k(meml,sizeof(struct SheetControl));
	if (scl==0)
		return 0;
	scl->vram=binfo->vram;
	scl->xsize=binfo->scrnx;
	scl->ysize=binfo->scrny;
	scl->top=-1;//û��ͼ�� 
	for (i=0;i<MAXSHEET;i++)
		scl->sheet[i].flags=SHEETFREE;//���Ϊδʹ��
	return scl; 
}
struct Sheet *sheet_alloc(struct SheetControl *scl)
{
	struct Sheet *sht;
	int i;
	//������е�ͼ�� 
	for (i=0;i<MAXSHEET;i++)
		if (scl->sheet[i].flags==SHEETFREE)
		{
			sht=&scl->sheet[i];
			sht->flags=SHEETUSE;
			sht->height=-1;//-1��ʾ����ʾ 
			return sht;
		}
	return 0;
}
//����ͼ�� 
void sheet_setbuf(struct Sheet *sht,unsigned char *buf,int xsize,int ysize,int col_inv)
{
	sht->buffer=buf;
	sht->xsize=xsize; 
	sht->ysize=ysize;
	sht->col_inv=col_inv;
}
//����ͼ�� ���޸ĸ߶ȣ� 
void sheet_updown(struct SheetControl *scl,struct Sheet *sht,int height)
{
	int old=sht->height,i;
	//����������ֵ 
	if (height>=scl->top+1)
		height=scl->top+1;
	else if (height<-1)
		height=-1;
	//�޸�ĳһͼ��߶���4����� 
	sht->height=height;
	if (old>height && height>=0)
	{
		//����ͼ������Ȼ��ʾ ,���м�ͼ�������ᣬ���޸�ͼ����и߶� 
		for (i=old;i>height;i--)
		{
			scl->sheetp[i]=scl->sheetp[i-1];
			scl->sheetp[i]->height=i;
		}
		scl->sheetp[height]=sht;
	}else if (old>height && height<0)
	{
		//����ͼ��,������ͼ��������,���޸�ͼ����и߶� 
		for (i=old;i<scl->top;i++)
		{ 
			scl->sheetp[i]=scl->sheetp[i+1];
			scl->sheetp[i]->height=i;
		}
		scl->top--;
	}else if (old <height && old>=0)
	{
		//���ͼ����ԭͼ������ʾ״̬ ,���м�ͼ�������������޸�ͼ����и߶� 
		for (i=old;i<height;i++)
		{
			scl->sheetp[i]=scl->sheetp[i+1];
			scl->sheetp[i]->height=i;
		}
		scl->sheetp[height]=sht;
	}else if (old<height && old<0)
	{
		//��ʾ���ص�ͼ�� ,������ͼ�������ᣬ���޸�ͼ����и߶� 
		for (i=scl->top+1;i<height;i--)
		{
			scl->sheetp[i]=scl->sheetp[i-1];
			scl->sheetp[i]->height=i;
			
		}
		scl->sheetp[height]=sht;
		scl->top++;
	}
	//���»���ͼ�� 
	sheet_refresh(scl);
}

void sheet_refresh(struct SheetControl *scl)
{
	int h,sx,sy,vx,vy;//(sx,sy)��ʾ����ͼ���е����� (vx,vy) ��ʾ���������е����� 
	unsigned char *buf,c,*vram=scl->vram;
	struct Sheet *sht;
	//�ӵ͵��߻���ͼ�� 
	for (h=0;h<=scl->top;h++)
	{
		sht=scl->sheetp[h];
		buf=sht->buffer;
		for (sy=0;sy<sht->ysize;sy++)
		{
			vy=sht->y0+sy;
			for (sx=0;sx<sht->xsize;sx++)
			{
				vx=sht->x0+sx;
				c=buf[sy*sht->xsize+sx];
				//�����ز���͸�� 
				if (c!=sht->col_inv)
					vram[vy*scl->xsize+vx]=c;
			}
		}
	}
}

//�ƶ�ͼ�� 
void sheet_slide(struct SheetControl *scl,struct Sheet* sht,int vx0,int vy0)
{
	sht->x0=vx0;
	sht->y0=vy0;
	if (sht->height>=0)//������ʾ 
		sheet_refresh(scl);
	return;
}
//�ͷ�ͼ�� 
void sheet_free(struct SheetControl *scl,struct Sheet* sht)
{
	//���ظ�ͼ�㲢��δʹ�ñ�־
	if (sht->height>=0) 
		sheet_updown(scl,sht,-1);
	sht->flags=0;
	return;
}
