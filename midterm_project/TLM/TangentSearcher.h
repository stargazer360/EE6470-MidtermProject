#ifndef TANGENT_SEARCHER_H_
#define TANGENT_SEARCHER_H_
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "Utility.h"

class TangentSearcher : public sc_module {
public:
    tlm_utils::simple_target_socket<TangentSearcher> t_skt;
    sc_fifo< sc_dt::sc_bigint<SCALE*8> > i_points;
    sc_fifo< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> > o_indices;

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

    unsigned int base_offset;
    void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif