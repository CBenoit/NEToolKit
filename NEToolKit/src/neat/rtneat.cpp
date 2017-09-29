#include <utility> // std::move
#include <chrono>

#include "netkit/neat/rtneat.h"

netkit::rtneat::rtneat(const parameters& params_)
	: base_neat(params_)
	, m_nb_replacements_performed(0)
	, m_replacement_occured(false)
	, m_replaced_genome_id(0)
	, m_current_tick(0)
	, m_all_organisms() {}

netkit::rtneat::rtneat(rtneat&& other) noexcept
	: base_neat(std::move(other))
	, m_nb_replacements_performed(other.m_nb_replacements_performed)
	, m_replacement_occured(other.m_replacement_occured)
	, m_replaced_genome_id(other.m_replaced_genome_id)
	, m_current_tick(other.m_current_tick)
	, m_all_organisms(std::move(other.m_all_organisms)) {}

void netkit::rtneat::generate_all_organisms() {
	m_all_organisms.empty();
	m_all_organisms.reserve(m_population.size());
	for (genome_id_t i = 0, size = m_population.size(); i < size; ++i) {
		m_all_organisms.emplace_back(&m_population, i, m_population[i].generate_network());
	}
}

std::vector<netkit::organism>& netkit::rtneat::get_all_organisms() {
	return m_all_organisms;
}

void netkit::rtneat::impl_epoch() {
	++m_current_tick;

	auto frequency = static_cast<tick_t>(std::ceil(
	  static_cast<double>(params.minmum_alive_time_before_being_replaced)
	  / (static_cast<double>(m_population.size()) * params.proportion_of_eligible_at_any_epoch)
	));
	if (m_current_tick % frequency != 0) {
		m_replacement_occured = false;
		return;
	}

	// first step: compute the adjusted fitness of every genomes
	for (species& spec : m_all_species) {
		spec.share_fitness();
	}

	// second step: pick the worst genome
	genome_id_t worst_genome = 0;
	bool found_candidate = false;
	double worst_fitness = std::numeric_limits<double>::max();
	for (genome_id_t gen_id = 0; gen_id < m_population.size(); ++gen_id) {
		if (m_population[gen_id].get_fitness() < worst_fitness && m_all_organisms[gen_id].get_time_alive() > params.minmum_alive_time_before_being_replaced) {
			worst_genome = gen_id;
			worst_fitness = m_population[gen_id].get_fitness();
			found_candidate = true;
		}
	}

	if (found_candidate) {
		// third step: remove the worst genome from its species and calculate the new adjusted fitness of its members.
		// (at the same time, compute the total of all species fitness average for next step)
		double summed_average = 0;
		for (species& spec : m_all_species) {
			if (spec.has(worst_genome)) {
				spec.remove_member(worst_genome);
				spec.share_fitness();
			}
			spec.update_stats();
			spec.sort_by_fitness();
			summed_average += spec.get_avg_adjusted_fitness();
		}
		summed_average /= static_cast<double>(m_population.size());

		// fourth step: select species for reproduction
		std::uniform_real_distribution<double> prob(0.0, 1.0);
		std::uniform_int_distribution<size_t> species_selector(0, m_all_species.size() - 1);
		double rnd_val = prob(rand_engine);
		for (species& spec : m_all_species) {
			double selection_probability = spec.get_avg_adjusted_fitness() / summed_average;
			if (rnd_val <= spec.get_avg_adjusted_fitness() / summed_average) {
				// we have a winner!

				if (prob(rand_engine) < params.crossover_prob) { // let's go for a crossover
					genome* genitor1 = &m_population.get_genome(spec.select_one_genitor());
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
						m_population.replace_genome(worst_genome, genitor1->random_crossover(*genitor2).get_random_mutation());
					} else {
						m_population.replace_genome(worst_genome, genitor1->random_crossover(*genitor2));
					}
				} else {
					genome* genitor = &m_population.get_genome(spec.select_one_genitor());
					m_population.replace_genome(worst_genome, genitor->get_random_mutation());
				}

				// speciate the new offspring
				helper_speciate_one_genome(worst_genome);

				break; // stop iterating
			} else {
				rnd_val -= selection_probability;
			}
		}

		// increment the number of replacements performed
		++m_nb_replacements_performed;

		// fifth step: reorganize species and adjust the compatibility threshold if it is dynamic.
		if (m_nb_replacements_performed % params.number_of_replacements_before_species_reorganization == 0) {
			for (species& spec : m_all_species) {
				if (!spec.empty()) { // the species could be empty. It will be deleted later.
					// If this check is not performed, there is a segfault for trying to get member in an empty set.
					spec.init_for_next_gen(m_population.get_genome(spec.get_random_member()));
				}
			}

			helper_speciate_all_population();

			// remove species that has no more member. They go extinct!
			m_all_species.erase(
			  std::remove_if(m_all_species.begin(), m_all_species.end(), [](const species& s) {
				  return s.empty();
			  }),
			  m_all_species.end()
			);

			if (params.dynamic_compatibility_threshold) {
				// adjust the compatibility threshold
				if (m_all_species.size() > params.target_number_of_species) {
					params.compatibility_threshold += params.compatibility_threshold_change_step;
				} else if (m_all_species.size() < params.target_number_of_species) {
					params.compatibility_threshold -= params.compatibility_threshold_change_step;
				}
			}
		}

		// Step 6: replacing the old agent with the new one aka replace the old organism.
		m_replacement_occured = true;
		m_replaced_genome_id = worst_genome;
		m_all_organisms[m_replaced_genome_id] = organism(&m_population, m_replaced_genome_id, m_population[m_replaced_genome_id].generate_network());
	} else {
		m_replacement_occured = false;
	}
}
