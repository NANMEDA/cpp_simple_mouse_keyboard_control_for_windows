#pragma once

void MouseControl_1(int kind,	//{ L"������", L"�Ҽ����", L"�����ס", L"�Ҽ���ס", L"����", L"<��>" } 
	int style,					//{ L"ƽ��", L"��̫", L"��׼", L"<��/��׼>" }
	int num,					//{ L"����/��", L"���/��", L"<��/��>" }
	double times,
	int style_arg = 1,
	int num_arg = 1);

void MouseControl_2(int kind,	//{ L"�����ƶ�", L"�����ƶ�", L"����ƶ�", L"����ƶ������ģ�", L"<��>" },
	int m_style,				//{ L"�ƶ������", L"�����������", L"<��/�����>" }, 
	int s_style,				//{ L"�ٶ������", L"�ٶ���̫", L"<��/�����>" }
	int p_style,				//{ L"�ް�ס", L"����϶�", L"�Ҽ��϶�", L"<��/�ް�ס>" }
	POINT L,					//�Ǹ����������
	double speed,				//������ٶ�
	double time = 500,			//����������ƶ���
	double move_arg = 3,		//����������ߵ�concentrate
	double speed_arg = 5		//�ٶ���̫
);

void TimerCanDetect(double times);