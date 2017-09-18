#include "netkit/neat/population.h"

std::optional<const netkit::specie&> netkit::population::find_appropriate_specie_for(const genome& geno) const {
	for (const specie& spec : m_all_species) {
		if (geno.is_compatible_with(*spec.get_representant())) {
			return spec;
		}
	}

	return {};
}
