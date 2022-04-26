#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "TangentSearcher_wrap.h"
#else
#include "TangentSearcher.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string in_file, std::string out_file);
	~System();
private:
  	Testbench tb;
#ifndef NATIVE_SYSTEMC
	TangentSearcher_wrapper tangent_searcher;
#else
	TangentSearcher tangent_searcher;
#endif
	sc_clock clk;
	sc_signal<bool> rst;

#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<SCALE*8> > points;
    cynw_p2p< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> > indices;
#else
    sc_fifo< sc_dt::sc_bigint<SCALE*8> > points;
    sc_fifo< sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> > indices;
#endif
    std::string out_file;
};
#endif
