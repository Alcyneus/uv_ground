/*
 * @Descripttion:   这是一个UV接收基带参数计算的源程序
 * @version:  v1.0
 * @author: CHEN LEI
 * @Date: 2020-04-15 02:16:27
 * @LastEditors: CHEN LEI
 * @LastEditTime: 2020-08-31 20:28:05
 */
#include <stdlib.h>
#include <math.h>

#include "baseband_param_calc.h"
 

#define FPGA_PAR_WIDTH   (double)(26.0) 
#define FPGA_DET_WIDTH   (double)(18.0)   
#define FPGA_DDS_WIDTH   (double)(32.0)   

#define PLL_PHASE_DOOR   (double)(30.0)   
 

#define PI               (double)(3.1415926)


 
struct loopfilterparainte_calout
{
    double calc_max_val;
    double calc_min_val;
    double calc_max_bit;
    double calc_min_bit;
    double calc_all_bit;
};

 
/*
*   环路参数，滤波器进行
*/
static void loop_filter3(double *res,double Bn,double Ts)
{
    double Wn,b0,b1,b2,A3,B3;

    A3 = 1.10;
    B3 = 2.40; 
    Wn = Bn / 0.7845;

    b0 = B3*Wn + ((A3*pow(Wn,2.0) * Ts)/2.0)+ ((pow(Wn,3.0) * pow(Ts,2.0))/4.0);
    b1 = -2.0*B3*Wn + ((pow(Wn,3.0) * pow(Ts,2.0))/2.0);
    b2 = B3*Wn - ((A3*pow(Wn,2.0) * Ts)/2.0)+ ((pow(Wn,3.0) * pow(Ts,2.0))/4.0);

    res[0] = b0;
    res[1] = b1 + 2.0*b0;
    res[2] = b2 - b0;
}


/*
*   这是一个比较绝对值最大的函数
*/
static double ismax_abs(double* array , int len)
{
    double max_value  = 0;
    int i;

    max_value = array[0];
    for(i = 0 ; i <  len ; i ++)
    {
        if(fabs(array[i]) >= fabs(max_value))
        {
            max_value = fabs(array[i]);
        }
    }
    return max_value;
}

/*
*   这是一个比较绝对值最小的函数
*/
static double ismin_abs(double* array , int len)
{
    double min_value  = 0;
    int i;

    min_value = array[0];
    for(i = 0 ; i <  len ; i ++)
    {
        if(fabs(array[i]) > fabs(min_value))
        {
           continue;
        }
         min_value = fabs(array[i]);
    }
    return min_value;
}

/*
*   这是一个计算环路滤波积分的函数 
*/
static void loop_filter_inte(struct loopfilterparainte_calout *calout, double coef1,double coef2 , double Fs , double exp_par) //3阶环路滤波器系数精细化量化计算
{
    double Tcoh ;
    double *pres,*bhz ;
    uint8_t i = 0;

    pres = malloc(sizeof(double)*12);
    bhz  = malloc(sizeof(double)*2);

    Tcoh = 0.5/(400 * 1e3);
    
    bhz[0] = coef1 / Tcoh;

    bhz[1] = coef2 / Tcoh;

    loop_filter3(&pres[0],bhz[0],Tcoh);
    for( i = 0 ; i < 3 ; i ++)
    {
        pres[0+i] = pres[0+i]*pow(2,exp_par)/Fs;
    }

    loop_filter3(&pres[3],bhz[1],Tcoh);
    for( i = 0 ; i < 3 ; i ++)
    {
        pres[3+i] = pres[3+i]*pow(2,exp_par)/Fs;
    }

    Tcoh = 0.5/(0.5 * 1e3);
    
    bhz[0] = coef1 / Tcoh;

    bhz[1] = coef2 / Tcoh;

    loop_filter3(&pres[6],bhz[0],Tcoh);
    for( i = 0 ; i < 3 ; i ++)
    {
        pres[6+i] = pres[6+i]*pow(2,exp_par)/Fs;
    }
    
    loop_filter3(&pres[9],bhz[1],Tcoh);
    for( i = 0 ; i < 3 ; i ++)
    {
        pres[9+i] = pres[9+i]*pow(2,exp_par)/Fs;
    }

    double *max,*min , outmax , outmin;
    max = malloc(sizeof(double)*4);
    min = malloc(sizeof(double)*4);

    max[0] = ismax_abs(&pres[0],3);
    max[1] = ismax_abs(&pres[3],3);
    max[2] = ismax_abs(&pres[6],3);
    max[3] = ismax_abs(&pres[9],3);
    
    outmax = ismax_abs(max,4);

    if(outmax < 1)
    {
        calout->calc_max_bit = 0;
    }
    else
    {
        calout->calc_max_bit = ceil(log2(outmax));
    }
    

    min[0] = ismin_abs(&pres[0],3);
    min[1] = ismin_abs(&pres[3],3);
    min[2] = ismin_abs(&pres[6],3);
    min[3] = ismin_abs(&pres[9],3);

    outmin = ismin_abs(min,4);

    if(outmin > 1)
    {
        calout->calc_min_bit = 0;
    }
    else
    {
        calout->calc_min_bit = ceil(fabs(log2(outmin)));
    } 

    calout->calc_all_bit = calout->calc_max_bit +  calout->calc_min_bit  + 1 ;
    calout->calc_max_val = outmax;
    calout->calc_min_val = outmin;
 
    free(max);
    free(min);
    free(pres);
    free(bhz);
}



/*
*   这是一个计算UV基带参数的函数 原matlab代码详见：Sys_Debug_BPSK_CMD_v4 不懂问万杰
*/
void uv_baseband_para_calc(struct uv_rx_para_config *uv_para,double fb,double fs,double npb)
{
    double      pllTcoh;
    uint32_t    pll_n_sample;
    uint8_t     i = 0;
    uint32_t    trk_inte_coh_select;
    double      PLL_BL;
    double      FPGA_NOR_WIDTH;
    struct loopfilterparainte_calout calout;

    pllTcoh = npb/fb;                   
 
    pll_n_sample = round(pllTcoh*fs);

    trk_inte_coh_select = 13 - ceil(log2((pow(2,15) - 1) * 23170 * 2 / pow(2,16) * pll_n_sample / pow(2,15) ));

    FPGA_NOR_WIDTH = floor(log2((pow(2,(FPGA_DET_WIDTH-1)) - 1)*2*PI*0.25));
 
    PLL_BL = 0.15 / pllTcoh;
 
    loop_filter_inte(&calout,4e-1,2e-3,fs,FPGA_PAR_WIDTH);

    double *res ;
    double *trk_step_1_para;

    res = malloc(3*sizeof(double));
    if(res == NULL)
    {
        free(res);
        return;
    }
    trk_step_1_para = malloc(3*sizeof(double));
    if(trk_step_1_para == NULL)
    {
        free(trk_step_1_para);
        return;
    }
    loop_filter3(res,PLL_BL,pllTcoh);
    for(i = 0 ; i < 3 ; i ++)
    {
        trk_step_1_para[i] = round((res[i]*pow(2,FPGA_PAR_WIDTH)/fs));
        if( trk_step_1_para[i] < 0)
        {
            trk_step_1_para[i] = trk_step_1_para[i] + pow(2,calout.calc_all_bit);
        }
    }
 
    uv_para->trk_bsyn_dot_num     = (int)round(1/npb) - 1;
    uv_para->trk_bsyn_buf_num     = (int)(1*round(1/npb)) - 1;
    uv_para->trk_inte_num         = (int)round(pow(2,32) * (fb * 2) / fs);
    uv_para->trk_inte_para_p      = (int)round((pow(2,FPGA_NOR_WIDTH) / (2*PI))/1);
    uv_para->trk_inte_select      = (int)trk_inte_coh_select & 0x0f;;
    uv_para->trk_bsyn_para_num    = (int)floor(round(pow(2,32)*((fb*2)/fs))/pow(2,8));
    uv_para->trk_dete_phase_door  = round(PLL_PHASE_DOOR*PI/180*pow(2,FPGA_DET_WIDTH-3));
    uv_para->trk_step_1_para_c0   = (int)trk_step_1_para[0];
    uv_para->trk_step_1_para_c1   = (int)trk_step_1_para[1];
    uv_para->trk_step_1_para_c2   = (int)trk_step_1_para[2];
    uv_para->trk_loop_endedge_num = 1600;
    uv_para->trk_loop_inibsyn_num = 900;
    uv_para->trk_loop_iniedge_num = 1200;
    uv_para->trk_loop_lostclr_num = 10000;
    uv_para->trk_loop_instant_num = 2000;
    uv_para->trk_lost_debug_door  = 1000000;
    free(res);
    free(trk_step_1_para);
} 



// void main(void)
// {
//     struct uv_para_config p;
//     uv_baseband_para_calc(&p,1200.0,10e6,0.5);

//     printf("uv_para->trk_step_1_para_c0:%d\n",p.trk_step_1_para_c0);
//     printf("uv_para->trk_step_1_para_c1:%d\n",p.trk_step_1_para_c1);
//     printf("uv_para->trk_step_1_para_c2:%d\n",p.trk_step_1_para_c2);

//     printf("uv_para->trk_bsyn_para_num:%d\n",p.trk_bsyn_para_num);
//     printf("uv_para->trk_inte_select:%d\n",p.trk_inte_select);
//     printf("uv_para->trk_inte_para_p:%d\n",p.trk_inte_para_p);

//     printf("uv_para->trk_bsyn_dot_num:%d\n",p.trk_bsyn_dot_num);
//     printf("uv_para->trk_bsyn_buf_num:%d\n",p.trk_bsyn_buf_num);
//     printf("uv_para->trk_inte_num:%d\n",p.trk_inte_num);
 
 
 
//     return;
// }

// void main(void)
// {
//     struct uv_para_config *p;

//     p = malloc(sizeof(struct uv_para_config ));

//     uv_baseband_para_calc(p,1200.0,10e6,0.5);

//     printf("uv_para->trk_step_1_para_c0:%d\n",p->trk_step_1_para_c0);
//     printf("uv_para->trk_step_1_para_c1:%d\n",p->trk_step_1_para_c1);
//     printf("uv_para->trk_step_1_para_c2:%d\n",p->trk_step_1_para_c2);

//     printf("uv_para->trk_bsyn_para_num:%d\n",p->trk_bsyn_para_num);
//     printf("uv_para->trk_inte_select:%d\n",p->trk_inte_select);
//     printf("uv_para->trk_inte_para_p:%d\n",p->trk_inte_para_p);
//     printf("uv_para->trk_dete_phase_door:%d\n",p->trk_dete_phase_door);

//     printf("uv_para->trk_bsyn_dot_num:%d\n",p->trk_bsyn_dot_num);
//     printf("uv_para->trk_bsyn_buf_num:%d\n",p->trk_bsyn_buf_num);
//     printf("uv_para->trk_inte_num:%d\n",p->trk_inte_num);
 
//     free(p);
//     return;
// }


