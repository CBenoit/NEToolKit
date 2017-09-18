#pragma once

namespace netkit {
struct parameters {
	// === general ===
	unsigned int number_of_inputs = 2;
	unsigned int number_of_outputs = 1;
	unsigned int population = 150;

	// If the entire population doesn't improve for more that "refocusing_threshold" generations,
	// only the top two species are allowed to reproduce, to refocus of the most promising species.
	unsigned int refocusing_threshold = 20;

	// === compatibility measurement coefficients ===
	/* Those are the coefficients for the compatibility distance formula which is
	 * dist = c1*E/N + c2*D/N + c3*W
	 * with N the number of genes of the larger genome,
	 *		E the number of excess genes,
	 *		D the number of disjoint genes,
	 *		W the average weight differences of matching genes.
	 */
	double distance_coef_c1 = 1.0;
	double distance_coef_c2 = 1.0;
	double distance_coef_c3 = 0.4;

	// If the compatibility distance between two genomes is lesser or equal than the threshold,
	// they are considered to be in the same specie.
	double compatibility_threshold = 3.0;

	// === mutations ===
	double mutation_prob = 0.4;

	unsigned int mutation_add_link_weight = 2;
	unsigned int mutation_add_neuron_weight = 1;
	unsigned int mutation_reenable_gene_weight = 1;
	unsigned int mutation_toggle_enable_weight = 1;
	unsigned int mutation_one_weight_weight = 3;
	unsigned int mutation_all_weights_weight = 3;

	unsigned int sum_all_mutation_weights() const {
		return mutation_add_link_weight + mutation_add_neuron_weight
			+ mutation_reenable_gene_weight + mutation_toggle_enable_weight
			+ mutation_one_weight_weight + mutation_all_weights_weight;
	}

	double initial_weight_perturbation = 10.0;
	double weight_mutation_power = 3.0;

	// === crossovers ===
	// crossover probability
	double crossover_prob = 0.3;
	double interspecies_crossover_prob = 0.05;

	unsigned int crossover_multipoint_rnd_weight = 2;
	unsigned int crossover_multipoint_best_weight = 1;
	unsigned int crossover_multipoint_avg_weight = 1;

	unsigned int sum_all_crossover_weights() const {
		return crossover_multipoint_avg_weight + crossover_multipoint_best_weight
			+ crossover_multipoint_rnd_weight;
	}

	// === other ===
	unsigned int babies_stolen = 0; // not yet implemented
};
}
