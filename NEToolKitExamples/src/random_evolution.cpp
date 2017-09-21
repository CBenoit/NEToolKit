#include <netkit/neat/neat.h>
#include <netkit/neat/parameters.h>
#include <netkit/neat/species.h>

#include "random_evolution.h"

void run_random_evolution() {
	std::cout << "Starting a random NEAT evolution (random fitness on each generation)." << std::endl;

	netkit::parameters params;
	params.initial_population_size = 10;
	params.compatibility_threshold = 1;
	netkit::neat neat(std::move(params));
	neat.init();

	for (unsigned int gen = 0; gen < 10; ++gen) {
		while (neat.has_more_organisms_to_process()) {
			netkit::organism org = neat.generate_and_get_next_organism();
			org.set_fitness(rand() / 1000000.0);
		}

		std::cout << "\n\n======== Here's the generation " << gen << "'s population. =========" << std::endl;
		for (netkit::species &spec : neat.get_all_species()) {
			spec.update_stats();
			std::cout << spec << std::endl;
		}

		neat.epoch(); // go to next generation
	}
}
