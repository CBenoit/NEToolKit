#include <netkit/neat/neat.h>
#include <netkit/neat/parameters.h>

#include "random_evolution.h"

void run_random_evolution() {
	std::cout << "Starting a random NEAT evolution (random fitness on each generation)." << std::endl;

	netkit::parameters params;
	params.initial_population_size = 30;
	netkit::neat neat(std::move(params));
	neat.init();

	// TODO: write the rest of the experiment.
}
