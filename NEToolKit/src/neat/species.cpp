#include <algorithm> // std::sort, std::find
#include <limits> // std::numeric_limits

#include "netkit/neat/species.h"
#include "netkit/neat/population.h"

netkit::species::species(population* population, species_id_t id, genome representant)
	: m_members()
	, m_avg_fitness(0)
	, m_best_fitness(0)
	, m_summed_fitnesses(0)
	, m_best_fitness_ever(0)
	, m_id(id)
	, m_age(0)
	, m_age_of_last_improvement(0)
	, m_expected_offsprings(0)
	, m_sorted(false)
	, m_representant(new genome(std::move(representant)))
	, m_population(population) {}

netkit::genome_id_t netkit::species::get_champion() const {
	if (m_sorted) {
		return m_members.front();
	} else {
		genome_id_t best = m_members.front();
		double best_fitness_so_far = std::numeric_limits<double>::min();
		for (genome_id_t g : m_members) {
			if (m_population->get_genome(g).get_fitness() > best_fitness_so_far) {
				best = g;
				best_fitness_so_far = m_population->get_genome(g).get_fitness();
			}
		}
		return best;
	}
}

bool netkit::species::has(genome_id_t geno_id) const {
	return std::find(m_members.cbegin(), m_members.cend(), geno_id) != m_members.cend();
}

netkit::genome_id_t netkit::species::select_one_genitor() const {
	if (m_best_fitness > 0) {
		double rnd_val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
		for (genome_id_t g : m_members) {
			double genome_selection_prop = m_population->get_genome(g).get_fitness() / m_summed_fitnesses;
			if (rnd_val < genome_selection_prop) {
				return g;
			}
			rnd_val -= genome_selection_prop;
		}

		return m_members.back(); // just in case
	} else {
		return m_members.front();
	}
}

void netkit::species::sort_by_fitness() {
	if (!m_sorted) {
		std::sort(m_members.begin(), m_members.end(), [&] (genome_id_t g1, genome_id_t g2) -> bool {
			return m_population->get_genome(g1).get_fitness() > m_population->get_genome(g2).get_fitness();
		});
		m_sorted = true;
	}
}

void netkit::species::update_stats() {
	m_avg_fitness = 0;
	m_best_fitness = 0;
	m_summed_fitnesses = 0;

	for (genome_id_t g : m_members) {
		m_summed_fitnesses += m_population->get_genome(g).get_fitness();

		if (m_population->get_genome(g).get_fitness() > m_best_fitness) {
			m_best_fitness = m_population->get_genome(g).get_fitness();
		}
	}
	m_avg_fitness = m_summed_fitnesses / m_members.size();

	if (m_best_fitness > m_best_fitness_ever) {
		m_best_fitness_ever = m_best_fitness;
		m_age_of_last_improvement = m_age;
	}
}

void netkit::species::init_for_next_gen(genome new_representant) {
	m_avg_fitness = 0;
	m_best_fitness = 0;
	m_summed_fitnesses = 0;
	++m_age;

	delete m_representant;
	m_representant = new genome(std::move(new_representant));

	m_members.clear();
	m_sorted = false;
}

void netkit::species::add_member(genome_id_t geno_id) {
	m_members.push_back(geno_id);
	m_sorted = false;
}

void netkit::species::share_fitness() const {
	for (genome_id_t g : m_members) {
		m_population->get_genome(g).set_fitness(
			m_population->get_genome(g).get_fitness() / m_members.size()
		);
	}
}
