#include <json_config.h>


JsonConfig::JsonConfig(const std::string& config_path, bool auto_save)
    : config_path_(config_path), auto_save_(auto_save) {
  using json = nlohmann::json;
  try {
    std::ifstream read_config_file(config_path_);
    json_ = json::parse(read_config_file);
  } catch (const json::parse_error& e) {
    ReportJsonParseError(config_path_, e);
  }
}

JsonConfig::~JsonConfig() {
  if (!auto_save_) return;
  Save();
}

bool JsonConfig::IsArray(std::string key) {
  using json = nlohmann::json;
  try {
    json value = json_.at(key);
    return value.is_array();
  } catch (const json::exception&) {
    return false;
  }
}

void JsonConfig::Save() {
  try {
    std::ofstream write_config_file(config_path_);
    write_config_file.exceptions(std::ofstream::failbit);
    write_config_file << std::setw(2) << json_;
  } catch (const std::ios_base::failure& e) {
    LOG(ERROR) << "Write " << config_path_ << " Failed!\n" << e.what();
  }
}

void JsonConfig::Debug() const { LOG(0) << json_.dump(2); }

nlohmann::json JsonConfig::At(std::string key) const {
  nlohmann::json j;
  try {
    j = json_.at(key);
  } catch (const nlohmann::json::exception& e) {
    LOG(ERROR) << "read `" << key << "` error: " << e.what();
  }
  return j;
}

nlohmann::json JsonConfig::AtP(std::string key) const {
  using json_pointer = nlohmann::json::json_pointer;
  nlohmann::json j;
  try {
    j = json_.at(json_pointer(key));
  } catch (nlohmann::json::exception& e) {
    LOG(ERROR) << "read `" << key << "` error: " << e.what();
  }
  return j;
}

int64_t GetLineNumFromByte(const std::string& filename, size_t byte) {
  int64_t line_no = -1;
  std::ifstream file(filename);
  std::string buf(byte, '\0');
  if (file.read(&buf[0], byte)) {
    std::istringstream iss(buf);
    line_no = std::count(std::istreambuf_iterator<char>(iss),
                         std::istreambuf_iterator<char>(), '\n') +
              1;
  }
  return line_no;
}

void ReportJsonParseError(const std::string& filename,
                          const nlohmann::json::parse_error& e) {
  auto error_line = GetLineNumFromByte(filename, e.byte);
  if (error_line == -1) {
    LOG(ERROR) << "`" << filename << "` does not existed or is empty.\n";
  } else {
    LOG(ERROR) << "Parsing `" << filename << "` error at line(" << error_line
               << ")\n"
               << e.what();
  }
}
