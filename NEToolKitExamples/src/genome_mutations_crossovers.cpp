#include <netkit/neat/genome.h>
#include <netkit/neat/neat.h>

#include "genome_mutations_crossovers.h"

void run_genome_mutations_crossovers() {
	std::cout << "Starting various mutations and crossover on simple genomes..." << std::endl;

	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 2;
	params.compatibility_threshold = 2;
	netkit::neat neat(std::move(params));

	netkit::genome geno(&neat);
	std::cout << "\nInitial genome: " << geno << std::endl;

	geno.mutate_add_link();
	std::cout << "\nMutate add link:" << std::endl << geno << std::endl;

	geno.mutate_add_link();
	std::cout << "\nMutate add link:" << std::endl << geno << std::endl;

	geno.mutate_toggle_enable();
	std::cout << "\nMutate toggle enable:" << std::endl << geno << std::endl;

	// make a new genome based on geno
	netkit::genome geno2(geno);

	geno.set_fitness(10);
	std::cout << "\nSetting genome's fitness to " << geno.get_fitness() << "." << std::endl;

	geno.mutate_reenable_gene();
	std::cout << "\nMutate reenable gene:" << std::endl << geno << std::endl;

	geno.mutate_one_weight();
	std::cout << "\nMutate one weight:" << std::endl << geno << std::endl;

	geno.mutate_all_weights();
	std::cout << "\nMutate all weights:" << std::endl << geno << std::endl;

	geno.mutate_add_neuron();
	std::cout << "\nMutate add neuron:" << std::endl << geno << std::endl;

	for (size_t i = 0; i < 20; i++) {
		geno2.random_mutate();
	}
	std::cout << "\nHere is another genome based on an older version of the previous one that has been randomly mutated:" <<
			  std::endl << geno2 << std::endl;

	std::cout << "\nDistance between the two genomes: " << geno.distance_to(geno2) << std::endl;
	std::cout << "Compatibility threshold is " << neat.params.compatibility_threshold << " so ";
	if (geno.is_compatible_with(geno2)) {
		std::cout << "they're considered compatibles." << std::endl;
	} else {
		std::cout << "they're NOT considered compatibles." << std::endl;
	}

	std::cout << "\nMultipoint crossover best:" << std::endl << geno.crossover_multipoint_best(geno2) << std::endl;

	std::cout << "\nMultipoint crossover random:" << std::endl << geno.crossover_multipoint_rnd(geno2) << std::endl;

	std::cout << "\nMultipoint crossover average (allow weights convergence):" << std::endl <<
			  geno.crossover_multipoint_avg(geno2) << std::endl;

	geno2.set_fitness(20);
	std::cout << "\nSetting second genome's fitness to " << geno2.get_fitness() << "." << std::endl;

	std::cout << "\nRandom crossover:" << std::endl << geno.random_crossover(geno2) << std::endl;
}
