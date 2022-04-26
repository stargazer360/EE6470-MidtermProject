#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <cmath>

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n), initiator("initiator") {
    SC_THREAD(find_tangent);
}

Testbench::~Testbench(){}

void Testbench::find_tangent() {
    sc_dt::sc_int<SCALE*2> _p, _q_pre, _q_cur, _q_nxt;
    
    _p.range(X_VAL) = p[0].x;
    _p.range(Y_VAL) = p[0].y;
    low = 0, high = NUM_OF_Q-1;
    unsigned char data[SCALE*8/8];
    unsigned char mask[SCALE*8/8];
    while (low != high) {
        int step = ceil((float)(high - low) / (MAX_PROCESS + 1));
        int cut = low + step;
        for (int i = 0; i < MAX_PROCESS; ++i) {
            if (cut == low) { // send NOT_A_POINT to make previous always below
                    _q_pre.range(X_VAL) = q[low].x;
                    _q_pre.range(Y_VAL) = NOT_A_POINT;
                    _q_cur.range(X_VAL) = q[low].x;
                    _q_cur.range(Y_VAL) = q[low].y;
                    _q_nxt.range(X_VAL) = q[low+1].x;
                    _q_nxt.range(Y_VAL) = q[low+1].y;
                    
            } else if (cut < high){
                    _q_pre.range(X_VAL) = q[cut-1].x;
                    _q_pre.range(Y_VAL) = q[cut-1].y;
                    _q_cur.range(X_VAL) = q[cut].x;
                    _q_cur.range(Y_VAL) = q[cut].y;
                    _q_nxt.range(X_VAL) = q[cut+1].x;
                    _q_nxt.range(Y_VAL) = q[cut+1].y;
            } else if (cut == high){  // send NOT_A_POINT to make next always below
                    _q_pre.range(X_VAL) = q[high-1].x;
                    _q_pre.range(Y_VAL) = q[high-1].y;
                    _q_cur.range(X_VAL) = q[high].x;
                    _q_cur.range(Y_VAL) = q[high].y;
                    _q_nxt.range(X_VAL) = q[high].x;
                    _q_nxt.range(Y_VAL) = NOT_A_POINT;
            } else {  // send NOT_A_POINT to make next always below 
                    _q_pre.range(X_VAL) = q[high].x;
                    _q_pre.range(Y_VAL) = q[high].y;
                    _q_cur.range(X_VAL) = q[high].x;
                    _q_cur.range(Y_VAL) = q[high].y;
                    _q_nxt.range(X_VAL) = q[high].x;
                    _q_nxt.range(Y_VAL) = NOT_A_POINT;
                }
            sc_dt::sc_bigint<SCALE*8> points;
            points.range(POINT_P) = _p;
            points.range(POINT_Q_PRE) = _q_pre;
            points.range(POINT_Q_CUR) = _q_cur;
            points.range(POINT_Q_NXT) = _q_nxt;
            for (int j = 0; j < SCALE*8/8; ++j) {
                sc_dt::sc_int<8> tmp;
                tmp = points.range((j+1)*8-1, (j)*8);
                data[j] = (unsigned char)tmp;
                mask[j] = 0xff;
            }
            initiator.write_to_socket(TANGENT_MM_BASE + TANGENT_R_ADDR, mask, data, SCALE*8/8);
            cut += step;
        }
        bool done = false;
        int output_num = 0;
        while (!done) {
            initiator.read_from_socket(TANGENT_MM_BASE + TANGENT_CHECK_ADDR, mask, data, 4);
            unsigned int tmp = 0;
            tmp |= (uint32_t)data[0];
            tmp |= (uint32_t)data[1] << 8;
            tmp |= (uint32_t)data[2] << 16;
            tmp |= (uint32_t)data[3] << 24;
            output_num = (int)tmp;
            if (output_num > 0) {
                done = true;
            }
        }
        wait(110 * CLOCK_PERIOD, SC_NS);
        initiator.read_from_socket(TANGENT_MM_BASE + TANGENT_RESULT_ADDR, mask, data, 2);
        sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> indices;
        indices.range(L_IDX) = data[0];
        indices.range(R_IDX) = data[1];
        int tmp = low + indices.range(R_IDX) * step;
        low = low + indices.range(L_IDX) * step;
        if (tmp == low) high = tmp;
        else if (tmp <= high) high = tmp - 1;
    }

    q_tan_idx[0] = low;
    sc_stop();
}

void Testbench::read_data(string filename) {
    std::fstream in_file;
    in_file.open(filename.c_str(), std::fstream::binary | std::fstream::in);
    if (in_file.is_open()) {
        for (int i = 0; i < NUM_OF_P; ++i) {
            in_file >> p[i].x >> p[i].y;
        }
        for (int i = 0; i < NUM_OF_Q; ++i) {
            in_file >> q[i].x >> q[i].y;
        } 
        in_file.close();
    } else {
        std::cout << "Fail to open: " << filename;
        exit(-1);
    }
}

void Testbench::output_result(string filename) {
    std::fstream out_file;
    std::cout << filename << std::endl;
    out_file.open(filename.c_str(), std::fstream::binary | std::fstream::out);
    if (out_file.is_open()) {
        for (int i = 0; i < NUM_OF_P; ++i) {
            out_file << q_tan_idx[i] << ",\n"[i==NUM_OF_P-1];
        }
        out_file.close();
    } else {
        std::cout << "Fail to open: " << filename;
    }
}
