#pragma once

void MouseControl_1(int kind,	//{ L"左键点击", L"右键点击", L"左键按住", L"右键按住", L"空闲", L"<空>" } 
	int style,					//{ L"平均", L"正太", L"精准", L"<空/精准>" }
	int num,					//{ L"单次/倍", L"多次/倍", L"<空/单>" }
	double times,
	int style_arg = 1,
	int num_arg = 1);

void MouseControl_2(int kind,	//{ L"增量移动", L"坐标移动", L"随机移动", L"随机移动（向心）", L"<空>" },
	int m_style,				//{ L"移动无误差", L"向心随机游走", L"<空/无误差>" }, 
	int s_style,				//{ L"速度无误差", L"速度正太", L"<空/无误差>" }
	int p_style,				//{ L"无按住", L"左键拖动", L"右键拖动", L"<空/无按住>" }
	POINT L,					//那个读入的坐标
	double speed,				//读入的速度
	double time = 500,			//仅限于随机移动的
	double move_arg = 3,		//向心随机游走的concentrate
	double speed_arg = 5		//速度正太
);

void TimerCanDetect(double times);