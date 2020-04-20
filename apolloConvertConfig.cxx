#include "apolloConvertConfig.h"

#include <fstream>
#include <iostream>
#include <set>

ApolloConvertConfig::ApolloConvertConfig() {
  file_name_m = "";
  pre_unit_m = "kPa";
  keep_impulse_m = false;
  compensate_m = true;
  file_version_m = "20201";
}

void ApolloConvertConfig::setFileName(std::string file_name) {
  file_name_m = file_name;
}

std::string ApolloConvertConfig::getFileName() { return file_name_m; }

void ApolloConvertConfig::setPressureUnit(std::string pre_unit) {
  pre_unit_m = pre_unit;
}

std::string ApolloConvertConfig::getPressureUnit() { return pre_unit_m; }

void ApolloConvertConfig::setKeepImpulse(bool keep_impulse) {
  keep_impulse_m = keep_impulse;
}

bool ApolloConvertConfig::getKeepImpulse() { return keep_impulse_m; }

void ApolloConvertConfig::setCompensate(bool compensate) {
  compensate_m = compensate;
}

bool ApolloConvertConfig::getCompensate() { return compensate_m; }

void ApolloConvertConfig::setFileVersion(std::string file_version) {
  file_version_m = file_version;
}

std::string ApolloConvertConfig::getFileVersion() { return file_version_m; }

bool ApolloConvertConfig::isValidFile() {
  if (file_name_m.length() == 0) {
    return false;
  }
  std::ifstream file_in(file_name_m);
  if (!file_in.is_open()) {
    std::cerr << "Error: File not found." << std::endl;
    return false;
  }
  file_in.close();
  return true;
}

bool ApolloConvertConfig::isValidFileVersion() {
  std::set<std::string> file_versions{"20181", "20182", "20201"};
  if (file_versions.count(file_version_m) == 1) {
    return true;
  } else {
    return false;
  }
}
