#pragma once

#include <vector>

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
	bool find_gene(neuron_id_t from, neuron_id_t to, const gene* gene_found);

	// like previous find_gene but search by innovation number.
	bool find_gene(innov_num_t innov_num, const gene* gene_found);

	// please register a gene only if it isn't already registerd!
	void register_gene(gene new_gene);

	// similar to find_gene.
	bool find_innovation(innov_type type, neuron_id_t from, neuron_id_t to, const innovation* innov_found);

	// similar to find_gene.
	bool find_innovation(innov_num_t innov_num, const innovation* innov_found);

	// please register an innovation only if it isn't already registerd!
	void register_innovation(innovation new_innov);

private:
	neuron_id_t m_next_hidden_neuron_id;
	innov_num_t m_next_innovation;
	std::vector<gene> m_all_genes;
	std::vector<innovation> m_all_innovations;
};
}
