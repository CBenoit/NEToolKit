#include "network/network.h"

#include "network/activation_functions.h"

netkit::network::network()
	: m_links()
	, m_all_neurons()
	, m_input_neuron_ids()
	, m_ouput_neuron_ids() {
	m_all_neurons.emplace_back(1, &sigmoid); // the bias neuron
	// in fact, the bias (as well as inputs functions) will never use the activation function.
}

netkit::network::~network() {}

void netkit::network::activate(std::vector<neuron_value_t> inputs) {
	if (inputs.size() != m_input_neuron_ids.size()) {
		throw std::invalid_argument("Incorrect number of neural network inputs.");
	}

	for (size_t i = 0; i < inputs.size(); ++i) {
		m_all_neurons[m_input_neuron_ids[i]].force_value(inputs[i]);
	}

	int i = 0;
	for (neuron& n : m_all_neurons) {
		neuron_value_t sum = 0;
		bool has_incoming = false;
		for (auto ilid_it = n.incoming_iterator(); ilid_it != n.incoming_iterator_end(); ++ilid_it) {
			has_incoming = true;
			link& incoming = m_links[*ilid_it];
			// FIXME : check if disabled?
			sum += m_all_neurons[incoming.from].get_value() * incoming.weight;
		}

		if (has_incoming) {
			// feed the neuron with the sum.
			n.feed(sum);
		}
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

	return nid;
}

netkit::link_id_t netkit::network::add_link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight) {
	link_id_t lid = m_links.size();
	m_links.emplace_back(from_id, to_id, weight);
	m_all_neurons[from_id].add_outgoing_link(lid);
	m_all_neurons[to_id].add_incoming_link(lid);
	return lid;
}
