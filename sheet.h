#pragma once
#define MAXSHEET 256 
#define SHEETFREE 0
#define SHEETUSE 1


struct Sheet
{
	unsigned char *buffer;//����
	int x0,y0,xsize,ysize;//��ʼ���꣬����
	int col_inv,height,flags; //͸��ɫɫ�š�ͼ��߶ȡ�������Ϣ 
};
struct SheetControl
{
	unsigned char *vram;
	int xsize,ysize,top;//top��ʾ���ϲ�ͼ��ĸ߶� 
	struct Sheet *sheetp[MAXSHEET];//ͼ��ĵ�ַ������������� 
	struct Sheet sheet[MAXSHEET];//����ͼ�� 
};
/*extern struct SheetControl *scl;
extern struct BootInfo *binfo;
extern struct MemoryList *meml;*/
struct SheetControl* SCL_init(struct MemoryList *meml,struct BootInfo *binfo);
struct Sheet *sheet_alloc(struct SheetControl *scl);
void sheet_setbuf(struct Sheet *sht,unsigned char *buf,int xsize,int ysize,int col_inv);
void sheet_updown(struct SheetControl *scl,struct Sheet *sht,int height);
void sheet_slide(struct SheetControl *scl,struct Sheet* sht,int vx0,int vy0);
void sheet_free(struct SheetControl *scl,struct Sheet* sht);
void sheet_refreshSub(struct SheetControl *scl,int x0,int y0,int pxsize,int pysize,int h0);
void sheet_refreshAll(struct SheetControl *scl);
void sheet_refreshSheet(struct SheetControl *scl,struct Sheet *sht);
void sheet_refreshSheetSub(struct SheetControl *scl,struct Sheet *sht,int x0,int y0,int xsize,int ysize);
