#include "netkit/csv/deserializer.h"

netkit::deserializer::deserializer(std::string filename, char separator)
	: m_file()
	, m_separator(separator) {
	m_file.open(filename, std::ios::in);
}

void netkit::deserializer::skip_line() {
	while (m_file.get() != '\n') {}
}

void netkit::deserializer::close() {
	m_file.close();
}
