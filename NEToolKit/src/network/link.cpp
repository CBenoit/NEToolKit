#include "netkit/network/link.h"

netkit::link::link(neuron_id_t from_id_, neuron_id_t to_id_, neuron_value_t weight_)
	: from(from_id_)
	, to(to_id_)
	, weight(weight_) {}

std::ostream& netkit::operator<<(std::ostream& os, const link& l) {
	os << "<link: from = " << l.from << " to = " << l.to << " weight = " << l.weight << ">";
	return os;
}
