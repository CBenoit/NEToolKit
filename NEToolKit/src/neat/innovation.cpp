#include "netkit/neat/innovation.h"

netkit::innovation::innovation(innov_type type_, innov_num_t innov_num_, innov_num_t innov_num_2_,
							   neuron_id_t from_, neuron_id_t to_, neuron_id_t new_neuron_id_)
	: type(type_)
	, innov_num(innov_num_)
	, innov_num_2(innov_num_2_)
	, from(from_)
	, to(to_)
	, new_neuron_id(new_neuron_id_) {}
