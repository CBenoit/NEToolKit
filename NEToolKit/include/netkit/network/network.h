#pragma once

#include <vector>
#include <iostream>

#include "neuron.h"
#include "link.h"

namespace netkit {
class network {
public:
	network();
	~network();

	void flush(); // TODO
	void activate(std::vector<neuron_value_t> inputs);
	std::vector<neuron_value_t> get_outputs();

	neuron_id_t add_neuron(neuron_type_t type, neuron n);
	const std::vector<neuron>& get_neurons() const;
	link_id_t add_link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight);
	const std::vector<link>& get_links() const;

	int max_depth();

public:
	static const neuron_id_t BIAS_ID = 0;

private:
	std::vector<link> m_links;
	std::vector<neuron> m_all_neurons;

	std::vector<neuron_id_t> m_input_neuron_ids;
	std::vector<neuron_id_t> m_ouput_neuron_ids;

	int m_max_depth; // cache the max depth of the network (-1 = invalid)

	friend std::ostream& operator<<(std::ostream& os, const network& n);
};
}
