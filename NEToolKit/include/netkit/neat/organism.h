#pragma once

#include "netkit/network/network.h"
#include "netkit/neat/neat_primitive_types.h"
#include "netkit/neat/genome.h"

namespace netkit {
class species; // forward declaration
class population;

class organism {
  public:
	organism(population* population, genome_id_t genome_id, const network& network);
	organism(population* population, genome_id_t genome_id, network&& network);
	organism(const organism& other) = default;
	organism(organism&& other) noexcept;
	organism& operator=(const organism& other) = default;
	organism& operator=(organism&& other) noexcept;

	genome_id_t get_genome_id() const { return m_genome_id; }
	network& get_network() { return m_network; }
	genome& get_genome() const;
	double get_fitness() const;
	void set_fitness(double value) const;
	tick_t get_time_alive() const;
	void increase_time_alive();

  private:
	population* m_population;
	genome_id_t m_genome_id; // the id of the genotype
	network m_network; // the phenotype
	tick_t m_time_alive;
};
}
