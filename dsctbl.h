#pragma once
//ȫ���������� 
struct Segment_Descriptor
{
	/*
	base �λ�ַ�����Ǽ����ԣ��ֳ������Σ� 
	limit ��������20λ��limit_low��limit_high��4λ����limit��λ��ҳ��1ҳΪ4KB�����ܿɱ�ʾ4KB*1M= 4GB
	������12λ��access_right��limit_high��4λ�� 
	*/
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};
//�жϼ�¼�� 
struct Gate_Descriptor
{
	/*
	offset ƫ��
	selector ѡ���
	�����жϺ� �����������λ�ã�ѡ���ȷ���Σ�ƫ��ȷ������λ��
	*/
	 
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct Segment_Descriptor *sd,unsigned int limit,int base,int ar);
void set_gatedesc(struct Gate_Descriptor *gd,int offset,int selector,int ar);
