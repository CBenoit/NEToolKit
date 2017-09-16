#include "netkit/network/link.h"

std::ostream & netkit::operator<<(std::ostream& os, const link& l) {
	os << "<link: from = " << l.from << " to = " << l.to << " weight = " << l.weight << ">";
	return os;
}
