#pragma once

namespace netkit {
struct parameters {
	// general
	unsigned int number_of_inputs = 2;
	unsigned int number_of_outputs = 1;

	// mutations
	unsigned int mutation_add_link_weight = 1;
	unsigned int mutation_add_neuron_weight = 1;
	unsigned int mutation_reenable_gene_weight = 1;
	unsigned int mutation_toggle_enable_weight = 1;
	unsigned int mutation_one_weight_weight = 1;
	unsigned int mutation_all_weights_weight = 1;

	unsigned int sum_all_mutation_weights() const {
		return mutation_add_link_weight + mutation_add_neuron_weight
			+ mutation_reenable_gene_weight + mutation_toggle_enable_weight
			+ mutation_one_weight_weight + mutation_all_weights_weight;
	}

	// crossover
	double crossover_prob = 0.5;
};
}
