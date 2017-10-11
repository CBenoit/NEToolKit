#include <string>

#include <netkit/csv/serializer.h>
#include <netkit/csv/deserializer.h>
#include <netkit/neat/neat.h>
#include <netkit/neat/gene.h>
#include <netkit/neat/genome.h>

#include "serialization_tests.h"
#include "utils.h"

void run_serialization_tests() {
	std::cout << "Starting serialization tests..." << std::endl;

	netkit::neat neat{netkit::parameters{}};

	std::string target_filename = "serialization_test.csv";
	std::cout << "Going to write in the file \"" << target_filename << "\"." << std::endl;

	{
		wait_user();
		std::cout << "\nGenes serialization test." << std::endl;

		netkit::serializer ser(target_filename);

		std::vector<netkit::gene> genes;
		genes.emplace_back(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 4, -15);
		genes.emplace_back(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 3, -5);
		genes.emplace_back(neat.innov_pool.next_innovation(), 1, 4, 10);
		genes.emplace_back(neat.innov_pool.next_innovation(), 2, 4, 10);

		for (auto& g : genes) {
			std::cout << g << std::endl;
			ser << g;
		}

		ser.close();

		std::cout << "\nDeserialization...\n" << std::endl;

		netkit::deserializer des(target_filename);

		for (size_t i = 0; i < genes.size(); ++i) {
			netkit::gene g(0, 0, 0, 0);
			des >> g;
			std::cout << g << std::endl;
		}

		des.close();
	}

	{
		wait_user();
		std::cout << "\nGenome serialization test." << std::endl;

		netkit::genome genome(&neat);
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 4, -15));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 3, -5));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 1, 4, 10));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 2, 4, 10));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 1, 3, 10));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 2, 3, 10));
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 4, 3, -20));
		std::cout << genome << std::endl;

		netkit::serializer ser(target_filename);
		ser << genome;
		ser.close();

		std::cout << "\nDeserialization...\n" << std::endl;

		netkit::deserializer des(target_filename);

		netkit::genome genome2(&neat);
		des >> genome2;
		std::cout << genome2 << std::endl;
	}

	{
		wait_user();
		std::cout << "\nPopulation serialization test." << std::endl;

		netkit::genome genome(&neat);
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 4, -15));
		netkit::population pop(&neat);
		for (int i = 0; i < 5; ++i) {
			netkit::genome_id_t last = pop.add_genome(genome.get_random_mutation().get_random_mutation().get_random_mutation());
			std::cout << pop[last] << std::endl;
		}

		netkit::serializer ser(target_filename);
		ser << pop;
		ser.close();

		std::cout << "\nDeserialization...\n" << std::endl;

		netkit::deserializer des(target_filename);

		netkit::population pop2(&neat);
		des >> pop2;
		for (auto& g : pop2.get_all_genomes()) {
			std::cout << g << std::endl;
		}
	}

	// TODO: test for species
	// TODO: test for NEAT
}
