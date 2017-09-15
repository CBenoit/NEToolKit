#include <algorithm>

#include "netkit/neat/innovation_pool.h"

std::optional<netkit::gene> netkit::innovation_pool::find_gene(neuron_id_t from, neuron_id_t to) {
	return helper_find_gene([&] (gene& g) { return g.from == from && g.to == to; });
}

std::optional<netkit::gene> netkit::innovation_pool::find_gene(innov_num_t innovation) {
	return helper_find_gene([&] (gene& g) { return g.innov_num == innovation; });
}

void netkit::innovation_pool::register_gene(gene new_gene) {
	m_all_genes.push_back(std::move(new_gene));
}

std::optional<netkit::innovation> netkit::innovation_pool::find_innovation(innov_type type, neuron_id_t from, neuron_id_t to) {
	std::cout << "look for " << from << " to " << to << std::endl;
	return helper_find_innovation(
		[&] (innovation& i) { return i.type == type && i.from == from && i.to == to; }
	);
}

std::optional<netkit::innovation> netkit::innovation_pool::find_innovation(innov_num_t innov_num) {
	return helper_find_innovation([&] (innovation& i) { return i.innov_num == innov_num; });
}

void netkit::innovation_pool::register_innovation(innovation new_innov) {
	m_all_innovations.push_back(std::move(new_innov));
}

inline std::optional<netkit::gene> netkit::innovation_pool::helper_find_gene(std::function<bool(gene)> predicate) {
	auto it = std::find_if(
		m_all_genes.begin(), m_all_genes.end(),
		predicate
	);

	if (it == m_all_genes.end()) {
		return {};
	}

	return *it;
}

inline std::optional<netkit::innovation> netkit::innovation_pool::helper_find_innovation(std::function<bool(innovation)> predicate) {
	auto it = std::find_if(
		m_all_innovations.begin(), m_all_innovations.end(),
		predicate
	);

	if (it == m_all_innovations.end()) {
		return {};
	}

	return *it;
}
