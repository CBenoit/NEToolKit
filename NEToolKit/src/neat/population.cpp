#include "netkit/neat/population.h"

netkit::population::population()
	: m_all_genomes() {}

netkit::population::population(const population& other)
	: m_all_genomes(other.m_all_genomes) {}

inline netkit::genome_id_t netkit::population::add_genome(genome geno) {
	m_all_genomes.push_back(std::move(geno));
	return m_all_genomes.size() - 1;
}
