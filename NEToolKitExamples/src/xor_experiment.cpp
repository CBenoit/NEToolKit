#include <iostream>
#include <ctime>

#include <netkit/network/network.h>
#include <netkit/network/network_primitive_types.h>
#include <netkit/network/activation_functions.h>
#include <netkit/neat/genome.h>
#include <netkit/neat/gene.h>
#include <netkit/neat/parameters.h>
#include <netkit/neat/neat.h>

#include "xor_experiment.h"

void print_species_stats(const netkit::species& spec);
void rate_xor_population(netkit::neat& neat);
void print_xor_network_results(netkit::network& net);

std::vector<std::vector<netkit::neuron_value_t>> inputs_per_run = {
	{0, 0},
	{1, 0},
	{0, 1},
	{1, 1}
};

std::vector<netkit::neuron_value_t> expected_output_per_run = {0, 1, 1, 0};

void xor_network_test() {
	std::cout << "Starting the (handmade) example xor network..." << std::endl;

	netkit::network net;

	netkit::neuron_id_t i1 = net.add_neuron(netkit::INPUT, netkit::neuron{&netkit::sigmoid});
	netkit::neuron_id_t i2 = net.add_neuron(netkit::INPUT, netkit::neuron{&netkit::sigmoid});
	netkit::neuron_id_t o = net.add_neuron(netkit::OUTPUT, netkit::neuron{&netkit::sigmoid});
	netkit::neuron_id_t h = net.add_neuron(netkit::HIDDEN, netkit::neuron{&netkit::sigmoid});

	net.add_link(netkit::network::BIAS_ID, h, -15);
	net.add_link(netkit::network::BIAS_ID, o, -5);
	net.add_link(i1, h, 10);
	net.add_link(i2, h, 10);
	net.add_link(i1, o, 10);
	net.add_link(i2, o, 10);
	net.add_link(h, o, -20);

	std::cout << "Neural network created." << std::endl;
	std::cout << net << std::endl;

	print_xor_network_results(net);

	std::cout << "\nPerforming the same with a network generated from a genome..." << std::endl;

	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 1;
	netkit::neat neat(params);

	netkit::genome genome(&neat);
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(),
								 netkit::genome::BIAS_ID, 4, -15));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(),
								 netkit::genome::BIAS_ID, 3, -5));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 1, 4, 10));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 2, 4, 10));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 1, 3, 10));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 2, 3, 10));
	genome.add_gene(netkit::gene(neat.innov_pool.next_innovation(), 4, 3, -20));

	std::cout << genome << std::endl;

	netkit::network gennet = genome.generate_network();
	std::cout << gennet << std::endl;

	print_xor_network_results(gennet);

	std::cout << "\nBenchmark: how much does it takes to activate the xor network 100 000 times?" << std::endl;
	std::cout << "Running the benchmark..." << std::endl;
	clock_t begin = std::clock();

	for (unsigned int i = 100000; i--; ) {
		net.activate();
	}

	clock_t end = std::clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Done!" << std::endl;
	std::cout << "It took " << elapsed_secs << " seconds of processor time to do so." << std::endl;
}

void run_xor_experiment() {
	std::cout << "Starting the xor evolution experiment..." << std::endl;

	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 1;
	netkit::neat neat(params);
	neat.init();

	std::cout << "\n\n======== Here's the initial generation's population. =========" << std::endl;
	rate_xor_population(neat);
	for (netkit::species &spec : neat.get_all_species()) {
		spec.update_stats();
		print_species_stats(spec);
	}

	// go for a 100 generations evolution.
	for (unsigned int gen = 1; gen <= 100; ++gen) {
		neat.epoch(); // go to next generation

		std::cout << "\n\n======== Here's the generation " << gen << "'s population. =========" << std::endl;
		rate_xor_population(neat);
		for (netkit::species &spec : neat.get_all_species()) {
			spec.update_stats();
			print_species_stats(spec);
		}
	}

	const netkit::genome& best_geno = neat.get_best_genome();
	std::cout << "\nHere's the best genome produced:" << std::endl;
	std::cout << best_geno << std::endl;
	netkit::network net = best_geno.generate_network();
	print_xor_network_results(net);
}

// ===== local functions =====

void print_species_stats(const netkit::species& spec) {
	std::cout << "<species: id = " << spec.get_id() << ", age = " << spec.get_age()
	          << ", age of last improvement = " << spec.get_age_of_last_improvement()
	          << "\n\tavg fitness = " << spec.get_avg_fitness()
	          << ", best fitness = " << spec.get_best_fitness()
	          << ", best fitness ever = " << spec.get_best_fitness_ever()
	          << "\n\tnumber of members = " << spec.number_of_members() << ">" << std::endl;
}

void rate_xor_population(netkit::neat& neat) {
	std::vector<size_t> runs = {0, 1, 2, 3};
	while (neat.has_more_organisms_to_process()) {
		netkit::organism org = neat.generate_and_get_next_organism();
		netkit::network& net = org.get_network();

		double fitness = 0;
		for (auto run : runs) {
			net.load_inputs(inputs_per_run[run]);
			net.activate_until_relaxation();
			fitness += 1 - std::abs(net.get_outputs()[0] - expected_output_per_run[run]);
		}

		org.set_fitness(fitness);
	}
}

void print_xor_network_results(netkit::network& net) {
	for (auto& inputs : inputs_per_run) {
		std::cout << "==================" << std::endl;
		std::cout << inputs[0] << " xor " << inputs[1] << std::endl;
		net.load_inputs(inputs);
		net.activate_until_relaxation();
		std::cout << "Result: " << net.get_outputs()[0] << std::endl;
	}
}