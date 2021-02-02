#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


// #include "net_queue.h"
#include "uv_config.h"
// #include "def.h"
// #include "uv_trx_thread.h"

static pthread_t  uv_config_thread;
struct uv_config_state uv_para_config;
struct uv_pl_state_info uv_pl_state;
 

#define dev_mem "/dev/mem"

#define SAMP_FREQ (double)10e6
#define CEN_FREQ  100e3  //1e6
#define BITRATE   100e3//1.2e3//115.2e3//1.2e3 //4096 50e3//

#define npb       (double)(0.5)

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE -1)
#define MAP_ADDR_TX 0x40000000
#define MAP_ADDR_RX 0x42000000
 
#define BRAM_TX_TYPE_REG            MAP_ADDR_TX + 0x0004    

//控制信号
#define BRAM_CONTROL_REG            MAP_ADDR_TX + 0x0008
#define BRAM_CONTROL_REG1           MAP_ADDR_TX + 0x000C


//发送模块
#define BRAM_CARRY_FREQ_REG         MAP_ADDR_TX + 0x0008
#define BRAM_INFO_RATE_REG          MAP_ADDR_TX + 0x000c
#define BRAM_INFO_DOPPLER_REG       MAP_ADDR_TX + 0x0010
#define BRAM_DOPPLER1_PINC_REG      MAP_ADDR_TX + 0x0014
#define BRAM_DOPPLER2_PINC_REG      MAP_ADDR_TX + 0x001c
#define BRAM_DOPPLER3_PINC_REG      MAP_ADDR_TX + 0x0024
#define BRAM_DOPPLER_MODE_SEL_REG   MAP_ADDR_TX + 0x002c
#define BRAM_INFO_THRESH_REG        MAP_ADDR_TX + 0x0030



#define BRAM_UPDATA_REG             MAP_ADDR_TX + 0x0004
#define BRAM_ENA_ADDR_REG           MAP_ADDR_TX + 0x0008
// #define BRAM_CARRY_FREQ_REG         MAP_ADDR_TX + 0x000C
// #define BRAM_INFO_RATE_REG          MAP_ADDR_TX + 0x0010
// #define BRAM_INFO_DOPPLER_REG       MAP_ADDR_TX + 0x0014
// #define BRAM_INFO_THRESH_REG        MAP_ADDR_TX + 0x0018
// #define BRAM_DOPPLER1_PINC_REG      MAP_ADDR_TX + 0x001C
// #define BRAM_DOPPLER2_PINC_REG      MAP_ADDR_TX + 0x0024
// #define BRAM_DOPPLER3_PINC_REG      MAP_ADDR_TX + 0x002C
// #define BRAM_DOPPLER_MODE_SEL_REG   MAP_ADDR_TX + 0x0034
#define BRAM_FSK_FREQ_OFFSET_REG    MAP_ADDR_TX + 0x0038

//接收模块
#define BRAM_TRK_INTE_SELECT_REG       MAP_ADDR_TX + 0x0008 //低通滤波器选择信号
#define BRAM_TRK_INTE_NUM_REG          MAP_ADDR_TX + 0x000c //相干积分采样点数nco
#define BRAM_TRK_BSYN_BUF_NUM_REG      MAP_ADDR_TX + 0x0010 //位同步调整周期
#define BRAM_TRK_INTE_PARA_P_REG       MAP_ADDR_TX + 0x0014 //鉴相输出相乘归一化参数
#define BRAM_TRK_STEP_1_PARA_C0_REG    MAP_ADDR_TX + 0x0018 //环路滤波器参数
#define BRAM_TRK_STEP_1_PARA_C1_REG    MAP_ADDR_TX + 0x001c //环路滤波器参数
#define BRAM_TRK_STEP_1_PARA_C2_REG    MAP_ADDR_TX + 0x0020 //环路滤波器参数
#define BRAM_TRK_LOOP_INIBSYN_NUM_REG  MAP_ADDR_TX + 0x0024 //位同步启动时刻
#define BRAM_TRK_LOOP_INIEDGE_NUM_REG  MAP_ADDR_TX + 0x0028 //位同步边沿搜索启动时刻
#define BRAM_TRK_LOOP_ENDEDGE_NUM_REG  MAP_ADDR_TX + 0x002c //位同步边沿搜索完成时刻
#define BRAM_TRK_LOOP_INSTANT_NUM_REG  MAP_ADDR_TX + 0x0030 //环路稳定时刻
#define BRAM_TRK_LOOP_LOSTCTRL_NUM_REG MAP_ADDR_TX + 0x0034 //环路失锁判断看门狗时间
#define BRAM_TRK_BSYN_PARA_NUM_REG     MAP_ADDR_TX + 0x0038 //位同步调整量
#define BRAM_TRK_DETE_PAHSE_DOOR_REG   MAP_ADDR_TX + 0x003c //鉴相门限
#define BRAM_TRK_LOST_DEBUG_DOOR_REG   MAP_ADDR_TX + 0x0040 //环路失锁预设门限

// #define BRAM_TRK_INTE_SELECT_REG       MAP_ADDR_TX + 0x003C //低通滤波器选择信号
#define BRAM_TRK_SYS_LPF_SELECT_REG    MAP_ADDR_TX + 0x0040 //混频、相干积分位宽信号
// #define BRAM_TRK_INTE_NUM_REG          MAP_ADDR_TX + 0x0044 //相干积分采样点数nco
// #define BRAM_TRK_BSYN_BUF_NUM_REG      MAP_ADDR_TX + 0x0048 //位同步调整周期
#define BRAM_TRK_BSYN_DOT_NUM_REG      MAP_ADDR_TX + 0x004c //符号周期内积分点数
// #define BRAM_TRK_INTE_PARA_P_REG       MAP_ADDR_TX + 0x0050 //鉴相输出相乘归一化参数
// #define BRAM_TRK_STEP_1_PARA_C0_REG    MAP_ADDR_TX + 0x0054 //环路滤波器参数
// #define BRAM_TRK_STEP_1_PARA_C1_REG    MAP_ADDR_TX + 0x0058 //环路滤波器参数
// #define BRAM_TRK_STEP_1_PARA_C2_REG    MAP_ADDR_TX + 0x005c //环路滤波器参数
// #define BRAM_TRK_LOOP_INIBSYN_NUM_REG  MAP_ADDR_TX + 0x0060 //位同步启动时刻
// #define BRAM_TRK_LOOP_INIEDGE_NUM_REG  MAP_ADDR_TX + 0x0064 //位同步边沿搜索启动时刻
// #define BRAM_TRK_LOOP_ENDEDGE_NUM_REG  MAP_ADDR_TX + 0x0068 //位同步边沿搜索完成时刻
// #define BRAM_TRK_LOOP_INSTANT_NUM_REG  MAP_ADDR_TX + 0x006c //环路稳定时刻
// #define BRAM_TRK_LOOP_LOSTCTRL_NUM_REG MAP_ADDR_TX + 0x0070 //环路失锁判断看门狗时间
// #define BRAM_TRK_BSYN_PARA_NUM_REG     MAP_ADDR_TX + 0x0074 //位同步调整量
// #define BRAM_TRK_DETE_PAHSE_DOOR_REG   MAP_ADDR_TX + 0x0078 //鉴相门限
// #define BRAM_TRK_LOST_DEBUG_DOOR_REG   MAP_ADDR_TX + 0x007c //环路失锁预设门限
#define BRAM_NRZ_CODE_PATTERN_REG      MAP_ADDR_TX + 0x0080 //NRZ编码类型

#define BRAM_FB_FIFO_STATUS_REG     MAP_ADDR_TX + 0x013C
#define BRAM_FB_CARRY_FREQ_REG      MAP_ADDR_TX + 0x0100
#define BRAM_FB_INFO_FIFO_REG       MAP_ADDR_TX + 0x0104
#define BRAM_FB_UV_VERSION1_REG     MAP_ADDR_TX + 0x0108
#define BRAM_FB_UV_VERSION2_REG     MAP_ADDR_TX + 0x010C
#define BRAM_FB_UV_VERSION3_REG     MAP_ADDR_TX + 0x0FFC

//bram_rd
#define BRAM_RD_PARA_UPD_REG        MAP_ADDR_RX + 0x0004
#define BRAM_RD_CONTROL_REG         MAP_ADDR_RX + 0x0008
#define BRAM_RD_CARR_CEN_REG        MAP_ADDR_RX + 0x000c
#define BRAM_RD_DATA_RATE_REG       MAP_ADDR_RX + 0x0010
#define BRAM_RD_DOPPLER_COFF_REG    MAP_ADDR_RX + 0x0014
#define BRAM_RD_FIFO_THRESH_REG     MAP_ADDR_RX + 0x0018
#define BRAM_RD_DOPPLER1_REG        MAP_ADDR_RX + 0x001c
#define BRAM_RD_DOPPLER2_REG        MAP_ADDR_RX + 0x0024
#define BRAM_RD_DOPPLER3_REG        MAP_ADDR_RX + 0x002c
#define BRAM_RD_DOPPLER_mode_REG    MAP_ADDR_RX + 0x0034
#define BRAM_RD_FSK_FREQ_REG        MAP_ADDR_RX + 0x0038
#define BRAM_RD_TRK_INTE_REG        MAP_ADDR_RX + 0x003c
#define BRAM_RD_SYS_LPF_REG         MAP_ADDR_RX + 0x0040
#define BRAM_RD_INTE_NUM_REG        MAP_ADDR_RX + 0x0044
#define BRAM_RD_BSYN_BUF_REG        MAP_ADDR_RX + 0x0048
#define BRAM_RD_BSYN_DOT_REG        MAP_ADDR_RX + 0x004C
#define BRAM_RD_PARA_REG            MAP_ADDR_RX + 0x0050
#define BRAM_RD_PARA_c0_REG         MAP_ADDR_RX + 0x0054
#define BRAM_RD_PARA_c1_REG         MAP_ADDR_RX + 0x0058
#define BRAM_RD_PARA_c2_REG         MAP_ADDR_RX + 0x005C
#define BRAM_RD_LOOP_INIBSYN_REG    MAP_ADDR_RX + 0x0060
#define BRAM_RD_LOOP_INIEDGE_REG    MAP_ADDR_RX + 0x0064
#define BRAM_RD_LOOP_ENDEDGE_REG    MAP_ADDR_RX + 0x0068
#define BRAM_RD_LOOP_INSTANT_REG    MAP_ADDR_RX + 0x006C
#define BRAM_RD_LOOP_LOSTCLR_REG    MAP_ADDR_RX + 0x0070
#define BRAM_RD_BSYN_PARA_REG       MAP_ADDR_RX + 0x0074
#define BRAM_RD_DETE_PHASE_REG      MAP_ADDR_RX + 0x0078
#define BRAM_RD_LOST_DEBUG_REG      MAP_ADDR_RX + 0x007c
#define BRAM_RD_NRZ_CODE_REG        MAP_ADDR_RX + 0x0080
#define BRAM_RD_FEEDBACK_FIFO_REG   MAP_ADDR_RX + 0x0084
#define BRAM_RD_FEEDBACK_STATE_REG  MAP_ADDR_RX + 0x0088
#define BRAM_RD_FEEDBACK_CARR_REG   MAP_ADDR_RX + 0x008c




// /* static scratch mem for strings */
// static char tmpstr[64];

static uint32_t CONFIG_ENA_REG = 0;
static uint32_t CONFIG_IF_REG = 0;
static uint32_t CONFIG_INFO_RATE_REG = 0;
static uint32_t CONFIG_INFO_DOPPLER_COFF_REG = 0;
static uint32_t CONFIG_INFO_FIFO_THRESH_REG = 0;
static uint64_t CONFIG_DOPPLER1_PINC_REG = 0;
static uint64_t CONFIG_DOPPLER2_PINC_REG = 0;
static uint64_t CONFIG_DOPPLER3_PINC_REG = 0;
static uint32_t CONFIG_DOPPLER_MODE_SEL_REG = 0;
static uint32_t CONFIG_FSK_FREQ_OFFSET_REG = 0;
static uint32_t CONFIG_FB_FIFO_STATUS_REG = 0;

static uint32_t CONFIG_TRK_INTE_SELECT_REG = 0;     
static uint32_t CONFIG_TRK_SYS_LPF_SELECT_REG = 0;
static uint32_t CONFIG_TRK_INTE_NUM_REG = 0;
static uint32_t CONFIG_TRK_BSYN_BUF_NUM_REG = 0;
static uint32_t CONFIG_TRK_BSYN_DOT_NUM_REG = 0;
static uint32_t CONFIG_TRK_INTE_PARA_P_REG = 0;
static uint64_t CONFIG_TRK_STEP_1_PARA_C0_REG = 0;
static uint64_t CONFIG_TRK_STEP_1_PARA_C1_REG = 0;
static uint64_t CONFIG_TRK_STEP_1_PARA_C2_REG = 0;
static uint32_t CONFIG_TRK_LOOP_INIBSYN_NUM_REG = 0;
static uint32_t CONFIG_TRK_LOOP_INIEDGE_NUM_REG = 0;
static uint32_t CONFIG_TRK_LOOP_ENDEDGE_NUM_REG = 0;
static uint32_t CONFIG_TRK_LOOP_INSTANT_NUM_REG = 0;
static uint32_t CONFIG_TRK_LOOP_LOSTCTRL_NUM_REG = 0;
static uint32_t CONFIG_TRK_BSYN_PARA_NUM_REG = 0;
static uint32_t CONFIG_TRK_DETE_PHASE_DOOR_REG = 0;
static uint32_t CONFIG_TRK_LOST_DEBUG_DOOR_REG = 0;
static uint32_t CONFIG_TRK_NRZ_CODE_PATTERN_REG = 0;


enum SW_CONFIG{
    chan_sw = 0,
    info_sw ,
    rx_rst_sw ,
    tx_rst_sw ,
    fsk_psk_sw,
    carr_ena_sw
}sw_config;

enum BOOL_ONOFF{
    DISABLE = 0,
    ENABLE  = 1
 }bool_onoff;
 


static uint32_t BitConfig(uint32_t reg,uint32_t pos,uint32_t polarity)
{
    uint32_t reg_t = reg;
    if(polarity)
    {
        reg_t |= (1 << pos);
    }
    else 
    { 
        reg_t &=~ (1 << pos);
    }
    return reg_t;
}

static void mem_out32(uint32_t phyaddr , uint32_t val)
{
    int fd;
    volatile uint8_t *map_base;
    uint32_t pgoffset ;

    pgoffset = (phyaddr & (MAP_MASK));
    fd = open(dev_mem,O_RDWR | O_SYNC);

    if( -1 == fd){
     printf("%s,failure\n",dev_mem);
        return ;
    }

    map_base = mmap(NULL,MAP_SIZE,PROT_READ | PROT_WRITE ,MAP_SHARED,fd,MAP_ADDR_TX);

    if(map_base < 0) {
        printf("mmap failed \n");
        close(fd);
        return ;
    }

    *(volatile unsigned int *)(map_base + pgoffset) = val;
    munmap((void *)map_base,MAP_SIZE);
    close(fd);
}

static int mem_in32(uint32_t phyaddr)
{
    int fd;
    uint32_t val;
    void *map_base;
    uint32_t pgoffset = phyaddr & (MAP_MASK);

    fd = open(dev_mem,O_RDWR | O_SYNC);

    if( -1 == fd){
     printf("%s,failure\n",dev_mem);
        return -1;
    }

    map_base = mmap(NULL,MAP_SIZE,PROT_READ | PROT_WRITE ,MAP_SHARED,fd,MAP_ADDR_RX);

    if(map_base < 0) {
        printf("mmap failed \n");
        return -1;
    }
    val = *((volatile unsigned int *)(map_base + pgoffset));//PL反应不过来，需要连续读取2次
    // usleep(1);
    // val = *((volatile unsigned int *)(map_base + pgoffset));
    munmap(map_base,MAP_SIZE);
    close(fd);
    return val;
}

 
 uint32_t config_feedback_fifo_status(void)
{
    CONFIG_FB_FIFO_STATUS_REG = mem_in32(BRAM_FB_FIFO_STATUS_REG);
    return CONFIG_FB_FIFO_STATUS_REG;
}

static int frequency_convert_reg_value (float frequency , float sample_frequency)
{
    float c  ;
    c =  ((frequency/sample_frequency) * pow(2,32));
   return (int)c;
}

static void config_if_frequency_param(uint32_t frequency , uint32_t sample_frequency)
{
 
    CONFIG_IF_REG = frequency_convert_reg_value(frequency,sample_frequency);
    MSG("CONFIG_IF_REG:0x%x\n",CONFIG_IF_REG);
    mem_out32(BRAM_CARRY_FREQ_REG,CONFIG_IF_REG);
}

static void config_info_rate_param(uint32_t info_rate , uint32_t sample_frequency)
{
    CONFIG_INFO_RATE_REG = frequency_convert_reg_value(info_rate,sample_frequency);
    MSG("CONFIG_INFO_RATE_REG:0x%x\n",CONFIG_INFO_RATE_REG);
    mem_out32(BRAM_INFO_RATE_REG,CONFIG_INFO_RATE_REG);
}


static void config_info_fsk_frequency_offset(uint32_t freq_offset , uint32_t sample_frequency)
{
    CONFIG_FSK_FREQ_OFFSET_REG = frequency_convert_reg_value(freq_offset,sample_frequency);;
    mem_out32(BRAM_FSK_FREQ_OFFSET_REG,CONFIG_FSK_FREQ_OFFSET_REG);
}

static void config_switch_onoff(uint8_t opt , uint8_t onoff)
{
    CONFIG_ENA_REG = BitConfig(CONFIG_ENA_REG,opt,onoff);
    MSG("*  CONFIG_ENA_REG :0x%x\n",CONFIG_ENA_REG);
    mem_out32(BRAM_ENA_ADDR_REG,CONFIG_ENA_REG);
    return;
}


// static void config_update_param(uint8_t c)
// {
//     mem_out32(BRAM_UPDATA_REG,0x0); 
//     if(c == RX) //rx updata
//     {
//         mem_out32(BRAM_UPDATA_REG,0x00000002); 

//         MSG("*  rx param_update \n");
//     }
//     else  //tx updata
//     {
//         mem_out32(BRAM_UPDATA_REG,0x00000001); 
//         MSG("*  tx param_update \n");
//     }
//     mem_out32(BRAM_UPDATA_REG,0x0); 
// }

static void config_info_doppler_coff_param(uint32_t c)
{
     CONFIG_INFO_DOPPLER_COFF_REG = c;
     mem_out32(BRAM_INFO_DOPPLER_REG,CONFIG_INFO_DOPPLER_COFF_REG); 
}

static void config_info_fullfifo_thresh_param(uint16_t fifo_thresh)
{ 
    if(fifo_thresh <= 0x03ff) 
    {
     CONFIG_INFO_FIFO_THRESH_REG |= (fifo_thresh & 0x03ff);
    }
    else
    {
     CONFIG_INFO_FIFO_THRESH_REG |=   0x03ff ;
    }
    
    MSG("*  CONFIG_INFO_FIFO_THRESH_REG :0x%x\n",CONFIG_INFO_FIFO_THRESH_REG);
    mem_out32(BRAM_INFO_THRESH_REG,CONFIG_INFO_FIFO_THRESH_REG); 
}

static void config_info_emptyfifo_thresh_param(uint16_t fifo_thresh)
{
    if(fifo_thresh <= 0x07ff) 
    {
     CONFIG_INFO_FIFO_THRESH_REG |= (fifo_thresh & 0x07ff) << 10;
    }
    else
    {
     CONFIG_INFO_FIFO_THRESH_REG |=   0x07ff << 10;
    }
    MSG("*  CONFIG_INFO_FIFO_THRESH_REG :0x%x\n",CONFIG_INFO_FIFO_THRESH_REG);
    mem_out32(BRAM_INFO_THRESH_REG,CONFIG_INFO_FIFO_THRESH_REG); 
}

static void config_doppler1_coff_param(uint64_t c)
{
    CONFIG_DOPPLER1_PINC_REG = c;
    mem_out32(BRAM_DOPPLER1_PINC_REG,(CONFIG_DOPPLER1_PINC_REG) & 0xffffffff); 
    mem_out32(BRAM_DOPPLER1_PINC_REG+4,(CONFIG_DOPPLER1_PINC_REG >> 32) & 0xffffffff); 
}

static void config_doppler2_coff_param(uint64_t c)
{
    CONFIG_DOPPLER2_PINC_REG = c;
    mem_out32(BRAM_DOPPLER2_PINC_REG,(CONFIG_DOPPLER2_PINC_REG) & 0xffffffff); 
    mem_out32(BRAM_DOPPLER2_PINC_REG+4,(CONFIG_DOPPLER2_PINC_REG >> 32) & 0xffffffff); 
}

static void config_doppler3_coff_param(uint64_t c)
{
    CONFIG_DOPPLER3_PINC_REG = c;
    mem_out32(BRAM_DOPPLER3_PINC_REG,(CONFIG_DOPPLER3_PINC_REG) & 0xffffffff); 
    mem_out32(BRAM_DOPPLER3_PINC_REG+4,(CONFIG_DOPPLER3_PINC_REG >> 32) & 0xffffffff); 
}

static void config_doppler_mode_sel_param(uint8_t c)
{
    CONFIG_DOPPLER_MODE_SEL_REG = c;
    mem_out32(BRAM_DOPPLER_MODE_SEL_REG,CONFIG_DOPPLER_MODE_SEL_REG); 
}

static void config_trk_inte_select_param(uint32_t c,uint32_t c1)
{
    CONFIG_TRK_INTE_SELECT_REG = (c & 0x0000003f) | (c1 & 0x00000001) << 6;
    mem_out32(BRAM_TRK_INTE_SELECT_REG,CONFIG_TRK_INTE_SELECT_REG); 
}

static void config_sys_lpf_select_param(uint32_t c)
{
    CONFIG_TRK_SYS_LPF_SELECT_REG = c;
    mem_out32(BRAM_TRK_SYS_LPF_SELECT_REG,CONFIG_TRK_SYS_LPF_SELECT_REG);   
}

static void config_trk_inte_num_param(uint32_t c)
{
    CONFIG_TRK_INTE_NUM_REG = c;
    mem_out32(BRAM_TRK_INTE_NUM_REG,CONFIG_TRK_INTE_NUM_REG);    
}

static void config_trk_bsyn_buf_num_param(uint32_t c,uint32_t c1)
{
    CONFIG_TRK_BSYN_BUF_NUM_REG = (c & 0x0000001f) | (c1 & 0x00000003) << 5;
    mem_out32(BRAM_TRK_BSYN_BUF_NUM_REG,CONFIG_TRK_BSYN_BUF_NUM_REG);   
}

static void config_trk_bsyn_dot_num_param(uint32_t c)
{
    CONFIG_TRK_BSYN_DOT_NUM_REG = c; 
    mem_out32(BRAM_TRK_BSYN_DOT_NUM_REG,CONFIG_TRK_BSYN_DOT_NUM_REG);   
}

static void config_trk_inte_para_p_param(uint32_t c)
{
    CONFIG_TRK_INTE_PARA_P_REG = c;
    mem_out32(BRAM_TRK_INTE_PARA_P_REG,CONFIG_TRK_INTE_PARA_P_REG);   
}

static void config_trk_step_1_c0_param(uint32_t c)
{
    CONFIG_TRK_STEP_1_PARA_C0_REG = c; 
    mem_out32(BRAM_TRK_STEP_1_PARA_C0_REG,CONFIG_TRK_STEP_1_PARA_C0_REG);      
}

static void config_trk_step_1_c1_param(uint32_t c)
{
    CONFIG_TRK_STEP_1_PARA_C1_REG = c;  
    mem_out32(BRAM_TRK_STEP_1_PARA_C1_REG,CONFIG_TRK_STEP_1_PARA_C1_REG);  
}

static void config_trk_step_1_c2_param(uint32_t c)
{
   CONFIG_TRK_STEP_1_PARA_C2_REG = c;  
   mem_out32(BRAM_TRK_STEP_1_PARA_C2_REG,CONFIG_TRK_STEP_1_PARA_C2_REG);  
}

static void config_trk_loop_inibsyn_num_param(uint32_t c)
{
    CONFIG_TRK_LOOP_INIBSYN_NUM_REG = c;
    mem_out32(BRAM_TRK_LOOP_INIBSYN_NUM_REG,CONFIG_TRK_LOOP_INIBSYN_NUM_REG);      
}

static void config_trk_loop_iniedge_num_param(uint32_t c)
{
    CONFIG_TRK_LOOP_INIEDGE_NUM_REG = c;
   mem_out32(BRAM_TRK_LOOP_INIEDGE_NUM_REG,CONFIG_TRK_LOOP_INIEDGE_NUM_REG);      
}

static void config_trk_loop_endedge_num_param(uint32_t c)
{
    CONFIG_TRK_LOOP_ENDEDGE_NUM_REG = c;
    mem_out32(BRAM_TRK_LOOP_ENDEDGE_NUM_REG,CONFIG_TRK_LOOP_ENDEDGE_NUM_REG);    
}

static void config_trk_loop_instant_num_param(uint32_t c)
{
    CONFIG_TRK_LOOP_INSTANT_NUM_REG = c;
    mem_out32(BRAM_TRK_LOOP_INSTANT_NUM_REG,CONFIG_TRK_LOOP_INSTANT_NUM_REG);
}

static void config_trk_loop_lostclr_num_param(uint32_t c)
{
    CONFIG_TRK_LOOP_LOSTCTRL_NUM_REG = c;
    mem_out32(BRAM_TRK_LOOP_LOSTCTRL_NUM_REG,CONFIG_TRK_LOOP_LOSTCTRL_NUM_REG);
}

static void config_trk_bsyn_para_num_param(uint32_t c)
{
    CONFIG_TRK_BSYN_PARA_NUM_REG = c;
    mem_out32(BRAM_TRK_BSYN_PARA_NUM_REG,CONFIG_TRK_BSYN_PARA_NUM_REG);    
}

static void config_trk_dete_phase_door_param(uint32_t c)
{
    CONFIG_TRK_DETE_PHASE_DOOR_REG = c;
    mem_out32(BRAM_TRK_DETE_PAHSE_DOOR_REG,CONFIG_TRK_DETE_PHASE_DOOR_REG);    
}

static void config_trk_lost_debug_door_param(uint32_t c)
{
    CONFIG_TRK_LOST_DEBUG_DOOR_REG = c;
    mem_out32(BRAM_TRK_LOST_DEBUG_DOOR_REG,CONFIG_TRK_LOST_DEBUG_DOOR_REG);    
}

static void config_trk_nrz_code_pattern_param(uint32_t c)
{
    CONFIG_TRK_NRZ_CODE_PATTERN_REG = c;
    mem_out32(BRAM_NRZ_CODE_PATTERN_REG,CONFIG_TRK_NRZ_CODE_PATTERN_REG);    
}

static void config_control_info_byte(uint32_t *c)
{
    *c =  (uv_para_config.modulation_mode & 0x01) | (uv_para_config.chan_en & 0x01) << 1 | \
          (uv_para_config.carr_gen_en & 0x01) << 2 | (uv_para_config.info_en & 0x01) << 3 | \
          (uv_para_config.rx_en & 0x01) << 4 | (uv_para_config.tx_nrz_code_pattern & 0x03) << 5 | \
          (uv_para_config.rx_nrz_code_pattern & 0x03) << 7;  
}

void uv_gpio_config_init(void)
{
    //gpio
	int exportfd, directionfd; 
	printf("GPIO init running...\n");

	// The GPIO has to be exported to be able to see it
    // in sysfs
    exportfd = open("/sys/class/gpio/export", O_WRONLY);
	if (exportfd < 0)
    {
        printf("Cannot open GPIO to export it\n");
        exit(1);
    }

    write(exportfd, "906", 4);  //led0(测试)
    write(exportfd, "915", 4);  //led1(发射)
    write(exportfd, "959", 4);  //led2(接收)

    write(exportfd, "977", 4);  //dma_debug
    write(exportfd, "978", 4);  //pl复位

    write(exportfd, "979", 4);  //bram_pl_tx, pl_data_valid
    write(exportfd, "980", 4);  //bram_pl_tx, ps_rd_start
    write(exportfd, "981", 4);  //bram_pl_tx, ps_rd_end
    write(exportfd, "982", 4);  //bram_pl_rx, ps_wr_start
    write(exportfd, "983", 4);  //bram_pl_rx, ps_wr_valid
    write(exportfd, "984", 4);  //bram_pl_rx, pl_rd_rdy

    write(exportfd, "985", 4);  //dma_fifo_full
    write(exportfd, "986", 4);  //dma_fifo_empty

    write(exportfd, "987", 4);  //adc_debug_switch
    close(exportfd);
	printf("GPIO exported successfully\n");

	// Update the direction of the GPIO to be an output
    //led0(测试)
    directionfd = open("/sys/class/gpio/gpio906/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO906 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //led1(发射)
    directionfd = open("/sys/class/gpio/gpio915/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO915 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //led2(发射)
    directionfd = open("/sys/class/gpio/gpio959/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO959 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //dma_debug
    directionfd = open("/sys/class/gpio/gpio977/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO977 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //pl复位
    directionfd = open("/sys/class/gpio/gpio978/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO978 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //bram_pl_tx, pl_data_valid
    directionfd = open("/sys/class/gpio/gpio979/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO979 direction it\n");
        exit(1);
    }
    write(directionfd, "in", 4);
    close(directionfd);
    //bram_pl_tx, ps_rd_start
    directionfd = open("/sys/class/gpio/gpio980/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO980 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //bram_pl_tx, ps_rd_end
    directionfd = open("/sys/class/gpio/gpio981/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO981 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //bram_pl_rx, ps_wr_start
    directionfd = open("/sys/class/gpio/gpio982/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO982 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //bram_pl_rx, ps_wr_valid
    directionfd = open("/sys/class/gpio/gpio983/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO983 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
    //bram_pl_rx, pl_rd_rdy
    directionfd = open("/sys/class/gpio/gpio984/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO984 direction it\n");
        exit(1);
    }
    write(directionfd, "in", 4);
    close(directionfd);
    //dma_fifo_full
    directionfd = open("/sys/class/gpio/gpio985/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO985 direction it\n");
        exit(1);
    }
    write(directionfd, "in", 4);
    close(directionfd);
    //dma_fifo_empty
    directionfd = open("/sys/class/gpio/gpio986/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO986 direction it\n");
        exit(1);
    }
    write(directionfd, "in", 4);
    close(directionfd);
    //adc_debug_switch
    directionfd = open("/sys/class/gpio/gpio987/direction", O_RDWR);
    if (directionfd < 0)
    {
        printf("Cannot open GPIO987 direction it\n");
        exit(1);
    }
    write(directionfd, "out", 4);
    close(directionfd);
	printf("GPIO direction set as output successfully\n");

	// // Get the GPIO value ready to be toggled
	// valuefd0 = open("/sys/class/gpio/gpio977/value", O_RDWR);
    // if (valuefd0 < 0)
    // {
    //     printf("Cannot open GPIO977 value\n");
    //     exit(1);
    // }
	// printf("GPIO value opened, now toggling...\n");
	// write(valuefd0,"1", 2);
	// close(valuefd0);

}

void uv_gpio906_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio906/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO906 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio915_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio915/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO915 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio959_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio959/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO959 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio977_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio977/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO977 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio978_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio978/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO978 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_pl_rst_gpio_write(void)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio978/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO978 value\n");
        exit(1);
    }

    write(valuefd0,"0", 2);
    usleep(1);
    write(valuefd0,"1", 2);
    usleep(1);
    write(valuefd0,"0", 2);

    close(valuefd0);
}

u_int8_t uv_gpio979_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio979/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO979 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO979 value\n");
        exit(1);
    }

    //printf("read GPIO979 value: %c \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO979 data: %d \n",data);

    close(valuefd0);

    return data;
}

void uv_gpio980_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio980/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO980 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio981_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio981/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO981 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio982_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio982/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO982 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

void uv_gpio983_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio983/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO983 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

u_int8_t uv_gpio984_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio984/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO984 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO979 value\n");
        exit(1);
    }

    //printf("read GPIO979 value: %c \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO979 data: %d \n",data);

    close(valuefd0);

    return data;
}

u_int8_t uv_gpio977_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio977/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO977 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO979 value\n");
        exit(1);
    }

    //printf("read GPIO979 value: %c \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO979 data: %d \n",data);

    close(valuefd0);

    return data;
}
u_int8_t uv_gpio978_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio978/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO978 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO978 value\n");
        exit(1);
    }

    //printf("read GPIO978 value: %c \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO978 data: %d \n",data);

    close(valuefd0);

    return data;
}

u_int8_t uv_gpio985_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio985/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO985 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO979 value\n");
        exit(1);
    }

    //printf("read GPIO979 value: %d \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO979 data: %d \n",data);

    close(valuefd0);

    return data;
}
u_int8_t uv_gpio986_read(void)
{   
    int valuefd0; 
    char value;
    u_int8_t data;
    valuefd0 = open("/sys/class/gpio/gpio986/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO986 value\n");
        exit(1);
    }

    if( read(valuefd0,&value, 2) == -1 )
    {

        printf("Cannot read GPIO979 value\n");
        exit(1);
    }

    //printf("read GPIO979 value: %d \n",value);

    if( value == '0' )
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    //printf("read GPIO979 data: %d \n",data);

    close(valuefd0);

    return data;
}

void uv_gpio987_write(u_int8_t flag)
{   
    int valuefd0; 
    valuefd0 = open("/sys/class/gpio/gpio987/value", O_RDWR);
    if (valuefd0 < 0)
    {
        printf("Cannot open GPIO987 value\n");
        exit(1);
    }
    if( flag )
    {
        write(valuefd0,"1", 2);
    }
    else
    {
        write(valuefd0,"0", 2);
    }
    close(valuefd0);
}

static void printf_uv_config(struct uv_config_state *q)
{
    DEBUG(" q->chan_en       = %d\n",q->chan_en );
    DEBUG(" q->info_en       = %d\n",q->info_en );
    DEBUG(" q->tx_mode       = %d\n",q->tx_mode);
    DEBUG(" q->info_source   = %d\n",q->info_source );
    DEBUG(" q->info_fill_byte = 0x%x\n",q->info_fill_byte );
    DEBUG(" q->info_random_encode = %d\n", q->info_random_encode);
    DEBUG(" q->carr_gen_en = %d\n",q->carr_gen_en);
    DEBUG(" q->carr_cen_pinc = %d\n",q->carr_cen_pinc);
    DEBUG(" q->info_data_rate = %d\n",q->info_data_rate);
    DEBUG(" q->info_doppler_coff = %d\n",q->info_doppler_coff );
    DEBUG(" q->info_fifo_thresh = %d\n",q->info_fifo_thresh);
    DEBUG(" q->doppler1_pinc = %llu\n",q->doppler1_pinc);
    DEBUG(" q->doppler2_pinc = %llu\n",q->doppler2_pinc);
    DEBUG(" q->doppler3_pinc = %u\n",q->doppler3_pinc);
    DEBUG(" q->doppler_mode_sel = %d\n",q->doppler_mode_sel);
    DEBUG(" q->fsk_offset_freq = %d\n",q->fsk_offset_freq);
    DEBUG(" q->tx_conv_code = %d\n",q->tx_conv_code );
    DEBUG(" q->trk_inte_select = %d\n",q->uv_rx.trk_inte_select);
    DEBUG(" q->sys_lpf_select = %d\n",q->uv_rx.sys_lpf_select);
    DEBUG(" q->trk_inte_num = %d\n",q->uv_rx.trk_inte_num);
    DEBUG(" q->trk_bsyn_buf_num = %d\n",q->uv_rx.trk_bsyn_buf_num);
    DEBUG(" q->trk_bsyn_dot_num = %d\n",q->uv_rx.trk_bsyn_dot_num);
    DEBUG(" q->trk_inte_para_p = %d\n", q->uv_rx.trk_inte_para_p);
    DEBUG(" q->trk_step_1_para_c0 = %d\n",q->uv_rx.trk_step_1_para_c0);
    DEBUG(" q->trk_step_1_para_c1 = %d\n",q->uv_rx.trk_step_1_para_c1);
    DEBUG(" q->trk_step_1_para_c2 = %d\n",q->uv_rx.trk_step_1_para_c2);
    DEBUG(" q->trk_loop_inibsyn_num = %d\n",q->uv_rx.trk_loop_inibsyn_num);
    DEBUG(" q->trk_loop_iniedge_num = %d\n",q->uv_rx.trk_loop_iniedge_num );
    DEBUG(" q->trk_loop_endedge_num = %d\n",q->uv_rx.trk_loop_endedge_num );
    DEBUG(" q->trk_loop_instant_num = %d\n",q->uv_rx.trk_loop_instant_num);
    DEBUG(" q->trk_loop_lostclr_num = %d\n",q->uv_rx.trk_loop_lostclr_num );
    DEBUG(" q->trk_bsyn_para_num = %d\n",q->uv_rx.trk_bsyn_para_num );
    DEBUG(" q->trk_dete_phase_door = %d\n", q->uv_rx.trk_dete_phase_door);
    DEBUG(" q->trk_lost_debug_door = %d\n", q->uv_rx.trk_lost_debug_door);
    DEBUG(" q->tx_nrz_code_pattern = %d\n",q->tx_nrz_code_pattern );
    DEBUG(" q->rx_nrz_code_pattern = %d\n",q->rx_nrz_code_pattern );
    DEBUG(" q->info_random_decode = %d\n",q->info_random_decode);
    DEBUG(" q->rx_conv_code = %d\n",q->rx_conv_code);
    DEBUG(" q->rx_info_rate = %d\n",q->rx_info_rate);
    DEBUG(" q->fsk_offset_freq = %d\n",q->fsk_offset_freq);
    DEBUG(" q->rx_frame_sync_word = %d\n",q->rx_frame_sync_word);
    DEBUG(" q->rx_frame_sync_len = %d\n",q->rx_frame_sync_len );
    DEBUG(" q->rx_preamble_byte = %d\n",q->rx_preamble_byte);
    DEBUG(" q->rx_preamble_length = %d\n",q->rx_preamble_length);
}
static void printf_uv_pl_state(struct uv_pl_state_info *q)
{
    DEBUG(" pl->tx_chan_en                     = %d\n",  q->tx_chan_en                  );
    DEBUG(" pl->tx_info_en                     = %d\n",  q->tx_info_en                  );
    DEBUG(" pl->rx_rst                         = %d\n",  q->rx_rst                      );
    DEBUG(" pl->tx_rst                         = %d\n",  q->tx_rst                      );
    DEBUG(" pl->tx_carr_gen_en                 = %d\n",  q->tx_carr_gen_en              );
    DEBUG(" pl->tx_carr_cen_pinc               = %d\n",  q->tx_carr_cen_pinc            );
    DEBUG(" pl->tx_info_data_rate              = %d\n",  q->tx_info_data_rate           );
    DEBUG(" pl->tx_info_doppler_coff           = %d\n",  q->tx_info_doppler_coff        );
    DEBUG(" pl->tx_info_fifo_thresh            = %d\n",  q->tx_info_fifo_thresh         );
    DEBUG(" pl->tx_doppler1                    = %llu\n",q->tx_doppler1                 );
    DEBUG(" pl->tx_doppler2                    = %llu\n",q->tx_doppler2                 );
    DEBUG(" pl->tx_doppler3                    = %llu\n",q->tx_doppler3                 );
    DEBUG(" pl->tx_doppler_mode                = %d\n",  q->tx_doppler_mode             );
    DEBUG(" pl->rx_inte_select                 = %d\n",  q->rx_inte_select              );
    DEBUG(" pl->rx_sys_lpf                     = %d\n",  q->rx_sys_lpf                  );
    DEBUG(" pl->rx_trk_inte_num                = %d\n",  q->rx_trk_inte_num             );
    DEBUG(" pl->rx_trk_bsyn_buf_num            = %d\n",  q->rx_trk_bsyn_buf_num         );
    DEBUG(" pl->rx_trk_bsyn_dot_num            = %d\n",  q->rx_trk_bsyn_dot_num         );
    DEBUG(" pl->rx_trk_inte_para               = %d\n",  q->rx_trk_inte_para            );
    DEBUG(" pl->rx_trk_step_para_c0            = %d\n",  q->rx_trk_step_para_c0         );
    DEBUG(" pl->rx_trk_step_para_c1            = %d\n",  q->rx_trk_step_para_c1         );
    DEBUG(" pl->rx_trk_step_para_c2            = %d\n",  q->rx_trk_step_para_c2         );
    DEBUG(" pl->rx_trk_loop_inibsyn_num        = %d\n",  q->rx_trk_loop_inibsyn_num     );
    DEBUG(" pl->rx_trk_loop_iniedge_num        = %d\n",  q->rx_trk_loop_iniedge_num     );
    DEBUG(" pl->rx_trk_loop_endedge_num        = %d\n",  q->rx_trk_loop_endedge_num     );
    DEBUG(" pl->rx_trk_loop_instant_num        = %d\n",  q->rx_trk_loop_instant_num     );
    DEBUG(" pl->rx_trk_loop_lostclr_num        = %d\n",  q->rx_trk_loop_lostclr_num     );
    DEBUG(" pl->rx_trk_bsyn_para_num           = %d\n",  q->rx_trk_bsyn_para_num        );
    DEBUG(" pl->rx_trk_dete_phase_door         = %d\n",  q->rx_trk_dete_phase_door      );
    DEBUG(" pl->rx_trk_lost_debug_door         = %d\n",  q->rx_trk_lost_debug_door      );
    DEBUG(" pl->rx_nrz_code_pattern            = %d\n",  q->rx_nrz_code_pattern         );
    DEBUG(" pl->feedback_info_fifo             = %d\n",  q->feedback_info_fifo          );
    DEBUG(" pl->feedback_sys_state             = %d\n",  q->feedback_sys_state          );
    DEBUG(" pl->feedback_carr_feq_pinc         = %d\n",  q->feedback_carr_feq_pinc      );
}

 void config_feedback_bram_status(struct uv_pl_state_info *q)
{
    uint32_t tmp;
    u_int8_t value = 0;

    value = uv_gpio979_read(); //pl_valid == 0
    while(value == 1)
    {
        value = uv_gpio979_read();
    }

    uv_gpio980_write(1); //ps_start 上升沿
    uv_gpio980_write(0); 

    value = uv_gpio979_read(); //pl_valid == 1
    while(value == 0)
    {
        value = uv_gpio979_read();
    }

    tmp = mem_in32(BRAM_RD_CONTROL_REG);
    q->tx_chan_en               = (tmp) & 0x00000001; 
    q->tx_info_en               = (tmp >> 1) & 0x00000001; 
    q->rx_rst                   = (tmp >> 2) & 0x00000001; 
    q->tx_rst                   = (tmp >> 3) & 0x00000001; 
    q->tx_carr_gen_en           = (tmp >> 4) & 0x00000001; 
    q->tx_carr_cen_pinc         = mem_in32(BRAM_RD_CARR_CEN_REG);
    q->tx_info_data_rate        = mem_in32(BRAM_RD_DATA_RATE_REG);
    q->tx_info_doppler_coff     = mem_in32(BRAM_RD_DOPPLER_COFF_REG);
    q->tx_info_fifo_thresh      = mem_in32(BRAM_RD_FIFO_THRESH_REG);
    q->tx_doppler1              = ((mem_in32(BRAM_RD_DOPPLER1_REG)&0x00000000ffffffff) | ((mem_in32(BRAM_RD_DOPPLER1_REG)&0x00000000ffffffff) << 32) );
    q->tx_doppler2              = ((mem_in32(BRAM_RD_DOPPLER2_REG)&0x00000000ffffffff) | ((mem_in32(BRAM_RD_DOPPLER2_REG)&0x00000000ffffffff) << 32) );
    q->tx_doppler3              = ((mem_in32(BRAM_RD_DOPPLER3_REG)&0x00000000ffffffff) | ((mem_in32(BRAM_RD_DOPPLER3_REG)&0x00000000ffffffff) << 32) );
    q->tx_doppler_mode          = mem_in32(BRAM_RD_DOPPLER_mode_REG);
    q->rx_inte_select           = mem_in32(BRAM_RD_TRK_INTE_REG);
    q->rx_sys_lpf               = mem_in32(BRAM_RD_SYS_LPF_REG);
    q->rx_trk_inte_num          = mem_in32(BRAM_RD_INTE_NUM_REG);
    q->rx_trk_bsyn_buf_num      = mem_in32(BRAM_RD_BSYN_BUF_REG);
    q->rx_trk_bsyn_dot_num      = mem_in32(BRAM_RD_BSYN_DOT_REG);
    q->rx_trk_inte_para         = mem_in32(BRAM_RD_PARA_REG);
    q->rx_trk_step_para_c0      = mem_in32(BRAM_RD_PARA_c0_REG);
    q->rx_trk_step_para_c1      = mem_in32(BRAM_RD_PARA_c1_REG);
    q->rx_trk_step_para_c2      = mem_in32(BRAM_RD_PARA_c2_REG);
    q->rx_trk_loop_inibsyn_num  = mem_in32(BRAM_RD_LOOP_INIBSYN_REG);
    q->rx_trk_loop_iniedge_num  = mem_in32(BRAM_RD_LOOP_INIEDGE_REG);
    q->rx_trk_loop_endedge_num  = mem_in32(BRAM_RD_LOOP_ENDEDGE_REG);
    q->rx_trk_loop_instant_num  = mem_in32(BRAM_RD_LOOP_INSTANT_REG);
    q->rx_trk_loop_lostclr_num  = mem_in32(BRAM_RD_LOOP_LOSTCLR_REG);
    q->rx_trk_bsyn_para_num     = mem_in32(BRAM_RD_BSYN_PARA_REG);
    q->rx_trk_dete_phase_door   = mem_in32(BRAM_RD_DETE_PHASE_REG);
    q->rx_trk_lost_debug_door   = mem_in32(BRAM_RD_LOST_DEBUG_REG);
    q->rx_nrz_code_pattern      = mem_in32(BRAM_RD_NRZ_CODE_REG);
    q->feedback_info_fifo       = mem_in32(BRAM_RD_FEEDBACK_FIFO_REG);
    q->feedback_sys_state       = mem_in32(BRAM_RD_FEEDBACK_STATE_REG);
    q->feedback_carr_feq_pinc   = mem_in32(BRAM_RD_FEEDBACK_CARR_REG);

    uv_gpio981_write(1); //ps_end 上升沿
    uv_gpio981_write(0); 


    printf_uv_pl_state(q);
}

// static void handle_sig1(int sig)
// {
// 	printf("Waiting for process to finish...\n");
// 	stop = true;
// }

static void uv_config_init(void)
{
    //---------------控制信号---------------------
    uv_para_config.modulation_mode = 1; //调制模式，1 --bpsk;0 --msk  
    uv_para_config.chan_en     = 1; //发送信道开关，0 --关闭，1 --打开 
    uv_para_config.carr_gen_en = 1; //载波生成开关，0 --关闭，1 --打开
    uv_para_config.info_en     = 1; //调制信息开关，0 --关闭，1 --打开
    uv_para_config.rx_en       = 0; //接收信道开关，1 --关闭，0 --打开   
    uv_para_config.tx_nrz_code_pattern = 0; //nrz编码模式，0 --NRZ-l,1 --NRZ-M,2 --NRZ-S
    uv_para_config.rx_nrz_code_pattern = 0; //nrz解码模式，0 --NRZ-l,1 --NRZ-M,2 --NRZ-S

    //-----------BPSK发送调制配置-------------------
    //PL配置参数
    // uv_para_config.carr_cen_pinc = CEN_FREQ;    //中频频点
    uv_para_config.carr_cen_pinc = 0;    //中频频点
    uv_para_config.info_data_rate = BITRATE;    //信息速率
    uv_para_config.info_doppler_coff = 0;       //信息速率多普勒计算系数
    uv_para_config.doppler_mode_sel = 0;        //多普勒模式选择开关
    uv_para_config.doppler1_pinc    = 0;        //载波多普勒系数1
    uv_para_config.doppler2_pinc    = 0;        //载波多普勒系数2
    uv_para_config.doppler3_pinc    = 0;        //载波多普勒系数3
    uv_para_config.info_fifo_thresh = 512;      //信息fifo门限值
    
    // uv_para_config.doppler1_pinc    = 14073748835533;
    // uv_para_config.doppler2_pinc    = 17307656195116;
    // uv_para_config.doppler3_pinc    = 768614336;
    // uv_para_config.doppler_mode_sel = 1;
    // uv_para_config.doppler1_pinc    = 28147497671066;
    // uv_para_config.doppler2_pinc    = 0;
    // uv_para_config.doppler3_pinc    = 0;
    // uv_para_config.doppler_mode_sel = 1;

    //PS配置遥控数据参数
    uv_para_config.info_fill_byte   = 0x55; //0x55  
    

    //---------------BPSK接收解调配置-------------------------
    uv_baseband_para_calc(&uv_para_config.uv_rx,uv_para_config.info_data_rate,SAMP_FREQ,npb);//接收参数设置

	//--------------ad9361射频发送通道配置-------------------
    // uv_para_config.txcfg.lo_hz = MHZ((159.51)); // 2.5 GHz rf frequency 401.69
    // uv_para_config.txcfg.tx_gain = -60; //发送衰减，单位：dB，范围：-89.75~0，精度：0.25dB 
	// uv_para_config.txcfg.bw_hz = MHZ(20); // 1.5 MHz rf bandwidth
	// //uv_para_config.txcfg.fs_hz = MHZ(10);   // 2.5 MS/s tx sample rate
	// //uv_para_config.txcfg.rfport = "A"; // port A (select for rf freq.)

	// //--------------ad9361射频接收通道配置---------------------
    // uv_para_config.rxcfg.lo_hz = MHZ(149.51); // 2.5 GHz rf frequency
	// uv_para_config.rxcfg.bw_hz = MHZ(20);   // 2 MHz rf bandwidth
	//uv_para_config.rxcfg.fs_hz = MHZ(10);   // 2.5 MS/s rx sample rate
	//uv_para_config.rxcfg.rfport = "A_BALANCED"; // port A (select for rf freq.)

    //----------------gpio初始化-----------------------------
    uv_gpio_config_init();


    // u_int8_t value=0;
    // uv_gpio906_write(0);
    // uv_gpio915_write(0);
    // uv_gpio959_write(0);
    // uv_gpio977_write(0);
    // value = uv_gpio977_read();
    // printf("test1 read GPIO977 data: %d !!\n",value);
    // uv_gpio977_write(1);
    // value = uv_gpio977_read();
    // printf("test2 read GPIO977 data: %d !!\n",value);
    // uv_gpio978_write(0);
    // uv_gpio980_write(0);
    // uv_gpio981_write(0);
    // uv_gpio982_write(0);
    // uv_gpio983_write(0);
    // uv_gpio987_write(0);
    // uv_gpio978_write(1);//pl_rst
    // //uv_pl_rst_gpio_write();
    // value = uv_gpio978_read();
    // printf("test1 read GPIO978 data: %d !!\n",value);
    // usleep(100);
    // uv_gpio978_write(0);
    // value = uv_gpio978_read();
    // printf("test1 read GPIO978 data: %d !!\n",value);
    // usleep(10000);

}


static void uv_control_config_init(void)
{
    u_int8_t value = 0;
    uint32_t cntrol_byte = 0;

    config_control_info_byte(&cntrol_byte); //控制信息组帧

    printf("test1 config control into ..\n");

    value = uv_gpio984_read(); //pl_rdy == 0
    while(value == 1)
    {
        value = uv_gpio984_read();
        printf("test1 read GPIO984 data: %d !!\n",value);

    }

    uv_gpio982_write(1); //ps_start 上升沿
    uv_gpio982_write(0); 

    value = uv_gpio984_read(); //pl_rdy == 1
    while(value == 0)
    {
        value = uv_gpio984_read();
        printf("test2 read GPIO984 data: %d !!\n",value);

    }

    mem_out32(BRAM_TX_TYPE_REG,1); //配置类型 --控制信号
    mem_out32(BRAM_CONTROL_REG,cntrol_byte);
    mem_out32(BRAM_CONTROL_REG1,frequency_convert_reg_value(0,SAMP_FREQ));  //DMA_debug,信息速率NCO

    uv_gpio983_write(1); //ps_valid 上升沿
    uv_gpio983_write(0); 

    printf("test1 config control successfully \n");

}


 
static void uv_transfer_config_init(void)
{
    u_int8_t value = 0;

    printf("test1 config tx into ..\n");

    value = uv_gpio984_read(); //pl_rdy == 0
    while(value == 1)
    {
        value = uv_gpio984_read();
    }

    uv_gpio982_write(1); //ps_start 上升沿
    uv_gpio982_write(0); 

    value = uv_gpio984_read(); //pl_rdy == 1
    while(value == 0)
    {
        value = uv_gpio984_read();
    }
    mem_out32(BRAM_TX_TYPE_REG,2); //配置类型 --发送调制配置参数
    config_if_frequency_param(uv_para_config.carr_cen_pinc,SAMP_FREQ); //中频频率设置
    config_info_rate_param(uv_para_config.info_data_rate,SAMP_FREQ); //PSK信息速率设置
    config_info_doppler_coff_param(uv_para_config.info_doppler_coff); //信息多普勒参数设置
    config_doppler1_coff_param(uv_para_config.doppler1_pinc);//多普勒参数1 
    config_doppler2_coff_param(uv_para_config.doppler2_pinc);//多普勒参数2
    config_doppler3_coff_param(uv_para_config.doppler3_pinc);//多普勒参数3
    config_doppler_mode_sel_param(uv_para_config.doppler_mode_sel);  //多普勒模式选择
    config_info_fullfifo_thresh_param(uv_para_config.info_fifo_thresh); //full 满阈值设置

    uv_gpio983_write(1); //ps_valid 上升沿
    uv_gpio983_write(0); 

    printf("test1 config tx successfully \n");


    // config_switch_onoff(tx_rst_sw,ENABLE);//复位
    // config_update_param(TX);
    // usleep(10);
    // config_info_doppler_coff_param(uv_para_config.info_doppler_coff); //信息多普勒参数设置
    // config_doppler_mode_sel_param(uv_para_config.doppler_mode_sel);  //多普勒模式选择
    // config_doppler1_coff_param(uv_para_config.doppler1_pinc);//多普勒参数1 
    // config_doppler2_coff_param(uv_para_config.doppler2_pinc);//多普勒参数2
    // config_doppler3_coff_param(uv_para_config.doppler3_pinc);//多普勒参数3
    // config_if_frequency_param(uv_para_config.carr_cen_pinc,SAMP_FREQ); //中频频率设置
    // config_info_rate_param(uv_para_config.info_data_rate,SAMP_FREQ); //PSK信息速率设置
    // config_info_fsk_frequency_offset(uv_para_config.fsk_offset_freq,SAMP_FREQ); //MSK信息速率设置
    // config_info_fullfifo_thresh_param(uv_para_config.info_fifo_thresh); //full 满阈值设置
    // config_switch_onoff(chan_sw,uv_para_config.chan_en?ENABLE:DISABLE);//通道开关
    // config_switch_onoff(info_sw,uv_para_config.info_en?ENABLE:DISABLE);//信息开关
    // config_switch_onoff(carr_ena_sw,uv_para_config.carr_gen_en?ENABLE:DISABLE);//载波开关
    // config_switch_onoff(fsk_psk_sw,uv_para_config.tx_mode?ENABLE:DISABLE);//FSK\PSK模式切换 
    // config_switch_onoff(tx_rst_sw,DISABLE); //复位
    // config_update_param(TX);

}

static void uv_recv_config_init(void)
{
    u_int8_t value = 0;


    printf("test1 config rx into ..\n");

    value = uv_gpio984_read(); //pl_rdy == 0
    while(value == 1)
    {
        value = uv_gpio984_read();
    }

    uv_gpio982_write(1); //ps_start 上升沿
    uv_gpio982_write(0); 

    value = uv_gpio984_read(); //pl_rdy == 1
    while(value == 0)
    {
        value = uv_gpio984_read();
    }

    mem_out32(BRAM_TX_TYPE_REG,3); //配置类型 --接收解调配置参数
    config_trk_inte_select_param(uv_para_config.uv_rx.trk_inte_select,uv_para_config.uv_rx.sys_lpf_select);  
    config_trk_inte_num_param(uv_para_config.uv_rx.trk_inte_num); 
    config_trk_bsyn_buf_num_param(uv_para_config.uv_rx.trk_bsyn_buf_num,uv_para_config.uv_rx.trk_bsyn_dot_num); 
    config_trk_inte_para_p_param(uv_para_config.uv_rx.trk_inte_para_p);  
    config_trk_step_1_c0_param(uv_para_config.uv_rx.trk_step_1_para_c0);  
    config_trk_step_1_c1_param(uv_para_config.uv_rx.trk_step_1_para_c1);  
    config_trk_step_1_c2_param(uv_para_config.uv_rx.trk_step_1_para_c2);  
    config_trk_loop_inibsyn_num_param(uv_para_config.uv_rx.trk_loop_inibsyn_num);  
    config_trk_loop_iniedge_num_param(uv_para_config.uv_rx.trk_loop_iniedge_num); 
    config_trk_loop_endedge_num_param(uv_para_config.uv_rx.trk_loop_endedge_num); 
    config_trk_loop_instant_num_param(uv_para_config.uv_rx.trk_loop_instant_num); 
    config_trk_loop_lostclr_num_param(uv_para_config.uv_rx.trk_loop_lostclr_num);
    config_trk_bsyn_para_num_param(uv_para_config.uv_rx.trk_bsyn_para_num);
    config_trk_dete_phase_door_param(uv_para_config.uv_rx.trk_dete_phase_door);
    config_trk_lost_debug_door_param(uv_para_config.uv_rx.trk_lost_debug_door);

    uv_gpio983_write(1); //ps_valid 上升沿
    uv_gpio983_write(0); 

    printf("test1 config rx successfully \n");

    // config_switch_onoff(rx_rst_sw,ENABLE);//复位
    // config_update_param(RX);
    // usleep(10);
    // config_trk_inte_select_param(uv_para_config.uv_rx.trk_inte_select);  
    // config_sys_lpf_select_param(uv_para_config.uv_rx.sys_lpf_select);  
    // config_trk_inte_num_param(uv_para_config.uv_rx.trk_inte_num); 
    // config_trk_bsyn_buf_num_param(uv_para_config.uv_rx.trk_bsyn_buf_num); 
    // config_trk_bsyn_dot_num_param(uv_para_config.uv_rx.trk_bsyn_dot_num); 
    // config_trk_bsyn_para_num_param(uv_para_config.uv_rx.trk_bsyn_para_num);
    // config_trk_inte_para_p_param(uv_para_config.uv_rx.trk_inte_para_p);  
    // config_trk_step_1_c0_param(uv_para_config.uv_rx.trk_step_1_para_c0);  
    // config_trk_step_1_c1_param(uv_para_config.uv_rx.trk_step_1_para_c1);  
    // config_trk_step_1_c2_param(uv_para_config.uv_rx.trk_step_1_para_c2);  
    // config_trk_loop_inibsyn_num_param(uv_para_config.uv_rx.trk_loop_inibsyn_num);  
    // config_trk_loop_iniedge_num_param(uv_para_config.uv_rx.trk_loop_iniedge_num); 
    // config_trk_loop_endedge_num_param(uv_para_config.uv_rx.trk_loop_endedge_num); 
    // config_trk_loop_instant_num_param(uv_para_config.uv_rx.trk_loop_instant_num); 
    // config_trk_loop_lostclr_num_param(uv_para_config.uv_rx.trk_loop_lostclr_num);
    // config_trk_dete_phase_door_param(uv_para_config.uv_rx.trk_dete_phase_door);
    // config_trk_lost_debug_door_param(uv_para_config.uv_rx.trk_lost_debug_door);
    // config_trk_nrz_code_pattern_param(uv_para_config.rx_nrz_code_pattern);//NRZ-L NRZ-M NRZ-S
    // config_switch_onoff(rx_rst_sw,DISABLE);//复位
    // config_update_param(RX);
 
    printf_uv_config(&uv_para_config);
    config_feedback_bram_status(&uv_pl_state);

    // struct timeval tv_start,tv_end;
    // long long timeuse;
    // uint32_t tmp;
    // while(1)
    // {
    // //     uv_gpio977_write(0);
    // //     //printf("write GPIO977 data: 0 \n");
    // //     //value = uv_gpio985_read();
    // //     //printf("test1 read GPIO985 data: %d !!\n",value);
    // //     //value = uv_gpio986_read();
    // //     //printf("test1 read GPIO986 data: %d !!\n",value);
    // //     uv_gpio977_write(1);
    // //     //printf("write GPIO977 data: 1 \n");
    // //     //value = uv_gpio985_read();
    // //     //printf("test1 read GPIO985 data: %d !!\n",value);
    // //     //value = uv_gpio986_read();
    // //     //printf("test1 read GPIO986 data: %d !!\n",value);
    //     gettimeofday(&tv_start, NULL);
    //     uv_gpio977_write(1);
    //     gettimeofday(&tv_end, NULL);
    //     timeuse = 1000000 * ( tv_end.tv_sec - tv_start.tv_sec ) + tv_end.tv_usec - tv_start.tv_usec;
    //     printf("test1 gpio_wr time: %lld !!\n",timeuse);
    //     gettimeofday(&tv_start, NULL);
    //     value = uv_gpio985_read();
    //     gettimeofday(&tv_end, NULL);
    //     timeuse = 1000000 * ( tv_end.tv_sec - tv_start.tv_sec ) + tv_end.tv_usec - tv_start.tv_usec;
    //     printf("test1 gpio_rd time: %lld !!\n",timeuse);
        // gettimeofday(&tv_start, NULL);
        // mem_out32(BRAM_TX_TYPE_REG,3); //配置类型 --接收解调配置参数
        // gettimeofday(&tv_end, NULL);
        // timeuse = 1000000 * ( tv_end.tv_sec - tv_start.tv_sec ) + tv_end.tv_usec - tv_start.tv_usec;
        // printf("test1 bram_wr time: %lld !!\n",timeuse);
    //     gettimeofday(&tv_start, NULL);
        // tmp = mem_in32(BRAM_RD_CONTROL_REG);
    //     gettimeofday(&tv_end, NULL);
    //     timeuse = 1000000 * ( tv_end.tv_sec - tv_start.tv_sec ) + tv_end.tv_usec - tv_start.tv_usec;
    //     printf("test1 bram_rd time: %lld !!\n",timeuse);
        // gettimeofday(&tv_start, NULL);
        // usleep(100);
        // gettimeofday(&tv_end, NULL);
        // timeuse = 1000000 * ( tv_end.tv_sec - tv_start.tv_sec ) + tv_end.tv_usec - tv_start.tv_usec;
        // printf("test1 usleep:100us time: %lld !!\n",timeuse);
    //     signal(SIGINT, handle_sig1);
    // }
}





// /* check return value of attr_write function */
// static void errchk(int v, const char* what) {
// 	 if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); shutdown(); }
// }

// /* write attribute: long long int */
// static void wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
// {
// 	errchk(iio_channel_attr_write_longlong(chn, what, val), what);
// }

// /* write attribute: string */
// static void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
// {
// 	errchk(iio_channel_attr_write(chn, what, str), what);
// }

// /* helper function generating channel names */
// static char* get_ch_name(const char* type, int id)
// {
// 	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
// 	return tmpstr;
// }

// /* returns ad9361 phy device */
// struct iio_device* get_ad9361_phy(struct iio_context *ctx)
// {
// 	struct iio_device *dev =  iio_context_find_device(ctx, "ad9361-phy");
// 	ASSERT(dev && "No ad9361-phy found");
// 	return dev;
// }

// /* finds AD9361 streaming IIO devices */
// bool get_ad9361_stream_dev(struct iio_context *ctx, enum iodev d, struct iio_device **dev)
// {
// 	switch (d) {
// 	case TX: *dev = iio_context_find_device(ctx, txdevice_name); return *dev != NULL;
// 	case RX: *dev = iio_context_find_device(ctx, rxdevice_name);  return *dev != NULL;
// 	default: ASSERT(0); return false;
// 	}
// }

// /* finds AD9361 streaming IIO channels */
// bool get_ad9361_stream_ch(struct iio_context *ctx, enum iodev d, struct iio_device *dev, int chid, struct iio_channel **chn)
// {
// 	*chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
// 	if (!*chn)
// 		*chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
// 	return *chn != NULL;
// }

// /* finds AD9361 phy IIO configuration channel with id chid */
// bool get_phy_chan(struct iio_context *ctx, enum iodev d, int chid, struct iio_channel **chn)
// { 
// 	switch (d) {
// 	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
// 	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
// 	default: ASSERT(0); return false;
// 	}
// }

// /* finds AD9361 local oscillator IIO configuration channels */
// bool get_lo_chan(struct iio_context *ctx, enum iodev d, struct iio_channel **chn)
// {
// 	switch (d) {
// 	 // LO chan is always output, i.e. true
// 	case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
// 	case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
// 	default: ASSERT(0); return false;
// 	}
// }

// /* applies streaming configuration through IIO */
// bool cfg_ad9361_streaming_ch(struct iio_context *ctx, struct stream_cfg *cfg, enum iodev type, int chid)
// {
// 	struct iio_channel *chn = NULL;

// 	// Configure phy and lo channels
// 	printf("* Acquiring AD9361 phy channel %d\n", chid);
// 	if (!get_phy_chan(ctx, type, chid, &chn)) { return false; }
// 	//wr_ch_str(chn, "rf_port_select",     cfg->rfport);
// 	wr_ch_lli(chn, "rf_bandwidth",       cfg->bw_hz);
//     if(type) //tx
//     {
//         wr_ch_lli(chn, "hardwaregain",       cfg->tx_gain);
//     }
    
//  //	wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);

// 	// Configure LO channel
// 	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
// 	if (!get_lo_chan(ctx, type, &chn)) { return false; }
// 	wr_ch_lli(chn, "frequency", cfg->lo_hz);
// 	return true;
// }

// static void uv_config_queue_init(void)
// {
//     DEBUG("* uv_config_queue_init                    \n");
 
//     //配置、数据通道
//     net_queue_array[UV_CONFIG_DATA_NET_RX_QUEUE_NUM].queue = pthread_queue_create(net_queue_array[UV_CONFIG_DATA_NET_RX_QUEUE_NUM].length,net_queue_array[UV_CONFIG_DATA_NET_RX_QUEUE_NUM].size);
//     if(net_queue_array[UV_CONFIG_DATA_NET_RX_QUEUE_NUM].queue == NULL) {
//         DEBUG("UV_CONFIG_DATA_NET_RX_QUEUE_ID init failed\n"); 
//         goto queue_free;
//     }
//     net_queue_array[UV_TX_QUEUE_NUM].queue = pthread_queue_create(net_queue_array[UV_TX_QUEUE_NUM].length,net_queue_array[UV_TX_QUEUE_NUM].size);
//     if(net_queue_array[UV_TX_QUEUE_NUM].queue == NULL) {
//         DEBUG("UV_TX_QUEUE_ID init failed\n"); 
//         goto queue_free;
//     }
//     DEBUG("* uv_config_queue_done                    \n"); 
//     queue_free:
//         pthread_queue_delete(net_queue_array[UV_CONFIG_DATA_NET_RX_QUEUE_NUM].queue);
//         pthread_queue_delete(net_queue_array[UV_TX_QUEUE_NUM].queue);
//     return ;
// }



// static void uv_config_updata_handler(uint8_t *buff , uint16_t len)
// {
//     uint8_t cc_flag;
//     uint8_t gd_flag;
//     struct uv_config_state *q = malloc(sizeof(struct uv_config_state));

//     DEBUG("uv_config_updata_handler  into\n"); 
//     memcpy(q,&uv_para_config,sizeof(struct uv_config_state));
//     //协议内容start
//     cc_flag = buff[0];
//     if(cc_flag == 0xcc)
//     {
//         q->carr_cen_pinc =    (buff[1] & 0x000000ff)<< 24 | (buff[2]  & 0x000000ff)<< 16 | \
//                               (buff[3]  & 0x000000ff) << 8 |  (buff[4]  & 0x000000ff)<< 0;
//         q->doppler_mode_sel = buff[5] ;
//         q->doppler1_pinc    = (buff[6]  & 0x00000000000000ff) << 40 | (buff[7]  & 0x00000000000000ff) << 32 |\
//                               (buff[8]  & 0x00000000000000ff) << 24 | (buff[9]  & 0x00000000000000ff) << 16 |\
//                               (buff[10] & 0x00000000000000ff) << 8  | (buff[11] & 0x00000000000000ff) << 0;
//         q->doppler2_pinc    = (buff[12] & 0x00000000000000ff) << 56 | (buff[13] & 0x00000000000000ff) << 48 | \
//                               (buff[14] & 0x00000000000000ff) << 40 | (buff[15] & 0x00000000000000ff) << 32 | \
//                               (buff[16] & 0x00000000000000ff) << 24 | (buff[17] & 0x00000000000000ff) << 16 | \
//                               (buff[18] & 0x00000000000000ff) << 8  | (buff[19] & 0x00000000000000ff) << 0;

//         q->doppler3_pinc    = (buff[20]  & 0x00000000000000ff) << 40 | (buff[21]  & 0x00000000000000ff) << 32 |\
//                               (buff[22]  & 0x00000000000000ff) << 24 | (buff[23]  & 0x00000000000000ff) << 16 |\
//                               (buff[24]  & 0x00000000000000ff) << 8  | (buff[25]  & 0x00000000000000ff) << 0;
//     }
//     cc_flag = buff[50];
//     if( cc_flag == 0xcc)
//     {
//         q->info_data_rate = (buff[51]  & 0x000000ff)<< 24 |  (buff[52]  & 0x000000ff)<< 16 | \
//                             (buff[53]  & 0x000000ff) << 8 |  (buff[54]  & 0x000000ff)<< 0;
//         q->info_source    = buff[55];
//         q->info_fill_byte = buff[56];
//         q->info_random_encode = buff[57];
//         q->info_doppler_coff =  (buff[58]  & 0x000000ff)<< 24 |  (buff[59]  & 0x000000ff)<< 16 | \
//                                 (buff[60]  & 0x000000ff) << 8 |  (buff[61]  & 0x000000ff)<< 0;
//         q->tx_nrz_code_pattern  =  buff[62] ;
//         q->tx_conv_code      =  (buff[63]  & 0x000000ff)<< 24 |  (buff[64]  & 0x000000ff)<< 16 | \
//                                 (buff[65]  & 0x000000ff) << 8 |  (buff[66]  & 0x000000ff)<< 0;

//     }
//     cc_flag = buff[127];
//     if( cc_flag == 0xcc)
//     {

//         q->rx_info_rate         = (buff[128]  & 0x000000ff)<< 24 |  (buff[129]  & 0x000000ff)<< 16 | \
//                                   (buff[130]  & 0x000000ff) << 8 |  (buff[131]  & 0x000000ff)<< 0;  
//         q->info_random_decode   = buff[132];

//         q->rx_nrz_code_pattern = buff[133];
 
//         q->rx_conv_code         =  buff[134] << 24 | buff[135] << 16 | buff[136] << 8 | buff[137] << 0;

//         q->rx_frame_sync_word   = buff[138] << 24 | buff[139] << 16 | buff[140] << 8 | buff[141] << 0;
//         q->rx_frame_sync_len    = buff[142];
//         q->rx_preamble_length   = buff[143];
//         q->rx_preamble_byte     = buff[144];

//         // q->uv_rx.trk_inte_select  = buff[145];
//         // q->uv_rx.sys_lpf_select   = buff[146];
//         // q->uv_rx.trk_inte_num     = (buff[147]  & 0x000000ff)<< 24 |  (buff[148]  & 0x000000ff)<< 16 | \
//         //                             (buff[149]  & 0x000000ff) << 8 |  (buff[150]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_bsyn_buf_num = buff[151];
//         // q->uv_rx.trk_bsyn_dot_num = buff[152];
//         // q->uv_rx.trk_inte_para_p  = (buff[153]  & 0x000000ff)<< 24 |  (buff[154]  & 0x000000ff)<< 16 | \
//         //                       (buff[155]  & 0x000000ff) << 8 |  (buff[156]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_step_1_para_c0 =  (buff[157]  & 0x000000ff)<< 24 |  (buff[158]  & 0x000000ff)<< 16 | \
//         //                          (buff[159]  & 0x000000ff) << 8 |  (buff[160]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_step_1_para_c1 =  (buff[161]  & 0x000000ff)<< 24 |  (buff[162]  & 0x000000ff)<< 16 | \
//         //                          (buff[163]  & 0x000000ff) << 8 |  (buff[164]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_step_1_para_c2 =  (buff[165]  & 0x000000ff)<< 24 |  (buff[166]  & 0x000000ff)<< 16 | \
//         //                          (buff[167]  & 0x000000ff) << 8 |  (buff[168]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_loop_inibsyn_num = (buff[169]  & 0x000000ff)<< 24 |  (buff[170]  & 0x000000ff)<< 16 | \
//         //                           (buff[171]  & 0x000000ff) << 8 |  (buff[172]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_loop_iniedge_num = (buff[173]  & 0x000000ff)<< 24 |  (buff[174]  & 0x000000ff)<< 16 | \
//         //                           (buff[175]  & 0x000000ff) << 8 |  (buff[176]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_loop_endedge_num = (buff[177]  & 0x000000ff)<< 24 |  (buff[178]  & 0x000000ff)<< 16 | \
//         //                           (buff[179]  & 0x000000ff) << 8 |  (buff[180]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_loop_instant_num = (buff[181]  & 0x000000ff)<< 24 |  (buff[182]  & 0x000000ff)<< 16 | \
//         //                           (buff[183]  & 0x000000ff) << 8 |  (buff[184]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_loop_lostclr_num = (buff[185]  & 0x000000ff)<< 24 |  (buff[186]  & 0x000000ff)<< 16 | \
//         //                           (buff[187]  & 0x000000ff) << 8 |  (buff[188]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_bsyn_para_num    = (buff[189]  & 0x000000ff)<< 24 |  (buff[190]  & 0x000000ff)<< 16 | \
//         //                           (buff[191]  & 0x000000ff) << 8 |  (buff[192]  & 0x000000ff)<< 0;
//         // q->uv_rx.trk_dete_phase_door  = (buff[193]  & 0x000000ff)<< 24 |  (buff[194]  & 0x000000ff)<< 16 | \
//         //                           (buff[195]  & 0x000000ff) << 8 |  (buff[196]  & 0x000000ff)<< 0;

//     }
//     if(memcmp(q,&uv_para_config,sizeof(struct uv_config_state)) != 0)
//     {
//         memcpy(&uv_para_config,q,sizeof(struct uv_config_state));
//         printf_uv_config(&uv_para_config);
//         uv_transfer_config_init();
//         uv_recv_config_init();
//     } 
//     //协议内容end
//     DEBUG("uv_config_updata_handler  exit\n"); 
//     free(q);
// }

// static void uv_config_sw_updata_handler(uint8_t *buff , uint16_t len)
// {
//     uint8_t chan_sw,tc_sw,tm_sw,cc_flag;

//     cc_flag = buff[0];
//     chan_sw = buff[1];
//     tm_sw   = buff[2];
//     tc_sw   = buff[3];

//     if(cc_flag == 0xcc)
//     {
//         uv_para_config.chan_en = chan_sw;
//         uv_para_config.carr_gen_en = tm_sw&0x01;
//         uv_para_config.info_en = (tm_sw&0x02 )>> 1;
//         config_switch_onoff(chan_sw,uv_para_config.chan_en?ENABLE:DISABLE);//通道开关
//         config_switch_onoff(info_sw,uv_para_config.info_en?ENABLE:DISABLE);//信息开关
//         config_switch_onoff(carr_ena_sw,uv_para_config.carr_gen_en?ENABLE:DISABLE);//载波开关
//         config_update_param(TX);
//     }
// }

// static void uv_config_rf_updata_handler(uint8_t *buff , uint16_t len)
// {
//     uint32_t tx_rf_frequency_t,rx_rf_frequency_t;
//     uint32_t db_t,rbw_t,tbw_t;

//     DEBUG("uv_config_rf_updata_handler  into\n"); 
//     db_t              = buff[1] << 8 | buff[2] << 0;
//     tbw_t             = buff[3] << 8 | buff[4] << 0;
//     tx_rf_frequency_t = buff[5] << 24 | buff[6] << 16 \
//                        |buff[7] << 8  | buff[8] << 0;
//     rbw_t             = buff[9] << 8 | buff[10] << 0;
//     rx_rf_frequency_t = buff[11] << 24 | buff[12] << 16 \
//                        |buff[13] << 8 | buff[14] << 0;
//     //协议内容start
//     if(buff[0] == 0xcc)
//     {
//         DEBUG("* uv_para_config.rxcfg.bw_hz : %dMHz\n",rbw_t);
//         DEBUG("* uv_para_config.rxcfg.lo_hz : %dHz\n",rx_rf_frequency_t);
//         DEBUG("* uv_para_config.txcfg.bw_hz : %dMHz\n",tbw_t);
//         DEBUG("* uv_para_config.txcfg.lo_hz : %dHz\n",tx_rf_frequency_t);
//         uv_para_config.rxcfg.bw_hz = MHZ(rbw_t);   // 2 MHz rf bandwidth
//         uv_para_config.rxcfg.lo_hz = rx_rf_frequency_t; // 2.5 GHz rf frequency
//         uv_para_config.txcfg.bw_hz = MHZ(tbw_t);   // 2 MHz rf bandwidth
//         uv_para_config.txcfg.lo_hz = tx_rf_frequency_t; // 2.5 GHz rf frequency
//         cfg_ad9361_streaming_ch(ctx, &uv_para_config.rxcfg, RX, 0); //config rx
//         cfg_ad9361_streaming_ch(ctx, &uv_para_config.txcfg, TX, 0); //config tx
//     }

//     //协议内容end
//     DEBUG("uv_config_updata_handler  exit\n"); 
// }

// static void uv_tc_msg_handler(uint8_t *buff , uint16_t len , uint8_t send_mode)
// {
//     uint8_t *pbuff;
//     float    c ;

//     DEBUG("uv_tc_msg_handler  into\n"); 
//     //协议内容start
    
//     pthread_mutex_lock(&(uv_ctrl->mutex));
//     uv_ctrl->send_mode = send_mode;
//     uv_ctrl->len = len - 3;
//     c = ((8*1e6)/uv_para_config.info_data_rate);
//     uv_ctrl->send_info_rate = (int)c;
//     uv_ctrl->send_interval  = (buff[0] << 8 | buff[1]) * 1e3 ;
//     memcpy(uv_ctrl->buff,&buff[2],len);
// 	pthread_mutex_unlock(&(uv_ctrl->mutex));
//     DEBUG("uv_ctrl->send_interval :%d\n",uv_ctrl->send_interval);
//     DEBUG("uv_ctrl->send_mode :%d\n",uv_ctrl->send_mode);
//     DEBUG("uv_ctrl->len :%d\n",uv_ctrl->len);
//     DEBUG("uv_ctrl->send_info_rate :%d\n",uv_ctrl->send_info_rate);
     
//     DEBUG("uv_tc_msg_handler  exit\n"); 
// }

 

// void uv_conifg_msg_handle(char *buf, int len)
// {
//     uint8_t send_mode;
//     uint8_t *buff;

//     buff = malloc(len);

//     memcpy(buff,buf,len);


//     if(buff[UV_FRMAE_HEADER_LEN+UV_FRAME_ID_LEN - 1] == UV_FRAME_CONFIG_ID)
//     {
//         DEBUG("* udp recv a new CONFIG frame \n");
//         uv_config_updata_handler(&buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN+UV_FRAME_ID_LEN],UV_FRAME_CONFIG_DATA_LENGTH);
//     }
//     else if(buff[UV_FRMAE_HEADER_LEN+UV_FRAME_ID_LEN - 1] == UV_FRAME_TC_DATA_ID) //协议高2位进行间隔、单帧发送设置
//     {
//                /* code */
//         len = buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN] << 8 | buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN+1];
//         send_mode = buff[UV_FRMAE_HEADER_LEN];
//         uv_tc_msg_handler(&buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN+UV_FRAME_ID_LEN],len,send_mode);
//     }
//     else if(buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN - 1] == UV_FRAME_RF_CONFIG_ID)
//     {
//         DEBUG("* udp recv a new rf-config frame \n");
//         uv_config_rf_updata_handler(&buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN+UV_FRAME_ID_LEN],UV_FRAME_RF_CONFIG_LENGTH);
//     }
//     else if(buff[UV_FRMAE_HEADER_LEN+UV_FRAME_ID_LEN - 1] == UV_FRAME_CTRL_CONFIG_ID)
//     {
//         DEBUG("* udp recv a new ctrl frame              \n");
//         uv_config_sw_updata_handler(&buff[UV_FRMAE_HEADER_LEN+UV_FRAME_LENGTH_LEN+UV_FRAME_ID_LEN ],UV_FRAME_CTRL_CONFIG_LENGTH);
//     }
//     else
//     {
//          DEBUG("* error: frame id is error :%d \n",buff[UV_FRMAE_HEADER_LEN+UV_FRAME_ID_LEN - 1]);
//     }

//     free(buff);
// }

void uv_configure_init_thread(void)
{
    uv_config_init( );
    uv_control_config_init();
    uv_transfer_config_init();
    uv_recv_config_init();
    // uv_config_queue_init();
}

void uv_configure_join_thread(void)
{
 
}