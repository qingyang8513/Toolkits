#ifndef _JSON_CONFIG_H_
#define _JSON_CONFIG_H_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <exception>
#include "json.hpp"

#define LOG(level) std::cout


typedef enum {
    INFOR = 0,
    ERROR = 1,
    DEBUG = 2
} LOG_LEVEL;

int64_t GetLineNumFromByte(const std::string& filename, size_t byte);
void ReportJsonParseError(const std::string& filename, const nlohmann::json::parse_error& e);

class JsonConfig {
 public:
  explicit JsonConfig(const std::string& config_path, bool auto_save = false);
  ~JsonConfig();

  template <typename T>
  T Get(const std::string& key, const T& default_value) {
    try {
      return json_.value(key, default_value);
    } catch (nlohmann::json::exception& e) {
      LOG(ERROR) << "read `" << key << "` error: " << e.what();
      return default_value;
    }
  }
  template <typename T>
  T GetByPointer(std::string key, const T& default_value) {
    using json_pointer = nlohmann::json::json_pointer;
    try {
      return json_.value(json_pointer(key), default_value);
    } catch (nlohmann::json::exception& e) {
      LOG(ERROR) << "read `" << key << "` error: " << e.what();
      return default_value;
    }
  }
  template <typename T>
  void Set(const std::string& key, const T& value) {
    json_[key] = value;
  }
  template <typename T>
  void SetByPointer(std::string key, const T& value) {
    using json_pointer = nlohmann::json::json_pointer;
    json_[json_pointer(key)] = value;
  }

  bool IsArray(std::string key);
  void Save();
  void Debug() const;

  nlohmann::json At(std::string key) const;
  nlohmann::json AtP(std::string key) const;

 private:
  nlohmann::json json_;
  std::string config_path_;
  bool auto_save_ = false;
};

#endif  // _JSON_CONFIG_H_
