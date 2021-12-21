#ifndef apollo2csv_h
#define apollo2csv_h

void display_usage(std::string prog_name);

void split(std::string str_in, char delimiter,
           std::vector<std::string>& target);

#endif
