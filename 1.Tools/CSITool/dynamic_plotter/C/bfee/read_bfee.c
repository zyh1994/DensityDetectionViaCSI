#include "mex.h"

/* 该函数实现了读取波束成形矩阵的操作 */
void read_bfee(unsigned char *inBytes, mxArray *outCell)
{
    // 解析输入字节流，得到有用的信息
    unsigned int timestamp_low = inBytes[0] + (inBytes[1] << 8) +
        (inBytes[2] << 16) + (inBytes[3] << 24);
    unsigned short bfee_count = inBytes[4] + (inBytes[5] << 8);
    unsigned int Nrx = inBytes[8];
    unsigned int Ntx = inBytes[9];
    unsigned int rssi_a = inBytes[10];
    unsigned int rssi_b = inBytes[11];
    unsigned int rssi_c = inBytes[12];
    char noise = inBytes[13];
    unsigned int agc = inBytes[14];
    unsigned int antenna_sel = inBytes[15];
    unsigned int len = inBytes[16] + (inBytes[17] << 8);
    unsigned int fake_rate_n_flags = inBytes[18] + (inBytes[19] << 8);

    // 计算波束成形矩阵的长度
    unsigned int calc_len = (30 * (Nrx * Ntx * 8 * 2 + 3) + 7) / 8;
    unsigned int i, j;
    unsigned int index = 0, remainder;
    unsigned char *payload = &inBytes[20];
    char tmp;

    // 创建存储波束成形矩阵的数组
    mwSize size[] = {Ntx, Nrx, 30};
    mxArray *csi = mxCreateNumericArray(3, size, mxDOUBLE_CLASS, mxCOMPLEX);
    mwSize perm_size[] = {1, 3};
    mxArray *perm = mxCreateNumericArray(2, perm_size, mxDOUBLE_CLASS, mxREAL);
    double* ptrR = (double *)mxGetPr(csi);
    double* ptrI = (double *)mxGetPi(csi);

    // 检查波束成形矩阵的长度是否正确
    if (len != calc_len)
        mexErrMsgIdAndTxt("MIMOToolbox:read_bfee_new:size","Wrong beamforming matrix size.");

    // 计算波束成形矩阵
    for (i = 0; i < 30; ++i)
    {
        index += 3;
        remainder = index % 8;
        for (j = 0; j < Nrx * Ntx; ++j)
        {
            tmp = (payload[index / 8] >> remainder) |
                (payload[index/8+1] << (8-remainder));
            *ptrR = (double) tmp;
            ++ptrR;
            tmp = (payload[index / 8+1] >> remainder) |
                (payload[index/8+2] << (8-remainder));
            *ptrI = (double) tmp;
            ++ptrI;
            index += 16;
        }
    }

    // 计算天线置换矩阵
    ptrR = (double *)mxGetPr(perm);
    ptrR[0] = ((antenna_sel) & 0x3) + 1;
    ptrR[1] = ((antenna_sel >> 2) & 0x3) + 1;
    ptrR[2] = ((antenna_sel >> 4) & 0x3) + 1;

    // 清空结构体中已有的数据，重新设置值
    mxDestroyArray(mxGetField(outCell, 0, "timestamp_low"));
    mxDestroyArray(mxGetField(outCell, 0, "bfee_count"));
    mxDestroyArray(mxGetField(outCell, 0, "Nrx"));
    mxDestroyArray(mxGetField(outCell, 0, "Ntx"));
    mxDestroyArray(mxGetField(outCell, 0, "rssi_a"));
    mxDestroyArray(mxGetField(outCell, 0, "rssi_b"));
    mxDestroyArray(mxGetField(outCell, 0, "rssi_c"));
    mxDestroyArray(mxGetField(outCell, 0, "noise"));
    mxDestroyArray(mxGetField(outCell, 0, "agc"));
    mxDestroyArray(mxGetField(outCell, 0, "perm"));
    mxDestroyArray(mxGetField(outCell, 0, "rate"));
    mxDestroyArray(mxGetField(outCell, 0, "csi"));

    // 将得到的信息存储到结构体中
    mxSetField(outCell, 0, "timestamp_low", mxCreateDoubleScalar((double)timestamp_low));
    mxSetField(outCell, 0, "bfee_count", mxCreateDoubleScalar((double)bfee_count));
    mxSetField(outCell, 0, "Nrx", mxCreateDoubleScalar((double)Nrx));
    mxSetField(outCell, 0, "Ntx", mxCreateDoubleScalar((double)Ntx));
    mxSetField(outCell, 0, "rssi_a", mxCreateDoubleScalar((double)rssi_a));
    mxSetField(outCell, 0, "rssi_b", mxCreateDoubleScalar((double)rssi_b));
    mxSetField(outCell, 0, "rssi_c", mxCreateDoubleScalar((double)rssi_c));
    mxSetField(outCell, 0, "noise", mxCreateDoubleScalar((double)noise));
    mxSetField(outCell, 0, "agc", mxCreateDoubleScalar((double)agc));
    mxSetField(outCell, 0, "perm", perm);
    mxSetField(outCell, 0, "rate", mxCreateDoubleScalar((double)fake_rate_n_flags));
    mxSetField(outCell, 0, "csi", csi);
}

/* The gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray prhs[])
{
    // 定义结构体字段
    const char fieldnames[] = {"timestamp_low",
    "bfee_count",
    "Nrx", "Ntx",
    "rssi_a", "rssi_b", "rssi_c",
    "noise",
    "agc",
    "perm",
    "rate",
    "csi"};

    unsigned char inBytes; // 波束成形矩阵
    mxArray outCell; // 结构体输出

    // 检查输入输出参数是否正确
    if(nrhs!=1) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_bfee_new:nrhs","One input required.");
    }
    if(nlhs!=1) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_bee_new:nlhs","One output required.");
    }

    // 检查输入参数是否为 uint8 类型的数据
    if (!mxIsClass(prhs[0], "uint8")) {
        mexErrMsgIdAndTxt("MIMOToolbox:read_bfee_new:notBytes","Input must be a char array");
    }

    // 将输入的 uint8 类型数据转化为 unsigned char 类型的数据
    inBytes = mxGetData(prhs[0]);

    // 创建输出结构体
    outCell = mxCreateStructMatrix(1, 1, 12, fieldnames);

    // 调用 read_bfee 函数计算波束成形矩阵
    read_bfee(inBytes,outCell);

    // 将输出结构体作为输出参数
    plhs[0] = outCell;
}
