#include <algorithm>

#include "neuron.h"

NEAT::neuron::neuron(neuron_value_t value, activation_func_t func)
	: m_activation_func(func)
	, m_value(value)
	, m_incoming()
	, m_outgoing() {}

NEAT::neuron::neuron(activation_func_t func) : neuron(0, func) {}

NEAT::neuron::~neuron() {}

void NEAT::neuron::feed(neuron_value_t input) {
	m_value = m_activation_func(input);
}

void NEAT::neuron::force_value(neuron_value_t value) {
	m_value = value;
}

NEAT::neuron_value_t NEAT::neuron::get_value() const {
	return m_value;
}

void NEAT::neuron::add_incoming_link(link_id_t id) {
	m_incoming.push_back(id);
}

void NEAT::neuron::remove_incoming_link(link_id_t id) {
	m_incoming.erase(std::remove_if(m_incoming.begin(), m_incoming.end(), [id] (const int& i) -> bool { return i == id; }), m_incoming.end());
}

void NEAT::neuron::add_outgoing_link(link_id_t id) {
	m_outgoing.push_back(id);
}

void NEAT::neuron::remove_outgoing_link(link_id_t id) {
	m_outgoing.erase(std::remove(m_outgoing.begin(), m_outgoing.end(), id), m_outgoing.end());
}

std::vector<NEAT::link_id_t>::const_iterator NEAT::neuron::incoming_iterator() const {
	return m_incoming.cbegin();
}

std::vector<NEAT::link_id_t>::const_iterator NEAT::neuron::incoming_iterator_end() const {
	return m_incoming.cend();
}

std::vector<NEAT::link_id_t>::const_iterator NEAT::neuron::outgoing_iterator() const {
	return m_outgoing.cbegin();
}

std::vector<NEAT::link_id_t>::const_iterator NEAT::neuron::outgoing_iterator_end() const {
	return m_outgoing.cend();
}
