/**
 * Copyright 2019 all rights reserved
 * @date 18/Jul/2019
 * @author jin.ma
 */

#include <fstream>
#include <iomanip>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  // Create a JSON object
  json j = {{"pi", 3.141},
            {"happy", true},
            {"name", "Niels"},
            {"nothing", nullptr},
            {"answer", {{"everything", 42}}},
            {"list", {1, 0, 2}},
            {"object", {{"currency", "USD"}, {"value", 42.99}}}};

  // Add new key-value
  j["new"]["key"]["value"] = {"another", "list"};

  // Count elements
  auto s = j.size();
  j["size"] = s;

  // Modify element's value
  j["name"] = "Martin";

  // Pretty print with indent of 4 spaces
  std::cout << "Output json context:" << std::endl;
  std::cout << std::setw(4) << j << "\n\n";
  std::cout << j.value("pi", 0.0) << std::endl << std::endl;

  // Save to file
  std::cout << "Save file: config.json" << std::endl;
  std::ofstream out("config.json");
  out << std::setw(4) << j << std::endl;

  std::system("pause");
  return 0;
}
