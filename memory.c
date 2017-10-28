#include"memory.h"
#include"nasmfunc.h"
/*
�ڴ�Ĺ���ʹ�ÿ��б�
ʹ���ڴ�ǰ�ȼ���ڴ� 
*/ 
struct MemoryList *meml;
unsigned int memtest(unsigned int start,unsigned int end)
{
	/*
	����ڴ�ķ��������ڴ�д��ֵ���ٶ�ȡ
	���ǰ��رջ��棨CPU486���ϲ��У�����Ϊ�л������������ȴӻ����д
	���ж��Ƿ�Ϊ486���ϵ�CPU
	486����EFLAGS��18λ��Զ��0������д��1�ڶ�ȡ���Ƿ�Ϊ1*/
	
	char flg486=0;
	unsigned int eflg,cr0,i;
	eflg=io_load_eflags();
	eflg|=EFLAGS_AC_BIT;//����18λд��1
	io_store_eflags(eflg);
	eflg=io_load_eflags();
	flg486=((eflg & EFLAGS_AC_BIT)!=0);//�ж��Ƿ�ָ�Ϊ0
	eflg&=~EFLAGS_AC_BIT;//�ָ�EFLAGS
	io_store_eflags(eflg);
	
	if (flg486)
	{
		cr0=load_cr0();
		cr0|=CR0_CACHE_DISABLE;//��ֹ����
		store_cr0(cr0);
	}
	
	i=memtestSub(start,end);//�ڴ���

	if (flg486)
	{
		cr0=load_cr0();
		cr0&=~CR0_CACHE_DISABLE;//������
		store_cr0(cr0);
	}
	
	return i;
		
}
unsigned int memtestSub(unsigned int start,unsigned int end)
{
	unsigned int i,*p,old,pat0=0xaa55aa55,pat1=0x55aa55aa;
	for (i=start;i<=end;i+=0x1000)
	{
		p=(unsigned int *)i+0xffc;//ֻ�ü������4���ֽڼ���
		//��ÿ����Ԫ��Ĳ��Է�����д��->��ת->�ж�->�ٷ�ת->�ж�->�ָ�
		old=*p;
		*p=pat0;//д�� 
		*p^=0xffffffff;//��ת
		if (*p!=pat1)//�ж�
			return i;
			
		*p^=0xffffffff;//�ٷ�ת
		if (*p!=pat0)
			return i;
		*p=old;
	}
	return i;
}

void initMem()
{
	meml->freesize=0;
	meml->maxfreesize=0;
}
unsigned int freeTotalMem()
{
	int i,t=0;
	for (i=0;i<meml->maxfreesize;i++) 
		t+=meml->free[i].size;
	return t;
}
//����ռ� 
unsigned int allocMem(unsigned int size)
{
	
	int i,a,j;
	for (i=0;i<meml->freesize;i++)
	{
		if (meml->free[i].size>=size)
		{
			a=meml->free[i].addr;
			meml->free[i].size-=size;
			meml->free[i].addr+=size;
			//ɾ������ 
			if (meml->free[i].size==0)
			{
				for (j=i;j<meml->freesize;j++)
					meml->free[j]=meml->free[j+1];
			}
			return a;
		}
	}
}
int freeMem(unsigned int addr,unsigned int size)
{
	int i,j;
	//��ȡ������λ�� 
	for (i=0;i<meml->freesize;i++)
		if (meml->free[i].addr>addr)
			break;
	//��ǰ������Ժϲ� 
	if (i>0 && meml->free[i-1].addr+meml->free[i-1].size==addr)
	{
		meml->free[i-1].size+=size;
		//������Ҳ���Ժϲ� 
		if (i<meml->freesize && meml->free[i].addr==addr+size)
		{
			meml->free[i-1].size+=meml->free[i].size;
			//ɾ�������
			meml->freesize--;
			for (j=i;j<meml->maxfreesize;j++) 
				meml->free[j]=meml->free[j+1];
		}
		return 0;
	}
	//����������Ժϲ� 
	if (i<meml->freesize && meml->free[i].addr==addr+size)
	{
		meml->free[i].addr=addr; 
		meml->free[i].size=addr;
		return 0;
	}
	//���ɺϲ�
	if (meml->freesize>=MEMORYLISTFREES-1)//������������ֵ 
		return -1;
	for (j=i;j<meml->freesize;j++) 
		meml->free[j+1]=meml->free[j];
	meml->free[i].addr=addr;
	meml->free[i].size=size;
	if (meml->free[i].size>meml->maxfreesize)
		meml->maxfreesize=meml->free[i].size;
	meml->freesize++; 
	return 0;
}
//һ������4KB
unsigned int allocMem_4k(unsigned int size)
{
	unsigned int a;
	size=(size+0xfff)&0xfffff000;
	a=allocMem(size);
	return a;
}
//һ���ͷ�4KB
int freeMem_4k(unsigned int addr,unsigned int size)
{
	unsigned int a;
	size=(size+0xfff)&0xfffff000;
	a=freeMem(addr,size);
	return a;
}
