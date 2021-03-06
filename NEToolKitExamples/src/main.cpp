#include <iostream>

#include "utils.h"
#include "xor_experiment.h"
#include "genome_mutations_crossovers.h"
#include "random_evolution.h"
#include "serialization_tests.h"
#include "novelty_tests.h"

enum choice_t {
	EXIT,

	XOR_EV_EXP,
	XOR_EV_100_EXP,
	XOR_EV_STATS_EXP,

	NOVELTY_EV_EXP,
	NOVELTY_EV_100_EXP,
	NOVELTY_EV_STATS_EXP,

	EX_XOR_NET,
	RT_XOR_EXP,
	RAND_EVO,
	GEN_MUT_CROSS,
	SERDES,
	NOVELTY_TESTS,

	COFFEE
};

int main() {
	std::cout << "    NEToolKit examples" << std::endl;
	std::cout << "~~------==========------~~" << std::endl;
	int choice = -1;

	while (choice != EXIT) {
		if (choice != -1)
			wait_user();

		std::cout << std::endl << "Would you like to:" << std::endl;
		std::cout << "\t" << XOR_EV_EXP << ". run one detailed xor network evolution experiment?" << std::endl;
		std::cout << "\t" << XOR_EV_100_EXP << ". run 100 xor network evolution experiments?" << std::endl;
		std::cout << "\t" << XOR_EV_STATS_EXP << ". run 1000 xor network evolution experiments and record in a csv file?" << std::endl;

		std::cout << "\t" << NOVELTY_EV_EXP << ". (novelty) run one detailed xor network evolution experiment?" << std::endl;
		std::cout << "\t" << NOVELTY_EV_100_EXP << ". (novelty) run 100 xor network evolution experiments?" << std::endl;
		std::cout << "\t" << NOVELTY_EV_STATS_EXP << ". (novelty) run 1000 xor network evolution experiments and record in a csv file?" << std::endl;

		std::cout << "\t" << EX_XOR_NET << ". run the example xor network?" << std::endl;
		std::cout << "\t" << RT_XOR_EXP << ". run one detailed xor network evolution experiment with rtNEAT?" << std::endl;
		std::cout << "\t" << RAND_EVO << ". run a random evolution (random fitness at each generation)?" << std::endl;
		std::cout << "\t" << GEN_MUT_CROSS << ". run various mutations and crossover on simple genomes?" << std::endl;
		std::cout << "\t" << SERDES << ". run the serialization tests?" << std::endl;
		std::cout << "\t" << NOVELTY_TESTS << ". run the novelty tests?" << std::endl;

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

		case XOR_EV_EXP:
			run_one_xor_experiment(false);
			break;
		case XOR_EV_100_EXP:
			run_n_xor_experiments(100, false, false);
			break;
		case XOR_EV_STATS_EXP:
			run_n_xor_experiments(1000, false, true, "1000_xor_results.csv");
			break;

		case NOVELTY_EV_EXP:
			run_one_xor_experiment(true);
			break;
		case NOVELTY_EV_100_EXP:
			run_n_xor_experiments(100, true, false);
			break;
		case NOVELTY_EV_STATS_EXP:
			run_n_xor_experiments(1000, true, true, "1000_xor_results_novelty.csv");
			break;

		case EX_XOR_NET:
			xor_network_test();
			break;
		case RT_XOR_EXP:
			run_one_real_time_xor_experiment(true);
			break;
		case RAND_EVO:
			run_random_evolution();
			break;
		case GEN_MUT_CROSS:
			run_genome_mutations_crossovers();
			break;
		case SERDES:
			run_serialization_tests();
			break;
		case NOVELTY_TESTS:
			run_novelty_tests();
			break;

		case COFFEE:
			std::cout << "I hope you will find one then." << std::endl;
			break;
		default:
			std::cout << "Well, try to choose something I proposed, OK?" << std::endl;
		}
	}
}
