#pragma once
#include <iostream>
#include <random>
#include <Windows.h>

double NormalDistribution(double mean, double stddev);
double AverageDistribution(double mean, double stddev);
double RandomWalk(double base, double steps);
POINT RandomWalk(POINT base, double steps);
double RandomWalkPreferCenter(double base, double steps, double center, double concentrate = 3);
POINT RandomWalkPreferCenter(POINT base, double steps, POINT center, double concentrate = 1);
POINT TrajPlan(double speedx, double speedy, double step_now, double step_total, POINT I);