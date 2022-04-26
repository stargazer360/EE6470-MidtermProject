#ifndef TANGENT_SEARCHER_H_
#define TANGENT_SEARCHER_H_
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif
#include "Utility.h"

class TangentSearcher : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<SCALE*8> >::in i_points;
    cynw_p2p< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> >::out o_indices;
#else
    sc_fifo_in< sc_dt::sc_bigint<SCALE*8> > i_points;
    sc_fifo_out< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> > o_indices;
#endif

    SC_HAS_PROCESS(TangentSearcher);
    TangentSearcher(sc_module_name n);
    ~TangentSearcher();
private:
    void search_tangent();
    sc_dt::sc_int<SCALE*2> p[MAX_PROCESS];
    sc_dt::sc_int<SCALE*2> q_pre[MAX_PROCESS];
    sc_dt::sc_int<SCALE*2> q_cur[MAX_PROCESS];
    sc_dt::sc_int<SCALE*2> q_nxt[MAX_PROCESS];
    sc_dt::sc_bigint<SCALE*8> points;
    sc_dt::sc_uint<1> is_last_above[MAX_PROCESS], is_next_above[MAX_PROCESS];
};
#endif