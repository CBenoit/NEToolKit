#pragma once

#include <functional>

namespace NEAT {
using neuron_value_t = double;

using neuron_id_t = unsigned int;

enum neuron_type_t {
	INPUT,
	HIDDEN,
	OUTPUT
};

using link_id_t = unsigned int;

using activation_func_t = std::function<neuron_value_t(neuron_value_t)>;
}
