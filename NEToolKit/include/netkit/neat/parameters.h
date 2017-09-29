#pragma once

#include "neat_primitive_types.h"

namespace netkit {
struct parameters {
	parameters() {
		mutation_probs[ADD_LINK] = 0.06;
		mutation_probs[ADD_NEURON] = 0.03;
		mutation_probs[REMOVE_GENE] = 0.03;
		mutation_probs[REENABLE_GENE] = 0.03;
		mutation_probs[TOGGLE_ENABLE] = 0.03;
		mutation_probs[RESET_WEIGHTS] = 0.03;
		mutation_probs[ONE_WEIGHT] = 0.09;
		mutation_probs[ALL_WEIGHTS] = 0.21;
	}

	// === general ===
	unsigned int number_of_inputs = 2;
	unsigned int number_of_outputs = 1;
	size_t initial_population_size = 150;

	// If the entire population doesn't improve for more that "refocusing_threshold" generations,
	// only the top two species are allowed to reproduce, to refocus on the most promising species.
	// TODO: not yet implemented.
	// Not used by rtNEAT.
	unsigned int refocusing_threshold = 40;

	// If a species doesn't improve for "no_reproduction_threshold" generations, it is not allowed to reproduce.
	// Not used by rtNEAT.
	unsigned int no_reproduction_threshold = 30;

	// === compatibility measurement coefficients ===
	/* Those are the coefficients for the compatibility distance formula which is
	 * dist = c1*E/N + c2*D/N + c3*W
	 * with N the number of genes of the larger genome,
	 *		E the number of excess genes,
	 *		D the number of disjoint genes,
	 *		W the average weight differences of matching genes.
	 */
	double distance_coef_c1 = 1.0;
	double distance_coef_c2 = 1.5;
	double distance_coef_c3 = 0.4;

	// If the compatibility distance between two genomes is lesser or equal than the threshold,
	// they are considered to be in the same specie.
	double compatibility_threshold = 3.0;

	// enable dynamic compatibility threshold to target a specific number of species.
	bool dynamic_compatibility_threshold = false;
	unsigned int target_number_of_species = 10;
	double compatibility_threshold_change_step = 0.05;

	// === mutations ===
	double mutation_during_crossover_prob = 0.25;
	double mutation_probs[NUMBER_OF_MUTATIONS];

	double initial_weight_perturbation = 10.0;
	double weight_mutation_power = 3.0;

	// === crossovers ===
	// crossover probability
	double crossover_prob = 0.20;
	double interspecies_crossover_prob = 0.05;

	unsigned int crossover_multipoint_rnd_weight = 2;
	unsigned int crossover_multipoint_best_weight = 0;
	unsigned int crossover_multipoint_avg_weight = 0;

	unsigned int sum_all_crossover_weights() const {
		return crossover_multipoint_avg_weight + crossover_multipoint_best_weight
			+ crossover_multipoint_rnd_weight;
	}

	// === other ===
	unsigned int babies_stolen = 0; // TODO: not yet implemented

	// === rtNEAT specifics ===
	unsigned int number_of_replacements_before_species_reorganization = 5;
	tick_t minmum_alive_time_before_being_replaced = 500;
	double proportion_of_eligible_at_any_epoch = 0.5;
};
}
