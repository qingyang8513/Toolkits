/**
 * Copyright 2019 all rights reserved
 * @date 18/Jul/2019
 * @author jin.ma
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <json.hpp>
#include <vector>

using json = nlohmann::json;

std::vector<char> ReadFile(const std::string &file_name) {
  std::vector<char> data;
  std::ifstream fs(file_name, std::ios::in);

  if (!fs.is_open()) {
    return data;
  }

  fs.seekg(0, std::ios::end);
  if (!fs.good()) {
    return data;
  }

  auto nlen = static_cast<int64_t>(fs.tellg());
  if (nlen <= 0) {
    fs.close();
    return data;
  }

  fs.seekg(0, std::ios::beg);
  if (!fs.good()) {
    return data;
  }

  data.resize(static_cast<size_t>(nlen));
  fs.read(data.data(), nlen);

  return std::move(data);
}

int main(int argc, char *argv[]) {
  // Ex1: Create a JSON object
  json j = {{"pi", 3.141},
            {"happy", true},
            {"name", "Niels"},
            {"nothing", nullptr},
            {"answer", {{"everything", 42}}},
            {"list", {1, 0, 2}},
            {"object", {{"currency", "USD"}, {"value", 42.99}}}};

  // Ex2: Add new key-value
  j["new"]["key"]["value"] = {"another", "list"};

  // Ex3: Count elements
  auto s = j.size();
  j["size"] = s;

  // Ex4: Modify element's value
  j["name"] = "Martin";

  // Ex5: Pretty print with indent of 2 spaces
  std::cout << "Output json context:" << std::endl;
  std::cout << std::setw(2) << j << std::endl << std::endl;
  std::cout << j.value("pi", 0.0) << std::endl << std::endl;

  // Ex6: Save to file
  std::string file_name = std::move<std::string>(std::string("out_01.json"));
  std::cout << "Save to file: " << file_name << std::endl << std::endl;
  std::ofstream out(file_name);
  out << std::setw(2) << j << std::endl;
  out.close();

  // Ex7: Read json file
  std::vector<char> data = ReadFile(file_name);
  json j_read;
  try {
    j_read = json::parse(data);
  } catch (const json::parse_error &e) {
    std::cout << "Parse json error: \n" << e.what() << std::endl;
  }
  std::cout << "Read json file: " << file_name << std::endl;
  std::cout << std::setw(2) << j_read << std::endl << std::endl;

  // Ex8: Get sub-nodes
  std::vector<std::string> sub_node_name_list;
  for (auto &sub_node : j_read.items()) {
    sub_node_name_list.emplace_back(sub_node.key());
  }
  std::cout << "Sub node's name list: " << std::endl;
  std::for_each(sub_node_name_list.begin(), sub_node_name_list.end(),
                [](const std::string &str) { std::cout << str << std::endl; });
  std::cout << "\n" << std::endl;

  std::system("pause");
  return 0;
}
