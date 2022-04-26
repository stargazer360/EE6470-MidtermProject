#include "TangentSearcher.h"

#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

TangentSearcher::TangentSearcher(sc_module_name n) : sc_module(n) {
#ifndef NATIVE_SYSTEMC
    HLS_FLATTEN_ARRAY(is_last_above);
    HLS_FLATTEN_ARRAY(is_next_above);    
    HLS_FLATTEN_ARRAY(p);    
    HLS_FLATTEN_ARRAY(q_pre);    
    HLS_FLATTEN_ARRAY(q_cur);
    HLS_FLATTEN_ARRAY(q_nxt);
#endif
    SC_THREAD(search_tangent);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
    i_points.clk_rst(i_clk, i_rst);
    o_indices.clk_rst(i_clk, i_rst);
#endif
}

TangentSearcher::~TangentSearcher() {}

void TangentSearcher::search_tangent() {
    {
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_points.reset();
        o_indices.reset();
#endif
        wait();
    }
    while (true) {
        for (int i = 0; i < MAX_PROCESS; ++i) {
           	HLS_PIPELINE_LOOP(HARD_STALL, 1, "Input pipeline");
            {
#ifndef NATIVE_SYSTEMC
			    HLS_DEFINE_PROTOCOL("input");
                
                points = i_points.get();
                wait();          
#else
                points = i_points.read(); 
#endif
            }
            p[i] = points.range(POINT_P);
            q_pre[i] = points.range(POINT_Q_PRE);
            q_cur[i] = points.range(POINT_Q_CUR);
            q_nxt[i] = points.range(POINT_Q_NXT);
        }
        for (int i = 0; i < MAX_PROCESS; ++i) {
            {
                #ifndef NATIVE_SYSTEMC
                HLS_UNROLL_LOOP(ON, "check tangent loop");
                HLS_CONSTRAIN_LATENCY(0, 0, "check tangent");
                #endif
                sc_dt::sc_int<SCALE> diff_x = q_cur[i].range(X_VAL) - p[i].range(X_VAL);
                sc_dt::sc_int<SCALE> diff_y = q_cur[i].range(Y_VAL) - p[i].range(Y_VAL);
                is_last_above[i] = ((q_pre[i].range(Y_VAL) != NOT_A_POINT) &&
                                    ((sc_dt::sc_int<SCALE*2>)(q_pre[i].range(Y_VAL) * diff_x) >= (
                                     (sc_dt::sc_int<SCALE*2>)(diff_x * p[i].range(Y_VAL)) + 
                                     (sc_dt::sc_int<SCALE*2>)(diff_y * (q_pre[i].range(X_VAL) - p[i].range(X_VAL))))));
                is_next_above[i] = (((q_nxt[i].range(Y_VAL) != NOT_A_POINT) &&
                                    ((sc_dt::sc_int<SCALE*2>)(q_nxt[i].range(Y_VAL) * diff_x) >=
                                    (sc_dt::sc_int<SCALE*2>)(diff_x * p[i].range(Y_VAL)) + 
                                    (sc_dt::sc_int<SCALE*2>)(diff_y * (q_nxt[i].range(X_VAL) - p[i].range(X_VAL))))));
            }
        }
        sc_dt::sc_uint<MAX_PROCESS_WIDTH> l_idx = MAX_PROCESS, r_idx= 1;
        for (int i = 0; i < MAX_PROCESS; ++i) {
            #ifndef NATIVE_SYSTEMC
            HLS_UNROLL_LOOP(ON, "find region loop");
            HLS_CONSTRAIN_LATENCY(0, 1, "find region");
            #endif
            l_idx -= (sc_dt::sc_uint<MAX_PROCESS_WIDTH>)is_last_above[i];
            r_idx += (sc_dt::sc_uint<MAX_PROCESS_WIDTH>)is_next_above[i];
        }
        sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> indices;
        indices.range(L_IDX) = l_idx;
        indices.range(R_IDX) = r_idx;
#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
            o_indices.put(indices);
		}
#else
		o_indices.write(indices);
#endif
    }
}


