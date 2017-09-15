#pragma once

#include "parameters.h"
#include "innovation_pool.h"

namespace netkit {
class neat {
public:
	neat(parameters params);
	neat(const neat& other);

	innovation_pool& innov_pool() {
		return m_innov_pool;
	}

	/*
	// Useful if you want to run and rate all the organisms at once.
	// Require more memory.
	std::vector<organism> generate_and_get_all_organisms();

	// Useful if you you run independant experiments for each organism.
	// Require less memory.
	organism generate_and_get_next_organism();

	// check if there is no more organism left to generate
	bool no_more_organism();

	// you should have rated every organisms beforme calling this
	void goto_next_generation();
	*/

public:
	const parameters params;

private:
	innovation_pool m_innov_pool;
};
}
