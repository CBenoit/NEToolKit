#include <cstdlib>
#include <ctime>
#include <utility> // std::move
#include <stdexcept> // std::invalid_argument

#include "netkit/neat/neat.h"

netkit::neat::neat(parameters params_)
	: params(std::move(params_))
	, m_all_species()
	, m_population()
	, m_innov_pool(this->params)
	, m_next_genome_id(0)
	, m_next_species_id(0) {
	if (this->params.number_of_outputs == 0 || this->params.number_of_inputs == 0) {
		throw std::invalid_argument("genomes needs at least one input and one output.");
	}

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

netkit::neat::neat(const neat& other)
	: params(other.params)
	, m_all_species(other.m_all_species)
	, m_population(other.m_population)
	, m_innov_pool(this->params) // the innovation pool has to differ from the other simulation
	, m_next_genome_id(other.m_next_genome_id)
	, m_next_species_id(other.m_next_species_id) {} // no need to use srand again.

std::vector<netkit::organism> netkit::neat::generate_and_get_all_organisms() {
	std::vector<organism> organisms;
	organisms.reserve(m_population.size());
	for (genome_id_t i = 0, size = m_population.size(); i < size; ++i) {
		organisms.push_back(generate_and_get_next_organism());
	}
	return std::move(organisms);
}

netkit::organism netkit::neat::generate_and_get_next_organism() {
	genome& geno = m_population[m_next_genome_id];

	std::optional<species*> opt_species = find_appropriate_specie_for(geno);
	species* geno_species = nullptr;
	if (opt_species.has_value()) {
		geno_species = opt_species.value();
		geno_species->add_member(m_next_genome_id);
	} else {
		m_all_species.push_back(species(&m_population, m_next_species_id++, geno));
		geno_species = &m_all_species.back();
	}

	return organism(geno_species, m_next_genome_id++, geno.generate_network());
}

bool netkit::neat::has_more_organisms_to_process() {
	return m_next_genome_id < m_population.size();
}

void netkit::neat::goto_next_generation() {
	// TODO: make selection, mutations, crossovers, etc...
	// update representants...

	m_next_genome_id = 0;
}

std::optional<netkit::species*> netkit::neat::find_appropriate_specie_for(const genome& geno) {
	for (species& spec : m_all_species) {
		if (geno.is_compatible_with(spec.get_representant())) {
			return &spec;
		}
	}

	return {};
}
