#pragma once

#include <vector>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "base_population.h"
#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
// this class handle dynamic renewal of the population by allowing automatic replacement of old genomes marked for removal.
class dynamic_population : public base_population {
  public:
	dynamic_population(base_neat* neat_instance);
	dynamic_population(const dynamic_population& other) = default;
	dynamic_population(dynamic_population&& other) noexcept;
	dynamic_population& operator=(const dynamic_population& other) = default;
	dynamic_population& operator=(dynamic_population&& other) noexcept;

	genome_id_t add_genome(genome geno);
	void clear();
	bool is_marked_for_removal(genome_id_t geno_id) { return m_marked_for_removal[geno_id]; }
	void set_genomes(const std::vector<genome>& genomes);
	void set_genomes(std::vector<genome>&& genomes);
	void replace_genome(genome_id_t id, genome geno);
	void mark_genome_for_removal(genome_id_t geno_id);

  private:
	std::vector<bool> m_marked_for_removal;
	genome_id_t m_lookup_genome_id;

	friend serializer& operator<<(serializer& ser, const dynamic_population& pop);
	friend deserializer& operator>>(deserializer& des, dynamic_population& pop);
};

serializer& operator<<(serializer& ser, const dynamic_population& pop);
deserializer& operator>>(deserializer& des, dynamic_population& pop);
}
