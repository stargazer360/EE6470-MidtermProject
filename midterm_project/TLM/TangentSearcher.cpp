#include "TangentSearcher.h"
#include <iomanip>

TangentSearcher::TangentSearcher(sc_module_name n) : sc_module(n), t_skt("t_skt"), base_offset(0) {
    SC_THREAD(search_tangent);

    t_skt.register_b_transport(this, &TangentSearcher::blocking_transport);
}

TangentSearcher::~TangentSearcher() = default;

void TangentSearcher::search_tangent() {
    while (true) {
        for (int i = 0; i < MAX_PROCESS; ++i) {
            points = i_points.read(); 
            p[i] = points.range(POINT_P);
            q_pre[i] = points.range(POINT_Q_PRE);
            q_cur[i] = points.range(POINT_Q_CUR);
            q_nxt[i] = points.range(POINT_Q_NXT);
            
        }
        for (int i = 0; i < MAX_PROCESS; ++i) {
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
        sc_dt::sc_uint<MAX_PROCESS_WIDTH> l_idx = MAX_PROCESS, r_idx= 1;
        for (int i = 0; i < MAX_PROCESS; ++i) {
            l_idx -= (sc_dt::sc_uint<MAX_PROCESS_WIDTH>)is_last_above[i];
            r_idx += (sc_dt::sc_uint<MAX_PROCESS_WIDTH>)is_next_above[i];
        }
        sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> indices;
        indices.range(L_IDX) = l_idx;
        indices.range(R_IDX) = r_idx;
		o_indices.write(indices);
    }
}

void TangentSearcher::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    unsigned char buffer[SCALE*8/8];
    sc_dt::sc_uint<MAX_PROCESS_WIDTH*2> indices;
    sc_dt::sc_bigint<SCALE*8> points;
    unsigned int tmp;
    switch (payload.get_command()) {
    case tlm::TLM_READ_COMMAND:
        switch (addr) {
        case TANGENT_RESULT_ADDR:
            indices = o_indices.read();
            buffer[0] = indices.range(L_IDX);
            buffer[1] = indices.range(R_IDX);
        break;
        case TANGENT_CHECK_ADDR:
            tmp = o_indices.num_available();
            buffer[0] = (unsigned char)(tmp & 0x000000ff);
            buffer[1] = (unsigned char)((tmp & 0x0000ff00) >> 8);
            buffer[2] = (unsigned char)((tmp & 0x00ff0000) >> 16);
            buffer[3] = (unsigned char)((tmp & 0xff000000) >> 24);
        break;
        default:
        std::cerr << "Error! TangentSearcher::blocking_transport: address 0x"
                    << std::setfill('0') << std::setw(8) << std::hex << addr
                    << std::dec << " is not valid" << std::endl;
                    exit(-1);
        }
        data_ptr[0] = buffer[0];
        data_ptr[1] = buffer[1];
        data_ptr[2] = buffer[2];
        data_ptr[3] = buffer[3];
        break;
    case tlm::TLM_WRITE_COMMAND:
        switch (addr) {
        case TANGENT_R_ADDR:
            for (int i = 0; i < SCALE*8/8; ++i) {
                if (mask_ptr[i] == 0xff) {
                    points.range((i+1)*8-1, (i)*8) = data_ptr[i];
                }
            }
            i_points.write(points);

        break;
        default:
            std::cerr << "Error! TangentSearcher::blocking_transport: address 0x"
                        << std::setfill('0') << std::setw(8) << std::hex << addr
                        << std::dec << " is not valid" << std::endl;
        }
        break;
    case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}
