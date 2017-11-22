#pragma once

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "population.h"
#include "base_neat.h"
#include "organism.h"

namespace netkit {
class base_population; // forward declaration

class neat : public base_neat {
  public:
	explicit neat(const parameters& params);
	neat(const neat& other) = default;
	neat(neat&& other) noexcept;

	// Useful if you want to run and rate all the organisms at once.
	// Require more memory.
	std::vector<organism> generate_and_get_all_organisms();

	// Useful if you you run independant experiments for each organism.
	// Require less memory.
	organism generate_and_get_next_organism();

	// check if there is no more organism left to generate
	bool has_more_organisms_to_process();

	base_population* pop() final;
	const base_population* pop() const final;

  private:
	void impl_init(const genome& initial_genome) final;

	void impl_epoch() final;

  private:
	population m_population;
	genome_id_t m_next_genome_id;

	friend serializer& operator<<(serializer& ser, const neat& n);
	friend deserializer& operator>>(deserializer& des, neat& n);
};

serializer& operator<<(serializer& ser, const neat& n);
deserializer& operator>>(deserializer& des, neat& n);
}
