#include"nasmfunc.h"
#include"memory.h"
#include"sheet.h"
#include"graphic.h"
void boxfill_sht(struct Sheet *sht,int x0,int y0,int pxsize,int pysize,unsigned char c)
{
	int i,j;
	for (j=y0;j<y0+pysize;j++)
		for (i=x0;i<x0+pxsize;i++)
			sht->buffer[j*sht->xsize+i]=c;
}

void putfont8_sht(struct Sheet *sht,int x,int y,char color,char c)
{
	int i,j;
	char tmp[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	char *font=fontlib+c*16;
	for (i=0;i<16;i++)
		for (j=0;j<8;j++)
			if ((font[i]&tmp[j])!=0)
				sht->buffer[(y+i)*sht->xsize+x+j]=color;
				
}
void putstr_sht(struct Sheet *sht,int x,int y,char color,char *str)
{
	char *t=str;
	while (*t!=0x00)
	{
		putfont8_sht(sht,x,y,color,*t);
		x+=8;
		t++;
	}
}

void init_mouse_cursor(struct Sheet *sht)
{
	//�������ͼ
	static char cursor[18][17] = {
		"****************",
		"****************",
		"**************..",
		"*OOOOOOOOOOO*...",
		"*OOOOOOOOOO*....",
		"*OOOOOOOOO*.....",
		"*OOOOOOOO*......",
		"*OOOOOOO*.......",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOO**OOO*.....",
		"*OOO*..*OOO*....",
		"*OO*....*OOO*...",
		"*O*......*OOO*..",
		"**........*OOO*.",
		"*..........*OOO*",
		"............*OO*",
		".............***"
	};
	//�˴��޷���⣬ǰ����Ϊ���޷���ʾ��������
	for(int i=0;i<16;i++)
		for (int j=0;j<16;j++)
			switch(cursor[i+2][j])
			{
				case '*':
					sht->buffer[i*16+j]=BLACK;
					break;
				case 'O':
					sht->buffer[i*16+j]=WHITE;
					break;
				default:
					sht->buffer[i*16+j]=sht->col_inv;
			}
}

void put_block_sht(struct Sheet *sht,int x0,int y0,int pxsize,int pysize,char *block)
{
	for (int y=0;y<pysize;y++)
		for (int x=0;x<pxsize;x++)
			if (block[y*pxsize+x]>=0)
				sht->buffer[(y0+y)*sht->xsize+x0+x]=block[y*pxsize+x];
}

void init_palette()
{	
	static unsigned char table_rgb[16][3] = 
	{
		{0x00, 0x00, 0x00},	//��
		{0xff, 0x00, 0x00},	//����
		{0x00, 0xff, 0x00},	//����
		{0xff, 0xff, 0x00},	//����
		{0x00, 0x00, 0xff},	//����
		{0xff, 0x00, 0xff},	//����
		{0x00, 0xff, 0xff},	//ǳ����
		{0xff, 0xff, 0xff},	//��
		{0xc6, 0xc6, 0xc6},	//����
		{0x84, 0x00, 0x00},	//����
		{0x00, 0x84, 0x00},	//����
		{0x84, 0x84, 0x00},	//����
		{0x00, 0x00, 0x84},	//����
		{0x84, 0x00, 0x84},	//����
		{0x00, 0x84, 0x84},	//ǳ����
		{0x84, 0x84, 0x84}	//����
	};
	set_palette(0, 15, table_rgb);
	return;
}

void set_palette(int start,int end,unsigned char rgb[16][3])
{
	int i,eflags;
	eflags=io_load_eflags();
	io_cli();
	io_out8(0x03c8,start);//��0�ŵ�ɫ�忪ʼ�趨
	
	for (i=start;i<=end;i++)
	{
		//��RGBд�룬��ɫ������Զ�����
		io_out8(0x03c9,rgb[i][0]/4);//��4��Ϊ�˸ı���ǳ
		io_out8(0x03c9,rgb[i][1]/4);
		io_out8(0x03c9,rgb[i][2]/4);
	}
	
	io_store_eflags(eflags);
}
void init_screen_sht(struct Sheet *sht)
{
	//char *vram=sht->buffer;
	
	
	int x=sht->xsize; 
	int y=sht->ysize;
	
	//�������
	boxfill_sht(sht,0,0,x,y-28,LIGHTBLUE);
	boxfill_sht(sht,0,y-28,x,1,LIGHTGRAY);
	boxfill_sht(sht,0,y-27,x,1,LIGHTGRAY);
	boxfill_sht(sht,0,y-26,x,26,WHITE);

	boxfill_sht(sht,3,y-24,57,1,WHITE);
	boxfill_sht(sht,2,y-24,1,21,WHITE);
	boxfill_sht(sht,3,y-4,57,1,DULLGRAY);
	boxfill_sht(sht,59,y-23,1,19,DULLGRAY);
	boxfill_sht(sht,2,y-3,57,1,BLACK);
	boxfill_sht(sht,60,y-24,1,22,BLACK);

	boxfill_sht(sht,x-47,y-24,44,1,DULLGRAY);
	boxfill_sht(sht,x-47,y-23,1,20,DULLGRAY);
	boxfill_sht(sht,x-47,y-3,44,1,WHITE);
	boxfill_sht(sht,x-3,y-24,1,22,WHITE);
	return;
}

void putfont8_srn(struct BootInfo *binfo,int x,int y,char color,char c)
{
	int i,j;
	char tmp[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	char *font=fontlib+c*16;
	for (i=0;i<16;i++)
		for (j=0;j<8;j++)
			if ((font[i]&tmp[j])!=0)
				binfo->vram[(y+i)*binfo->scrnx+x+j]=color;
				
}
void putstr_srn(struct BootInfo *binfo,int x,int y,char color,char *str)
{
	char *t=str;
	while (*t!=0x00)
	{
		putfont8_srn(binfo,x,y,color,*t);
		x+=8;
		t++;
	}
}

