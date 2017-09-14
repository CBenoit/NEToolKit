#pragma once

#include <vector>
#include <iostream>

#include "gene.h"
#include "parameters.h"
#include "netkit/network/network_primitive_types.h"
#include "netkit/network/network.h"

namespace netkit {
class genome {
public:
	genome(unsigned int number_of_inputs, unsigned int number_of_outputs)
		: m_number_of_inputs(number_of_inputs)
		, m_number_of_outputs(number_of_outputs)
		, m_genes() {}

	genome(const genome& other)
		: m_number_of_inputs(other.m_number_of_inputs)
		, m_number_of_outputs(other.m_number_of_outputs)
		, m_genes(other.m_genes) {}

	void add_gene(gene new_gene);

	// mutations
	genome random_mutate(const parameters& params) const;
	genome mutate_add_link() const;
	genome mutate_remove_link() const;
	genome mutate_add_neuron() const;
	genome mutate_remove_neuron() const;
	genome mutate_reenable_gene() const;
	genome mutate_toggle_enable() const;
	genome mutate_link_weight() const;

	// crossovers
	genome random_crossover(const genome& other, const parameters& params) const;
	genome crossover_multipoint_rnd(const genome& other) const;
	genome crossover_multipoint_avg(const genome& other) const;

	network generate_network() const;

public:
	static const neuron_id_t BIAS_ID = 0; // the first neuron is always the bias.

private:
	unsigned int m_number_of_inputs; // [1:m_number_of_inputs] are the inputs.
	unsigned int m_number_of_outputs; // [m_number+1:m_number+m_number_of_outputs] are the outputs.

	std::vector<gene> m_genes;

	friend std::ostream& operator<<(std::ostream& os, const genome& genome);
};
}
