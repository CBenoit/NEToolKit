#include <iostream>

#include "xor_experiment.h"
#include "genome_mutations_crossovers.h"

enum choice_t {
	EXIT,
	EX_XOR_NET,
	XOR_EV_EXP,
	GEN_MUT_CROSS,
	COFFEE
};

int main() {
	std::cout << "    NEToolKit examples" << std::endl;
	std::cout << "~~------==========------~~" << std::endl;
	unsigned int choice = -1;

	while (choice != EXIT) {
		std::cout << std::endl << "Would you like to:" << std::endl;
		std::cout << "\t" << EX_XOR_NET << ". run the example xor network?" << std::endl;
		std::cout << "\t" << XOR_EV_EXP << ". run the xor network evolution experiment?" << std::endl;
		std::cout << "\t" << GEN_MUT_CROSS << ". run various mutations and crossover on simple genomes." << std::endl;
		std::cout << "\t" << COFFEE << ". get a cup of coffee?" << std::endl;
		std::cout << "\t" << EXIT << ". exit this program?" << std::endl;

		std::cout << ">>> ";
		std::cin >> choice;

		// for bad people who doesn't give numerical inputs...
		// get rid of failure state
		std::cin.clear();
		// discard 'bad' character(s)
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (choice) {
		case EXIT:
			std::cout << "Exiting..." << std::endl;
			break;
		case EX_XOR_NET:
			xor_network_test();
			break;
		case XOR_EV_EXP:
			run_xor_experiment();
			break;
		case GEN_MUT_CROSS:
			run_genome_mutations_crossovers();
			break;
		case COFFEE:
			std::cout << "I hope you will find one then." << std::endl;
			break;
		default:
			std::cout << "Well, try to choose something I proposed, OK?" << std::endl;
		}
	}
}
