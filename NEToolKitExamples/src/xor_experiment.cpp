#include <iostream>

#include <netkit/network/network.h>
#include <netkit/network/network_primitive_types.h>
#include <netkit/network/activation_functions.h>
#include <netkit/neat/genome.h>
#include <netkit/neat/gene.h>

#include "xor_experiment.h"

void xor_network_test() {
	std::cout << "Starting the (handmade) example xor network..." << std::endl;

	netkit::network net;

	netkit::neuron_id_t i1 = net.add_neuron(netkit::INPUT, {&netkit::sigmoid});
	netkit::neuron_id_t i2 = net.add_neuron(netkit::INPUT, {&netkit::sigmoid});
	netkit::neuron_id_t o = net.add_neuron(netkit::OUTPUT, {&netkit::sigmoid});
	netkit::neuron_id_t h = net.add_neuron(netkit::HIDDEN, {&netkit::sigmoid});

	net.add_link(netkit::network::BIAS_ID, h, -15);
	net.add_link(netkit::network::BIAS_ID, o, -5);
	net.add_link(i1, h, 10);
	net.add_link(i2, h, 10);
	net.add_link(i1, o, 10);
	net.add_link(i2, o, 10);
	net.add_link(h, o, -20);

	std::cout << "Neural network created." << std::endl;
	std::cout << net << std::endl;

	std::vector<std::vector<netkit::neuron_value_t>> inputs_per_run = {
		{0, 0},
		{1, 0},
		{0, 1},
		{1, 1}
	};

	for (auto& inputs : inputs_per_run) {
		std::cout << "==================" << std::endl;
		std::cout << inputs[0] << " xor " << inputs[1] << std::endl;
		net.load_inputs(inputs);
		net.activate_until_relaxation();
		std::cout << "Result: " << net.get_outputs()[0] << std::endl;
	}

	std::cout << std::endl << "...Performing the same with a network generated from a genome..." << std::endl;

	netkit::genome genome(2, 1);
	genome.add_gene(netkit::gene(netkit::genome::BIAS_ID, 4, -15));
	genome.add_gene(netkit::gene(netkit::genome::BIAS_ID, 3, -5));
	genome.add_gene(netkit::gene(1, 4, 10));
	genome.add_gene(netkit::gene(2, 4, 10));
	genome.add_gene(netkit::gene(1, 3, 10));
	genome.add_gene(netkit::gene(2, 3, 10));
	genome.add_gene(netkit::gene(4, 3, -20));

	std::cout << genome << std::endl;

	netkit::network gennet = genome.generate_network();
	std::cout << gennet << std::endl;

	for (auto& inputs : inputs_per_run) {
		std::cout << "==================" << std::endl;
		std::cout << inputs[0] << " xor " << inputs[1] << std::endl;
		gennet.load_inputs(inputs);
		gennet.activate_until_relaxation();
		std::cout << "Result: " << gennet.get_outputs()[0] << std::endl;
	}
}

void run_xor_experiment() {
	std::cout << "Starting the xor evolution experiment..." << std::endl;
}
