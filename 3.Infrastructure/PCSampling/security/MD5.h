
#ifndef MD5_H
#define MD5_H

#include <string>
#include <cstring>

std::string md5check(std::string dat);
std::string md5check(const void* dat, size_t len);
std::string md5file(const char* filename);
std::string md5file(std::FILE* file);
std::string md5sum6(std::string dat);
std::string md5sum6(const void* dat, size_t len);


#endif // end of MD5_H
