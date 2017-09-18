#include <algorithm> // std::sort
#include <limits> // std::numeric_limits

#include "netkit/neat/species.h"
#include "netkit/neat/population.h"

netkit::species::species(population* population, species_id_t id, genome representant)
	: m_members()
	, m_current_avg_fitness(0)
	, m_current_best_fitness(0)
	, m_best_fitness_ever(0)
	, m_id(id)
	, m_age(0)
	, m_age_of_last_improvement(0)
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

void netkit::species::sort_by_fitness() {
	std::sort(m_members.begin(), m_members.end(), [&] (genome_id_t g1, genome_id_t g2) -> bool {
		return m_population->get_genome(g1).get_fitness() > m_population->get_genome(g2).get_fitness();
	});
	m_sorted = true;
}

void netkit::species::update_stats() {
	m_current_avg_fitness = 0;
	m_current_best_fitness = 0;

	for (genome_id_t g : m_members) {
		m_current_avg_fitness += m_population->get_genome(g).get_fitness();

		if (m_population->get_genome(g).get_fitness() > m_current_best_fitness) {
			m_current_best_fitness = m_population->get_genome(g).get_fitness();
		}
	}

	if (m_current_best_fitness > m_best_fitness_ever) {
		m_best_fitness_ever = m_current_best_fitness;
		m_age_of_last_improvement = m_age;
	}
}

void netkit::species::init_for_next_gen(genome new_representant) {
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
