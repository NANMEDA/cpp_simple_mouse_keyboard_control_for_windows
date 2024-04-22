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
	int t = 0;
	while (t < times) {
		if (exitFlag) { return; }
		Sleep(20);
		t = t+20;
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
	int style_0,	//{ L"�����ƶ�", L"�����ƶ�", L"����ƶ�", L"����ƶ������ģ�", L"<��>" },
	int style_1,	//{ L"�ƶ������", L"�����������", L"<��/�����>" }, 
	int style_2,	//{ L"�ٶ������", L"�ٶ���̫", L"<��/�����>" }
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
					//û����.?
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

void MouseControl_2(int kind,	//{ L"�����ƶ�", L"�����ƶ�", L"����ƶ�", L"����ƶ������ģ�", L"<��>" },
	int m_style,				//{ L"�ƶ������", L"�����������", L"<��/�����>" }, 
	int s_style,				//{ L"�ٶ������", L"�ٶ���̫", L"<��/�����>" }
	int p_style,				//{ L"�ް�ס", L"����϶�", L"�Ҽ��϶�", L"<��/�ް�ס>" }
	POINT L,					//�Ǹ����������
	double speed,				//������ٶ�
	double time = 500,			//����������ƶ���
	double move_arg = 3,		//����������ߵ�concentrate
	double speed_arg = 5		//�ٶ���̫
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

void MouseControl_1(int kind,	//{ L"������", L"�Ҽ����", L"�����ס", L"�Ҽ���ס", L"����", L"<��>" } 
	int style,					//{ L"ƽ��", L"��̫", L"��׼", L"<��/��׼>" }
	int num,					//{ L"����/��", L"���/��", L"<��/��>" }
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