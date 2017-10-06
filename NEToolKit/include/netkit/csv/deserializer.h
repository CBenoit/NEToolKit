#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace netkit {
class deserializer {
public:
	explicit deserializer(std::string filename, char separator = ';');

	template<typename T>
	void get_next(T& var);

	void skip_line();
	void close();

private:
	std::ifstream m_file;
	char m_separator;
};

template<typename T>
void deserializer::get_next(T& var) {
	std::stringstream ss;
	char c;
	while (m_file.get(c) && c != m_separator && c != '\n' && c != EOF) {
		ss << c;
	}
	ss >> var;
}
}
