#include "netkit/neat/dynamic_population.h"

netkit::dynamic_population::dynamic_population()
	: m_all_genomes()
	, m_marked_for_removal()
	, m_lookup_genome_id(0) {}

netkit::dynamic_population::dynamic_population(dynamic_population&& other) noexcept
	: m_all_genomes(std::move(other.m_all_genomes))
	, m_marked_for_removal(std::move(other.m_marked_for_removal))
	, m_lookup_genome_id(other.m_lookup_genome_id) {}

netkit::dynamic_population& netkit::dynamic_population::operator=(dynamic_population&& other) noexcept {
	m_all_genomes = std::move(other.m_all_genomes);
	m_marked_for_removal = std::move(other.m_marked_for_removal);
	m_lookup_genome_id = other.m_lookup_genome_id;
	return *this;
}

netkit::genome_id_t netkit::dynamic_population::add_genome(genome geno) {
	while (m_lookup_genome_id < m_all_genomes.size()) {
		if (m_marked_for_removal[m_lookup_genome_id]) { // found a genome marked for removal.
			break;
		}
		++m_lookup_genome_id;
	}
	// If none is found, m_lookup_genome_id shall be equal to m_all_genomes.size().
	// thus...
	if (m_lookup_genome_id >= m_all_genomes.size()) {
		m_all_genomes.push_back(std::move(geno));
		m_marked_for_removal.emplace_back(false);
		m_lookup_genome_id = m_all_genomes.size();
		return m_all_genomes.size() - 1;
	}

	// otherwise, simply replace the old unused genome:
	m_all_genomes[m_lookup_genome_id] = std::move(geno);
	m_marked_for_removal[m_lookup_genome_id] = false;
	return m_lookup_genome_id++; // to clarify: this is effectively a POST incrementation thus the returned value is m_lookup_genome_id - 1.
}

void netkit::dynamic_population::clear() {
	m_all_genomes.clear();
	m_marked_for_removal.clear();
	m_lookup_genome_id = 0;
}

void netkit::dynamic_population::set_genomes(const std::vector<genome>& genomes) {
	m_all_genomes.clear();
	m_all_genomes.reserve(genomes.size());
	m_marked_for_removal.clear();
	m_marked_for_removal.reserve(genomes.size());
	for (const genome& geno : genomes) {
		m_all_genomes.push_back(geno);
		m_marked_for_removal.emplace_back(false);
	}
	m_lookup_genome_id = m_all_genomes.size();
}

void netkit::dynamic_population::set_genomes(std::vector<genome>&& genomes) {
	m_all_genomes = std::move(genomes);

	// update active list
	m_marked_for_removal.clear();
	m_marked_for_removal.reserve(m_all_genomes.size());
	for (size_t i = 0; i < m_all_genomes.size(); ++i) {
		m_marked_for_removal.emplace_back(false);
	}
	m_lookup_genome_id = m_all_genomes.size();
}

void netkit::dynamic_population::replace_genome(genome_id_t id, genome geno) {
	m_all_genomes[id] = std::move(geno);
	m_marked_for_removal[id] = false;
}

void netkit::dynamic_population::mark_genome_for_removal(genome_id_t geno_id) {
	m_marked_for_removal[geno_id] = true;
	m_lookup_genome_id = geno_id;
}
