#pragma once

#include <optional>
#include <random>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "parameters.h"
#include "innovation_pool.h"
#include "population.h"
#include "species.h"

namespace netkit {
class base_neat {
  public:
	explicit base_neat(const parameters& params);
	base_neat(const base_neat& other);
	base_neat(base_neat&& other) noexcept;
	virtual ~base_neat() { delete m_best_genome_ever; }

	// init population with a default genome (all inputs connected to all outputs)
	void init();

	// init population with the given genome.
	void init(const genome& initial_genome);

	// you should have rated every organisms before calling this.
	void epoch();

	std::vector<species>& get_all_species() { return m_all_species; }

	std::optional<species*> find_appropriate_species_for(const genome& geno);

	const genome& get_current_best_genome() const;

	std::optional<genome> get_best_genome_ever() const;

  protected:
	void helper_speciate_all_population();

	void helper_speciate_one_genome(genome_id_t geno_id);

	// overload this function to do a custom epoch implementation
	virtual void impl_epoch() = 0;

  public:
	parameters params;
	innovation_pool innov_pool;
	std::minstd_rand0 rand_engine;

  protected:
	std::vector<species> m_all_species;
	population m_population;
	species_id_t m_next_species_id;
	genome* m_best_genome_ever;
	unsigned int m_age_of_best_genome_ever;

	friend serializer& operator<<(serializer& ser, const base_neat& neat);
	friend deserializer& operator>>(deserializer& des, base_neat& neat);
};

serializer& operator<<(serializer& ser, const base_neat& neat);
deserializer& operator>>(deserializer& des, base_neat& neat);
}
