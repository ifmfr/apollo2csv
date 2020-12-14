#include <unistd.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#include "apollo2csv.h"
#include "apollo2csvConfig.h"
#include "apolloConvertConfig.h"

void split(std::string str_in, char delimiter,
           std::vector<std::string>& target) {
  std::string str;
  std::stringstream stream(str_in);
  while (std::getline(stream, str, delimiter)) {
    target.push_back(str);
  }
}

int main(int argc, char* argv[]) {
  ApolloConvertConfig config;
  int opt;
  while ((opt = getopt(argc, argv, "f:piav:")) != EOF) {
    switch (opt) {
      case 'f':
        config.setFileName(optarg);
        break;
      case 'p':
        config.setPressureUnit("psi");
        break;
      case 'i':
        config.setKeepImpulse(true);
        break;
      case 'a':
        config.setCompensate(false);
        break;
      case 'v':
        config.setFileVersion(optarg);
        break;
    }
  }

  if (!config.isValidFile() || !config.isValidFileVersion()) {
    std::cerr << "Usage: " << argv[0] << " -f file [-p -i -a -v]" << std::endl;
    std::cerr << "       -f Apollo gauges file" << std::endl;
    std::cerr << "       -p Pressure unit psi, default kPa" << std::endl;
    std::cerr << "       -i Keep impulse, default drop" << std::endl;
    std::cerr << "       -a Keep ambient pressure, default conpensate"
              << std::endl;
    std::cerr << "       -v Apollo version 2018/2020, default 2020"
              << std::endl;
    std::cerr << "Version " << apollo2csv_version_major << '.';
    std::cerr << apollo2csv_version_minor << '.';
    std::cerr << apollo2csv_version_patch << std::endl;
    return 1;
  }

  size_t num_gauges = 0;                // number of gauges
  std::vector<std::string> gauges;      // names of gauges
  unsigned int num_quantities = 0;      // number of quantities
  std::vector<std::string> quantities;  // names of quantities
  std::vector<std::string> units;       // unit of quantities
  double offset = 0;                    // ambient pressure

  std::ifstream file_in(config.getFileName());
  std::string line;
  while (getline(file_in, line)) {
    if (line.compare(0, 18, "#number of gauges:") == 0) {
      num_gauges = std::stoul(line.substr(18, line.size()));
      printf("Number of gauges: %zu\n", num_gauges);
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
    split(line, '\"', temp_vec);
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
      num_quantities = std::stoul(line.substr(23, line.size())) -
                       1;  // subtract quantity Time
      printf("Number of quantities: %d\n", num_quantities);
      break;
    }
  }

  getline(file_in, line);
  getline(file_in, line);
  for (unsigned int i = 0; i < num_quantities; i++) {
    getline(file_in, line);
    temp_vec.clear();
    split(line, '\"', temp_vec);
    quantities.push_back(temp_vec.at(1));  // Name
    line = temp_vec.at(3);
    line.pop_back();                                   // delete last char "]"
    units.push_back(line.substr(line.find("[") + 1));  // Unit
  }

  for (size_t i = 0; i < quantities.size(); i++) {
    std::cout << "      " << quantities.at(i);
    std::cout << " [" << units.at(i) << "]" << std::endl;
  }

  if (quantities.at(0).compare("Pressure") !=
      0) {  // first col must be pressure
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

  std::string file_name_out = config.getFileName().append(".csv");
  std::ofstream file_out(file_name_out);
  if (!file_out.is_open()) {
    std::cerr << "Error: File not found." << std::endl;
    return 1;
  }

  file_out << "Time [ms], ";
  for (size_t i = 0; i < quantities.size(); i++) {
    if (!config.getKeepImpulse() && i == 1) {  // skip impulse
      continue;
    }
    for (size_t j = 0; j < gauges.size(); j++) {
      file_out << gauges.at(j);
      if (i == 0) {  // handle pressure differently, just name and unit
        if (config.getPressureUnit().compare("kPa") == 0) {
          file_out << " [kPa], ";
        } else {
          file_out << " [psi], ";
        }
      } else if (i == 1) {  // handle impulse differently, just name and unit
        if (config.getPressureUnit().compare("kPa") == 0) {
          file_out << " [Pa s], ";
        } else {
          file_out << " [psi ms], ";
        }
      } else {
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

    if (config.getFileVersion().compare("2020") == 0) {
      time = std::stod(line.substr(0, 14));  // s
    } else {                                 // file_version == 2018
      time = std::stod(line.substr(0, 13));  // s
    }
    time *= 1000;  // s to ms

    file_out.precision(8);
    file_out << time << ", ";

    file_out.precision(2);
    for (size_t i = 0; i < quantities.size(); i++) {
      if (!config.getKeepImpulse() && i == 1) {  // skip impulse
        continue;
      }
      for (size_t j = 0; j < gauges.size(); j++) {
        if (config.getFileVersion().compare("2020") == 0) {
          pos = 13 * (i * gauges.size() + j + 1) + 1;
        } else {  // file_version == 2018
          pos = 13 * (i * gauges.size() + j + 1);
        }
        value = std::stod(line.substr(pos, 13));
        if (i == 0) {  // quantity pressure

          if (config.getCompensate()) {
            value -= offset;
          }

          value /= 1000;                                       // Pa to kPa
          if (config.getPressureUnit().compare("psi") == 0) {  // psi
            value /= 6.89476;                                  // kPa to psi
          }
          file_out.precision(2);
          file_out << value << ", ";
        } else if (i == 1) {  // quantity impulse
          if (config.getPressureUnit().compare("psi") == 0) {  // psi
            value /= 6.89476;                                  // kPa to psi
          }
          file_out.precision(2);
          file_out << value << ", ";
        } else {  // all other quantities
          file_out.precision(4);
          file_out << value << ", ";
        }
      }
    }
    file_out << std::endl;
  }

  file_out.close();
  file_in.close();
}
