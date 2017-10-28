#include"int.h"
#include"nasmfunc.h"
#include"graphic.h"
#include"buffer.h"
#include<stdio.h>
/*
PIC�ǿɱ���жϿ����� ������PIC�������ӣ�ÿ��PIC��8·IRQ���ж����� 
IMR �ж���������8λ��Ӧ8��IRQ�ź� ��PIC�ļĴ��� 
io_out8���Ӧ�˿�д���� 
*/
struct Buffer buffer;
void init_pic(void)
{
	//��ֹ�����ж� 
	io_out8(PIC0_IMR,0xff);	
	io_out8(PIC1_IMR,0xff);
	
	//������PIC 
	//ICW��4��2�ֽ� ICW1��ICW4 Ϊ�������ԣ��̶�ֵ
	//ICW2����IRQ����һ���ж�֪ͨCPU 
	//ICW3������PIC�����趨 0x00000100  ��PIC���ӵ���PIC��IRQ2�� �̶�ֵ 
	io_out8(PIC0_ICW1,0x11); //���ش���ģʽ
	io_out8(PIC0_ICW2,0x20); //IRQ0-7��INT20-27����
	io_out8(PIC0_ICW3,1<<2); //��PIC��IRQ2���� 
	io_out8(PIC0_ICW4,0x01); //�޻�����ģʽ
	
	//���ô�PIC 
	io_out8(PIC1_ICW1,0x11); //���ش���ģʽ
	io_out8(PIC1_ICW2,0x28); //IRQ8-15��INT28-2f����
	io_out8(PIC1_ICW3,2); 	 //��PIC��IRQ2���� 
	io_out8(PIC1_ICW4,0x01); //�޻�����ģʽ
	
	io_out8(PIC0_IMR,0xfb);	//11111011 PIC1���� ȫ����ֹ 
	io_out8(PIC1_IMR,0xff);	//11111111 ��ֹ�����ж�

	return; 
	 
}
//�����жϴ������ 
void inthandler21(int *esp)
{
	struct BootInfo *binfo=(struct BootInfo *) 0x0ff0;	
	//֪ͨPIC�ж��Ѿ����� 
	io_out8(PIC0_OCW2,0x61);
	unsigned char data=io_in8(PORT_KEYDAT);
	buffer_put(&buffer,data);
	return;
} 

