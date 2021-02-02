
#ifndef _uv_config_H_
#define _uv_config_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

#include "baseband_param_calc.h"
#include "iio.h"

#define _DEBUG_ 1


// #define ASSERT(expr) { \
// 	if (!(expr)) { \
// 		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
// 		(void) abort(); \
// 	} \
// }

// #define  txdevice_name  "iio:device4"
// #define  rxdevice_name  "iio:device3"


#if _DEBUG_
#define MSG(...) printf(__VA_ARGS__)
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define MSG(...)
#define DEBUG(...)  
#endif




// /* helper macros */
// #define MHZ(x) ((long long)(x*1000000.0 + .5))
// #define GHZ(x) ((long long)(x*1000000000.0 + .5))

// /* RX is input, TX is output */
// enum iodev { RX, TX };

/* common RX and TX streaming params */
typedef struct stream_cfg {
	long long bw_hz; // Analog banwidth in Hz
	long long fs_hz; // Baseband sample rate in Hz
	long long lo_hz; // Local oscillator frequency in Hz
    long long tx_gain; // Local oscillator frequency in Hz
	const char* rfport; // Port name
};


typedef struct uv_config_state{
    struct stream_cfg txcfg;
    uint8_t modulation_mode;
    uint8_t rx_en;
    uint8_t chan_en;
    uint8_t info_en;
    uint8_t tx_mode;//0：psk 1:fsk
    uint8_t info_source;
    uint8_t info_fill_byte;
    uint8_t info_random_encode;
    uint8_t carr_gen_en;
    uint32_t carr_cen_pinc;
    uint32_t info_data_rate;
    uint32_t info_doppler_coff;
    uint32_t info_fifo_thresh;
    uint64_t doppler1_pinc;
    uint64_t doppler2_pinc;
    uint32_t doppler3_pinc;
    uint8_t  doppler_mode_sel;
    uint32_t fsk_offset_freq;
    uint32_t tx_conv_code;
    uint16_t db;
    uint8_t  tx_nrz_code_pattern;
    //接收参数
    struct stream_cfg rxcfg;
    struct uv_rx_para_config uv_rx;
    uint8_t  rx_nrz_code_pattern;
    uint8_t  info_random_decode;
    uint32_t rx_conv_code;
    uint32_t rx_info_rate;
    uint32_t rx_frame_sync_word;
    uint32_t rx_frame_sync_len;
    uint32_t rx_preamble_byte;
    uint32_t rx_preamble_length;
}uv_configT;


typedef struct uv_pl_state_info{
    uint8_t tx_carr_gen_en;
    uint8_t tx_rst;
    uint8_t rx_rst;
    uint8_t tx_info_en;
    uint8_t tx_chan_en;

    uint32_t tx_carr_cen_pinc;
    uint32_t tx_info_data_rate;
    uint32_t tx_info_doppler_coff;
    uint32_t tx_info_fifo_thresh;
    uint64_t tx_doppler1;
    uint64_t tx_doppler2;
    uint64_t tx_doppler3;
    uint32_t tx_doppler_mode;
    
    uint32_t rx_inte_select;
    uint32_t rx_sys_lpf;
    uint32_t rx_trk_inte_num;
    uint32_t rx_trk_bsyn_buf_num;
    uint32_t rx_trk_bsyn_dot_num;
    uint32_t rx_trk_inte_para;
    uint32_t rx_trk_step_para_c0;
    uint32_t rx_trk_step_para_c1;
    uint32_t rx_trk_step_para_c2;
    uint32_t rx_trk_loop_inibsyn_num;
    uint32_t rx_trk_loop_iniedge_num;
    uint32_t rx_trk_loop_endedge_num;
    uint32_t rx_trk_loop_instant_num;
    uint32_t rx_trk_loop_lostclr_num;
    uint32_t rx_trk_bsyn_para_num;
    uint32_t rx_trk_dete_phase_door;
    uint32_t rx_trk_lost_debug_door;
    uint32_t rx_nrz_code_pattern;
    uint32_t feedback_info_fifo;
    uint32_t feedback_sys_state;
    uint32_t feedback_carr_feq_pinc;
}uv_pl_info;

extern struct uv_config_state uv_para_config;

#define UV_FRMAE_HEADER_LEN             2
#define UV_FRAME_LENGTH_LEN             2
#define UV_FRAME_ID_LEN                 2
#define UV_FRAME_ECC_LEN                1

#define UV_FRAME_CONFIG_ID              0x42
#define UV_FRAME_CONFIG_DATA_LENGTH     50

#define UV_FRAME_TC_DATA_ID             0x43
#define UV_FRAME_TC_DATA_LENGTH         512 

#define UV_FRAME_RF_CONFIG_ID           0x45
#define UV_FRAME_RF_CONFIG_LENGTH       15

#define UV_FRAME_CTRL_CONFIG_ID         0x40
#define UV_FRAME_CTRL_CONFIG_LENGTH     8



uint32_t config_feedback_fifo_status(void);
void uv_configure_init_thread(void);
void uv_configure_join_thread(void);
void uv_conifg_msg_handle(char *buf, int len);

// bool cfg_ad9361_streaming_ch(struct iio_context *ctx, struct stream_cfg *cfg, enum iodev type, int chid);
// bool get_lo_chan(struct iio_context *ctx, enum iodev d, struct iio_channel **chn);
// bool get_phy_chan(struct iio_context *ctx, enum iodev d, int chid, struct iio_channel **chn);
// bool get_ad9361_stream_ch(struct iio_context *ctx, enum iodev d, struct iio_device *dev, int chid, struct iio_channel **chn);
// bool get_ad9361_stream_dev(struct iio_context *ctx, enum iodev d, struct iio_device **dev);
// struct iio_device* get_ad9361_phy(struct iio_context *ctx);

u_int8_t uv_gpio985_read(void);
u_int8_t uv_gpio986_read(void);




#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //  




