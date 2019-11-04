/**
 * Copyright 2019 all rights reserved
 * @brief
 * @date 22/Aug/2019
 * @author jin.ma
 */

#include "timestamp.h"

#include <iostream>
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
    time_t tt = time(nullptr);
    SYSTEMTIME systime;
    GetLocalTime(&systime);

    struct tm *tm_time;
    tm_time = localtime(&tt);

    return tt * 1000LL + systime.wMilliseconds;
  }
#elif defined(__linux__) || defined(__ANDROID__) || defined(LINUX) || \
    defined(__QNX__)
  {
    struct timeval curr_time;
    gettimeofday(&curr_time, nullptr);
    return (static_cast<int64_t>(curr_time.tv_sec) * 1000LL +
            static_cast<int64_t>(curr_time.tv_usec) / 1000LL);
  }
#elif defined(__MACH__)
#include <chrono>
  std::chrono::time_point<std::chrono::high_resolution_clock> now =
      std::chrono::high_resolution_clock::now();
  TimeStamp microsecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                            .count();
  return microsecs;
#else
  return -1;
#endif
}

std::string GetLocalDataTime(void) {
#ifdef _WIN32
  time_t tt = time(nullptr);
  SYSTEMTIME systime;
  GetLocalTime(&systime);

  struct tm *tm_time = localtime(&tt);
  char str_time[255] = {0};
  snprintf(str_time, sizeof(str_time), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
           tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
           tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec,
           static_cast<int>(systime.wMilliseconds % 1000));
  return std::move<std::string>(std::string(str_time));

#elif defined(__linux__)
  {
    time_t tt;
    time(&tt);
    // tt = tt + 8 * 3600;  // Transform the time zone
    tm* tm_time = gmtime(&tt);
    struct timeval curr_time;
    gettimeofday(&curr_time, nullptr);

    char str_time[255] = {0};
    snprintf(str_time, sizeof(str_time), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
             tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec,
             static_cast<int>(curr_time.tv_usec / 1000));
    return std::move<std::string>(std::string(str_time));
  }

#elif defined(__ANDROID__) || defined(LINUX) || defined(__QNX__)
  { return std::string(); }
#elif defined(__MACH__)
#include <chrono>
  { return std::string(); }
#else
  { return std::string(); }
#endif
}

TimeRegister::TimeRegister(const std::string info, int verbose,
                           const char *file, int line)
    : info_(info), verbose_(verbose), file_(file), line_(line) {
  expend_ = GetTimeStamp();
}

TimeRegister::~TimeRegister() {
  expend_ = GetTimeStamp() - expend_;
  // Log print out, you can use glog instead.
  std::cout << "[Level " << verbose_ << " " << file_ << ": " << line_ << "] "
            << expend_ << "ms " << info_ << std::endl;
}

}  // namespace Utils
