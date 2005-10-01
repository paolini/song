#include <string>

// iso converter

char iso(const std::string &utf8,size_t &i);

std::string iso(const std::string &utf8);

int add_utf8_char(unsigned char iso_char, std::string &to);
