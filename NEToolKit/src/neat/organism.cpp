#include "netkit/neat/population.h"
#include "netkit/neat/organism.h"

netkit::organism::organism(population* population, genome_id_t genome_id, const network& network)
	: m_population(population)
	, m_genome_id(genome_id)
	, m_network(network) {}

netkit::organism::organism(population* population, genome_id_t genome_id, network&& network)
	: m_population(population)
	, m_genome_id(genome_id)
	, m_network(std::move(network)) {}

netkit::organism::organism(organism&& other) noexcept
	: m_population(other.m_population)
	, m_genome_id(other.m_genome_id)
	, m_network(std::move(other.m_network)) {}

netkit::organism& netkit::organism::operator=(organism&& other) noexcept {
	m_population = other.m_population;
	m_genome_id = other.m_genome_id;
	m_network = std::move(other.m_network);
	return *this;
}

netkit::genome& netkit::organism::get_genome() const {
	return m_population->get_genome(m_genome_id);
}

double netkit::organism::get_fitness() const {
	return m_population->get_genome(m_genome_id).get_fitness();
}


void netkit::organism::set_fitness(double value) const {
	m_population->get_genome(m_genome_id).set_fitness(value);
}
