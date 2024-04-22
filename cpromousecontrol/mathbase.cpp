#include <iostream>
#include <random>
#include <Windows.h>


double NormalDistribution(double mean, double stddev) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(mean, stddev);
 
    double randomNum = distribution(gen);
    return randomNum;
}

double AverageDistribution( double mean, double stddev) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(mean- stddev, mean + stddev);

    double randomNum = distribution(gen);
    return randomNum;
}

double RandomWalk(double base, double steps) {  //衰减就是减小steps
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(-1,1); // 在 [-1, 1) 范围内均匀分布的随机数

    double future = base;
    future += distribution(gen) * steps;

    return future;
}

POINT RandomWalk(POINT base,double steps) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(0.0, 2 * 3.1415926); // 在 [0, 2pi) 范围内均匀分布的随机数

    double angle = distribution(gen);

    // 计算下一个点的坐标
    POINT nextPoint;
    nextPoint.x = base.x + (steps * cos(angle));
    nextPoint.y = base.y + (steps * sin(angle));

    return nextPoint;
}

double RandomWalkPreferCenter(double base, double steps,double center, double concentrate = 3) {  //衰减就是减小concentrate
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(center-base, concentrate);

    double future = base;
    future += distribution(gen) + steps;
    return future;
}

POINT RandomWalkPreferCenter(POINT base, double steps, POINT center,double concentrate = 1) {  //衰减就是减小concentrate
    std::random_device rd;
    std::mt19937 gen(rd());
    double Angle = atan2(center.y - base.y, center.x - base.x);
    std::normal_distribution<double> distribution(Angle, concentrate* 3.1415926);
    double angle = distribution(gen);

    POINT nextPoint = base;
    nextPoint.x += steps * cos(angle);
    nextPoint.y += steps * sin(angle);
    return nextPoint;
}

POINT TrajPlan(double speedx, double speedy, double step_now,double step_total,POINT I) {
    //traj是，输入的
    POINT NOW;
    NOW.x =I.x - 1.0/3 * speedx* step_now * step_now * step_now + 0.5 * speedx*step_total* step_now * step_now;
    NOW.y =I.y - 1.0/3 * speedy * step_now * step_now * step_now + 0.5 * speedy * step_total * step_now * step_now;
    return NOW;
}

