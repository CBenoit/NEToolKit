#include <iostream>
#include <ctime>
#include <random> // random_device, mt19937
#include <functional>
#include <thread> // std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds

#include <netkit/network/network.h>
#include <netkit/network/network_primitive_types.h>
#include <netkit/network/activation_functions.h>
#include <netkit/neat/genome.h>
#include <netkit/neat/gene.h>
#include <netkit/neat/parameters.h>
#include <netkit/neat/neat.h>
#include <netkit/neat/rtneat.h>
#include <netkit/neat/novelbank.h>

#include "xor_experiment.h"
#include "utils.h"

void rate_xor_population(netkit::neat& neat);
double evaluate_network(netkit::network &net);
void print_xor_network_results(netkit::network& net);
bool is_a_xor_solution(const netkit::genome& geno);
exp_stats run_xor_experiment(bool display_xor_experiment_details, bool with_novelty = false);

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

	for (unsigned int i = 100000; i--;) {
		net.activate();
	}

	clock_t end = std::clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Done!" << std::endl;
	std::cout << "It took " << elapsed_secs << " seconds of processor time to do so." << std::endl;
}

void run_n_xor_experiments(unsigned int number_of_experiments, bool with_novelty, bool record_csv, std::string output_filename) {
	std::cout << "Starting " << number_of_experiments << " xor evolution experiments..." << std::endl;

	std::vector<exp_stats> all_results;
	all_results.reserve(number_of_experiments);

	double average_number_of_generations = 0;
	double average_neurons = 0;
	double average_links = 0;
	double average_fitness = 0;
	unsigned int number_of_success = 0;
	for (unsigned int i = 0; i < number_of_experiments; ++i) {
		exp_stats stats = run_xor_experiment(false, with_novelty);

		std::cout << "\n======= experiment " << i;
		if (stats.success) {
			std::cout << " (success)";
		} else {
			std::cout << " (failure)";
		}
		std::cout << " ===========" << std::endl;
		print_exp_stats(stats);

		// I do not move the division outside of the loop to avoid too much float point precision loss...
		average_number_of_generations += static_cast<double>(stats.number_of_generations) / static_cast<double>
										 (number_of_experiments);
		average_neurons += static_cast<double>(stats.number_of_neurons) / static_cast<double>(number_of_experiments);
		average_links += static_cast<double>(stats.number_of_links) / static_cast<double>(number_of_experiments);
		average_fitness += stats.best_fitness_ever / static_cast<double>(number_of_experiments);
		if (stats.success)
			number_of_success += 1;

		all_results.push_back(stats);

		if (record_csv) {
			netkit::serializer ser(output_filename, ";", true);

			ser.append(stats.success);
			ser.append(stats.number_of_generations);
			ser.append(stats.number_of_neurons);
			ser.append(stats.number_of_links);
			ser.append(stats.best_fitness_ever);

			ser.new_line();
			ser.close();
		}
	}

	double std_number_of_generations = compute_standard_deviation(number_of_experiments,
	average_number_of_generations, [&](size_t i) {
		return static_cast<double>(all_results[i].number_of_generations);
	});

	double std_neurons = compute_standard_deviation(number_of_experiments, average_neurons, [&](size_t i) {
		return static_cast<double>(all_results[i].number_of_neurons);
	});

	double std_links = compute_standard_deviation(number_of_experiments, average_links, [&](size_t i) {
		return static_cast<double>(all_results[i].number_of_links);
	});

	double std_fitness = compute_standard_deviation(number_of_experiments, average_fitness, [&](size_t i) {
		return all_results[i].best_fitness_ever;
	});

	std::cout << "\n====== summary ======" << std::endl;
	std::cout << "Average generations required to get a solution:\t" << average_fitness
			  << " (std = " << std_number_of_generations << ")" << std::endl;
	std::cout << "Average neurons in the best solutions:\t-\t" << average_neurons
			  << " (std = " << std_neurons << ")" << std::endl;
	std::cout << "Average links in the best solutions:\t-\t" << average_links
			  << " (std = " << std_links << ")" << std::endl;
	std::cout << "Average fitness of the best solutions:\t-\t" << average_fitness
			  << " (std = " << std_fitness << ")" << std::endl;
	std::cout << "Number of successes:\t-\t-\t-\t" << number_of_success
			  << " (which means " << number_of_experiments - number_of_success << " failure(s))" << std::endl;
}

void run_one_xor_experiment(bool with_novelty) {
	std::cout << "Starting one detailed xor evolution experiment..." << std::endl;
	exp_stats stats = run_xor_experiment(true, with_novelty);
	std::cout << "\nSummary:" << std::endl;
	print_exp_stats(stats);
}

void run_one_real_time_xor_experiment(bool display_details) {
	std::cout << "Starting one detailed realtime xor evolution experiment..." << std::endl;

	// init rtNEAT
	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 1;
	params.initial_population_size = 20;
	params.minmum_alive_time_before_being_replaced = 6;
	params.interspecies_crossover_prob = 0.15;
	netkit::rtneat rtneat(params);
	rtneat.init();
	rtneat.generate_all_organisms();
	std::vector<netkit::organism>& organisms = rtneat.get_all_organisms();

	if (display_details) {
		std::cout << "\n\n======== Here's the initial population. =========" << std::endl;
		for (netkit::species& spec : rtneat.get_all_species()) {
			spec.update_stats();
			print_species_stats(spec);
		}
		wait_user();
	}

	// go for a simulation of 1000 ticks.
	for (netkit::tick_t tick = 0; tick < 10000; ++tick) {
		if (display_details)
			std::cout << "\n\n======== tick " << tick << " =========" << std::endl;

		for (netkit::organism& org : organisms) {
			evaluate_network(org.get_network());
			org.increase_time_alive(); // very important!!
			if (display_details) {
				std::cout << "org " << org.get_genome_id() << "'s fitness = " << org.get_fitness() << std::endl;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		rtneat.epoch();

		// you may want to perform the following less often.
		rtneat.update_best_genome_ever();
		if (is_a_xor_solution(*rtneat.get_best_genome_ever())) {
			if (display_details)
				std::cout << "\n=====> Found a solution!" << std::endl;
			break;
		}
	}

	if (display_details) {
		auto opt_best_geno = rtneat.get_best_genome_ever();
		netkit::network net = opt_best_geno->generate_network();
		std::cout << "\nHere's the best genome ever produced for this run:" << std::endl;
		std::cout << *opt_best_geno << std::endl;
		std::cout << net << std::endl;
		print_xor_network_results(net);
	}
}

// ===== local functions =====

// --- for novelty only ---
std::optional<netkit::genome> novelty_rate_population(netkit::neat& neat, netkit::novelbank<xor_position>& novelbank) {
	std::vector<netkit::novelgenome<xor_position>> all_ng;
	all_ng.reserve(neat.pop()->size());

	while (neat.has_more_organisms_to_process()) {
		netkit::organism org = neat.generate_and_get_next_organism();
		netkit::novelgenome<xor_position> ng{org.get_genome_id()};

		std::vector<size_t> runs = {0, 1, 2, 3};
		netkit::network &net = org.get_network();
		for (auto run : runs) {
			net.flush();
			net.load_inputs(inputs_per_run[run]);
			net.activate_until_relaxation();
			ng.get_pos().values.push_back(net.get_outputs()[0]);
		}

		novelbank.pop_register(ng);
		all_ng.push_back(std::move(ng));
	}

	std::optional<netkit::genome> valid_candidate;

	// update fitness by evaluating novelty.
	for (auto& ng : all_ng) {
		neat.pop()->get_genome(ng.get_genome_id()).set_fitness(novelbank.evaluate(ng));

		if (is_a_xor_solution(neat.pop()->get_genome(ng.get_genome_id()))) {
			valid_candidate = {neat.pop()->get_genome(ng.get_genome_id())};
		}
	}

	novelbank.bank_update(); // update the novelbank using all new element evaluated so far.

	// clear the population (because no evaluation afterward)
	novelbank.pop_clear();

	return valid_candidate;
}
// ------------------------

exp_stats run_xor_experiment(bool display_xor_experiment_details, bool with_novelty) {
	// init novelty bank
	netkit::novelbank<xor_position> novelbank(999, 0.25, 10);

	// init NEAT
	netkit::parameters params;
	params.number_of_inputs = 2;
	params.number_of_outputs = 1;
	params.initial_population_size = 300;
	netkit::neat neat(params);
	neat.init();

	exp_stats stats;
	stats.success = false; // will be set to true if a solution is found.
	stats.number_of_generations = 150; // will be changed to the actual number of generations until a solution is found.

	if (with_novelty) {
		novelty_rate_population(neat, novelbank);
	} else {
		rate_xor_population(neat);
	}

	if (display_xor_experiment_details) {
		std::cout << "\n\n======== Here's the initial generation's population. =========" << std::endl;
		for (netkit::species& spec : neat.get_all_species()) {
			spec.update_stats();
			print_species_stats(spec);
		}
		wait_user();
	}

	std::optional<netkit::genome> opt_best_geno;

	// go for a maximum of stats.number_of_generations generations evolution.
	for (size_t gen = 1; gen <= stats.number_of_generations; ++gen) {
		if (display_xor_experiment_details)
			std::cout << "\n\n======== Here's the generation " << gen << "'s population. =========" << std::endl;

		neat.epoch(); // go to next generation

		if (with_novelty) {
			opt_best_geno = novelty_rate_population(neat, novelbank);

			if (display_xor_experiment_details) {
				for (netkit::species& spec : neat.get_all_species()) {
					spec.update_stats();
					print_species_stats(spec);
				}
			}

			if (opt_best_geno.has_value()) {
				if (display_xor_experiment_details)
					std::cout << "\n=====> Found a solution!" << std::endl;
				stats.success = true;
				stats.number_of_generations = gen;
				auto net = opt_best_geno->generate_network();
				stats.best_fitness_ever = evaluate_network(net);
				break;
			}
		} else {
			rate_xor_population(neat);
			neat.update_best_genome_ever();

			if (display_xor_experiment_details) {
				for (netkit::species& spec : neat.get_all_species()) {
					spec.update_stats();
					print_species_stats(spec);
				}
			}

			if (is_a_xor_solution(*neat.get_best_genome_ever())) {
				if (display_xor_experiment_details)
					std::cout << "\n=====> Found a solution!" << std::endl;
				stats.success = true;
				stats.number_of_generations = gen;
				opt_best_geno = neat.get_best_genome_ever();
				stats.best_fitness_ever = opt_best_geno->get_fitness();
				break;
			}
		}

		if (display_xor_experiment_details)
			wait_user();
	}

	netkit::network net = opt_best_geno->generate_network();
	stats.number_of_neurons = net.number_of_neurons();
	stats.number_of_links = net.number_of_links();

	if (display_xor_experiment_details) {
		std::cout << "\nHere's the best genome ever produced for this run:" << std::endl;
		std::cout << *opt_best_geno << std::endl;
		std::cout << net << std::endl;
		print_xor_network_results(net);
	}

	return stats;
}

void rate_xor_population(netkit::neat& neat) {
	while (neat.has_more_organisms_to_process()) {
		netkit::organism org = neat.generate_and_get_next_organism();
		org.set_fitness(evaluate_network(org.get_network()));
	}
}

double evaluate_network(netkit::network& net) {
	std::vector<size_t> runs = {0, 1, 2, 3};

	double error_sum = 0;
	for (auto run : runs) {
		net.flush();
		net.load_inputs(inputs_per_run[run]);
		net.activate_until_relaxation();
		error_sum += std::abs(net.get_outputs()[0] - expected_output_per_run[run]);
	}

	return std::pow(4.0 - error_sum, 2);
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
