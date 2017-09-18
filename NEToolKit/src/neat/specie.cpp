#include <algorithm> // std::sort
#include <limits> // std::numeric_limits

#include "netkit/neat/specie.h"

netkit::specie::specie(specie_id_t id)
	: m_members()
	, m_current_avg_fitness(0)
	, m_current_best_fitness(0)
	, m_best_fitness_ever(0)
	, m_id(id)
	, m_age(0)
	, m_age_of_last_improvement(0)
	, m_representant(nullptr)
	, m_sorted(false) {}

netkit::specie::~specie() {
	if (m_representant != nullptr) {
		delete m_representant;
	}
}

const std::vector<netkit::genome*>& netkit::specie::get_members() const {
	return m_members;
}

double netkit::specie::get_avg_fitness() const {
	return m_current_avg_fitness;
}

double netkit::specie::get_best_fitness() const {
	return m_current_best_fitness;
}

double netkit::specie::get_best_fitness_ever() const {
	return m_best_fitness_ever;
}

netkit::specie_id_t netkit::specie::get_id() const {
	return m_id;
}

netkit::specie_age_t netkit::specie::get_age() const {
	return m_age;
}

netkit::specie_age_t netkit::specie::get_age_of_last_improvement() const {
	return m_age_of_last_improvement;
}

bool netkit::specie::are_members_sorted_by_fitness() const {
	return m_sorted;
}

netkit::genome* netkit::specie::get_representant() const {
	return m_representant;
}

netkit::genome* netkit::specie::get_champion() const {
	if (m_sorted) {
		return m_members.front();
	} else {
		genome* best = nullptr;
		double best_fitness_so_far = std::numeric_limits<double>::min();
		for (genome* g : m_members) {
			if (g->get_fitness() > best_fitness_so_far) {
				best = g;
				best_fitness_so_far = g->get_fitness();
			}
		}
		return best;
	}
}

void netkit::specie::sort_by_fitness() {
	std::sort(m_members.begin(), m_members.end(), [&] (genome* g1, genome* g2) -> bool {
		return g1->get_fitness() > g2->get_fitness();
	});
	m_sorted = true;
}

void netkit::specie::update_stats() {
	m_current_avg_fitness = 0;
	m_current_best_fitness = 0;

	for (const genome* g : m_members) {
		m_current_avg_fitness += g->get_fitness();

		if (g->get_fitness() > m_current_best_fitness) {
			m_current_best_fitness = g->get_fitness();
		}
	}

	if (m_current_best_fitness > m_best_fitness_ever) {
		m_best_fitness_ever = m_current_best_fitness;
		m_age_of_last_improvement = m_age;
	}
}

void netkit::specie::init_for_next_gen(genome new_representant) {
	++m_age;
	m_representant = new genome(std::move(new_representant));
	m_members.clear();
	m_sorted = false;
}

void netkit::specie::add_genome(genome* genome) {
	m_members.push_back(genome);
	m_sorted = false;
}
