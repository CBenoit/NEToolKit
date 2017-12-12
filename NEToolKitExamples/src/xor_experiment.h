#pragma once

void xor_network_test();

void run_n_xor_experiments(unsigned int number_of_experiments, bool with_novelty, bool record_csv, std::string output_filename = "default_filename.csv");

void run_one_xor_experiment(bool with_novelty);

void run_one_real_time_xor_experiment(bool display_details);
