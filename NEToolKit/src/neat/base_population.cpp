#include "netkit/neat/base_population.h"
#include "netkit/neat/base_neat.h"

netkit::base_population::base_population(base_neat* neat_instance)
	: m_all_genomes()
	, m_neat(neat_instance) {}

netkit::base_population::base_population(base_population&& other) noexcept
	: m_all_genomes(std::move(other.m_all_genomes))
	, m_neat(other.m_neat) {}

netkit::base_population& netkit::base_population::operator=(base_population&& other) noexcept {
	m_all_genomes = std::move(other.m_all_genomes);
	m_neat = other.m_neat;
	return *this;
}
