#include <utility> // std::move
#include <stdexcept> // std::invalid_argument
#include <algorithm> // std::find
#include <chrono> //

#include "netkit/neat/neat.h"

netkit::neat::neat(const parameters& params_)
	: params(params_)
    , innov_pool(this->params)
	, m_all_species()
	, m_population()
	, m_next_genome_id(0)
	, m_next_species_id(0)
	, m_best_genome_ever(nullptr)
	, m_age_of_best_genome_ever() {
	if (this->params.number_of_outputs == 0 || this->params.number_of_inputs == 0) {
		throw std::invalid_argument("genomes needs at least one input and one output.");
	}

	m_all_species.reserve(15); // reserve some memory to store the species.

	long seed = std::chrono::system_clock::now().time_since_epoch().count();
	rand_engine = std::minstd_rand0(seed);
}

netkit::neat::neat(const neat& other)
	: params(other.params)
	, innov_pool(other.innov_pool)
	, m_all_species(other.m_all_species)
	, m_population(other.m_population)
	, m_next_genome_id(other.m_next_genome_id)
	, m_next_species_id(other.m_next_species_id)
	, m_best_genome_ever(new genome{*other.m_best_genome_ever})
	, m_age_of_best_genome_ever(other.m_age_of_best_genome_ever) {
	long seed = std::chrono::system_clock::now().time_since_epoch().count();
	rand_engine = std::minstd_rand0(seed);
}

netkit::neat::neat(neat&& other) noexcept
	: params(other.params)
	, innov_pool(std::move(other.innov_pool))
	, rand_engine(std::move(other.rand_engine))
	, m_all_species(std::move(other.m_all_species))
	, m_population(std::move(other.m_population))
	, m_next_genome_id(other.m_next_genome_id)
	, m_next_species_id(other.m_next_species_id)
	, m_best_genome_ever(other.m_best_genome_ever)
	, m_age_of_best_genome_ever(other.m_age_of_best_genome_ever) {
	other.m_best_genome_ever = nullptr;
}

void netkit::neat::init() {
	// produce the default initial genome.
	genome initial_genome(this);

	const neuron_id_t starting_idx_outputs = 1 + initial_genome.number_of_inputs();

	// links from the bias
	for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
		initial_genome.add_gene({innov_pool.next_innovation(), genome::BIAS_ID, starting_idx_outputs + i});
	}

	// links from the inputs
	for (neuron_id_t j = 0; j < initial_genome.number_of_inputs(); j++) {
		for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
			initial_genome.add_gene({innov_pool.next_innovation(), j + 1, starting_idx_outputs + i});
		}
	}

	init(initial_genome);
}

void netkit::neat::init(const genome& initial_genome) {
	m_best_genome_ever = nullptr;
	m_age_of_best_genome_ever = 0;

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
	++m_age_of_best_genome_ever;
	// TODO: take into account "refocusing_threshold" thanks to the m_age_of_best_genome_ever attribute.

	// initialize generation-specific variables
	m_next_genome_id = 0;

	// Compute the overall average fitness.
	species* best_species = nullptr;
	double best_fitnesses_so_far = std::numeric_limits<double>::min();
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
	overall_average /= static_cast<double>(m_population.size());

	// Compute the expected number of offsprings for each species.
	unsigned int total_expected_offsprings = 0;
	for (species& spec : m_all_species) {
		unsigned int this_species_expected_offsprings;
		if (spec.get_age() - spec.get_age_of_last_improvement() >= params.no_reproduction_threshold) {
			// if a species doesn't improve for "extinction threshold" generations it goes extinct.
			this_species_expected_offsprings = 0;
		} else {
			this_species_expected_offsprings = static_cast<unsigned int>(spec.get_summed_fitnesses() / overall_average);
		}

		spec.set_expected_offsprings(this_species_expected_offsprings);
		total_expected_offsprings += this_species_expected_offsprings;
	}

	auto next_generation_pop_size = static_cast<unsigned int>(m_population.size()); // TODO: dynamic population?

	// Need to make up for lost floating point precision in offsprings assignation
	// by giving the missing offsprings to the current best species.
	best_species->set_expected_offsprings(
	  best_species->get_expected_offsprings() + next_generation_pop_size - total_expected_offsprings
	);

	// Build the next generation offsprings.
	std::vector<genome> offsprings;
	offsprings.reserve(next_generation_pop_size);
	std::uniform_real_distribution<double> prob(0.0,1.0);
	std::uniform_int_distribution<size_t> species_selector(0, m_all_species.size() - 1);
	for (species& spec : m_all_species) {
		unsigned int offsprings_produced = 0;

		// keep the champion of species with 5 or more members
		if (offsprings_produced < spec.get_expected_offsprings() && spec.number_of_members() >= 5) { // TODO: externalize in parameters
			offsprings.emplace_back(m_population.get_genome(spec.get_champion()));
			++offsprings_produced;
		}

		while (offsprings_produced < spec.get_expected_offsprings()) {
			++offsprings_produced;
			if (prob(rand_engine) < params.crossover_prob) { // let's go for a crossover
				genome* genitor1 = &m_population.get_genome(spec.select_one_genitor());
				genome* genitor2 = nullptr;

				// interspecies crossover prob
				double rnd_val2 = prob(rand_engine);
				if (rnd_val2 < params.interspecies_crossover_prob) {
					unsigned long rnd_spec_val = species_selector(rand_engine);
					genitor2 = &m_population.get_genome(m_all_species[rnd_spec_val].select_one_genitor());
				} else {
					genitor2 = &m_population.get_genome(spec.select_one_genitor());
				}

				// mutate the offspring or not
				if (prob(rand_engine) < params.mutation_during_crossover_prob) {
					offsprings.push_back(genitor1->random_crossover(*genitor2).get_random_mutation());
				} else {
					offsprings.push_back(genitor1->random_crossover(*genitor2));
				}
			} else {
				genome* genitor = &m_population.get_genome(spec.select_one_genitor());
				offsprings.push_back(genitor->get_random_mutation());
			}
		}

		// prepare the species for the next generation
		spec.init_for_next_gen(m_population.get_genome(spec.get_random_member()));
	}

	// update the population with the new offsprings.
	m_population.clear(); // just in case...
	m_population.set_genomes(std::move(offsprings));

	// finally speciate the population
	helper_speciate();

	// remove species that has no more member. They go extinct!
	m_all_species.erase(
	  std::remove_if(m_all_species.begin(), m_all_species.end(), [](const species& s) {
		  return s.empty();
	  }),
	  m_all_species.end()
	);
	// /!\ from now, best_species pointer may be invalid!!!

	// adjust the compatibility threshold if it is dynamic.
	if (params.dynamic_compatibility_threshold) {
		if (m_all_species.size() > params.target_number_of_species) {
			params.compatibility_threshold += params.compatibility_threshold_change_step;
		} else if (m_all_species.size() < params.target_number_of_species) {
			params.compatibility_threshold -= params.compatibility_threshold_change_step;
		}
	}
}

std::optional<netkit::species*> netkit::neat::find_appropriate_species_for(const genome& geno) {
	for (species& spec : m_all_species) {
		if (geno.is_compatible_with(spec.get_representant())) {
			return {&spec};
		}
	}
	return {};
}

const netkit::genome& netkit::neat::get_current_best_genome() const {
	double best_fitness_so_far = std::numeric_limits<double>::min();
	const genome* champion = nullptr;
	for (const genome& geno : m_population.get_all_genomes()) {
		if (geno.get_fitness() > best_fitness_so_far) {
			best_fitness_so_far = geno.get_fitness();
			champion = &geno;
		}
	}

	return *champion;
}

void netkit::neat::notify_end_of_fitness_rating() {
	if (m_best_genome_ever == nullptr) {
		m_best_genome_ever = new genome{get_current_best_genome()};
	} else {
		const genome &current_best_genome = get_current_best_genome();
		if (current_best_genome.get_fitness() > m_best_genome_ever->get_fitness()) {
			delete m_best_genome_ever;
			m_best_genome_ever = new genome{current_best_genome};
			m_age_of_best_genome_ever = 0;
		}
	}
}

void netkit::neat::helper_speciate() {
	genome_id_t geno_id = 0;
	for (const genome& geno : m_population.get_all_genomes()) {
		std::optional<species*> opt_species = find_appropriate_species_for(geno);
		if (opt_species.has_value()) {
			opt_species.value()->add_member(geno_id);
		} else {
			m_all_species.emplace_back(this, &m_population, m_next_species_id++, geno);
			m_all_species.back().add_member(geno_id);
		}
		++geno_id;
	}
}

