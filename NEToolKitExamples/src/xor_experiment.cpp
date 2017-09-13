#include <iostream>

#include <NEAT/network/network.h>
#include <NEAT/network/network_primitive_types.h>
#include <NEAT/network/activation_functions.h>

#include "xor_experiment.h"

void xor_network_test() {
	std::cout << "Starting the example xor network..." << std::endl;

	NEAT::network net;

	NEAT::neuron_id_t i1 = net.add_neuron(NEAT::INPUT, {&NEAT::sigmoid});
	NEAT::neuron_id_t i2 = net.add_neuron(NEAT::INPUT, {&NEAT::sigmoid});
	NEAT::neuron_id_t h = net.add_neuron(NEAT::HIDDEN, {&NEAT::sigmoid});
	NEAT::neuron_id_t o = net.add_neuron(NEAT::OUTPUT, {&NEAT::sigmoid});

	net.add_link(NEAT::network::BIAS_ID, h, -15);
	net.add_link(NEAT::network::BIAS_ID, o, -5);
	net.add_link(i1, h, 10);
	net.add_link(i2, h, 10);
	net.add_link(i1, o, 10);
	net.add_link(i2, o, 10);
	net.add_link(h, o, -20);

	std::cout << "Neural network created." << std::endl;

	std::vector<std::vector<NEAT::neuron_value_t>> inputs_per_run = {
		{0, 0},
		{1, 0},
		{0, 1},
		{1, 1}
	};

	for (auto& inputs : inputs_per_run) {
		std::cout << "==================" << std::endl;
		std::cout << inputs[0] << " xor " << inputs[1] << std::endl;
		net.activate(inputs);
		std::cout << "Result: " << net.get_outputs()[0] << std::endl;
	}
}

void run_xor_experiment() {
	std::cout << "Starting the xor evolution experiment..." << std::endl;
}
