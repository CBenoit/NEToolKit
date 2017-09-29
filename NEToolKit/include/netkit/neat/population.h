#pragma once

#include <vector>

#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
class neat; // forward declaration

class population {
public:
	population();
	population(const population& other) = default;
	population(population&& other) noexcept;
	population& operator=(const population& other) = default;
	population& operator=(population&& other) noexcept;

	genome_id_t add_genome(genome geno);
	genome& get_genome(genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	genome& operator[](genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	void clear() { m_all_genomes.clear(); }
	size_t size() const { return m_all_genomes.size(); }
	const std::vector<genome>& get_all_genomes() const { return m_all_genomes; }
	void set_genomes(const std::vector<genome>& genomes);
	void set_genomes(std::vector<genome>&& genomes);
	void replace_genome(genome_id_t id, genome geno);

private:
	std::vector<genome> m_all_genomes;
};
}
