#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;
#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

#include "Utility.h"

class Testbench : public sc_module {
public:
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<SCALE*8> >::base_out o_points;
    cynw_p2p< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> >::base_in i_indices;
#else
    sc_fifo_out< sc_dt::sc_bigint<SCALE*8> > o_points;
    sc_fifo_in< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> > i_indices;
#endif

    SC_HAS_PROCESS(Testbench);

    Testbench(sc_module_name n);
    ~Testbench();

    void read_data(string filename);
    void output_result(string filename);

private:
    Point p[NUM_OF_P], q[NUM_OF_Q];
    int q_tan_idx[NUM_OF_P];
    int low, high;  
    sc_event index_updated;

    void find_tangent(); // find upper common tangent
    void fetch_result();
};
#endif

