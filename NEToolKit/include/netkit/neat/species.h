#pragma once

#include "neat_primitive_types.h"
#include "genome.h"

namespace netkit {
class population; // forward declaration

class species {
public:
	species(population* population, species_id_t id, genome representant);
	~species() { delete m_representant; }

	const std::vector<genome_id_t>& get_members_ids() const { return m_members; }
	double get_avg_fitness() const { return m_current_avg_fitness; }
	double get_best_fitness() const { return m_current_best_fitness; }
	double get_best_fitness_ever() const { return m_best_fitness_ever; }
	species_id_t get_id() const { return m_id; }
	species_age_t get_age() const { return m_age; }
	species_age_t get_age_of_last_improvement() const { return m_age_of_last_improvement; }
	bool are_members_sorted_by_fitness() const { return m_sorted; }
	const genome& get_representant() const { return *m_representant; }
	genome_id_t get_champion() const;

	void sort_by_fitness();
	void update_stats();
	void init_for_next_gen(genome new_representant);
	void add_member(genome_id_t geno_id);

private:
	std::vector<genome_id_t> m_members;

	double m_current_avg_fitness;
	double m_current_best_fitness;
	double m_best_fitness_ever;

	species_id_t m_id;
	species_age_t m_age;
	species_age_t m_age_of_last_improvement;

	bool m_sorted;

	genome* m_representant;
	population* m_population;
};
}
