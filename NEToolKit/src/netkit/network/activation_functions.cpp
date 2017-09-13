#include <cmath>

#include "netkit/network/activation_functions.h"

netkit::neuron_value_t netkit::sigmoid(neuron_value_t input) {
	return 1 / (1 + std::exp(-input));
}
