#pragma once

#include <optional>

#include "parameters.h"
#include "innovation_pool.h"
#include "population.h"
#include "species.h"
#include "organism.h"

namespace netkit {
class neat {
public:
	neat(parameters params);
	// TODO: another constructor with the initial genome
	neat(const neat& other);

	innovation_pool& innov_pool() { return m_innov_pool; }

	// Useful if you want to run and rate all the organisms at once.
	// Require more memory.
	std::vector<organism> generate_and_get_all_organisms();

	// Useful if you you run independant experiments for each organism.
	// Require less memory.
	organism generate_and_get_next_organism();

	// check if there is no more organism left to generate
	bool has_more_organisms_to_process();

	// you should have rated every organisms beforme calling this
	void goto_next_generation();

	std::optional<species*> find_appropriate_specie_for(const genome& geno);

public:
	const parameters params;

private:
	std::vector<species> m_all_species;
	population m_population;
	innovation_pool m_innov_pool;
	genome_id_t m_next_genome_id;
	species_id_t m_next_species_id;
};
}
