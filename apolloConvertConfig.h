#ifndef apolloConvertConfig_h
#define apolloConvertConfig_h

#include <string>

class ApolloConvertConfig {
 public:
  ApolloConvertConfig();

  void setFileName(const std::string& file_name);
  std::string getFileName();
  void setPressureUnit(const std::string& pre_unit);
  std::string getPressureUnit();
  void setKeepImpulse(bool keep_impulse);
  bool getKeepImpulse();
  void setCompensate(bool compensate);
  bool getCompensate();
  void setFileVersion(const std::string& file_version);
  std::string getFileVersion();
  bool isValidFile();
  bool isValidFileVersion();

 private:
  // file name Apollo gauges file
  std::string file_name_m;
  // Pressure unit in kPa or psi
  std::string pre_unit_m;
  // Keep impulse in output
  bool keep_impulse_m;
  // componsate ambient pressure
  bool compensate_m;
  // gauges file version 2018 or 2020
  std::string file_version_m;
};

#endif
