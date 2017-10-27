#include "netkit/neat/population.h"
#include "netkit/neat/base_neat.h"

netkit::population::population(base_neat* neat_instance) : base_population(neat_instance) {}

netkit::population::population(population&& other) noexcept : base_population(std::move(other)) {}

netkit::population& netkit::population::operator=(population&& other) noexcept {
	base_population::operator=(std::move(other));
	return *this;
}

netkit::genome_id_t netkit::population::add_genome(genome geno) {
	m_all_genomes.push_back(std::move(geno));
	return m_all_genomes.size() - 1;
}

void netkit::population::set_genomes(const std::vector<genome>& genomes) {
	m_all_genomes.clear();
	m_all_genomes.reserve(genomes.size());
	for (const genome& geno : genomes) {
		m_all_genomes.push_back(geno);
	}
}

void netkit::population::set_genomes(std::vector<genome>&& genomes) {
	m_all_genomes = std::move(genomes);
}

void netkit::population::replace_genome(genome_id_t id, genome geno) {
	m_all_genomes[id] = std::move(geno);
}

netkit::serializer& netkit::operator<<(serializer& ser, const population& pop) {
	ser.append(pop.m_all_genomes.size());
	ser.new_line();
	for (const genome& g : pop.m_all_genomes) {
		ser << g;
	}

	return ser;
}

netkit::deserializer& netkit::operator>>(deserializer& des, population& pop) {
	pop.clear();
	size_t number_of_genomes;
	des.get_next(number_of_genomes);
	for (size_t i = 0; i < number_of_genomes; ++i) {
		genome g(pop.m_neat);
		des >> g;
		pop.add_genome(std::move(g));
	}

	return des;
}
