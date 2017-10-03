#include "netkit/neat/gene.h"

netkit::gene::gene(innov_num_t innov_num_, neuron_id_t from_id_, neuron_id_t to_id_, neuron_value_t weight_)
	: innov_num(innov_num_)
	, from(from_id_)
	, to(to_id_)
	, weight(weight_)
	, enabled(true) {}

std::ostream& netkit::operator<<(std::ostream& os, const gene& g) {
	os << "<gene: from = " << g.from << " to = " << g.to << " weight = " << g.weight
	   << " innovation = " << g.innov_num << " enabled = " << g.enabled << ">";
	return os;
}
