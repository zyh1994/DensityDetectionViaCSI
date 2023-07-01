#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../inc/FileStorage.h"
#include "../inc/SHA.h"


std::string calculateDataHash(uint8_t* data, size_t size) {
    SHA256 sha256;
    sha256.update(data, size);
    auto digest = sha256.digest();
    std::string hashCode = SHA256::toString(digest);
    delete[] digest;

    return hashCode;
}

std::string calculateFileHash(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        // 文件打开失败，返回错误或抛出异常
        return "";
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取文件内容到缓冲区
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // 关闭文件
    file.close();

    // 计算哈希值
    std::string hashCode = calculateDataHash(buffer.data(), buffer.size());

    return hashCode;
}

void displayBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Failed to open the file." << std::endl;
        return;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 读取文件内容到缓冲区
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    // 关闭文件
    file.close();

    // 以十六进制显示每个字节的值
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
    }
}

int main() {

//    displayBinaryFile("data_20230702021310.bin");

    FileStorage storage("data");

    // 生成一些数据
    unsigned char data[1024];
    for (int i = 0; i < 1024; ++i) {
        data[i] = static_cast<unsigned char>(i % 256);
    }

    // 生成临时数据用于最后的比对
    auto* tempData = new unsigned char[1024 * 1000];

    // 写入数据
    for (int i = 0; i < 1000; ++i) {
        storage.write(data, sizeof(data));

        // 保存数据用于最后的比对
        std::memcpy(tempData + i * sizeof(data), data, sizeof(data));
    }

    // 刷新并关闭文件
    storage.flush();

    // 等待刷新线程结束
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 输出文件名
    std::cout << "File Names:" << std::endl;
    for (auto& filename : storage.get_filenames()) {
        std::cout << filename << std::endl;
    }

    std::cout << "----------------" << std::endl;

    // 计算文件的哈希码
    std::string expectedHash =  calculateDataHash(tempData, 1024 * 1000);
    std::string generatedHash = calculateFileHash(storage.get_filenames()[0]);

    std::cout << "Expected Hash: " << expectedHash << std::endl;
    std::cout << "Generated Hash: " << generatedHash << std::endl;

    // 判断哈希码是否匹配
    if (expectedHash == generatedHash) {
        std::cout << "File hash matches the expected hash." << std::endl;
    } else {
        std::cout << "File hash does not match the expected hash." << std::endl;
    }

    return 0;
}
