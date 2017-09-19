#pragma once

#include "netkit/network/network.h"
#include "netkit/neat/neat_primitive_types.h"
#include "netkit/neat/genome.h"

namespace netkit {
class species; // forward declaration
class population;

class organism {
public:
	organism(population* population, genome_id_t genome_id, network network);

	genome_id_t get_genome_id() const { return m_genome_id; }
	network& get_network() { return m_network; }
	genome& get_genome() const;

private:
	population* m_population;
	genome_id_t m_genome_id; // the id of the genotype
	network m_network; // the phenotype
};
}
