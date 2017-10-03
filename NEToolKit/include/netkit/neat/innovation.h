#pragma once

#include "netkit/network/network_primitive_types.h"
#include "neat_primitive_types.h"

namespace netkit {
enum innov_type {
	NEW_LINK,
	NEW_NEURON
};

class innovation {
  public:
	static innovation new_link_innovation(innov_num_t innov_num, neuron_id_t from, neuron_id_t to) {
		return innovation(NEW_LINK, innov_num, 0, from, to, 0);
	}

	static innovation new_neuron_innovation(innov_num_t innov_num_1, innov_num_t innov_num_2,
											neuron_id_t from, neuron_id_t to,
											neuron_id_t new_neuron_id) {
		return innovation(NEW_NEURON, innov_num_1, innov_num_2, from, to, new_neuron_id);
	}

	innovation(const innovation& other) = default;

	const innov_type type;
	const innov_num_t innov_num;
	const innov_num_t innov_num_2; // only used for the new neuron innovation (two genes produced)
	const neuron_id_t from;
	const neuron_id_t to;
	const neuron_id_t new_neuron_id; // only used for the new neuron innovation

  private:
	innovation(innov_type type, innov_num_t innov_num, innov_num_t innov_num_2,
			   neuron_id_t from, neuron_id_t to, neuron_id_t new_neuron_id);
};
}
