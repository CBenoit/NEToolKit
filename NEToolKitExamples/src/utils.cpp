#include <limits>
#include <iostream>
#include <cmath>

#include "utils.h"

void wait_user() {
	std::cout << "Press Enter to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void print_species_stats(const netkit::species& spec) {
	std::cout << "<species: id = " << spec.get_id() << ", age = " << spec.get_age()
			  << ", age of last improvement = " << spec.get_age_of_last_improvement()
			  << "\n\tavg fitness = " << spec.get_avg_fitness()
			  << ", best fitness = " << spec.get_best_fitness()
			  << ", best fitness ever = " << spec.get_best_fitness_ever()
			  << "\n\tnumber of members = " << spec.number_of_members() << ">" << std::endl;
}

double compute_standard_deviation(unsigned int number_of_values, double average, std::function<double(size_t)> getter) {
	double result = 0;
	for (unsigned int i = 0; i < number_of_values; ++i) {
		result += std::pow(getter(i) - average, 2) / number_of_values;
	}
	// result is currently the variance.
	return std::sqrt(result);
}

void print_exp_stats(exp_stats& stats) {
	std::cout << "Number of generations = " << stats.number_of_generations
			  << ", number of neurons = " << stats.number_of_neurons
			  << ", number of links = " << stats.number_of_links
			  << ", best fitness = " << stats.best_fitness_ever
			  << std::endl;
}
