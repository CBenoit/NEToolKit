#include <cstdlib>
#include <ctime>

#include "netkit/neat/neat.h"

void netkit::init_neat() {
	std::srand(std::time(nullptr));
}
