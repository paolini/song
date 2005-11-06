#include <string>

// iso converter

char iso(const std::string &utf8,size_t &i); // estrae un carattere iso8859-1 da utf8 e incrementa i

std::string iso(const std::string &utf8); // from utf8 to iso8859-1 Latin-1

int add_utf8_char(unsigned char iso_char, std::string &to);

wchar_t unicode(const std::string &utf8,size_t &i); // estrae un carattere wchar da utf8 e incrementa i

std::wstring unicode(const std::string &utf8); // from utf8 to wstring
