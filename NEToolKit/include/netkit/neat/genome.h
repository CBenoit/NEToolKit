#pragma once

#include <vector>
#include <iostream>

#include "gene.h"
#include "neat.h"
#include "netkit/network/network_primitive_types.h"
#include "netkit/network/network.h"

namespace netkit {
class genome {
public:
	genome(neat* neat_instance)
		: m_number_of_inputs(neat_instance->params.number_of_inputs)
		, m_number_of_outputs(neat_instance->params.number_of_outputs)
		, m_genes()
		, m_known_neuron_ids()
		, m_neat(neat_instance)
		, m_fitness(0) {
		m_known_neuron_ids.push_back(BIAS_ID);

		for (neuron_id_t i = 0; i < m_number_of_inputs; i++) {
			m_known_neuron_ids.push_back(i + 1);
		}

		for (neuron_id_t i = 0; i < m_number_of_outputs; i++) {
			m_known_neuron_ids.push_back(i + 1 + m_number_of_inputs);
		}
	}

	genome(const genome& other)
		: m_number_of_inputs(other.m_number_of_inputs)
		, m_number_of_outputs(other.m_number_of_outputs)
		, m_genes(other.m_genes)
		, m_known_neuron_ids(other.m_known_neuron_ids)
		, m_neat(other.m_neat)
	    , m_fitness(0) {}

	void add_gene(gene new_gene);

	bool link_exists(neuron_id_t from, neuron_id_t to);

	void set_fitness(double fitness);
	double get_fitness();

	// mutations
	genome get_random_mutation() const; // produce an offspring
	// all these directly modify the current genome
	bool random_mutate();
	bool mutate_add_link();
	bool mutate_add_neuron(); // pick a random enabled link and split it
	//bool mutate_remove_neuron(); TODO: maybe?
	bool mutate_reenable_gene(); // reenable a random disabled gene
	bool mutate_toggle_enable();
	bool mutate_one_weight();
	bool mutate_all_weights();

	// crossovers
	genome random_crossover(const genome& other) const;
	genome crossover_multipoint_best(const genome& other) const; // a crossover for topology convergence
	genome crossover_multipoint_rnd(const genome& other) const; // the original crossover
	genome crossover_multipoint_avg(const genome& other) const; // a crossover for weights convergence

	network generate_network() const;

public:
	static const neuron_id_t BIAS_ID; // the first neuron is always the bias (see definition).

private:
	const unsigned int m_number_of_inputs; // [1:m_number_of_inputs] are the inputs.
	const unsigned int m_number_of_outputs; // [m_number+1:m_number+m_number_of_outputs] are the outputs.

	std::vector<gene> m_genes;
	std::vector<neuron_id_t> m_known_neuron_ids;

	neat* m_neat;

	double m_fitness;

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
				new_gene.enabled = (oya1_gene_it->enabled and oya2_gene_it->enabled) or rand() % 2 == 0;

				offspring.add_gene(std::move(new_gene));

				++oya1_gene_it;
				++oya2_gene_it;
				// otherwise, we've got a disjoint gene
			} else if (oya1_gene_it->innov_num < oya2_gene_it->innov_num) { // keep genes sorted by innovation number
				if (this->m_fitness >= other.m_fitness) {
					gene new_gene(*oya1_gene_it);
					new_gene.enabled = oya1_gene_it->enabled or rand() % 2 == 0;
					offspring.add_gene(std::move(new_gene));
				}
				++oya1_gene_it;
			} else {
				if (other.m_fitness >= this->m_fitness) {
					gene new_gene(*oya2_gene_it);
					new_gene.enabled = oya2_gene_it->enabled or rand() % 2 == 0;
					offspring.add_gene(std::move(new_gene));
				}
				++oya2_gene_it;
			}
		}

		// add excess genes of the fittest parent (both are considered to be the fittest in case of same fitness)

		if (this->m_fitness >= other.m_fitness) {
			while (oya1_gene_it != this->m_genes.cend()) {
				gene new_gene(*oya1_gene_it);
				new_gene.enabled = oya1_gene_it->enabled or rand() % 2 == 0;
				offspring.add_gene(std::move(new_gene));
				++oya1_gene_it;
			}
		}

		if (other.m_fitness >= this->m_fitness) {
			while (oya2_gene_it != other.m_genes.cend()) {
				gene new_gene(*oya2_gene_it);
				new_gene.enabled = oya2_gene_it->enabled or rand() % 2 == 0;
				offspring.add_gene(std::move(new_gene));
				++oya2_gene_it;
			}
		}

		return std::move(offspring);
	}

	friend std::ostream& operator<<(std::ostream& os, const genome& genome);
};

std::ostream& operator<<(std::ostream& os, const genome& genome);
}
