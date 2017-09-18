#pragma once

#include "netkit/network/network.h"
#include "species.h"
#include "genome.h"

namespace netkit {
class organism {
public:
	organism(species* species, genome_id_t genome_id, network network);

	species& get_species() const;
	genome_id_t get_genome_id() const;
	network& get_network();

private:
	species* m_species;
	genome_id_t m_genome_id; // the id of the genotype
	network m_network; // the phenotype
};
}
