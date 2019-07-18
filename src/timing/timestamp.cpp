//
// Created by jin.ma on 7/18/2019
//

#include <timestamp.h>

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <stdio.h>
#include <time.h>


namespace Utils {

TimeStamp GetTimeStamp() {
#ifdef _WIN32
  {
    time_t tt = time(NULL);
    SYSTEMTIME systime;
    GetLocalTime(&systime);

    struct tm *tm_time;
    tm_time = localtime(&tt);

    return tt * 1000LL + systime.wMilliseconds;
  }
#elif defined(__linux__) || defined(__ANDROID__) || defined(LINUX) \
  || defined(__QNX__)
  {
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    return (static_cast<int64_t>(curr_time.tv_sec) * 1000LL
            + static_cast<int64_t>(curr_time.tv_usec) / 1000LL);
  }
#elif defined(__MACH__)
#include <chrono>
  std::chrono::time_point<std::chrono::high_resolution_clock> now =
    std::chrono::high_resolution_clock::now();
  TimeStamp microsecs =
    std::chrono::duration_cast<std::chrono::milliseconds>(
                                  now.time_since_epoch()).count();
  return microsecs;
#else
  REPORT_ERROR_POSITION;
#endif
}

TimeRegister::TimeRegister(const std::string info, int verbose,
             const char * file, int line) {
  info_ = info;
  verbose_ = verbose;
  toc_ = GetTimeStamp();
  this->line = line;
  this->file = file;
}

TimeRegister::~TimeRegister() {
  toc_ = GetTimeStamp() - toc_;
}

}  // end of namespace Utils
