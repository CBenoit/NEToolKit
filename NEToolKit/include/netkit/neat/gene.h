#pragma once

#include "netkit/network/network_primitive_types.h"
#include "neat_primitive_types.h"

namespace netkit {
static innov_num_t next_innovation = 0;

struct gene {
	gene(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight)
		: from(from_id)
		, to(to_id)
		, innovation(next_innovation++)
		, weight(weight)
		, enabled(true) {}

	gene(neuron_id_t from_id, neuron_id_t to_id)
		: gene(from_id, to_id, 0) {}

	gene(const gene& other)
		: from(other.from)
		, to(other.to)
		, innovation(other.innovation)
		, weight(other.weight)
		, enabled(other.enabled) {}

	const neuron_id_t from;
	const neuron_id_t to;
	const innov_num_t innovation;
	neuron_value_t weight;
	bool enabled;
};
}
