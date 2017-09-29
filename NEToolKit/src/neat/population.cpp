#include "netkit/neat/population.h"

netkit::population::population()
	: m_all_genomes() {}

netkit::population::population(population&& other) noexcept
	: m_all_genomes(std::move(other.m_all_genomes)) {}

netkit::population& netkit::population::operator=(population&& other) noexcept {
	m_all_genomes = std::move(other.m_all_genomes);
	return *this;
}

netkit::genome_id_t netkit::population::add_genome(genome geno) {
	m_all_genomes.push_back(std::move(geno));
	return m_all_genomes.size() - 1;
}

void netkit::population::set_genomes(const std::vector<genome> &genomes) {
	m_all_genomes.clear();
	m_all_genomes.reserve(genomes.size());
	for (const genome& geno : genomes) {
		m_all_genomes.push_back(geno);
	}
}

void netkit::population::set_genomes(std::vector<genome>&& genomes) {
	m_all_genomes = std::move(genomes);
}

void netkit::population::replace_genome(genome_id_t id, genome geno) {
	m_all_genomes[id] = std::move(geno);
}
