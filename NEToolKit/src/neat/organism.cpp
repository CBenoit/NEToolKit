#include "netkit/neat/organism.h"

netkit::organism::organism(species* species, genome_id_t genome_id, network network)
	: m_species(species)
	, m_genome_id(genome_id)
	, m_network(std::move(network)) {}

netkit::species& netkit::organism::get_species() const {
	return *m_species;
}

netkit::genome_id_t netkit::organism::get_genome_id() const {
	return m_genome_id;
}

netkit::network& netkit::organism::get_network() {
	return m_network;
}
