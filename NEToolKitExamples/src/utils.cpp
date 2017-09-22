#include <limits>
#include <iostream>

#include "utils.h"

void wait_user() {
	std::cout << "Press Enter to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
