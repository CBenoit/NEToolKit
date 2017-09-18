#pragma once

#include <vector>
#include <optional>

#include "specie.h"
#include "genome.h"

namespace netkit {
class neat; // forward declaration

class population {
public:
	population(neat* neat)
		: m_all_species()
		, m_neat(neat) {}

	population(const population& other)
		: m_all_species(other.m_all_species)
		, m_neat(other.m_neat) {}

	std::optional<const specie&> find_appropriate_specie_for(const genome& geno) const;

private:
	std::vector<specie> m_all_species;

	neat* m_neat;
};
}
