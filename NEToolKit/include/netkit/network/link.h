#pragma once

#include "network_primitive_types.h"

namespace netkit {
struct link {
	link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight)
		: from(from_id)
		, to(to_id)
		, weight(weight)
		, disabled(false) {}

	link(neuron_id_t from_id, neuron_id_t to_id)
		: link(from_id, to_id, 0) {}

	neuron_id_t from;
	neuron_id_t to;
	neuron_value_t weight;
	bool disabled; // FIXME: not yet used
};
}
