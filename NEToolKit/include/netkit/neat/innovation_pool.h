#pragma once

#include <vector>
#include <functional>
#include <optional>

#include "netkit/network/network_primitive_types.h"
#include "neat_primitive_types.h"
#include "parameters.h"
#include "gene.h"
#include "innovation.h"

namespace netkit {
class innovation_pool {
public:
	innovation_pool(const parameters& params)
		: m_next_innovation(0)
		, m_next_hidden_neuron_id(1 + params.number_of_inputs + params.number_of_outputs)
		, m_all_genes()
		, m_all_innovations() {}

	innov_num_t next_innovation() {
		return m_next_innovation++;
	}

	neuron_id_t next_hidden_neuron_id() {
		return m_next_hidden_neuron_id++;
	}

	// just in case, const gene* is a pointer to a constant gene,
	// not a const pointer to a gene (that would be gene* const) nor
	// a const pointer to a const gene (const gene* const).
	std::optional<gene> find_gene(neuron_id_t from, neuron_id_t to);

	// like previous find_gene but search by innovation number.
	std::optional<gene> find_gene(innov_num_t innov_num);

	// please register a gene only if it isn't already registerd!
	void register_gene(gene new_gene);

	// similar to find_gene.
	std::optional<innovation> find_innovation(innov_type type, neuron_id_t from, neuron_id_t to);

	// similar to find_gene.
	std::optional<innovation> find_innovation(innov_num_t innov_num);

	// please register an innovation only if it isn't already registerd!
	void register_innovation(innovation new_innov);

	// NOTE: the find methods would be far better with std::optional.

private:
	neuron_id_t m_next_hidden_neuron_id;
	innov_num_t m_next_innovation;
	std::vector<gene> m_all_genes;
	std::vector<innovation> m_all_innovations;

	inline std::optional<gene> helper_find_gene(std::function<bool(gene)> predicate);

	inline std::optional<innovation> helper_find_innovation(std::function<bool(innovation)> predicate);
};
}
