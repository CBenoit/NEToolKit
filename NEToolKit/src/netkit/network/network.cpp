#include <algorithm>

#include "netkit/network/network.h"
#include "netkit/network/activation_functions.h"

netkit::network::network()
	: m_links()
	, m_all_neurons()
	, m_input_neuron_ids()
	, m_ouput_neuron_ids()
	, m_max_depth(-1) {
	m_all_neurons.emplace_back(1, &sigmoid); // the bias neuron
	// in fact, the bias (as well as inputs functions) will never use the activation function.
}

netkit::network::~network() {}

void netkit::network::flush() {
	for (neuron& n : m_all_neurons) {
		n.force_value(0);
	}
	m_all_neurons[BIAS_ID].force_value(1);
}

void netkit::network::load_inputs(std::vector<neuron_value_t> inputs) {
	if (inputs.size() != m_input_neuron_ids.size()) {
		throw std::invalid_argument("Incorrect number of neural network inputs.");
	}

	for (size_t i = 0; i < inputs.size(); ++i) {
		m_all_neurons[m_input_neuron_ids[i]].force_value(inputs[i]);
	}
}

void netkit::network::activate() {
	int i = 0;
	for (neuron& n : m_all_neurons) {
		neuron_value_t sum = 0;
		bool has_incoming = false;
		for (link_id_t lid : n.incoming_link_ids()) {
			has_incoming = true;
			link& incoming = m_links[lid];
			sum += m_all_neurons[incoming.from].get_value() * incoming.weight;
		}

		if (has_incoming) {
			// feed the neuron with the sum.
			n.feed(sum);
		}
	}
}

void netkit::network::activate_until_relaxation() {
	for (int i = max_depth(); i--;) {
		activate();
	}
}

std::vector<netkit::neuron_value_t> netkit::network::get_outputs() {
	std::vector<neuron_value_t> output_values;
	output_values.reserve(m_ouput_neuron_ids.size());

	for (neuron_id_t onid : m_ouput_neuron_ids) {
		output_values.push_back(m_all_neurons[onid].get_value());
	}

	return std::move(output_values);
}

netkit::neuron_id_t netkit::network::add_neuron(neuron_type_t type, neuron n) {
	neuron_id_t nid = m_all_neurons.size();

	m_all_neurons.push_back(std::move(n));

	switch (type) {
	case INPUT:
		m_input_neuron_ids.push_back(nid);
		break;
	case OUTPUT:
		m_ouput_neuron_ids.push_back(nid);
		break;
	}

	m_max_depth = -1; // invalidate the max depth cache

	return nid;
}

const std::vector<netkit::neuron>& netkit::network::get_neurons() const {
	return m_all_neurons;
}

netkit::link_id_t netkit::network::add_link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight) {
	link_id_t lid = m_links.size();
	m_links.emplace_back(from_id, to_id, weight);
	m_all_neurons[from_id].add_outgoing_link(lid);
	m_all_neurons[to_id].add_incoming_link(lid);
	return lid;
}

const std::vector<netkit::link>& netkit::network::get_links() const {
	return m_links;
}

// used to compute the max depth
struct path_node {
	path_node(netkit::neuron_id_t nid, unsigned int dist)
		: nid(nid), dist(dist) {}

	netkit::neuron_id_t nid;
	int dist;
};

int netkit::network::max_depth_for(neuron_id_t input_nid) const {
	std::vector<path_node> open_list;
	std::vector<neuron_id_t> closed_list;

	open_list.emplace_back(input_nid, 0);
	int worst_dist = 0;

	while (!open_list.empty()) {
		path_node current = open_list.back();
		open_list.pop_back();

		if (std::find(m_input_neuron_ids.begin(), m_input_neuron_ids.end(), current.nid) != m_input_neuron_ids.end()) {
			if (current.dist > worst_dist) {
				worst_dist = current.dist;
			}
		}

		closed_list.emplace_back(current.nid);

		for (auto lid : m_all_neurons[current.nid].incoming_link_ids()) {
			if (std::find(closed_list.begin(), closed_list.end(), m_links[lid].from) == closed_list.end()) {
				open_list.emplace_back(m_links[lid].from, current.dist + 1);
			}
		}
	}

	return worst_dist;
}

int netkit::network::max_depth() const {
	if (m_max_depth == -1) {
		m_max_depth = 0;
		for (neuron_id_t nid : m_ouput_neuron_ids) {
			int depth = max_depth_for(nid);
			if (depth > m_max_depth) {
				m_max_depth = depth;
			}
		}
	}
	return m_max_depth;
}

std::ostream & netkit::operator<<(std::ostream & os, const network & net) {
	os << "<network:" << std::endl;

	os << "\tneurons are:" << std::endl;
	unsigned int i = 0;
	for (const neuron& n : net.m_all_neurons) {
		os << "\t" << i++ << ": " << n << std::endl;
	}
	os << "\ttotal: " << net.m_all_neurons.size() << " neurons" << std::endl;

	os << "\tlinks are:" << std::endl;
	i = 0;
	for (const link& l : net.m_links) {
		os << "\t" << i++ << ": " << l << std::endl;
	}
	os << "\ttotal: " << net.m_links.size() << " links" << std::endl;

	os << "\tmax depth = " << net.max_depth() << ">" << std::endl;

	return os;
}
