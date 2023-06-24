#include "mex.h"

// 定义常量
#define TONE_40M 114
#define BITS_PER_BYTE 8
#define BITS_PER_COMPLEX_SYMBOL (2 * BITS_PER_SYMBOL)
#define BITS_PER_SYMBOL      10

// 定义复数结构体
typedef struct
{
    int real; // 实部
    int imag; // 虚部
}COMPLEX;

// 负数转换函数
int signbit_convert(int data, int maxbit)
{
    // 如果数据的最高位为1，说明是负数，需要做转换
    if (data & (1 << (maxbit - 1))) 
    { 
        data -= (1 << maxbit);
    }
    return data;
}

// MATLAB调用的主函数
void mexFunction(int nlhs, mxArray *plhs[],
            int nrhs, const mxArray *prhs[])
{
    unsigned char *local_h;
    unsigned int  *csi_len_p,csi_len;
    unsigned int  *nr_p,nr;
    unsigned int  *nc_p,nc;
    unsigned int  *num_tones_p,num_tones;

    unsigned int  bitmask, idx, current_data;
    unsigned int  h_data, h_idx;
    

    int  k;
    int  real, imag;
    int  bits_left, nc_idx, nr_idx;
    
    // 检查输入输出参数的正确性
    if(nrhs!=4) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_csi_new:nrhs","Four input required.");
    }
    if(nlhs!=1) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_csi_new:nlhs","One output required.");
    }
    // 确保输入参数是字符数组
    if (!mxIsClass(prhs[0], "uint8")) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_csi_new:notBytes","Input must be a char array");
    }
  
    // 获取输入数据
    local_h = mxGetData(prhs[0]);

    // 获取接收天线数、发送天线数和子载波数
    nr_p  = mxGetPr(prhs[1]);
    nr    = *nr_p;

    nc_p  = mxGetPr(prhs[2]);
    nc    = *nc_p;

    num_tones_p = mxGetPr(prhs[3]);
    num_tones   = *num_tones_p;

    // 创建输出数据的结构
    const mwSize size[]  = {nr, nc, num_tones};

    mxArray *csi  = mxCreateNumericArray(3, size, mxDOUBLE_CLASS, mxCOMPLEX);
    double * ptrR =(double *)mxGetPr(csi);
    double * ptrI =(double *)mxGetPi(csi);
    
    bits_left = 16; // 每次处理16位数据

    // 为H的实部和虚部创建10位掩码
    bitmask = (1 << BITS_PER_SYMBOL) - 1;
    idx = h_idx = 0;
    h_data = local_h[idx++];
    h_data += (local_h[idx++] << BITS_PER_BYTE);
    current_data = h_data & ((1 << 16) - 1); // 先获取16位最低位数据
    
    // 循环处理每个子载波、每个发送天线、每个接收天线的数据
    for (k = 0; k < num_tones; k++) {
        for (nc_idx = 0; nc_idx < nc; nc_idx++) {
            for (nr_idx = 0; nr_idx < nr; nr_idx++) {
                // 如果剩余位数不足一个符号的位数，获取下一组16位数据
                if ((bits_left - BITS_PER_SYMBOL) < 0) {
                    h_data = local_h[idx++];
                    h_data += (local_h[idx++] << BITS_PER_BYTE);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }
                // 获取虚部数据并进行负数转换
                imag = current_data & bitmask;
                
                imag = signbit_convert(imag, BITS_PER_SYMBOL);
		        *ptrI = (double) imag;
		        ++ptrI;
                bits_left -= BITS_PER_SYMBOL;
                // 移出已使用的数据
                current_data = current_data >> BITS_PER_SYMBOL; 

                // 同样处理实部数据
                if ((bits_left - BITS_PER_SYMBOL) < 0) {
                    h_data = local_h[idx++];
                    h_data += (local_h[idx++] << BITS_PER_BYTE);
                    current_data += h_data << bits_left;
                    bits_left += 16;
                }
                real = current_data & bitmask;
                
                real = signbit_convert(real, BITS_PER_SYMBOL);
		        *ptrR = (double) real;
		        ++ptrR;
                bits_left -= BITS_PER_SYMBOL;
                
                // 移出已使用的数据
                current_data = current_data >> BITS_PER_SYMBOL;
                
            }
        }
    }
    // 返回结果
    plhs[0] = csi;
}