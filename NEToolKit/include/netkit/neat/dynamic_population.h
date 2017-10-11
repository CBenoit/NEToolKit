#pragma once

#include <vector>

#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
	// this class handle dynamic renewal of the population by allowing automatic replacement of old genomes marked for removal.
class dynamic_population {
public:
	dynamic_population();
	dynamic_population(const dynamic_population& other) = default;
	dynamic_population(dynamic_population&& other) noexcept;
	dynamic_population& operator=(const dynamic_population& other) = default;
	dynamic_population& operator=(dynamic_population&& other) noexcept;

	genome_id_t add_genome(genome geno);
	genome& get_genome(genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	genome& operator[](genome_id_t geno_id) { return m_all_genomes[geno_id]; }
	void clear();
	size_t size() const { return m_all_genomes.size(); }
	const std::vector<genome>& get_all_genomes() const { return m_all_genomes; }
	bool is_marked_for_removal(genome_id_t geno_id) { return m_marked_for_removal[geno_id]; }
	void set_genomes(const std::vector<genome>& genomes);
	void set_genomes(std::vector<genome>&& genomes);
	void replace_genome(genome_id_t id, genome geno);
	void mark_genome_for_removal(genome_id_t geno_id);

private:
	std::vector<genome> m_all_genomes;
	std::vector<bool> m_marked_for_removal;
	genome_id_t m_lookup_genome_id;
};
}
