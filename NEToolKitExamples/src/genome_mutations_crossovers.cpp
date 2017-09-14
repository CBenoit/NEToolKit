#include <netkit/neat/genome.h>
#include <netkit/neat/neat.h>

#include "genome_mutations_crossovers.h"

void run_genome_mutations_crossovers() {
	std::cout << "Starting various mutations and crossover on simple genomes..." << std::endl;

	netkit::init_neat();

	netkit::genome geno(2, 2);
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
}
