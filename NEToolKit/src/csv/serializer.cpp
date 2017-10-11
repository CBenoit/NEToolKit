#include "netkit/csv/serializer.h"

netkit::serializer::serializer(std::string filename, std::string separator)
	: m_file()
	, m_separator(std::move(separator))
	, m_beggining_of_line(true) {
	m_file.open(filename, std::ios::out | std::ios::trunc);
}

void netkit::serializer::new_line() {
	m_file << '\n';
	m_beggining_of_line = true;
}

void netkit::serializer::close() {
	m_file.close();
}
