#include <algorithm>

#include "netkit/neat/innovation_pool.h"

netkit::innovation_pool::innovation_pool(const parameters& params)
	: m_next_innovation(0)
	, m_next_hidden_neuron_id(1 + params.number_of_inputs + params.number_of_outputs)
	, m_all_genes()
	, m_all_innovations() {}

netkit::innovation_pool::innovation_pool(innovation_pool&& other) noexcept
	: m_next_innovation(other.m_next_innovation)
	, m_next_hidden_neuron_id(other.m_next_hidden_neuron_id)
	, m_all_genes(std::move(other.m_all_genes))
	, m_all_innovations(std::move(other.m_all_innovations)) {}

netkit::innovation_pool& netkit::innovation_pool::operator=(innovation_pool&& other) noexcept {
	m_next_innovation = other.m_next_innovation;
	m_next_hidden_neuron_id = other.m_next_hidden_neuron_id;
	m_all_genes = std::move(other.m_all_genes);
	m_all_innovations = std::move(other.m_all_innovations);
	return *this;
}

std::optional<netkit::gene> netkit::innovation_pool::find_gene(neuron_id_t from, neuron_id_t to) {
	return helper_find_gene([&](gene & g) -> bool { return g.from == from && g.to == to; });
}

std::optional<netkit::gene> netkit::innovation_pool::find_gene(innov_num_t innovation) {
	return helper_find_gene([&](gene & g) -> bool { return g.innov_num == innovation; });
}

void netkit::innovation_pool::register_gene(gene new_gene) {
	m_all_genes.emplace_back(new_gene);
}

std::optional<netkit::innovation> netkit::innovation_pool::find_innovation(innov_type type, neuron_id_t from,
																		   neuron_id_t to) {
	return helper_find_innovation([&](innovation & i) { return i.type == type && i.from == from && i.to == to; });
}

std::optional<netkit::innovation> netkit::innovation_pool::find_innovation(innov_num_t innov_num) {
	return helper_find_innovation([&](innovation & i) { return i.innov_num == innov_num; });
}

void netkit::innovation_pool::register_innovation(innovation new_innov) {
	m_all_innovations.push_back(std::move(new_innov));
}

void netkit::innovation_pool::clear() {
	m_all_innovations.clear();
	m_all_genes.clear();
}

netkit::serializer& netkit::operator<<(serializer& ser, const innovation_pool& innov_pool) {
	// serialize useful variables
	ser.append(innov_pool.m_next_innovation);
	ser.append(innov_pool.m_next_hidden_neuron_id);
	ser.new_line();

	// serialize genes
	ser.append(innov_pool.m_all_genes.size());
	ser.new_line();
	for (const gene& g : innov_pool.m_all_genes) {
		ser << g;
	}

	// serialize innovations
	ser.append(innov_pool.m_all_innovations.size());
	ser.new_line();
	for (const innovation& i : innov_pool.m_all_innovations) {
		ser << i;
	}

	return ser;
}

netkit::deserializer& netkit::operator>>(deserializer& des, innovation_pool& innov_pool) {
	// deserialize useful variables
	des.get_next(innov_pool.m_next_innovation);
	des.get_next(innov_pool.m_next_hidden_neuron_id);

	// deserialize genes
	size_t num_genes;
	des.get_next(num_genes);
	innov_pool.m_all_genes.clear();
	innov_pool.m_all_genes.reserve(num_genes);
	for (size_t i = 0; i < num_genes; ++i) {
		gene g(0, 0, 0, 0);
		des >> g;
		innov_pool.m_all_genes.push_back(g);
	}

	// deserialize innovations
	size_t num_innovations;
	des.get_next(num_innovations);
	innov_pool.m_all_innovations.clear();
	innov_pool.m_all_innovations.reserve(num_innovations);
	for (size_t i = 0; i < num_innovations; ++i) {
		innov_pool.m_all_innovations.push_back(innovation::next_innovation_from_des(des));
	}

	return des;
}
