#include <map>

#include "netkit/neat/genome.h"
#include "netkit/network/activation_functions.h"

void netkit::genome::add_gene(gene new_gene) {
	m_genes.push_back(std::move(new_gene));
}

netkit::neuron_id_t helper_find_neuron_id(netkit::network& net, std::map<netkit::neuron_id_t, netkit::neuron_id_t>& map, netkit::neuron_id_t neuron_to_find) {
	auto it = map.find(neuron_to_find);
	if (it == map.end()) {
		netkit::neuron_id_t id = net.add_neuron(netkit::HIDDEN, netkit::neuron(&netkit::sigmoid));
		map.emplace(neuron_to_find, id);
		return id;
	} else {
		return it->second;
	}
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

	for (const gene& g : m_genes) {
		if (g.enabled) {
			neuron_id_t from_id = helper_find_neuron_id(net, ids_map, g.from);
			neuron_id_t to_id = helper_find_neuron_id(net, ids_map, g.to);
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
	os << "\ttotal: " << genome.m_genes.size() << " genes>";

	return os;
}
