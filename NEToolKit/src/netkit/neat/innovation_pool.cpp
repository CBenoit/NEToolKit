#include <algorithm>

#include "netkit/neat/innovation_pool.h"

bool netkit::innovation_pool::find_gene(neuron_id_t from, neuron_id_t to, const gene * gene_found) {
	std::vector<gene>::iterator it;
	if (std::find_if(m_all_genes.begin(), m_all_genes.end(),
					 [&] (gene& g) {return g.from == from && g.to == to; }) != m_all_genes.end()) {
		gene_found = &(*it);
		return true;
	}

	return false;
}

bool netkit::innovation_pool::find_gene(innov_num_t innovation, const gene * gene_found) {
	std::vector<gene>::iterator it;
	if (std::find_if(m_all_genes.begin(), m_all_genes.end(),
					 [&] (gene& g) {return g.innov_num == innovation; }) != m_all_genes.end()) {
		gene_found = &(*it);
		return true;
	}

	return false;
}

void netkit::innovation_pool::register_gene(gene new_gene) {
	m_all_genes.push_back(std::move(new_gene));
}

bool netkit::innovation_pool::find_innovation(innov_type type, neuron_id_t from, neuron_id_t to, const innovation* innov_found) {
	std::vector<innovation>::iterator it;
	if (std::find_if(m_all_innovations.begin(), m_all_innovations.end(),
					 [&] (innovation& i) {return i.type == type && i.from == from && i.to == to; }) != m_all_innovations.end()) {
		//std::vector<innovation>::iterator zisisziend = m_all_innovations.end();
		innov_found = &(*it);
		return true;
	}

	return false;
}

bool netkit::innovation_pool::find_innovation(innov_num_t innov_num, const innovation* innov_found) {
	std::vector<innovation>::iterator it;
	if (std::find_if(m_all_innovations.begin(), m_all_innovations.end(),
					 [&] (innovation& i) {return i.innov_num == innov_num; }) != m_all_innovations.end()) {
		innov_found = &(*it);
		return true;
	}

	return false;
}

void netkit::innovation_pool::register_innovation(innovation new_innov) {
	m_all_innovations.push_back(std::move(new_innov));
}
