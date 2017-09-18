#pragma once

#include <vector>
#include <iostream>

#include "network_primitive_types.h"

namespace netkit {
class neuron {
public:
	neuron(neuron_value_t value, activation_func_t func);
	neuron(activation_func_t func);
	~neuron();

	// feed the neuron with an input value which gives the neuron's value once processed by the activation function.
	void feed(neuron_value_t input);

	void force_value(neuron_value_t value);
	neuron_value_t get_value() const;

	void add_incoming_link(link_id_t id);
	void remove_incoming_link(link_id_t id);
	void add_outgoing_link(link_id_t id);
	void remove_outgoing_link(link_id_t id);

	const std::vector<link_id_t>& incoming_links_ids() const;
	const std::vector<link_id_t>& outgoing_links_ids() const;

private:
	std::vector<link_id_t> m_incoming;
	std::vector<link_id_t> m_outgoing;

	activation_func_t m_activation_func;

	neuron_value_t m_value;

	friend std::ostream& operator<<(std::ostream& os, const neuron& n);
};

std::ostream& operator<<(std::ostream& os, const neuron& n);
}
