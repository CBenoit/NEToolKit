#pragma once

#include <netkit/neat/species.h>

struct exp_stats {
	size_t number_of_generations;
	size_t number_of_neurons;
	size_t number_of_links;
	double best_fitness_ever;
	bool success;
};

void wait_user();

void print_species_stats(const netkit::species& spec);

double compute_standard_deviation(unsigned int number_of_values, double average, std::function<double(size_t)> getter);

void print_exp_stats(exp_stats& stats);
