#include <cmath>

#include "activation_functions.h"

NEAT::neuron_value_t NEAT::sigmoid(neuron_value_t input) {
	return 1 / (1 + std::exp(-input));
}
