#pragma once

#include "neat_primitive_types.h"
#include "genome.h"

namespace netkit {
class specie {
public:
	specie(specie_id_t id);
	virtual ~specie();

	const std::vector<genome*>& get_members() const;
	double get_avg_fitness() const;
	double get_best_fitness() const;
	double get_best_fitness_ever() const;
	specie_id_t get_id() const;
	specie_age_t get_age() const;
	specie_age_t get_age_of_last_improvement() const;
	bool are_members_sorted_by_fitness() const;
	genome* get_representant() const;
	genome* get_champion() const;

	void sort_by_fitness();
	void update_stats();
	void init_for_next_gen(genome new_representant);
	void add_genome(genome* genome);

private:
	std::vector<genome*> m_members;

	double m_current_avg_fitness;
	double m_current_best_fitness;
	double m_best_fitness_ever;

	specie_id_t m_id;
	specie_age_t m_age;
	specie_age_t m_age_of_last_improvement;

	genome* m_representant;

	bool m_sorted;
};
}
