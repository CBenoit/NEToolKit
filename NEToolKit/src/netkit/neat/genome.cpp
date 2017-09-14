#include <map>
#include <cstdlib>
#include <algorithm> // find
#include <numeric> // iota

#include "netkit/neat/genome.h"
#include "netkit/network/activation_functions.h"

void netkit::genome::add_gene(gene new_gene) {
	m_genes.push_back(std::move(new_gene));

	// if this genes refers to an unknown neuron, add it to the known neurons list.
	if (std::find(m_known_neuron_ids.begin(), m_known_neuron_ids.end(), new_gene.from) == m_known_neuron_ids.end()) {
		m_known_neuron_ids.push_back(new_gene.from);
	}

	if (std::find(m_known_neuron_ids.begin(), m_known_neuron_ids.end(), new_gene.to) == m_known_neuron_ids.end()) {
		m_known_neuron_ids.push_back(new_gene.to);
	}
}

bool netkit::genome::link_exists(neuron_id_t from, neuron_id_t to) {
	for (const gene& g : m_genes) {
		if (g.from == from && g.to == to) {
			return true;
		}
	}
	return false;
}

netkit::neuron_value_t generate_weight_perturbations() {
	return static_cast<netkit::neuron_value_t>(rand()) * 6 / static_cast<netkit::neuron_value_t>(RAND_MAX) - 3;
}

bool netkit::genome::mutate_add_link() {
	neuron_id_t from = rand() % m_known_neuron_ids.size();

	// select a destination that is not an input nor the bias...
	neuron_id_t to = rand() % (m_known_neuron_ids.size() - m_number_of_inputs - 1) + m_number_of_inputs + 1;

	if (link_exists(m_known_neuron_ids[from], m_known_neuron_ids[to])) {
		// already exist in this genome...
		return false;
	}

	add_gene({m_known_neuron_ids[from], m_known_neuron_ids[to], generate_weight_perturbations()});

	return true;
}

bool netkit::genome::mutate_add_neuron() {
	// iterate in genes in a random order...
	std::vector<size_t> candidates_idx(m_genes.size());
	std::iota(candidates_idx.begin(), candidates_idx.end(), 0);
	std::random_shuffle(candidates_idx.begin(), candidates_idx.end());

	gene* selected_gene = nullptr;
	for (size_t idx : candidates_idx) {
		if (m_genes[idx].enabled) {
			selected_gene = &m_genes[idx];
			break;
		}
	}

	if (selected_gene == nullptr) {
		return false; // no enabled gene available
	}

	selected_gene->enabled = false;

	// TODO: add two new genes.
	// need to know the next unkown neuron id.
	// the two new genes should have the same weight as the old one.

	return true;
}

bool netkit::genome::mutate_reenable_gene() {
	std::vector<gene*> candidates;
	for (gene& g : m_genes) {
		if (!g.enabled) {
			candidates.push_back(&g);
		}
	}

	if (candidates.empty()) {
		return false;
	} else {
		candidates[rand() % candidates.size()]->enabled = true;
		return true;
	}
}

bool netkit::genome::mutate_toggle_enable() {
	if (m_genes.empty()) {
		return false;
	}

	int rnd_val = rand() % m_genes.size();
	m_genes[rnd_val].enabled = !m_genes[rnd_val].enabled;

	return true;
}

bool netkit::genome::mutate_one_weight() {
	if (m_genes.empty()) {
		return false;
	}

	int rnd_val = rand() % m_genes.size();
	m_genes[rnd_val].weight += generate_weight_perturbations();

	return true;
}

bool netkit::genome::mutate_all_weights() {
	for (gene& g : m_genes) {
		g.weight += generate_weight_perturbations();
	}

	return true;
}

netkit::network netkit::genome::generate_network() const {
	network net;

	// we need to map the genome neuron ids to
	// the network neuron ids.
	std::map<neuron_id_t, neuron_id_t> ids_map;

	ids_map.emplace(BIAS_ID, network::BIAS_ID);

	for (size_t i = 0; i < m_number_of_inputs; i++) {
		neuron_id_t net_neuron_id = net.add_neuron(INPUT, neuron(&sigmoid));
		ids_map.emplace(i + 1, net_neuron_id);
	}

	for (size_t i = 0; i < m_number_of_outputs; i++) {
		neuron_id_t net_neuron_id = net.add_neuron(OUTPUT, neuron(&sigmoid));
		ids_map.emplace(i + m_number_of_inputs + 1, net_neuron_id);
	}

	for (size_t i = m_number_of_inputs + m_number_of_outputs + 1; i < m_known_neuron_ids.size(); i++) {
		neuron_id_t net_neuron_id = net.add_neuron(HIDDEN, neuron(&sigmoid));
		ids_map.emplace(m_known_neuron_ids[i], net_neuron_id);
	}

	for (const gene& g : m_genes) {
		if (g.enabled) {
			neuron_id_t from_id = ids_map.find(g.from)->second;
			neuron_id_t to_id = ids_map.find(g.to)->second;
			net.add_link(from_id, to_id, g.weight);
		}
	}

	return std::move(net);
}

std::ostream & netkit::operator<<(std::ostream & os, const genome & genome) {
	os << "<genome: " << genome.m_number_of_inputs << " input(s) "
		<< genome.m_number_of_outputs << " output(s)" << std::endl;

	os << "\tgenes are:" << std::endl;
	for (const gene& g : genome.m_genes) {
		os << "\t" << g << std::endl;
	}
	os << "\ttotal: " << genome.m_genes.size() << " genes and " << genome.m_known_neuron_ids.size() << " neurons>";

	return os;
}
