//
// Created by jin.ma on 7/18/2019
//

#ifndef TOOLKITS_UTILS_TIMING_TIMESTAMP_H_
#define TOOLKITS_UTILS_TIMING_TIMESTAMP_H_

#include <string>

namespace Utils {

typedef int64_t TimeStamp;

/**
 * Get time stamp, in millisecond
 * @return time stamp in millisecond
 */
TimeStamp GetTimeStamp(void);

/**
* Get locat data time in formation "yyyy-MM-dd hh:mm:ss.zzz"
* @return local data time in std::string
*/
std::string GetLocalDataTime(void);

class TimeRegister {
 protected:
  TimeStamp toc_;
  std::string info_;
  const char* file;
  int line;
  int verbose_;

 public:
  TimeRegister(const std::string info, int verbose, const char* file, int line);
  ~TimeRegister();
};

#define TIME_REGISTER_VERBOSE(info, verbose) \
  TimeRegister timer(info, verbose, __FILE__, __LINE__)
#define TIME_REGISTER(info) TIME_REGISTER_VERBOSE(info, LOG_VERBOSE_LOW)

}  // end of namespace Utils

#endif  // TOOLKITS_UTILS_TIMING_TIMESTAMP_H_
