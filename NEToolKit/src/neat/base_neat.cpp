#include <utility> // std::move
#include <stdexcept> // std::invalid_argument
#include <algorithm> // std::find, std::shuffle
#include <chrono> // std::chrono::system_clock

#include "netkit/neat/base_neat.h"

netkit::base_neat::base_neat(const parameters& params_)
	: params(params_)
	, innov_pool(this->params)
	, m_all_species()
	, m_best_genomes_library()
	, m_population(this)
	, m_next_species_id(0)
	, m_best_genome_ever(nullptr)
	, m_age_of_best_genome_ever() {
	if (this->params.number_of_outputs == 0 || this->params.number_of_inputs == 0) {
		throw std::invalid_argument("genomes needs at least one input and one output.");
	}

	m_all_species.reserve(15); // reserve some memory to store the species.
	m_best_genomes_library.reserve(params.best_genomes_library_max_size);

	long seed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
	rand_engine = std::minstd_rand0(seed);
}

netkit::base_neat::base_neat(const base_neat& other)
	: params(other.params)
	, innov_pool(other.innov_pool)
	, m_all_species(other.m_all_species)
	, m_best_genomes_library(other.m_best_genomes_library)
	, m_population(other.m_population)
	, m_next_species_id(other.m_next_species_id)
	, m_best_genome_ever(new genome{*other.m_best_genome_ever})
	, m_age_of_best_genome_ever(other.m_age_of_best_genome_ever) {
	long seed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
	rand_engine = std::minstd_rand0(seed);
}

netkit::base_neat::base_neat(base_neat&& other) noexcept
	: params(other.params)
	, innov_pool(std::move(other.innov_pool))
	, rand_engine(std::move(other.rand_engine))
	, m_all_species(std::move(other.m_all_species))
	, m_best_genomes_library(std::move(other.m_best_genomes_library))
	, m_population(std::move(other.m_population))
	, m_next_species_id(other.m_next_species_id)
	, m_best_genome_ever(other.m_best_genome_ever)
	, m_age_of_best_genome_ever(other.m_age_of_best_genome_ever) {
	other.m_best_genome_ever = nullptr;
}

void netkit::base_neat::init() {
	// produce the default initial genome.
	genome initial_genome(this);

	const neuron_id_t starting_idx_outputs = 1 + initial_genome.number_of_inputs();

	// links from the bias
	for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
		initial_genome.add_gene({innov_pool.next_innovation(), genome::BIAS_ID, starting_idx_outputs + i});
	}

	// links from the inputs
	for (neuron_id_t j = 0; j < initial_genome.number_of_inputs(); j++) {
		for (neuron_id_t i = 0; i < initial_genome.number_of_outputs(); i++) {
			initial_genome.add_gene({innov_pool.next_innovation(), j + 1, starting_idx_outputs + i});
		}
	}

	init(initial_genome);
}

void netkit::base_neat::init(const genome& initial_genome) {
	m_best_genome_ever = nullptr;
	m_age_of_best_genome_ever = 0;

	// populate with random mutations from the initial genome.
	for (size_t i = 0; i < params.initial_population_size; i++) {
		m_population.add_genome(initial_genome.get_random_mutation());
	}

	// speciate the population
	helper_speciate_all_population();
}

void netkit::base_neat::epoch() {
	// FIXME: should probably be moved elsewhere? Probable performance issue for rtNEAT which run this method often.
	// track the best genome ever produced.
	if (m_best_genome_ever == nullptr) {
		m_best_genome_ever = new genome{get_current_best_genome()};
		m_age_of_best_genome_ever = 0;
	} else {
		const genome& current_best_genome = get_current_best_genome();
		if (current_best_genome.get_fitness() > m_best_genome_ever->get_fitness()) {
			delete m_best_genome_ever;
			m_best_genome_ever = new genome{current_best_genome};
			m_age_of_best_genome_ever = 0;
		} else {
			++m_age_of_best_genome_ever;
		}
	}

	impl_epoch();
}

std::optional<netkit::species*> netkit::base_neat::find_appropriate_species_for(const genome& geno) {
	for (species& spec : m_all_species) {
		if (geno.is_compatible_with(spec.get_representant())) {
			return {&spec};
		}
	}
	return {};
}

const netkit::genome& netkit::base_neat::get_current_best_genome() const {
	double best_fitness_so_far = std::numeric_limits<double>::min();
	const genome* champion = nullptr;
	for (const genome& geno : m_population.get_all_genomes()) {
		if (geno.get_fitness() > best_fitness_so_far) {
			best_fitness_so_far = geno.get_fitness();
			champion = &geno;
		}
	}

	return *champion;
}

std::optional<netkit::genome> netkit::base_neat::get_best_genome_ever() const {
	if (m_best_genome_ever == nullptr) {
		return {};
	}

	return {*m_best_genome_ever};
}

void netkit::base_neat::helper_speciate_all_population() {
	for (genome_id_t geno_id = 0; geno_id < m_population.size(); ++geno_id) {
		helper_speciate_one_genome(geno_id);
	}
}

void netkit::base_neat::helper_speciate_one_genome(genome_id_t geno_id) {
	std::optional<species*> opt_species = find_appropriate_species_for(m_population[geno_id]);
	if (opt_species.has_value()) {
		opt_species.value()->add_member(geno_id);
	} else {
		m_all_species.emplace_back(this, &m_population, m_next_species_id++, m_population[geno_id]);
		m_all_species.back().add_member(geno_id);
	}
}

void netkit::base_neat::helper_update_best_genomes_library_with(const genome& geno) {
	if (std::find(m_best_genomes_library.begin(), m_best_genomes_library.end(), geno) != m_best_genomes_library.end()) {
		return; // already in the library.
	}

	if (m_best_genomes_library.size() < params.best_genomes_library_max_size) {
		m_best_genomes_library.emplace_back(geno);
	} else {
		auto worst = std::min_element(m_best_genomes_library.begin(), m_best_genomes_library.end(), [&geno](const genome & g1,
		const genome & g2) {
			return g1.get_fitness() < g2.get_fitness();
		});

		if (worst->get_fitness() < geno.get_fitness()) {
			*worst = geno;
		}
	}
}

std::optional<netkit::genome> netkit::base_neat::helper_get_genome_from_best_genome_library() {
	if (m_best_genomes_library.empty()) {
		return {};
	}

	std::uniform_int_distribution<size_t> index_selector(0, m_best_genomes_library.size() - 1);
	return {m_best_genomes_library[index_selector(rand_engine)]};
}

netkit::serializer& netkit::operator<<(serializer& ser, const base_neat& neat) {
	// serialize important values
	ser.append(neat.m_next_species_id);
	ser.append(neat.m_age_of_best_genome_ever);
	ser.append(neat.params.compatibility_threshold);

	// serialize best genome ever
	if (neat.m_best_genome_ever == nullptr) {
		ser.append(false);
		ser.new_line();
	} else {
		ser.append(true);
		ser.new_line();
		ser << *neat.m_best_genome_ever;
	}

	// serialize population
	ser << neat.m_population;

	// serialize species
	ser.append(neat.m_all_species.size());
	ser.new_line();
	for (auto& species : neat.m_all_species) {
		ser << species;
	}

	// serialize best genomes library
	ser.append(neat.m_best_genomes_library.size());
	ser.new_line();
	for (auto& geno : neat.m_best_genomes_library) {
		ser << geno;
	}

	// serialize innovation pool
	ser << neat.innov_pool;

	return ser;
}

netkit::deserializer& netkit::operator>>(deserializer& des, base_neat& neat) {
	// deserialize important values
	des.get_next(neat.m_next_species_id);
	des.get_next(neat.m_age_of_best_genome_ever);

	double compat_thres;
	des.get_next(compat_thres);
	if (neat.params.dynamic_compatibility_threshold) {
		neat.params.compatibility_threshold = compat_thres;
	}

	// deserialize best genome ever
	bool has_best_genome_ever;
	des.get_next(has_best_genome_ever);
	if (has_best_genome_ever) {
		genome best_g(&neat);
		des >> best_g;
		delete neat.m_best_genome_ever; // important!
		neat.m_best_genome_ever = new genome(std::move(best_g));
	}

	// deserialize population
	des >> neat.m_population;

	// deserialize species
	size_t number_of_species;
	des.get_next(number_of_species);
	neat.m_all_species.clear();
	for (size_t i = 0; i < number_of_species; ++i) {
		// FIXME: find a better way to handle that? Maybe with a constructor that uses the deserializer?
		// the dummy will be replaced by the right representant during deserialization.
		netkit::genome dummy(&neat);
		netkit::species species(&neat, &neat.m_population, 0, dummy);
		des >> species;
		neat.m_all_species.push_back(std::move(species));
	}

	// deserialize best genomes library
	neat.m_best_genomes_library.clear();
	size_t number_of_genomes;
	des.get_next(number_of_genomes);
	for (size_t i = 0; i < number_of_genomes; ++i) {
		genome g(&neat);
		des >> g;
		neat.m_best_genomes_library.push_back(std::move(g));
	}

	// deserialize the innovation pool
	des >> neat.innov_pool;

	return des;
}
