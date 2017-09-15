#include <netkit/neat/genome.h>
#include <netkit/neat/neat.h>

#include "genome_mutations_crossovers.h"

void run_genome_mutations_crossovers() {
	std::cout << "Starting various mutations and crossover on simple genomes..." << std::endl;

	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 2;
	netkit::neat neat(std::move(params));

	netkit::genome geno(&neat);
	netkit::genome geno2(&neat);
	std::cout << "\nInitial genome: " << geno << std::endl;

	geno.mutate_add_link();
	std::cout << "\nMutate add link:" << std::endl << geno << std::endl;

	geno.mutate_add_link();
	std::cout << "\nMutate add link:" << std::endl << geno << std::endl;

	geno.mutate_toggle_enable();
	std::cout << "\nMutate toggle enable:" << std::endl << geno << std::endl;

	geno.mutate_reenable_gene();
	std::cout << "\nMutate reenable gene:" << std::endl << geno << std::endl;

	geno.mutate_one_weight();
	std::cout << "\nMutate one weight:" << std::endl << geno << std::endl;

	geno.mutate_all_weights();
	std::cout << "\nMutate all weights:" << std::endl << geno << std::endl;

	geno.mutate_add_neuron();
	std::cout << "\nMutate add neuron:" << std::endl << geno << std::endl;
}
