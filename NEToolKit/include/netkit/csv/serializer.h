#pragma once

#include <iostream>
#include <fstream>

namespace netkit {
class serializer {
public:
	explicit serializer(std::string filename, std::string separator = ";");

	template<typename T>
	void append(T o);

	void new_line();
	void close();

private:
	std::ofstream m_file;
	std::string m_separator;
	bool m_beggining_of_line;
};

template<typename T>
void serializer::append(T o) {
	if (!m_beggining_of_line) {
		m_file << m_separator;
	}
	m_beggining_of_line = false;
	m_file << o;
}
}
