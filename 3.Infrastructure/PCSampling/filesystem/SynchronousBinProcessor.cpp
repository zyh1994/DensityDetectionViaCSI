//
// Created by Seagosoft on 8/20/2023.
//



#include "SynchronousBinProcessor.h"

SynchronousBinProcessor::SynchronousBinProcessor() {

    // buffer for csi raw data
    bin_buf_main = new char[BUF_SIZE];
    bin_buf_swap = new char[BUF_SIZE];

    // flag for continuing the loop
    is_continue = true;

    // time recorder for last time flashed
    last_updated = std::chrono::system_clock::now();

    // 写入文件的数据大小
    buf_data_size = 0;

    // 缓存剩余空间大小
    buf_left_size = BUF_SIZE;

    // start the backend thread

}

SynchronousBinProcessor::~SynchronousBinProcessor() {

    // set the loop to false to end the loop
    is_continue = false;



    //
    delete bin_buf_main;

}

void SynchronousBinProcessor::open_file() {

}

void SynchronousBinProcessor::close_file() {

}

void SynchronousBinProcessor::append_data(long long int timestamp, cv::Mat &mat) {

}

void SynchronousBinProcessor::append_data(long long int timestamp, char *buf, size_t data_size) {

}

void SynchronousBinProcessor::save_data_to_bin(std::vector<cv::Mat> cv_frames, char *ptr_raw, size_t raw_size) {

}
