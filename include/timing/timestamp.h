/**
 * Copyright 2019 all rights reserved
 * @brief
 * @date 22/Aug/2019
 * @author jin.ma
 */

#ifndef TIMING_TIMESTAMP_H_
#define TIMING_TIMESTAMP_H_

#include <string>

namespace Utils {

#define LOG_VERBOSE_HIGHEST 5
#define LOG_VERBOSE_HIGH 4
#define LOG_VERBOSE_NORMAL 3
#define LOG_VERBOSE_LOW 2
#define LOG_VERBOSE_LOWEST 1
#define LOG_VERBOSE_NONE 0

typedef int64_t TimeStamp;

/**
 * Get time stamp, in millisecond
 * @return time stamp in millisecond
 */
TimeStamp GetTimeStamp(void);

/**
 * Get local data time in formation "yyyy-MM-dd hh:mm:ss.zzz"
 * @return local data time in std::string
 */
std::string GetLocalDataTime(void);

class TimeRegister {
 protected:
  TimeStamp expend_;
  std::string info_;
  int verbose_;
  const char* file_;
  int line_;

 public:
  TimeRegister(const std::string info, int verbose, const char* file, int line);
  ~TimeRegister();
};

#define TIME_REGISTER_VERBOSE(info, verbose) \
  TimeRegister timer(info, verbose, __FILE__, __LINE__)
#define TIME_REGISTER(info) TIME_REGISTER_VERBOSE(info, LOG_VERBOSE_LOW)

}  // namespace Utils

#endif  // TIMING_TIMESTAMP_H_
