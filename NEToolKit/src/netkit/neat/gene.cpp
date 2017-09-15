#include "netkit/neat/gene.h"

std::ostream& netkit::operator<<(std::ostream& os, const gene& g) {
	os << "<gene: from = " << g.from << " to = " << g.to << " weight = " << g.weight
		<< " innovation = " << g.innov_num << " enabled = " << g.enabled << ">";
	return os;
}
