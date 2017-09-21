#pragma once

#include "neat_primitive_types.h"
#include "genome.h"

namespace netkit {
class population; // forward declaration

class species {
public:
	species(population* population, species_id_t id, const genome& representant);
	species(const species& other);
	species(species&& other) noexcept;
	species& operator=(const species& other);
	species& operator=(species&& other) noexcept;
	~species() { delete m_representant; }

	const std::vector<genome_id_t>& get_members_ids() const { return m_members; }
	double get_avg_fitness() const { return m_avg_fitness; }
	double get_best_fitness() const { return m_best_fitness; }
	double get_summed_fitnesses() const { return m_summed_fitnesses; }
	double get_best_fitness_ever() const { return m_best_fitness_ever; }
	species_id_t get_id() const { return m_id; }
	species_age_t get_age() const { return m_age; }
	species_age_t get_age_of_last_improvement() const { return m_age_of_last_improvement; }
	bool are_members_sorted_by_fitness() const { return m_sorted; }
	const genome& get_representant() const { return *m_representant; }
	genome_id_t get_champion() const;
	genome_id_t get_random_member() const;
	bool has(genome_id_t geno_id) const;
	unsigned int get_expected_offsprings() const { return m_expected_offsprings; }
	genome_id_t select_one_genitor() const;
	bool empty() const { return m_members.empty(); }
	size_t number_of_members() const { return m_members.size(); }

	void sort_by_fitness();
	void update_stats();
	void init_for_next_gen(genome new_representant);
	void add_member(genome_id_t geno_id);
	void share_fitness() const; // share the fitness amongst members
	void set_expected_offsprings(unsigned int value) { m_expected_offsprings = value; }

private:
	std::vector<genome_id_t> m_members;

	double m_avg_fitness;
	double m_best_fitness;
	double m_summed_fitnesses;
	double m_best_fitness_ever;

	species_id_t m_id;
	species_age_t m_age;
	species_age_t m_age_of_last_improvement;

	unsigned int m_expected_offsprings;

	bool m_sorted;

	genome* m_representant;
	population* m_population;

	friend std::ostream& operator<<(std::ostream& os, const species& spec);
};

std::ostream& operator<<(std::ostream& os, const species& spec);
}
