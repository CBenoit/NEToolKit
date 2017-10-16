#include "netkit/neat/innovation.h"

netkit::innovation netkit::innovation::next_innovation_from_des(deserializer& des) {
	innov_type type;
	innov_num_t innov_num;
	innov_num_t innov_num_2;
	neuron_id_t from;
	neuron_id_t to;
	neuron_id_t new_neuron_id;

	des.get_next(innov_num);
	des.get_next(from);
	des.get_next(to);

	bool is_new_link;
	des.get_next(is_new_link);
	if (is_new_link) {
		type = NEW_LINK;
		innov_num_2 = 0;
		new_neuron_id = 0;
	} else {
		type = NEW_NEURON;
		des.get_next(innov_num_2);
		des.get_next(new_neuron_id);
	}

	return innovation(type, innov_num, innov_num_2, from, to, new_neuron_id);
}

netkit::innovation::innovation(innov_type type_, innov_num_t innov_num_, innov_num_t innov_num_2_,
							   neuron_id_t from_, neuron_id_t to_, neuron_id_t new_neuron_id_)
	: type(type_)
	, innov_num(innov_num_)
	, innov_num_2(innov_num_2_)
	, from(from_)
	, to(to_)
	, new_neuron_id(new_neuron_id_) {}

netkit::serializer& netkit::operator<<(serializer& ser, const innovation& innov) {
	ser.append(innov.innov_num);
	ser.append(innov.from);
	ser.append(innov.to);
	if (innov.type == NEW_NEURON) {
		ser.append(false); // false = new neuron
		ser.append(innov.innov_num_2);
		ser.append(innov.new_neuron_id);
	} else {
		ser.append(true); // true = new link
	}
	ser.new_line();

	return ser;
}
