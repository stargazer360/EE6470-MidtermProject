#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <cmath>

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n) {
    SC_THREAD(find_tangent);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();
}

Testbench::~Testbench(){}

void Testbench::find_tangent() {
#ifndef NATIVE_SYSTEMC
    o_points.reset();
#endif
    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);
    sc_dt::sc_int<SCALE*2> _p, _q_pre, _q_cur, _q_nxt;
    
    _p.range(X_VAL) = p[0].x;
    _p.range(Y_VAL) = p[0].y;
    while (low != high) {
        wait(index_updated);    // wait for low and high being set up by fetch_result
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
#ifndef NATIVE_SYSTEMC
            o_points.put(points);
#else
            o_points.write(points);
#endif        
            cut += step;
        }
    }
}

void Testbench::fetch_result() {
#ifndef NATIVE_SYSTEMC
    i_indices.reset();
#endif
    wait(5);
    wait(1);
    // reset low and high
    low = 0, high = NUM_OF_Q-1;
    index_updated.notify(SC_ZERO_TIME);
    sc_time last_out = sc_time_stamp();
    while (low != high) {
        int step = ceil((float)(high - low) / (MAX_PROCESS + 1));
        sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> indices;
#ifndef NATIVE_SYSTEMC
        indices = i_indices.get();
#else
        indices = i_indices.read();
#endif
        std::cout << sc_time_stamp() - last_out << std::endl;
        last_out = sc_time_stamp();
        int tmp = low + indices.range(R_IDX) * step;
        low = low + indices.range(L_IDX) * step;
        if (tmp == low) high = tmp;
        else if (tmp <= high) high = tmp - 1;
        //std::cout << low << " " << high << std::endl;
        index_updated.notify(SC_ZERO_TIME);
    }
    q_tan_idx[0] = low;
    std::cout << q_tan_idx[0];
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
