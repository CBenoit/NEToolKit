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
		, m_neat(neat_instance) {
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
		, m_neat(other.m_neat) {}

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
	genome random_crossover(const genome& other, const parameters& params) const;
	genome crossover_multipoint_rnd(const genome& other) const;
	genome crossover_multipoint_avg(const genome& other) const;

	network generate_network() const;

public:
	static const neuron_id_t BIAS_ID = 0; // the first neuron is always the bias.

private:
	const unsigned int m_number_of_inputs; // [1:m_number_of_inputs] are the inputs.
	const unsigned int m_number_of_outputs; // [m_number+1:m_number+m_number_of_outputs] are the outputs.

	std::vector<neuron_id_t> m_known_neuron_ids;
	std::vector<gene> m_genes;

	neat* m_neat;

	double m_fitness;

	friend std::ostream& operator<<(std::ostream& os, const genome& genome);
};
}
