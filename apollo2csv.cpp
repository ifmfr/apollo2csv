#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>
#include <vector>

#include "apollo2csvConfig.h"


void split(std::string str, std::string delim, std::vector<std::string>& token) {
  std::string::size_type lastPos = str.find_first_not_of(delim, 0);
  std::string::size_type pos = str.find_first_of(delim, lastPos);
  while (std::string::npos != pos || std::string::npos != lastPos) {
    token.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delim, pos);
    pos = str.find_first_of(delim, lastPos);
  }
}


int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " file_gauges" << std::endl;
    std::cerr << 'v' << apollo2csv_VERSION_MAJOR << '.';
    std::cerr << apollo2csv_VERSION_MINOR << std::endl;
    return 1;
  }

  std::string file_name = argv[1];

  size_t num_gauges = 0; // number of gauges
  std::vector<std::string> gauges; // names of gauges
  unsigned int num_quantities = 0; // number of quantities
  std::vector<std::string> quantities; // names of quantities
  std::vector<std::string> units; // unit of quantities
  int pre_unit = 0; // 0: kPa, 1: psi
  bool skipImpulse = true; // true: skip impulse for output, false include impulse
  bool compensate = true; // true: compensate ambient pressure, false do not compensate
  double offset = 0; // ambient pressure

  std::string line;
  std::cout << "Pressure unit? kPa/[psi]: ";
  getline(std::cin, line);
  std::transform(line.begin(), line.end(), line.begin(), ::tolower);
  if (line.compare("kpa") == 0) {
    pre_unit = 0; // kPa
    std::cout << "--> kPa" << std::endl;
  }
  else {
    pre_unit = 1; // psi
    std::cout << "--> psi" << std::endl;
  }

  std::cout << "Skip impulse? [y]/n: ";
  getline(std::cin, line);
  std::transform(line.begin(), line.end(), line.begin(), ::tolower);
  if (line.compare("n") == 0) {
    skipImpulse = false; // no
    std::cout << "--> no" << std::endl;
  }
  else {
    skipImpulse = true; // yes
    std::cout << "--> yes" << std::endl;
  }
  
  std::cout << "Compensate ambient pressure? [y]/n: ";
  getline(std::cin, line);
  std::transform(line.begin(), line.end(), line.begin(), ::tolower);
  if (line.compare("n") == 0) {
    compensate = false; // no
    std::cout << "--> no" << std::endl;
  }
  else {
    compensate = true; // yes
    std::cout << "--> yes" << std::endl;
  }

  std::ifstream file_in(file_name);
  if (!file_in.is_open()) {
    std::cerr << "Error: File not found." << std::endl;
    return 1;
  }

  while (getline(file_in, line)) {
    if (line.compare(0, 18, "#number of gauges:") == 0) {
      num_gauges = std::stoul(line.substr(18, line.size()));
      printf ("Number of gauges: %zu\n", num_gauges);
      break;
    }
  }

  char ch;
  file_in.get(ch);
  getline(file_in, line, '#');
  line = std::regex_replace(line, std::regex("\n"), " ");

  std::vector<std::string> temp_vec;
  temp_vec.clear();
  if (line.compare(0, 16, "names of gauges:") == 0) {
    split(line, "\"", temp_vec);
  } else {
    std::cerr << "Error: Parsing file. Gauges" << std::endl;
    return 1;
  }

  for (size_t i = 0; i < temp_vec.size(); i++) {
    if (i % 2 == 1) {
      gauges.push_back(temp_vec.at(i));
    }
  }

  if (num_gauges != gauges.size()) {
    std::cerr << "Error: Parsing file." << std::endl;
    return 1;
  }
  
  while (getline(file_in, line)) {
    if (line.compare(0, 21, "number of quantities:") == 0) {
      num_quantities = std::stoul(line.substr(23, line.size())) - 1; //subtract quantity Time
      printf ("Number of quantities: %d\n", num_quantities);
      break;
    }
  }

  getline(file_in, line);
  getline(file_in, line);
  for (unsigned int i = 0; i < num_quantities; i++) {
    getline(file_in, line);
    temp_vec.clear();
    split(line, "\"", temp_vec);
    quantities.push_back(temp_vec.at(1)); // Name
    line = temp_vec.at(3);
    line.pop_back(); // delete last char "]"
    units.push_back(line.substr(line.find("[") + 1)); // Unit
  }

  for (size_t i = 0; i < quantities.size(); i++) {
    std::cout << "      " << quantities.at(i);
    std::cout << " [" << units.at(i) << "]" << std::endl;
  }

  if (quantities.at(0).compare("Pressure") != 0) { // first col must be pressure
    std::cerr << "Error: Parsing file." << std::endl;
    return 1;
  }

  while (getline(file_in, line)) {
    if (line.compare(0, 1, "#") == 0) {
     continue;
    }
    offset = std::stod(line.substr(14, 13));
    file_in.seekg(0);
    break;
  }

  std::string file_name_out = file_name.append(".csv");
  std::ofstream file_out(file_name_out);
  if (!file_out.is_open()) {
    std::cerr << "Error: File not found." << std::endl;
    return 1;
  }

  file_out << "Time [ms]; ";
  for (size_t i = 0; i < quantities.size(); i++) {
    if (skipImpulse && i == 1) { // skip impulse
      continue;
    }
    for (size_t j = 0; j < gauges.size(); j++) {
      file_out << gauges.at(j);
      if (i == 0) { // handle pressure differently, just name and unit
        if (pre_unit == 0) {
          file_out << " [kPa]; ";
        } else {
          file_out << " [psi]; ";
        }
      }
      else if (i == 1) { // handle impulse differently, just name and unit
        if (pre_unit == 0) {
          file_out << " [kPa*ms]; ";
        } else {
          file_out << " [psi*ms]; ";
        }
      }
      else {
        file_out << " " << quantities.at(i);
        file_out << " [" << units.at(i) << "]; ";
      }
    }
  }
  file_out << std::endl;

  double time, value;
  int pos;
  file_out.setf(std::ios::fixed);
  while (getline(file_in, line)) {
    if (line.compare(0, 1, " ") != 0) {
     continue;
    }

    time = std::stod(line.substr(0, 13)); // s
    time *= 1000; // s to ms

    file_out.precision(8);
    file_out << time << "; ";

    file_out.precision(2);
    for (size_t i = 0; i < quantities.size(); i++) {
      if (skipImpulse && i == 1) { // skip impulse
        continue;
      }
      for (size_t j = 0; j < gauges.size(); j++) {
        pos = 13 * (i * gauges.size() + j + 1);
        
        value = std::stod(line.substr(pos, 13));
        if (i == 0) { // quantity pressure

          if (compensate) {
            value -= offset;
          }

          value /= 1000; // Pa to kPa
          if (pre_unit == 1) { // psi
            value /= 6.89476; // kPa to psi
          }
          file_out.precision(2);
          file_out << value << "; ";
        }
        else if (i == 1) { // quantity impulse
          if (pre_unit == 1) { // psi
            value /= 6.89476; // kPa to psi
          }
          file_out.precision(2);
          file_out << value << "; ";
        }
        else { // all other quantities
          file_out.precision(4);
          file_out << value << "; ";
        }
      }
    }
    file_out << std::endl;
  }

  file_out.close();
  file_in.close();
}
