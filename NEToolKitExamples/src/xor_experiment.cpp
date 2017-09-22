#include <iostream>
#include <ctime>
#include <random> // random_device, mt19937

#include <netkit/network/network.h>
#include <netkit/network/network_primitive_types.h>
#include <netkit/network/activation_functions.h>
#include <netkit/neat/genome.h>
#include <netkit/neat/gene.h>
#include <netkit/neat/parameters.h>
#include <netkit/neat/neat.h>

#include "xor_experiment.h"
#include "utils.h"

struct exp_stats {
	size_t number_of_generations;
	size_t number_of_neurons;
	size_t number_of_links;
	double best_fitness_ever;
	bool success;
};

void print_species_stats(const netkit::species& spec);
void rate_xor_population(netkit::neat& neat);
void print_xor_network_results(netkit::network& net);
bool is_a_xor_solution(const netkit::genome& geno);
exp_stats run_xor_experiment(bool display_xor_experiment_details);

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

	netkit::neuron_id_t i1 = net.add_neuron(netkit::INPUT, netkit::neuron{&netkit::steepened_sigmoid});
	netkit::neuron_id_t i2 = net.add_neuron(netkit::INPUT, netkit::neuron{&netkit::steepened_sigmoid});
	netkit::neuron_id_t o = net.add_neuron(netkit::OUTPUT, netkit::neuron{&netkit::steepened_sigmoid});
	netkit::neuron_id_t h = net.add_neuron(netkit::HIDDEN, netkit::neuron{&netkit::steepened_sigmoid});

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

	wait_user();
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

	wait_user();
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

void run_100_xor_experiments() {
	unsigned int number_of_experiments = 100;
	std::cout << "Starting 100 xor evolution experiments..." << std::endl;

	double average_number_of_generations = 0;
	double average_neurons = 0;
	double average_links = 0;
	double average_fitness = 0;
	unsigned int number_of_success = 0;
	for (unsigned int i = 0; i < 100; ++i) {
		exp_stats stats = run_xor_experiment(false);

		std::cout << "\n======= experiment " << i;
		if (stats.success) {
			std::cout << " (success)";
		} else {
			std::cout << " (failure)";
		}
		std::cout << " ===========" << std::endl;
		std::cout << "Number of generations = " << stats.number_of_generations
		          << ", number of neurons = " << stats.number_of_neurons
		          << ", number of links = " << stats.number_of_links
		          << ", fitness = " << stats.best_fitness_ever
		          << std::endl;

		// I do not move the division outside of the loop to avoid too much float point precision loss...
		average_number_of_generations += static_cast<double>(stats.number_of_generations) / static_cast<double>(number_of_experiments);
		average_neurons += static_cast<double>(stats.number_of_neurons) / static_cast<double>(number_of_experiments);
		average_links += static_cast<double>(stats.number_of_links) / static_cast<double>(number_of_experiments);
		average_fitness += stats.best_fitness_ever / static_cast<double>(number_of_experiments);
		if (stats.success)
			number_of_success += 1;
	}

	std::cout << "\n====== summary ======" << std::endl;
	std::cout << "Average generations required to get a solution:\t" << average_fitness << std::endl;
	std::cout << "Average neurons in the best solutions:\t-\t" << average_neurons << std::endl;
	std::cout << "Average links in the best solutions:\t-\t" << average_links << std::endl;
	std::cout << "Average fitness of the best solutions:\t-\t" << average_fitness << std::endl;
	std::cout << "Number of successes:\t-\t-\t-\t" << number_of_success << " (which means " << number_of_experiments - number_of_success << " failure(s))" << std::endl;
}

void run_one_xor_experiments() {
	std::cout << "Starting one detailed xor evolution experiment..." << std::endl;
	run_xor_experiment(true);
}

// ===== local functions =====

exp_stats run_xor_experiment(bool display_xor_experiment_details) {
	// init NEAT
	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 1;
	params.initial_population_size = 300;
	netkit::neat neat(params);
	neat.init();

	exp_stats stats;
	stats.success = false; // will be set to true if a solution is found.
	stats.number_of_generations = 100; // will be changed to the actual number of generations until a solution is found.

	rate_xor_population(neat);

	if (display_xor_experiment_details) {
		std::cout << "\n\n======== Here's the initial generation's population. =========" << std::endl;
		for (netkit::species &spec : neat.get_all_species()) {
			spec.update_stats();
			print_species_stats(spec);
		}
		wait_user();
	}

	// go for a maximum of 100 generations evolution.
	for (size_t gen = 1; gen <= stats.number_of_generations; ++gen) {
		if (display_xor_experiment_details)
			std::cout << "\n\n======== Here's the generation " << gen << "'s population. =========" << std::endl;

		neat.epoch(); // go to next generation

		rate_xor_population(neat);
		neat.notify_end_of_fitness_rating();

		if (display_xor_experiment_details) {
			for (netkit::species &spec : neat.get_all_species()) {
				spec.update_stats();
				print_species_stats(spec);
			}
		}

		if (is_a_xor_solution(neat.get_best_genome_ever())) {
			if (display_xor_experiment_details)
				std::cout << "\n=====> Found a solution!" << std::endl;
			stats.success = true;
			stats.number_of_generations = gen;
			break;
		}

		//if (display_xor_experiment_details)
		//  wait_user();
	}

	const netkit::genome &best_geno = neat.get_best_genome_ever();
	netkit::network net = best_geno.generate_network();
	stats.best_fitness_ever = best_geno.get_fitness();
	stats.number_of_neurons = net.number_of_neurons();
	stats.number_of_links = net.number_of_links();

	if (display_xor_experiment_details) {
		std::cout << "\nHere's the best genome ever produced for this run:" << std::endl;
		std::cout << best_geno << std::endl;
		std::cout << net << std::endl;
		print_xor_network_results(net);
	}

	return stats;
}

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

		double error_sum = 0;
		for (auto run : runs) {
			//std::cout << "-- ";
			//std::cout << inputs_per_run[run][0] << " ~& " << inputs_per_run[run][1] << " ";
			net.flush();
			net.load_inputs(inputs_per_run[run]);
			net.activate_until_relaxation();
			error_sum += std::abs(net.get_outputs()[0] - expected_output_per_run[run]);
			//std::cout << "res: " << net.get_outputs()[0] << " (" << std::abs(net.get_outputs()[0] - expected_output_per_run[run]) << ") ";
		}

		double fitness = std::pow(4.0 - error_sum, 2);
		org.set_fitness(fitness);
		//std::cout << "fitness: " << fitness << std::endl;
	}
}

void print_xor_network_results(netkit::network& net) {
	for (auto& inputs : inputs_per_run) {
		std::cout << "==================" << std::endl;
		std::cout << inputs[0] << " xor " << inputs[1] << std::endl;
		net.flush();
		net.load_inputs(inputs);
		net.activate_until_relaxation();
		std::cout << "Result: " << net.get_outputs()[0] << std::endl;
	}
}

bool is_a_xor_solution(const netkit::genome& geno) {
	netkit::network net = geno.generate_network();

	std::vector<size_t> runs = {0, 1, 2, 3};
	for (auto run : runs) {
		net.flush();
		net.load_inputs(inputs_per_run[run]);
		net.activate_until_relaxation();
		if (std::abs(net.get_outputs()[0] - expected_output_per_run[run]) > 0.45) {
			return false;
		}
	}

	return true;
}
