#include <iostream>
#include <string>
using namespace std;

#ifndef NATIVE_SYSTEMC
#include "esc.h"
#endif

// Wall Clock Time Measurement
#include <sys/time.h>

#include "System.h"

struct timeval start_time, end_time;

System * sys = NULL;

#ifndef NATIVE_SYSTEMC
extern void esc_elaborate()
{
	sys = new System("sys", esc_argv(1), esc_argv(2));
}
extern void esc_cleanup()
{
	delete sys;
}
#endif

int sc_main(int argc, char **argv) {
    if ((argc < 3) || (argc > 4)) {
        cout << "No arguments for the executable : " << argv[0] << endl;
        cout << "Usage : >" << argv[0] << "data_file_name result_file_name"
         << endl;
        return 0;
    }

#ifndef NATIVE_SYSTEMC
	esc_initialize(argc, argv);
#endif

#ifndef NATIVE_SYSTEMC
	esc_elaborate();
#else
	sys = new System("sys", argv[1], argv[2]);
#endif
    sc_start();
#ifndef NATIVE_SYSTEMC
	esc_cleanup();
#else
	delete sys;
#endif

    std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
    return 0;
}
