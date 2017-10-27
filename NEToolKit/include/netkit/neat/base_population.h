#pragma once

#include <vector>

#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
class base_neat; // forward declaration

class base_population {
  protected:
	explicit base_population(base_neat* neat_instance);
	base_population(const base_population& other) = default;
	base_population(base_population&& other) noexcept;
	base_population& operator=(const base_population& other) = default;
	base_population& operator=(base_population&& other) noexcept;

  public:
	const genome& get_genome(genome_id_t geno_id) const { return m_all_genomes[geno_id]; }
	genome& get_genome(genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	const genome& operator[](genome_id_t geno_id) const { return m_all_genomes[geno_id]; }
	genome& operator[](genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	size_t size() const { return m_all_genomes.size(); }
	const std::vector<genome>& get_all_genomes() const { return m_all_genomes; }

  protected:
	std::vector<genome> m_all_genomes;
	base_neat* m_neat;
};
}
