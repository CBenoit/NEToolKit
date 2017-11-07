#pragma once

#include <vector>
#include <iostream>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "netkit/network/network_primitive_types.h"
#include "netkit/network/network.h"
#include "gene.h"

namespace netkit {
class base_neat;

class genome {
  public:
	explicit genome(base_neat* neat_instance);
	genome(const genome& other) = default;
	genome(genome&& other) noexcept;
	genome& operator=(const genome& other) = default;
	genome& operator=(genome&& other) noexcept;
	bool operator==(const genome& other);

	void add_gene(gene new_gene);
	const std::vector<gene>& get_genes() const { return m_genes; }

	bool link_exists(neuron_id_t from, neuron_id_t to) const;

	void set_fitness(double fitness) { m_fitness = fitness; }
	double get_fitness() const { return m_fitness; }
	void set_adjusted_fitness(double fitness) { m_adjusted_fitness = fitness; }
	double get_adjusted_fitness() const { return m_adjusted_fitness; }

	unsigned int number_of_inputs() const { return m_number_of_inputs; }
	unsigned int number_of_outputs() const { return m_number_of_outputs; }

	// specie distance to another genome.
	double distance_to(const genome& other) const;
	bool is_compatible_with(const genome& other) const;

	// mutations
	genome get_random_mutation() const; // produce an offspring
	// all these directly modify the current genome
	bool random_mutate();
	bool mutate_add_link();
	bool mutate_add_neuron(); // pick a random enabled link and split it
	bool mutate_add_cascade(); // add a hidden neuron that has inputs from all input nodes
	// and hidden nodes and is connected to all outputs.
	// In addition, all existing connection are frozen.
	bool mutate_remove_neuron();
	bool mutate_reenable_gene(); // reenable a random disabled gene
	bool mutate_toggle_enable();
	bool mutate_weights();
	bool mutate_reset_weights();
	bool mutate_remove_gene();

	// crossovers
	genome random_crossover(const genome& other) const;
	genome crossover_multipoint_best(const genome& other) const; // a crossover for topology convergence
	genome crossover_multipoint_rnd(const genome& other) const; // the original crossover
	genome crossover_multipoint_avg(const genome& other) const; // a crossover for weights convergence

	network generate_network() const;

  private:
	std::vector<size_t> helper_generate_candidate_idx();

  public:
	static const neuron_id_t BIAS_ID; // the first neuron is always the bias (see definition).

  private:
	unsigned int m_number_of_inputs; // [1:m_number_of_inputs] are the inputs.
	unsigned int m_number_of_outputs; // [m_number+1:m_number+m_number_of_outputs] are the outputs.

	std::vector<gene> m_genes;
	std::vector<neuron_id_t> m_known_neuron_ids;

	base_neat* m_neat;

	double m_fitness;
	double m_adjusted_fitness;

	bool reenable_gene_ok() const;

	// assumes the genes are sorted by innovation number
	template<typename func_t>
	inline genome helper_crossover_multipoint(const genome& other, const func_t& get_gene_from_parents) const {
		genome offspring(m_neat);

		auto oya1_gene_it = this->m_genes.cbegin();
		auto oya2_gene_it = other.m_genes.cbegin();

		while (oya1_gene_it != this->m_genes.cend() && oya2_gene_it != other.m_genes.cend()) {
			if (oya1_gene_it->innov_num == oya2_gene_it->innov_num) {
				gene new_gene = get_gene_from_parents(*this, *oya1_gene_it, other, *oya2_gene_it);

				// randomly disable the gene if a parent has it disabled
				new_gene.enabled = (oya1_gene_it->enabled && oya2_gene_it->enabled) || reenable_gene_ok();

				offspring.add_gene(std::move(new_gene));

				++oya1_gene_it;
				++oya2_gene_it;
				// otherwise, we've got a disjoint gene
			} else if (oya1_gene_it->innov_num < oya2_gene_it->innov_num) { // keep genes sorted by innovation number
				if (this->m_fitness >= other.m_fitness) {
					gene new_gene(*oya1_gene_it);
					new_gene.enabled = oya1_gene_it->enabled || reenable_gene_ok();
					offspring.add_gene(std::move(new_gene));
				}
				++oya1_gene_it;
			} else {
				if (other.m_fitness >= this->m_fitness) {
					gene new_gene(*oya2_gene_it);
					new_gene.enabled = oya2_gene_it->enabled || reenable_gene_ok();
					offspring.add_gene(std::move(new_gene));
				}
				++oya2_gene_it;
			}
		}

		// add excess genes of the fittest parent (both are considered to be the fittest in case of same fitness)

		if (this->m_fitness >= other.m_fitness) {
			while (oya1_gene_it != this->m_genes.cend()) {
				gene new_gene(*oya1_gene_it);
				new_gene.enabled = oya1_gene_it->enabled || reenable_gene_ok();
				offspring.add_gene(std::move(new_gene));
				++oya1_gene_it;
			}
		}

		if (other.m_fitness >= this->m_fitness) {
			while (oya2_gene_it != other.m_genes.cend()) {
				gene new_gene(*oya2_gene_it);
				new_gene.enabled = oya2_gene_it->enabled || reenable_gene_ok();
				offspring.add_gene(std::move(new_gene));
				++oya2_gene_it;
			}
		}

		return std::move(offspring);
	}

	friend std::ostream& operator<<(std::ostream& os, const genome& genome);
	friend serializer& operator<<(serializer& ser, const genome& genome);
	friend deserializer& operator>>(deserializer& des, genome& genome);
};

std::ostream& operator<<(std::ostream& os, const genome& genome);
serializer& operator<<(serializer& ser, const genome& genome);
deserializer& operator>>(deserializer& des, genome& genome);
}
