#include <string>

#include <netkit/csv/serializer.h>
#include <netkit/csv/deserializer.h>
#include <netkit/neat/neat.h>
#include <netkit/neat/gene.h>
#include <netkit/neat/genome.h>

#include "serialization_tests.h"
#include "utils.h"

void print_neat_state(netkit::neat& neat);
void rate_population(netkit::neat& neat);

void run_serialization_tests() {
	std::cout << "Starting serialization tests..." << std::endl;

	netkit::parameters params;
	params.initial_population_size = 5;
	netkit::neat neat(params);

	std::string target_filename = "serialization_test.csv";
	std::cout << "Going to write in the file \"" << target_filename << "\"." << std::endl;

	{ // gene
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

	{ // genome
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

		des.close();
	}

	{ // population
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

		des.close();
	}

	{ // species
		wait_user();
		std::cout << "\nSpecies serialization test." << std::endl;

		netkit::genome genome(&neat);
		genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), netkit::genome::BIAS_ID, 4, -15));
		netkit::population pop(&neat);
		netkit::species species(&neat, &pop, 0, genome);
		for (int i = 0; i < 5; ++i) {
			netkit::genome_id_t id = pop.add_genome(genome.get_random_mutation().get_random_mutation().get_random_mutation());
			pop.get_genome(id).set_fitness(i * 5);
			species.add_member(id);
		}
		species.update_stats();
		species.sort_by_fitness();
		std::cout << species << std::endl;

		netkit::serializer ser(target_filename);
		ser << species;
		ser.close();

		std::cout << "\nDeserialization...\n" << std::endl;

		netkit::deserializer des(target_filename);

		netkit::genome dummy(&neat); // FIXME: find a better way to handle that? Maybe with a constructor that uses the deserializer?
		netkit::species species2(&neat, &pop, 0, dummy); // the dummy will be replaced by the right representant during deserialization.
		des >> species2;
		species2.update_stats();
		std::cout << species2 << std::endl;
		des.close();
	}

	{ // whole NEAT simulation state
		wait_user();
		std::cout << "\nWhole NEAT simulation state serialization test." << std::endl;

		neat.init();
		rate_population(neat);

		std::cout << "Initial population:" << std::endl;
		print_neat_state(neat);

		{
			netkit::serializer ser(target_filename);
			ser << neat;
			ser.close();
		}

		for (size_t gen = 1; gen <= 5; ++gen) {
			netkit::neat neat2(params);

			netkit::deserializer des(target_filename);
			des >> neat2;
			des.close();

			std::cout << "\nDeserialization:" << std::endl;
			print_neat_state(neat2);

			neat2.epoch();
			rate_population(neat2);

			wait_user();

			std::cout << "\n\n======== Here's the generation " << gen << "'s population. =========" << std::endl;
			print_neat_state(neat2);

			netkit::serializer ser(target_filename);
			ser << neat2;
			ser.close();
		}
	}
}

void print_neat_state(netkit::neat& neat) {
	for (auto& species : neat.get_all_species()) {
		species.update_stats();
		species.sort_by_fitness();
		std::cout << species << std::endl;
	}

	auto opt_best_geno = neat.get_best_genome_ever();
	if (opt_best_geno.has_value()) {
		std::cout << "\nCurrent best genome:" << std::endl;
		std::cout << *opt_best_geno << std::endl;
	}
}

void rate_population(netkit::neat& neat) {
	while (neat.has_more_organisms_to_process()) {
		netkit::organism org = neat.generate_and_get_next_organism();
		org.set_fitness(rand() / 1000000.0);
	}
}
