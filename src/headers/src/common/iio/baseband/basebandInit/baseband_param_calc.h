/*
 * @Descripttion: 
 * @version:  
 * @author: CHEN LEI
 * @Date: 2020-04-19 06:00:49
 * @LastEditors: CHEN LEI
 * @LastEditTime: 2020-08-21 11:47:37
 */



#ifndef _baseband_param_calc_h_
#define _baseband_param_calc_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct uv_rx_para_config
{
    uint32_t trk_inte_select;
    uint32_t sys_lpf_select;
    uint32_t trk_inte_num;
    uint8_t  trk_bsyn_buf_num;
    uint8_t  trk_bsyn_dot_num;
    uint32_t trk_inte_para_p;
    uint32_t trk_step_1_para_c0;
    uint32_t trk_step_1_para_c1;
    uint32_t trk_step_1_para_c2;
    uint32_t trk_loop_inibsyn_num;
    uint32_t trk_loop_iniedge_num;
    uint32_t trk_loop_endedge_num;
    uint32_t trk_loop_instant_num;
    uint32_t trk_loop_lostclr_num;
    uint32_t trk_bsyn_para_num;
    uint32_t trk_dete_phase_door;
    uint32_t trk_lost_debug_door;
    uint8_t  nrz_decode_pattern;
};   

void uv_baseband_para_calc(struct uv_rx_para_config *uv_para,double fb,double fs,double npb);

 
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //  
