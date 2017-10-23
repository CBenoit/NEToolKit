#include <map>
#include <algorithm> // find, random_shuffle
#include <numeric> // iota
#include <random>

#include "netkit/network/activation_functions.h"
#include "netkit/network/network_primitive_types.h"
#include "netkit/neat/base_neat.h"
#include "netkit/neat/genome.h"
#include "netkit/neat/innovation.h"

const netkit::neuron_id_t netkit::genome::BIAS_ID = 0;

netkit::genome::genome(base_neat* neat_instance)
	: m_number_of_inputs(neat_instance->params.number_of_inputs)
	, m_number_of_outputs(neat_instance->params.number_of_outputs)
	, m_genes()
	, m_known_neuron_ids()
	, m_neat(neat_instance)
	, m_fitness(0)
	, m_adjusted_fitness(0) {
	m_known_neuron_ids.push_back(BIAS_ID);

	for (neuron_id_t i = 0; i < m_number_of_inputs; i++) {
		m_known_neuron_ids.push_back(i + 1);
	}

	for (neuron_id_t i = 0; i < m_number_of_outputs; i++) {
		m_known_neuron_ids.push_back(i + 1 + m_number_of_inputs);
	}
}

netkit::genome::genome(genome&& other) noexcept
	: m_number_of_inputs(other.m_number_of_inputs)
	, m_number_of_outputs(other.m_number_of_outputs)
	, m_genes(std::move(other.m_genes))
	, m_known_neuron_ids(std::move(other.m_known_neuron_ids))
	, m_neat(other.m_neat)
	, m_fitness(other.m_fitness)
	, m_adjusted_fitness(other.m_adjusted_fitness) {}

netkit::genome& netkit::genome::operator=(genome&& other) noexcept {
	m_number_of_inputs = other.m_number_of_inputs;
	m_number_of_outputs = other.m_number_of_outputs;
	m_genes = std::move(other.m_genes);
	m_known_neuron_ids = std::move(other.m_known_neuron_ids);
	m_neat = other.m_neat;
	m_fitness = other.m_fitness;
	m_adjusted_fitness = other.m_adjusted_fitness;

	return *this;
}

void netkit::genome::add_gene(gene new_gene) {
	// if this genes refers to an unknown neuron, add it to the known neurons list.
	if (std::find(m_known_neuron_ids.begin(), m_known_neuron_ids.end(), new_gene.from) == m_known_neuron_ids.end()) {
		m_known_neuron_ids.push_back(new_gene.from);
	}

	if (std::find(m_known_neuron_ids.begin(), m_known_neuron_ids.end(), new_gene.to) == m_known_neuron_ids.end()) {
		m_known_neuron_ids.push_back(new_gene.to);
	}

	// make sure the genes are sorted by innovation number.
	size_t current_position = m_genes.size();
	while (current_position >= 1) {
		if (m_genes[current_position - 1].innov_num < new_gene.innov_num) {
			m_genes.insert(m_genes.begin() + current_position, new_gene);
			return;
		}
		--current_position;
	}

	m_genes.emplace_back(new_gene);
}

bool netkit::genome::link_exists(neuron_id_t from, neuron_id_t to) const {
	for (const gene& g : m_genes) {
		if (g.from == from && g.to == to) {
			return true;
		}
	}
	return false;
}

double netkit::genome::distance_to(const genome& other) const {
	// find a better way of calculating the genetic distance.
	unsigned int larger_size = static_cast<unsigned int>(std::max(this->m_genes.size(), other.m_genes.size()));
	if (larger_size <= 4) { // TODO: add to parameters?
		return 0.;
	}

	unsigned int nb_disjoint_genes = 0;
	unsigned int nb_excess_genes = 0;
	unsigned int nb_matching_genes = 0;
	neuron_value_t sum_weight_difference = 0;

	auto oya1_gene_it = this->m_genes.cbegin();
	auto oya2_gene_it = other.m_genes.cbegin();

	while (oya1_gene_it != this->m_genes.cend() && oya2_gene_it != other.m_genes.cend()) {
		if (oya1_gene_it->innov_num == oya2_gene_it->innov_num) {
			++nb_matching_genes;
			sum_weight_difference += std::abs(oya1_gene_it->weight - oya2_gene_it->weight);
			++oya1_gene_it;
			++oya2_gene_it;
		} else if (oya1_gene_it->innov_num < oya2_gene_it->innov_num) {
			++nb_disjoint_genes;
			++oya1_gene_it;
		} else {
			++nb_disjoint_genes;
			++oya2_gene_it;
		}
	}

	neuron_value_t average_weight_difference = sum_weight_difference / nb_matching_genes;

	while (oya1_gene_it != this->m_genes.cend()) {
		++nb_excess_genes;
		++oya1_gene_it;
	}

	while (oya2_gene_it != other.m_genes.cend()) {
		++nb_excess_genes;
		++oya2_gene_it;
	}

	return m_neat->params.distance_coef_c1 * nb_excess_genes / larger_size
		   + m_neat->params.distance_coef_c2 * nb_disjoint_genes / larger_size
		   + m_neat->params.distance_coef_c3 * average_weight_difference;
}

bool netkit::genome::is_compatible_with(const genome& other) const {
	return distance_to(other) < m_neat->params.compatibility_threshold;
}

netkit::genome netkit::genome::get_random_mutation() const {
	genome offspring(*this);

	unsigned int remaining_tries = 3; // a mutation can fail, so let's give it three tries. TODO: add it to the parameters.
	while (!offspring.random_mutate() && remaining_tries--) { }

	offspring.set_fitness(0);
	offspring.set_adjusted_fitness(0);

	return std::move(offspring);
}

bool netkit::genome::random_mutate() {
	std::bernoulli_distribution add_neuron_true(m_neat->params.mutation_probs[ADD_NEURON]);
	std::bernoulli_distribution remove_neuron_true(m_neat->params.mutation_probs[REMOVE_NEURON]);
	std::bernoulli_distribution add_link_true(m_neat->params.mutation_probs[ADD_LINK]);
	if (add_neuron_true(m_neat->rand_engine)) {
		// let's add a new neuron!
		return mutate_add_neuron();
	} else if (remove_neuron_true(m_neat->rand_engine)) {
		// let's remove a neuron, wee!
		return mutate_remove_neuron();
	} else if (add_link_true(m_neat->rand_engine)) {
		// We do not add a new neuron AND a new link at the same time.
		return mutate_add_link();
	} else { // We can do any other mutation if there was no structural mutation.
		// The two first values are the add neuron and add link mutations.
		std::vector<mutation_t> mutations_to_perform;
		mutations_to_perform.reserve(NUMBER_OF_MUTATIONS - 3);
		for (size_t mut_id = 2; mut_id < NUMBER_OF_MUTATIONS; ++mut_id) {
			std::bernoulli_distribution select_mutation(m_neat->params.mutation_probs[mut_id]);
			if (select_mutation(m_neat->rand_engine)) {
				mutations_to_perform.push_back(static_cast<mutation_t>(mut_id));
			}
		}

		bool return_value = false;
		for (mutation_t mut_id : mutations_to_perform) {
			switch (mut_id) {
			case REMOVE_GENE:
				return_value |= mutate_remove_gene();
				break;
			case REENABLE_GENE:
				return_value |= mutate_reenable_gene();
				break;
			case TOGGLE_ENABLE:
				return_value |= mutate_toggle_enable();
				break;
			case RESET_WEIGHTS:
				return_value |= mutate_reset_weights();
				break;
			case PERTURBATE_WEIGHTS:
				return_value |= mutate_weights();
				break;
			default: // should not happen
				break;
			}
		}

		return return_value;
	}
}

bool netkit::genome::mutate_add_link() {
	std::uniform_int_distribution<neuron_id_t> from_selector(
		0, static_cast<neuron_id_t>(m_known_neuron_ids.size() - 1)
	);
	auto from = from_selector(m_neat->rand_engine);

	// select a destination that is not an input nor the bias...
	std::uniform_int_distribution<neuron_id_t> destination_selector(
		0, static_cast<neuron_id_t>(m_known_neuron_ids.size() - m_number_of_inputs  - 2)
	);
	auto to = destination_selector(m_neat->rand_engine) + m_number_of_inputs + 1;

	if (link_exists(m_known_neuron_ids[from], m_known_neuron_ids[to])) {
		// already exist in this genome...
		return false;
	}

	std::uniform_real_distribution<netkit::neuron_value_t> perturbator(-m_neat->params.initial_weight_perturbation,
																	   m_neat->params.initial_weight_perturbation);
	std::optional<gene> existing_gene =
		m_neat->innov_pool.find_gene(m_known_neuron_ids[from], m_known_neuron_ids[to]);
	if (existing_gene.has_value()) {
		gene copied_gene(*existing_gene);
		copied_gene.weight = perturbator(m_neat->rand_engine);
		add_gene(copied_gene);
	} else {
		gene new_gene(m_neat->innov_pool.next_innovation(), m_known_neuron_ids[from], m_known_neuron_ids[to],
					  perturbator(m_neat->rand_engine));
		m_neat->innov_pool.register_gene(new_gene);
		m_neat->innov_pool.register_innovation(innovation::new_link_innovation(
												   new_gene.innov_num,
												   new_gene.from,
												   new_gene.to
											   ));
		add_gene(new_gene);
	}

	return true;
}

bool netkit::genome::mutate_add_neuron() {
	// iterate in genes in a random order...
	std::vector<int> candidates_idx(m_genes.size());
	std::iota(candidates_idx.begin(), candidates_idx.end(), 0);
	std::shuffle(candidates_idx.begin(), candidates_idx.end(), m_neat->rand_engine);

	int sel_idx = -1; // selected gene index
	for (int idx : candidates_idx) {
		if (m_genes[idx].enabled) {
			sel_idx = idx;
			break;
		}
	}

	if (sel_idx == -1) {
		return false; // no enabled gene available
	}

	m_genes[sel_idx].enabled = false;

	std::optional<innovation> existing_innovation =
		m_neat->innov_pool.find_innovation(NEW_NEURON, m_genes[sel_idx].from, m_genes[sel_idx].to);

	if (existing_innovation.has_value()) {
		// this existing innovation already occurred in this genome.
		if (std::find(m_known_neuron_ids.cbegin(), m_known_neuron_ids.cend(),
					  existing_innovation->new_neuron_id) != m_known_neuron_ids.cend()) {
			return false;
		}

		add_gene({existing_innovation->innov_num, existing_innovation->from,
				  existing_innovation->new_neuron_id, m_genes[sel_idx].weight
				 });
		add_gene({existing_innovation->innov_num_2, existing_innovation->new_neuron_id,
				  existing_innovation->to, m_genes[sel_idx].weight
				 });
	} else {
		neuron_id_t new_neuron_id = m_neat->innov_pool.next_hidden_neuron_id();

		gene new_gene_1(m_neat->innov_pool.next_innovation(),
						m_genes[sel_idx].from, new_neuron_id, m_genes[sel_idx].weight);
		gene new_gene_2(m_neat->innov_pool.next_innovation(), new_neuron_id,
						m_genes[sel_idx].to, m_genes[sel_idx].weight);

		m_neat->innov_pool.register_gene(new_gene_1);
		m_neat->innov_pool.register_gene(new_gene_2);
		m_neat->innov_pool.register_innovation(innovation::new_neuron_innovation(
												   new_gene_1.innov_num,
												   new_gene_2.innov_num,
												   m_genes[sel_idx].from,
												   m_genes[sel_idx].to,
												   new_neuron_id
											   ));

		add_gene(new_gene_1);
		add_gene(new_gene_2);
	}

	return true;
}

bool netkit::genome::mutate_remove_neuron(){
	if (m_known_neuron_ids.size() == m_number_of_inputs + m_number_of_outputs + 1) {
		return false;
	}

	std::uniform_int_distribution<size_t> neuron_selector(m_number_of_inputs + m_number_of_outputs + 1, m_known_neuron_ids.size() - 1);
	neuron_id_t selected_neuron = m_known_neuron_ids[neuron_selector(m_neat->rand_engine)];
	m_known_neuron_ids.erase(std::remove(m_known_neuron_ids.begin(), m_known_neuron_ids.end(), selected_neuron), m_known_neuron_ids.end());
	m_genes.erase(std::remove_if(m_genes.begin(), m_genes.end(), [&selected_neuron](const gene& g) {
		return g.from == selected_neuron || g.to == selected_neuron;
	}), m_genes.end());

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
		std::uniform_int_distribution<size_t> candidate_selector(0, candidates.size() - 1);
		candidates[candidate_selector(m_neat->rand_engine)]->enabled = true;
		return true;
	}
}

bool netkit::genome::mutate_toggle_enable() {
	if (m_genes.empty()) {
		return false;
	}

	std::uniform_int_distribution<size_t> gene_selector(0, m_genes.size() - 1);
	size_t rnd_val = gene_selector(m_neat->rand_engine);
	m_genes[rnd_val].enabled = !m_genes[rnd_val].enabled;

	return true;
}

bool netkit::genome::mutate_weights() {
	if (m_genes.empty()) {
		return false;
	}

	std::uniform_real_distribution<netkit::neuron_value_t> perturbator(-m_neat->params.weight_mutation_power,
	                                                                   m_neat->params.weight_mutation_power);
	std::vector<size_t>  candidates_idx(m_genes.size());
	std::iota(candidates_idx.begin(), candidates_idx.end(), 0);
	std::shuffle(candidates_idx.begin(), candidates_idx.end(), m_neat->rand_engine);

	size_t nb_genes = 1;
	if (m_genes.size() > 1) {
		std::uniform_int_distribution<size_t> nb_genes_selector(0, m_genes.size() - 1);
		nb_genes = nb_genes_selector(m_neat->rand_engine);
	}
	for (size_t i = 0; i < nb_genes; ++i) {
		m_genes[candidates_idx[i]].weight += perturbator(m_neat->rand_engine);
	}

	return true;
}

bool netkit::genome::mutate_reset_weights() {
	std::uniform_real_distribution<netkit::neuron_value_t> perturbator(-m_neat->params.initial_weight_perturbation,
																	   m_neat->params.initial_weight_perturbation);
	for (gene& g : m_genes) {
		g.weight = perturbator(m_neat->rand_engine);
	}

	return true;
}

bool netkit::genome::mutate_remove_gene() {
	if (m_genes.empty()) {
		return false;
	}

	std::uniform_int_distribution<size_t> gene_selector(0, m_genes.size() - 1);
	m_genes.erase(m_genes.begin() + gene_selector(m_neat->rand_engine));
	// TODO: check if a neuron goes unknown afterward. /!\ do not remove bias, input and output neurons.

	return true;
}

netkit::genome netkit::genome::random_crossover(const genome& other) const {
	std::uniform_int_distribution<unsigned int> crossover_selector(0, m_neat->params.sum_all_crossover_weights() - 1);
	unsigned int rnd_val = crossover_selector(m_neat->rand_engine);

	if (rnd_val < m_neat->params.crossover_multipoint_avg_weight) {
		return crossover_multipoint_avg(other);
	}
	rnd_val -= m_neat->params.crossover_multipoint_avg_weight;

	if (rnd_val < m_neat->params.crossover_multipoint_best_weight) {
		return crossover_multipoint_best(other);
	}

	// the last option is...
	return crossover_multipoint_rnd(other);
}

netkit::genome netkit::genome::crossover_multipoint_best(const genome& other) const {
	return helper_crossover_multipoint(other, [](const genome & p1, const gene & g1, const genome & p2,
	const gene & g2) -> gene {
		if (p1.m_fitness > p2.m_fitness) {
			return g1;
		} else {
			return g2;
		}
	});
}

netkit::genome netkit::genome::crossover_multipoint_rnd(const genome& other) const {
	return helper_crossover_multipoint(other, [this](const genome& /*p1*/, const gene & g1, const genome& /*p2*/,
	const gene & g2) -> gene {
		std::bernoulli_distribution true_false_selector(0.5);
		if (true_false_selector(this->m_neat->rand_engine)) {
			return g1;
		} else {
			return g2;
		}
	});
}

netkit::genome netkit::genome::crossover_multipoint_avg(const genome& other) const {
	return helper_crossover_multipoint(other, [&](const genome& /*p1*/, const gene & g1, const genome& /*p2*/,
	const gene & g2) -> gene {
		gene new_gene(g1);
		new_gene.weight = (g1.weight + g2.weight) / 2;
		return new_gene;
	});
}

netkit::network netkit::genome::generate_network() const {
	network net;

	// we need to map the genome neuron ids to
	// the network neuron ids.
	std::map<neuron_id_t, neuron_id_t> ids_map;

	ids_map.emplace(BIAS_ID, network::BIAS_ID);

	for (size_t i = 0; i < m_number_of_inputs; i++) {
		neuron_id_t net_neuron_id = net.add_neuron(INPUT, neuron(&steepened_sigmoid));
		ids_map.emplace(i + 1, net_neuron_id);
	}

	for (size_t i = 0; i < m_number_of_outputs; i++) {
		neuron_id_t net_neuron_id = net.add_neuron(OUTPUT, neuron(&steepened_sigmoid));
		ids_map.emplace(i + m_number_of_inputs + 1, net_neuron_id);
	}

	for (size_t i = m_number_of_inputs + m_number_of_outputs + 1; i < m_known_neuron_ids.size(); i++) {
		neuron_id_t net_neuron_id = net.add_neuron(HIDDEN, neuron(&steepened_sigmoid));
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

bool netkit::genome::reenable_gene_ok() const {
	std::bernoulli_distribution distrib(0.25); // TODO: externalize in parameters
	return distrib(m_neat->rand_engine);
}

std::ostream& netkit::operator<<(std::ostream& os, const genome& genome) {
	os << "<genome: (fitness = " << genome.m_fitness << ") "
	   << genome.m_number_of_inputs << " input(s) "
	   << genome.m_number_of_outputs << " output(s)" << std::endl;

	os << "\tgenes are:" << std::endl;
	for (const gene& g : genome.m_genes) {
		os << "\t" << g << std::endl;
	}
	os << "\ttotal: " << genome.m_genes.size() << " genes and " << genome.m_known_neuron_ids.size() << " neurons>";

	return os;
}

netkit::serializer& netkit::operator<<(serializer& ser, const genome& genome) {
	// The number of inputs and outputs depends on the NEAT parameters and the NEAT class is required to build
	// a genome, so we will assume these don't need to be serialized.
	// No need to serialize the known neurons as well since the list can be reconstructed from the genes.

	// serialize fitness values
	ser.append(genome.m_fitness);
	ser.append(genome.m_adjusted_fitness);
	ser.new_line();

	// serialize genes
	ser.append(genome.m_genes.size());
	ser.new_line();
	for (const gene& g : genome.m_genes) {
		ser << g;
	}

	return ser;
}

netkit::deserializer& netkit::operator>>(netkit::deserializer& des, genome& genome) {
	// deserialize fitness values
	des.get_next(genome.m_fitness);
	des.get_next(genome.m_adjusted_fitness);

	// deserialize genes
	size_t number_of_genes;
	des.get_next(number_of_genes);
	genome.m_genes.clear();

	// clean known neurons list.
	genome.m_known_neuron_ids.clear();
	genome.m_known_neuron_ids.push_back(genome.BIAS_ID);
	for (neuron_id_t i = 0; i < genome.m_number_of_inputs; i++) {
		genome.m_known_neuron_ids.push_back(i + 1);
	}
	for (neuron_id_t i = 0; i < genome.m_number_of_outputs; i++) {
		genome.m_known_neuron_ids.push_back(i + 1 + genome.m_number_of_inputs);
	}

	genome.m_genes.reserve(number_of_genes);
	for (size_t i = 0; i < number_of_genes; ++i) {
		gene g(0, 0, 0, 0);
		des >> g;
		genome.add_gene(g);
	}

	return des;
}
