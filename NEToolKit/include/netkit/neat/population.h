#pragma once

#include <vector>

#include "genome.h"
#include "neat_primitive_types.h"

namespace netkit {
class neat; // forward declaration

class population {
public:
	population();
	population(const population& other);

	inline genome_id_t add_genome(genome geno);
	inline genome& get_genome(genome_id_t geno_id);
	inline genome& operator[](genome_id_t geno_id);
	inline void clear();

	inline size_t size() const;

private:
	std::vector<genome> m_all_genomes;
};
}
