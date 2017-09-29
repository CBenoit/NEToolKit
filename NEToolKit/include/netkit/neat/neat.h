#pragma once

#include "base_neat.h"
#include "organism.h"

namespace netkit {
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

private:
	void impl_epoch() final;

private:
	genome_id_t m_next_genome_id;
};
}
