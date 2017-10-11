#pragma once

#include <iostream>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "netkit/network/network_primitive_types.h"
#include "neat_primitive_types.h"

namespace netkit {
struct gene {
	gene(innov_num_t innov_num, neuron_id_t from_id, neuron_id_t to_id, neuron_value_t weight = 0);
	gene(const gene& other) = default;
	gene& operator=(const gene& other) = default;

	innov_num_t innov_num;
	neuron_id_t from;
	neuron_id_t to;
	neuron_value_t weight;
	bool enabled;
	// TODO: ability to be frozen?

	friend std::ostream& operator<<(std::ostream& os, const gene& g);
	friend serializer& operator<<(serializer& ser, const gene& g);
	friend deserializer& operator>>(deserializer& des, gene& g);
};

std::ostream& operator<<(std::ostream& os, const gene& g);
serializer& operator<<(serializer& ser, const gene& g);
deserializer& operator>>(deserializer& des, gene& g);
}
