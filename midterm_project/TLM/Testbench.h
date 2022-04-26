#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;
#include <systemc>
using namespace sc_core;

#include "Utility.h"

#include "Initiator.h"
#include "MemoryMap.h"

class Testbench : public sc_module {
public:
    Initiator initiator;
    SC_HAS_PROCESS(Testbench);

    Testbench(sc_module_name n);
    ~Testbench();

    void read_data(string filename);
    void output_result(string filename);

private:
    Point p[NUM_OF_P], q[NUM_OF_Q];
    int q_tan_idx[NUM_OF_P];
    int low, high;

    void find_tangent(); // find upper common tangent
};
#endif

