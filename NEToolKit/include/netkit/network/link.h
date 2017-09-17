#pragma once

#include <iostream>

#include "network_primitive_types.h"

namespace netkit {
struct link {
	link(neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight);
	link(neuron_id_t from_id, neuron_id_t to_id);

	neuron_id_t from;
	neuron_id_t to;
	neuron_value_t weight;

	friend std::ostream& operator<<(std::ostream& os, const link& l);
};

std::ostream& operator<<(std::ostream& os, const link& l);
}
