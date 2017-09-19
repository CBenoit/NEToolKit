#include <cstdlib>
#include <ctime>
#include <utility> // std::move
#include <stdexcept> // std::invalid_argument
#include <algorithm> // std::find

#include "netkit/neat/neat.h"

netkit::neat::neat(parameters params_)
	: params(std::move(params_))
	, m_all_species()
	, m_population()
	, m_innov_pool(this->params)
	, m_next_genome_id(0)
	, m_next_species_id(0) {
	if (this->params.number_of_outputs == 0 || this->params.number_of_inputs == 0) {
		throw std::invalid_argument("genomes needs at least one input and one output.");
	}

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

netkit::neat::neat(const neat& other)
	: params(other.params)
	, m_all_species(other.m_all_species)
	, m_population(other.m_population)
	, m_innov_pool(this->params) // the innovation pool has to differ from the other simulation
	, m_next_genome_id(other.m_next_genome_id)
	, m_next_species_id(other.m_next_species_id) {} // no need to use srand again.

void netkit::neat::init() {
	// produce the default initial genome.
	genome initial_genome(this);

	const neuron_id_t starting_idx_outputs = 1 + initial_genome.number_of_inputs();

	// links from the bias
	for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
		initial_genome.add_gene({m_innov_pool.next_innovation(), genome::BIAS_ID, starting_idx_outputs + i});
	}

	// links from the inputs
	for (neuron_id_t j = 0; j < initial_genome.number_of_inputs(); j++) {
		for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
			initial_genome.add_gene({m_innov_pool.next_innovation(), j + 1, starting_idx_outputs + i});
		}
	}

	init(std::move(initial_genome));
}

void netkit::neat::init(genome initial_genome) {
	// populate with random mutations from the initial genome.
	for (size_t i = 0; i < params.initial_population_size; i++) {
		m_population.add_genome(initial_genome.get_random_mutation());
	}

	// speciate the population
	helper_speciate();
}

std::vector<netkit::organism> netkit::neat::generate_and_get_all_organisms() {
	std::vector<organism> organisms;
	organisms.reserve(m_population.size());
	for (genome_id_t i = 0, size = m_population.size(); i < size; ++i) {
		organisms.push_back(generate_and_get_next_organism());
	}
	return std::move(organisms);
}

netkit::organism netkit::neat::generate_and_get_next_organism() {
	if (m_next_genome_id >= m_population.size()) {
		throw std::runtime_error("attempted to generate the next organism even though all organisms have already been generated.");
	}

	genome& geno = m_population[m_next_genome_id];
	return {&m_population, m_next_genome_id++, geno.generate_network()};
}

bool netkit::neat::has_more_organisms_to_process() {
	return m_next_genome_id < m_population.size();
}

void netkit::neat::epoch() {
	// TODO: take into account refocusing_threshold
	// TODO: take into account extinction_threshold.

	// initialize generation-specific variables
	m_next_genome_id = 0;

	// Compute the overall average fitness.
	species* best_species = 0;
	double best_fitnesses_so_far = 0;
	double overall_average = 0;
	for (species& spec : m_all_species) {
		spec.sort_by_fitness();
		spec.share_fitness();
		spec.update_stats();
		overall_average += spec.get_summed_fitnesses();
		if (spec.get_summed_fitnesses() > best_fitnesses_so_far) {
			best_fitnesses_so_far = spec.get_summed_fitnesses();
			best_species = &spec;
		}
	}
	overall_average /= m_population.size();

	// Compute the the expected number of offsprings for each species.
	unsigned int total_expected_offsprings = 0;
	for (species& spec : m_all_species) {
		unsigned int this_species_expected_offsprings = static_cast<unsigned int>(spec.get_summed_fitnesses() / overall_average);
		spec.set_expected_offsprings(this_species_expected_offsprings);
		total_expected_offsprings += this_species_expected_offsprings;
	}

	size_t next_generation_pop_size = m_population.size();

	// Need to make up for lost floating point precision in offsprings assignation
	// by giving the missing offsprings to the current best species.
	best_species->set_expected_offsprings(
		best_species->get_expected_offsprings() + next_generation_pop_size - total_expected_offsprings
	);

	// Build the next generation offsprings.
	std::vector<genome> offsprings;
	offsprings.reserve(next_generation_pop_size);
	for (species& spec : m_all_species) {
		unsigned int offsprings_produced = 0;
		while (offsprings_produced < spec.get_expected_offsprings()) {
			++offsprings_produced;
			double rnd_val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
			if (rnd_val < params.crossover_prob) { // let's go for a crossover
				genome* genitor1 = &m_population.get_genome(spec.select_one_genitor());
				genome* genitor2 = nullptr;

				// interspecies crossover prob
				double rnd_val2 = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
				if (rnd_val2 < params.interspecies_crossover_prob) {
					unsigned int rnd_spec_val = rand() % m_all_species.size();
					genitor2 = &m_population.get_genome(m_all_species[rnd_spec_val].select_one_genitor());
				} else {
					genitor2 = &m_population.get_genome(spec.select_one_genitor());
				}

				offsprings.push_back(genitor1->random_crossover(*genitor2));
			} else {
				genome* genitor = &m_population.get_genome(spec.select_one_genitor());
				offsprings.push_back(genitor->get_random_mutation());
			}
		}

		// prepare the species for the next generation
		spec.init_for_next_gen(m_population.get_genome(spec.get_champion()));

		// TODO: mark for removal empty species? Where would it be appropriate?
	}

	// update the population with the new offsprings.
	m_population.clear();
	m_population.set_genomes(std::move(offsprings));

	// finally speciate the population
	helper_speciate();
}

std::optional<netkit::species*> netkit::neat::find_appropriate_species_for(const genome& geno) {
	for (species& spec : m_all_species) {
		if (geno.is_compatible_with(spec.get_representant())) {
			return &spec;
		}
	}

	return {};
}

void netkit::neat::helper_speciate() {
	for (const genome& geno : m_population.get_all_genomes()) {
		std::optional<species*> opt_species = find_appropriate_species_for(geno);
		if (opt_species.has_value()) {
			opt_species.value()->add_member(m_next_genome_id);
		} else {
			m_all_species.push_back({&m_population, m_next_species_id++, geno});
		}
	}
}
