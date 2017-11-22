#pragma once

#include <map>
#include <chrono>

#include "dynamic_population.h"
#include "base_neat.h"
#include "organism.h"

namespace netkit {
// you should call the epoch method on every game tick.
class rtneat : public base_neat {
  public:
	explicit rtneat(const parameters& params);
	rtneat(const rtneat& other) = default;
	rtneat(rtneat&& other) noexcept;

	void generate_all_organisms();
	// You should work on the vector to increase alive time and so on.
	// /!\ call generate_all_organisms before!
	std::vector<organism>& get_all_organisms();
	genome_id_t get_last_replaced_genome() { return m_replaced_genome_id; }
	bool has_replacement_occured_in_last_epoch() { return m_replacement_occured; }

	// TODO: to accommodate dynamic population.
	void increase_population(unsigned int number_of_organisms);
	void decrease_population(unsigned int number_of_organisms);

	base_population* pop() final;
	const base_population* pop() const final;

  private:
	void impl_init(const genome& initial_genome) final;

	void impl_epoch() final;

  private:
	dynamic_population m_population;
	unsigned int m_nb_replacements_performed;
	bool m_replacement_occured;
	genome_id_t m_replaced_genome_id;
	tick_t m_current_tick;
	std::vector<organism> m_all_organisms;

	friend serializer& operator<<(serializer& ser, const rtneat& n);
	friend deserializer& operator>>(deserializer& des, rtneat& n);
};

serializer& operator<<(serializer& ser, const rtneat& n);
deserializer& operator>>(deserializer& des, rtneat& n);
}
