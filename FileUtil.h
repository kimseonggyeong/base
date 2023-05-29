/**
 * File Utils
 *
 * Author: KIM SEONG GYEONG < seonggyeong.kim@medithinq.com >
 */
#ifndef __FILE_UTIL_H_
#define __FILE_UTIL_H_

#include <sys/stat.h>

namespace FileUtil
{

int copy(const char* src, const char* dest);

int mkdir(const char* path, mode_t mode);

int rm(const char* path);

} // namespace FileUtil

#endif /* __FILE_UTIL_H_ */
