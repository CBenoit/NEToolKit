#pragma once

#include <vector>

#include "neuron.h"
#include "link.h"

namespace NEAT {
class network {
public:
	network();
	~network();

	void flush(); // TODO
	void activate(std::vector<neuron_value_t> inputs);
	std::vector<neuron_value_t> get_outputs();

	neuron_id_t add_neuron(neuron_type_t type, neuron n);
	link_id_t add_link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight);

public:
	static const neuron_id_t BIAS_ID = 0;

private:
	std::vector<link> m_links;
	std::vector<neuron> m_all_neurons;

	std::vector<neuron_id_t> m_input_neuron_ids;
	std::vector<neuron_id_t> m_ouput_neuron_ids;
};
}
