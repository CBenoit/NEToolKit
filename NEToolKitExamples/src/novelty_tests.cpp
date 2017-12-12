#include <cmath>

#include <netkit/neat/genome.h>
#include <netkit/neat/novelgenome.h>
#include <netkit/neat/novelbank.h>
#include <netkit/neat/neat.h>

#include "novelty_tests.h"

struct novelty_position {
	novelty_position(double a_, double b_) : a(a_), b(b_) {}

	double novelty_distance(const novelty_position& other) const {
		double a_diff = a - other.a;
		double b_diff = b - other.b;
		return std::sqrt(a_diff*a_diff + b_diff*b_diff);
	}

	double a, b;
};

void run_novelty_tests() {
	std::cout << "Starting novelty tests..." << std::endl;

	netkit::parameters params;
	params.initial_population_size = 10;
	params.compatibility_threshold = 1;
	netkit::neat neat(params);

	std::vector<netkit::novelgenome<novelty_position>> all_ng;
	netkit::novelbank<novelty_position> novelbank(999, 5, 3);

	for (int i = 0; i < 6; ++i) {
		netkit::genome g(&neat);
		netkit::novelgenome<novelty_position> ng(static_cast<netkit::genome_id_t>(i), novelty_position(i * 5 % 9, i * 4 % 9));
		novelbank.pop_register(ng);
		all_ng.push_back(std::move(ng));
		std::cout << i << ": " << all_ng[i].get_pos().a << ", " << all_ng[i].get_pos().b << std::endl;
	}

	std::cout << "dist 0->1 : " << all_ng[0].distance(all_ng[1]) << std::endl;

	std::cout << "sparseness of 0: " << novelbank.evaluate(all_ng[0]) << std::endl;
}
