#ifndef UTILITY_H_
#define UTILITY_H_
#include <iostream>
#include <systemc>

#define MAX_PROCESS 4
#define MAX_PROCESS_WIDTH 8 // ceil(log2(MAX_PROCESS)), use constexpr if c++11 availible
#define SCALE 16
#define NOT_A_POINT 0x8000
#define NUM_OF_P 1
#define NUM_OF_Q 256
#define P_RAND_SEED 9527
#define Q_RAND_SEED 8888

#define X_VAL SCALE*1-1, SCALE*0
#define Y_VAL SCALE*2-1, SCALE*1
#define POINT_P     SCALE*2-1, SCALE*0
#define POINT_Q_PRE SCALE*4-1, SCALE*2
#define POINT_Q_CUR SCALE*6-1, SCALE*4
#define POINT_Q_NXT SCALE*8-1, SCALE*6

#define L_IDX MAX_PROCESS_WIDTH*1-1, MAX_PROCESS_WIDTH*0
#define R_IDX MAX_PROCESS_WIDTH*2-1, MAX_PROCESS_WIDTH*1


struct Point
{
    sc_dt::sc_int<SCALE> x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    Point& operator= (const Point& rhs) {
        x = rhs.x;
        y = rhs.y;
        return *this;
    }
    bool operator==(const Point& rhs) {
        return x ==rhs.x && y == rhs.y;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point& val);
};

#endif
