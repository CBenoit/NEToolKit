#include <utility> // std::move

#include "netkit/neat/neat.h"

netkit::neat::neat(const parameters& params_)
	: base_neat(params_)
	, m_population(this)
	, m_next_genome_id(0) {}

netkit::neat::neat(neat&& other) noexcept
	: base_neat(std::move(other))
	, m_population(this)
	, m_next_genome_id(other.m_next_genome_id) {} // it's actually OK to get the trivial member from the moved object.

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

void netkit::neat::impl_init(const genome& initial_genome) {
	// populate with random mutations from the initial genome.
	for (size_t i = 0; i < params.initial_population_size; i++) {
		m_population.add_genome(initial_genome.get_random_mutation());
	}
}

void netkit::neat::impl_epoch() {
	// initialize generation-specific variables
	m_next_genome_id = 0;

	// Compute the overall average fitness.
	species* best_species = nullptr;
	double best_fitness_so_far = -1 * std::numeric_limits<double>::max();
	double overall_average = 0;
	for (species& spec : m_all_species) {
		spec.sort_by_fitness();
		spec.share_fitness();
		spec.update_stats();
		overall_average += spec.get_summed_adjusted_fitnesses();
		if (spec.get_best_fitness() > best_fitness_so_far) {
			best_fitness_so_far = spec.get_best_fitness();
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
			this_species_expected_offsprings = static_cast<unsigned int>(spec.get_summed_adjusted_fitnesses() / overall_average);
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
	std::uniform_real_distribution<double> prob(0.0, 1.0);
	std::uniform_int_distribution<size_t> species_selector(0, m_all_species.size() - 1);
	for (species& spec : m_all_species) {
		unsigned int offsprings_produced = 0;

		// keep the champion of species with 5 or more members
		if (offsprings_produced < spec.get_expected_offsprings()
			&& spec.number_of_members() >= 5) { // TODO: externalize in parameters
			if (params.use_best_genomes_library) {
				// update genome library using the champion of the species.
				helper_update_best_genomes_library_with(m_population.get_genome(spec.get_champion()));
			}
			offsprings.emplace_back(m_population.get_genome(spec.get_champion()));
			++offsprings_produced;
		}

		while (offsprings_produced < spec.get_expected_offsprings()) {
			++offsprings_produced;
			genome* genitor = &m_population.get_genome(spec.select_one_genitor());

			// if using the best genomes library.
			bool replacement_occured = false;
			if (!m_best_genomes_library.empty() && genitor->get_fitness() < params.bad_genome_max_fitness
				&& prob(rand_engine) < params.replace_bad_genes_using_best_genomes_library_prob) {
				auto good_geno = get_random_genome_from_best_genome_library();
				if (good_geno->get_fitness() > genitor->get_fitness()) {
					offsprings.push_back(*good_geno);
					replacement_occured = true;
				}
			}

			if (!replacement_occured) {
				if (prob(rand_engine) < params.crossover_prob) { // let's go for a crossover
					genome* genitor2 = nullptr;

					// interspecies crossover prob
					if (prob(rand_engine) < params.interspecies_crossover_prob) {
						unsigned long rnd_spec_val = species_selector(rand_engine);
						genitor2 = &m_population.get_genome(m_all_species[rnd_spec_val].select_one_genitor());
					} else {
						genitor2 = &m_population.get_genome(spec.select_one_genitor());
					}

					// mutate the offspring or not
					if (prob(rand_engine) < params.mutation_during_crossover_prob) {
						offsprings.push_back(genitor->random_crossover(*genitor2).get_random_mutation());
					} else {
						offsprings.push_back(genitor->random_crossover(*genitor2));
					}
				} else {
					offsprings.push_back(genitor->get_random_mutation());
				}
			}
		}
	}

	// prepare the species for the next generation
	// Can't be performed in the previous loop since init_for_next_gen method clear the members for
	// next speciation, and we need the members for interspecies crossovers.
	for (species& spec : m_all_species) {
		spec.init_for_next_gen(params.keep_same_representant_for_species
							   ? spec.get_representant()
							   : m_population.get_genome(spec.get_random_member()));
	}

	// update the population with the new offsprings.
	m_population.clear(); // just in case...
	m_population.set_genomes(std::move(offsprings));

	// finally speciate the population
	helper_speciate_all_population();

	// remove species that has no more member. They go extinct!
	m_all_species.erase(
	std::remove_if(m_all_species.begin(), m_all_species.end(), [](const species & s) {
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

	// The original paper says it's important to remember innovations that already occurred within only one generation.
	// Memorizing for every generation or not doesn't seems to have performance issues. Further tests needed.
	//innov_pool.clear();
}

netkit::base_population* netkit::neat::pop() {
	return &m_population;
}

const netkit::base_population* netkit::neat::pop() const {
	return &m_population;
}

netkit::serializer& netkit::operator<<(serializer& ser, const neat& n) {
	n.helper_serialize_base_neat(ser);

	// serialize next_genome_id
	ser.append(n.m_next_genome_id);
	ser.new_line();

	// serialize population
	ser << n.m_population;

	return ser;
}

netkit::deserializer& netkit::operator>>(deserializer& des, neat& n) {
	n.helper_deserialize_base_neat(des);

	// serialize next_genome_id
	des.get_next(n.m_next_genome_id);

	// deserialize population
	des >> n.m_population;

	return des;
}
