#include "netkit/neat/population.h"

netkit::population::population()
	: m_all_genomes() {}

netkit::population::population(const population& other)
	: m_all_genomes(other.m_all_genomes) {}

inline netkit::genome_id_t netkit::population::add_genome(genome geno) {
	m_all_genomes.push_back(std::move(geno));
	return m_all_genomes.size() - 1;
}

inline netkit::genome& netkit::population::get_genome(genome_id_t geno_id) {
	return m_all_genomes[geno_id];
}

inline netkit::genome& netkit::population::operator[](genome_id_t geno_id) {
	return m_all_genomes[geno_id];
}

inline void netkit::population::clear() {
	m_all_genomes.clear();
}

inline size_t netkit::population::size() const {
	return m_all_genomes.size();
}
