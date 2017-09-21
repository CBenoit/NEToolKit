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
	explicit neat(const parameters& params);
	neat(const neat& other) = default;
	neat(neat&& other) noexcept;

	// init population with a default genome (all inputs connected to all outputs)
	void init();

	// init population with the given genome.
	void init(const genome& initial_genome);

	// Useful if you want to run and rate all the organisms at once.
	// Require more memory.
	std::vector<organism> generate_and_get_all_organisms();

	// Useful if you you run independant experiments for each organism.
	// Require less memory.
	organism generate_and_get_next_organism();

	// check if there is no more organism left to generate
	bool has_more_organisms_to_process();

	// you should have rated every organisms beforme calling this.
	void epoch();

	std::vector<species>& get_all_species() { return m_all_species; }

	std::optional<species*> find_appropriate_species_for(const genome& geno);

private:
	void helper_speciate();

public:
	const parameters params;
	innovation_pool innov_pool;

private:
	std::vector<species> m_all_species;
	population m_population;
	genome_id_t m_next_genome_id;
	species_id_t m_next_species_id;
};
}
