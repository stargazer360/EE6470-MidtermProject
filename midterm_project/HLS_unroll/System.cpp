#include "System.h"
System::System( sc_module_name n, string in_file, string out_file): sc_module( n ), 
	tb("tb"), tangent_searcher("tangent_searcher"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), out_file(out_file)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	tangent_searcher.i_clk(clk);
	tangent_searcher.i_rst(rst);
	tb.o_points(points);
	tb.i_indices(indices);
	tangent_searcher.i_points(points);
	tangent_searcher.o_indices(indices);
  	tb.read_data(in_file);
}

System::~System() {

	tb.output_result(out_file);
}
