#include <string>
#include <windows.h>
#include <chrono>
#include "mathbase.h"

extern std::atomic<bool> exitFlag;
double screenWidth = GetSystemMetrics(SM_CXSCREEN);
double screenHeight = GetSystemMetrics(SM_CYSCREEN);
POINT SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE;
bool isSpecialPointShouldReget = true;

void TimerCanDetect(int times) {
	auto start = std::chrono::high_resolution_clock::now();
	while (duration < times) {
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		if (exitFlag) { return; }
		Sleep(20);
	}
}


void Click(int kind) {
	INPUT input;
	input.type = INPUT_MOUSE;
	if (kind == 1) {
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if(kind == 2){
		input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}

	int randtime = (int)AverageDistribution(50, 20);
	if (randtime < 0) {
		randtime = -randtime;
	}
	Sleep(randtime);

	if (kind == 1) {
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (kind == 2) {
		input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	return;
}


void Move(POINT I, POINT L, double speed,
	int style_0,	//{ L"增量移动", L"坐标移动", L"随机移动", L"随机移动（向心）", L"<空>" },
	int style_1,	//{ L"移动无误差", L"向心随机游走", L"<空/无误差>" }, 
	int style_2,	//{ L"速度无误差", L"速度正太", L"<空/无误差>" }
	double time=500,
	double move_arg=3,
	double speed_arg=5
	) {
	
	I.x = I.x * 65535 / screenWidth;
	L.x = L.x * 65535 / screenWidth;
	I.y = I.y * 65535 / screenHeight;
	L.y = L.y * 65535 / screenHeight;

	double step_total;
	POINT NOW = I;
	POINT GO;
	double ex=0, ey=0;
	INPUT input;
	speed = speed / 50.0;
	if (isSpecialPointShouldReget) {
		isSpecialPointShouldReget = false;
		SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE = NOW;

		std::cout << "SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE.x" << SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE.x << std::endl;
	}

	switch (style_0) {
		case 0:
		case 1: {
			double dx = L.x - I.x; double dy = L.y - I.y; double dis = sqrt(dx * dx + dy * dy);
			double angle = atan2(dy, dx);
			step_total = pow(dis * 6 / speed, 1.0 / 3);
			for (double step = 0; step < step_total; step++) {
				auto start = std::chrono::high_resolution_clock::now();
				if (style_2) {
					//没做捏？.?
					GO = TrajPlan(speed * cos(angle), speed * sin(angle), step, step_total, I);
				}
				else {
					GO = TrajPlan(speed * cos(angle), speed * sin(angle), step, step_total, I);
				}
				if (style_1) {
					ex = RandomWalkPreferCenter(ex, 50.0 , 0, move_arg / exp(5.0 * step / step_total));
					ey = RandomWalkPreferCenter(ey, 50.0 , 0, move_arg / exp(5.0 * step / step_total));
					GO.x = GO.x + ex;
					GO.y = GO.y + ey;
				}
				if (!style_0) {
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_MOVE;
					input.mi.dx = (GO.x- NOW.x)* screenWidth/65535.0;
					input.mi.dy = (GO.y - NOW.y) * screenHeight / 65535.0;
					input.mi.time = 0;
					int randt = NormalDistribution(20, 5);
					auto end = std::chrono::high_resolution_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					if (duration.count() < randt) {
						Sleep(randt - duration.count());
					}
					if (exitFlag) { return; }
					SendInput(1, &input, sizeof(INPUT));
					NOW = GO;
				}
				else {

					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
					POINT nowPos;
					GetCursorPos(&nowPos);
					nowPos.x = nowPos.x * 65535 / screenWidth;
					nowPos.y = nowPos.y * 65535 / screenHeight;
					input.mi.dx = GO.x;
					input.mi.dy = GO.y;
					input.mi.time = 0;
					int randt = NormalDistribution(20, 5);
					auto end = std::chrono::high_resolution_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					if (duration.count() < randt) {
						Sleep(randt - duration.count());
					}
					if (exitFlag) { return; }
					SendInput(1, &input, sizeof(INPUT));
					NOW = GO;
				}
			}
		}
			  break;

		case 2:
			speed *=1000;
			for (double i = 0; i < time / 20; i++) {
				auto start = std::chrono::high_resolution_clock::now();
				if (style_2) {
					double randspeed = speed + NormalDistribution(speed, speed_arg);
					GO = RandomWalk(NOW, randspeed);
				}
				else {
					GO = RandomWalk(NOW, speed);
				}	
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_MOVE;
				input.mi.dx = (GO.x - NOW.x) * screenWidth / 65535.0;
				input.mi.dy = (GO.y - NOW.y) * screenHeight / 65535.0;
				input.mi.time = 0;
				int randt = NormalDistribution(20, 5);
				auto end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				if (duration.count()< randt) {
					Sleep(randt - duration.count());
				}
				if (exitFlag) { return; }
				
				SendInput(1, &input, sizeof(INPUT));
				NOW = GO;
			}
			break;

		case 3:
			speed *= 1000;
			for (double i = 0; i < time / 20; i++) {
				auto start = std::chrono::high_resolution_clock::now();
				if (style_2) {
					double randspeed = speed + NormalDistribution(speed, speed_arg);
					GO = RandomWalkPreferCenter(NOW, randspeed, SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE, 0.7);
				}
				else {
					GO = RandomWalkPreferCenter(NOW, speed, SPECIAL_FOR_RANDOMGOCENTER_AS_NO_SPACE, 0.7);
				}
				int randt = NormalDistribution(20, 5);
				auto end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				if (duration.count() < randt) {
					Sleep(randt - duration.count());
				}
				if (exitFlag) { return; }

				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_MOVE;
				input.mi.dx = (GO.x - NOW.x) * screenWidth / 65535.0;
				input.mi.dy = (GO.y - NOW.y) * screenHeight / 65535.0;
				input.mi.time = 0;
				SendInput(1, &input, sizeof(INPUT));
				NOW = GO;
			}
			break;
	}
}

void MouseControl_2(int kind,	//{ L"增量移动", L"坐标移动", L"随机移动", L"随机移动（向心）", L"<空>" },
	int m_style,				//{ L"移动无误差", L"向心随机游走", L"<空/无误差>" }, 
	int s_style,				//{ L"速度无误差", L"速度正太", L"<空/无误差>" }
	int p_style,				//{ L"无按住", L"左键拖动", L"右键拖动", L"<空/无按住>" }
	POINT L,					//那个读入的坐标
	double speed,				//读入的速度
	double time = 500,			//仅限于随机移动的
	double move_arg = 3,		//向心随机游走的concentrate
	double speed_arg = 5		//速度正太
	) {
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	INPUT input;

	if (p_style == 1) {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (p_style == 2) {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}

	switch (kind) {
		case 0: {
			L.x += cursorPos.x;
			L.y += cursorPos.y;
			Move(cursorPos, L, speed, kind, m_style, s_style, NULL, NULL, speed_arg);
		}
		break;

		case 1:
		case 2:
		case 3:
			Move(cursorPos, L, speed, kind, m_style, s_style, time, move_arg, speed_arg);
			break;
	}

	if (p_style == 1) {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (p_style == 2) {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}

	if (exitFlag) { return; }
	return;
}

void MouseControl_1(int kind,	//{ L"左键点击", L"右键点击", L"左键按住", L"右键按住", L"空闲", L"<空>" } 
	int style,					//{ L"平均", L"正太", L"精准", L"<空/精准>" }
	int num,					//{ L"单次/倍", L"多次/倍", L"<空/单>" }
	int times,					
	int style_arg = 1,			
	int num_arg = 1) {		
	INPUT input;
	input.type = INPUT_MOUSE;
	int randomtime;
	for (int i = 0; i < num_arg; i++) {
		if (exitFlag) {return;}
		switch(style) {
			case 0:
				randomtime = (int)AverageDistribution(times, style_arg);
				break;
			case 1:
				randomtime = (int)NormalDistribution(times, style_arg);
				break;
			case 2:
				randomtime = times;
				break;
		}
		switch (kind) {
			case 0:
				Click(1);
				TimerCanDetect(randomtime);
				break;
			case 1:
				Click(2);
				TimerCanDetect(randomtime);
				break;
			case 2:
				input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				input.mi.time = 0;
				SendInput(1, &input, sizeof(INPUT));
				TimerCanDetect(randomtime);
				break;
			case 3:
				input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				input.mi.time = 0;
				SendInput(1, &input, sizeof(INPUT));
				TimerCanDetect(randomtime);
				break;
			case 4:
				TimerCanDetect(randomtime);
				break;
		}
	}
	if (kind == 2) {
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
	else if (kind == 3) {
		input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		input.mi.time = 0;
		SendInput(1, &input, sizeof(INPUT));
	}
}
