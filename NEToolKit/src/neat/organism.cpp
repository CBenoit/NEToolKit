#include "netkit/neat/population.h"
#include "netkit/neat/organism.h"

netkit::organism::organism(population* population, genome_id_t genome_id, const network& network)
	: m_population(population)
	, m_genome_id(genome_id)
	, m_network(network)
	, m_time_alive(0) {}

netkit::organism::organism(population* population, genome_id_t genome_id, network&& network)
	: m_population(population)
	, m_genome_id(genome_id)
	, m_network(std::move(network))
	, m_time_alive(0) {}

netkit::organism::organism(organism&& other) noexcept
	: m_population(other.m_population)
	, m_genome_id(other.m_genome_id)
	, m_network(std::move(other.m_network))
	, m_time_alive(other.m_time_alive) {}

netkit::organism& netkit::organism::operator=(organism&& other) noexcept {
	m_population = other.m_population;
	m_genome_id = other.m_genome_id;
	m_network = std::move(other.m_network);
	m_time_alive = other.m_time_alive;
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

netkit::tick_t netkit::organism::get_time_alive() const {
	return m_time_alive;
}

void netkit::organism::increase_time_alive() {
	++m_time_alive;
}
