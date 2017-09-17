#include <cstdlib>
#include <ctime>
#include <utility> // std::move
#include <stdexcept> // std::invalid_argument

#include "netkit/neat/neat.h"

netkit::neat::neat(parameters params_)
	: params(std::move(params_))
	, m_innov_pool(this->params) {
	if (this->params.number_of_outputs == 0 || this->params.number_of_inputs == 0) {
		throw std::invalid_argument("genomes needs at least one input and one output.");
	}

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

netkit::neat::neat(const neat& other)
	: params(other.params)
	, m_innov_pool(this->params) // the innovation pool has to differ from the other simulation
{} // no need to use srand again.
