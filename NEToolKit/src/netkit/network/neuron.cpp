#include <algorithm>

#include "netkit/network/neuron.h"

netkit::neuron::neuron(neuron_value_t value, activation_func_t func)
	: m_activation_func(func)
	, m_value(value)
	, m_incoming()
	, m_outgoing() {}

netkit::neuron::neuron(activation_func_t func) : neuron(0, func) {}

netkit::neuron::~neuron() {}

void netkit::neuron::feed(neuron_value_t input) {
	m_value = m_activation_func(input);
}

void netkit::neuron::force_value(neuron_value_t value) {
	m_value = value;
}

netkit::neuron_value_t netkit::neuron::get_value() const {
	return m_value;
}

void netkit::neuron::add_incoming_link(link_id_t id) {
	m_incoming.push_back(id);
}

void netkit::neuron::remove_incoming_link(link_id_t id) {
	m_incoming.erase(std::remove_if(m_incoming.begin(), m_incoming.end(), [id] (const int& i) -> bool { return i == id; }), m_incoming.end());
}

void netkit::neuron::add_outgoing_link(link_id_t id) {
	m_outgoing.push_back(id);
}

void netkit::neuron::remove_outgoing_link(link_id_t id) {
	m_outgoing.erase(std::remove(m_outgoing.begin(), m_outgoing.end(), id), m_outgoing.end());
}

const std::vector<netkit::link_id_t>& netkit::neuron::incoming_link_ids() const {
	return m_incoming;
}

const std::vector<netkit::link_id_t>& netkit::neuron::outgoing_link_ids() const {
	return m_outgoing;
}

std::ostream& netkit::operator<<(std::ostream& os, const neuron& n) {
	os << "<neuron: val = " << n.m_value << " incoming = {";
	for (link_id_t id : n.m_incoming) {
		os << id << " ";
	}
	os << "} outgoing = {";
	for (link_id_t id : n.m_outgoing) {
		os << id << " ";
	}
	os << "}>";
	return os;
}
