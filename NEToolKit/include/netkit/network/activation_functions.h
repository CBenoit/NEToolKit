#pragma once

#include "network_primitive_types.h"

namespace netkit {
neuron_value_t sigmoid(neuron_value_t input);

neuron_value_t steepened_sigmoid(neuron_value_t input);
}
