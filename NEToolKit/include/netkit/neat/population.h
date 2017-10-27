#pragma once

#include <vector>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "base_population.h"
#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
class population : public base_population {
  public:
	population(base_neat* neat_instance);
	population(const population& other) = default;
	population(population&& other) noexcept;
	population& operator=(const population& other) = default;
	population& operator=(population&& other) noexcept;

	genome_id_t add_genome(genome geno);
	void clear() { m_all_genomes.clear(); }
	void set_genomes(const std::vector<genome>& genomes);
	void set_genomes(std::vector<genome>&& genomes);
	void replace_genome(genome_id_t id, genome geno);

	friend serializer& operator<<(serializer& ser, const population& pop);
	friend deserializer& operator>>(deserializer& des, population& pop);
};

serializer& operator<<(serializer& ser, const population& pop);
deserializer& operator>>(deserializer& des, population& pop);
}
