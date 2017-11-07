#pragma once

namespace netkit {
using innov_num_t = unsigned int;

using species_id_t = int;
using species_age_t = unsigned int;

using genome_id_t = size_t;

using tick_t = unsigned long;

enum mutation_t {
	ADD_LINK = 0,
	ADD_NEURON = 1,
	REMOVE_NEURON = 2,
	ADD_CASCADE = 3, // these four are treated a bit differently (see genome.cpp random_mutate method)
	REMOVE_GENE,
	REENABLE_GENE,
	TOGGLE_ENABLE,
	RESET_WEIGHTS,
	PERTURBATE_WEIGHTS,
	NUMBER_OF_MUTATIONS
};
}
