#include "netkit/neat/population.h"
#include "netkit/neat/organism.h"

netkit::organism::organism(population* population, species* species, genome_id_t genome_id, network network)
	: m_population(population)
	, m_species(species)
	, m_genome_id(genome_id)
	, m_network(std::move(network)) {}

netkit::genome& netkit::organism::get_genome() const {
	return m_population->get_genome(m_genome_id);
}
