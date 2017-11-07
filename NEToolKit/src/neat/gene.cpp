#include "netkit/neat/gene.h"

netkit::gene::gene(innov_num_t innov_num_, neuron_id_t from_id_, neuron_id_t to_id_, neuron_value_t weight_)
	: innov_num(innov_num_)
	, from(from_id_)
	, to(to_id_)
	, weight(weight_)
	, enabled(true)
	, frozen(false) {}

std::ostream& netkit::operator<<(std::ostream& os, const gene& g) {
	os << "<gene: from = " << g.from << " to = " << g.to << " weight = " << g.weight
	   << " innovation = " << g.innov_num << " enabled = " << g.enabled << " frozen = " << g.frozen << ">";
	return os;
}

netkit::serializer& netkit::operator<<(serializer& ser, const gene& g) {
	ser.append(g.innov_num);
	ser.append(g.from);
	ser.append(g.to);
	ser.append(g.weight);
	ser.append(g.enabled);
	ser.append(g.frozen);
	ser.new_line();

	return ser;
}

netkit::deserializer& netkit::operator>>(netkit::deserializer& des, gene& g) {
	des.get_next(g.innov_num);
	des.get_next(g.from);
	des.get_next(g.to);
	des.get_next(g.weight);
	des.get_next(g.enabled);
	des.get_next(g.frozen);

	return des;
}
