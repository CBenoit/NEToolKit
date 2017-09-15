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

	// completly discharge the network (initial state).
	void flush();
	void load_inputs(std::vector<neuron_value_t> inputs);
	void activate();
	void activate_until_relaxation();
	std::vector<neuron_value_t> get_outputs();

	neuron_id_t add_neuron(neuron_type_t type, neuron n);
	const std::vector<neuron>& get_neurons() const;
	link_id_t add_link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight);
	const std::vector<link>& get_links() const;

	// find the maximum depth for the given neuron that is the size longest path to an input.
	// It uses a modified Dijkstra's algorithm to perform a full search following incoming links.
	// Works (hopefully) even for recurrent networks.
	// /!\ can be a bit costly on large networks.
	// /?\ Maybe buggy on large networks because it handles recurrent networks...
	int max_depth_for(neuron_id_t nid) const;

	// /!\ based on max_depth_for so it can be costly too.
	// that being said, it uses a cache system to avoid unnecessary recomputation.
	int max_depth() const;

public:
	static const neuron_id_t BIAS_ID;

private:
	std::vector<link> m_links;
	std::vector<neuron> m_all_neurons;

	std::vector<neuron_id_t> m_input_neuron_ids;
	std::vector<neuron_id_t> m_ouput_neuron_ids;

	mutable int m_max_depth; // cache the max depth of the network (-1 = invalid)

	friend std::ostream& operator<<(std::ostream& os, const network& n);
};

std::ostream& operator<<(std::ostream& os, const network& n);
}
