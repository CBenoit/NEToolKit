#pragma once

#include <iostream>

#include "netkit/network/network_primitive_types.h"
#include "neat_primitive_types.h"

namespace netkit {
struct gene {
	gene(innov_num_t innov_num, neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight = 0);
	gene(const gene& other) = default;

	const innov_num_t innov_num;
	const neuron_id_t from;
	const neuron_id_t to;
	neuron_value_t weight;
	bool enabled;
	// TODO: ability to be frozen?

	friend std::ostream& operator<<(std::ostream& os, const gene& g);
};

std::ostream& operator<<(std::ostream& os, const gene& g);
}
