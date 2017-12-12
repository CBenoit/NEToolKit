#pragma once

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "neat_primitive_types.h"

namespace netkit {
// wrapper over a genome for the novelty metrics.
// pos_t is a type representing a novelty position to be compared to others.
// The class provided should have a method called novelty_distance taking another pos_t and return the novelty distance as a double.
// For the novelgenome to be serializable, the pos_t type should be serializable (with the NEToolKit serialization system)
template<typename pos_t>
class novelgenome {
  public:
	explicit novelgenome(genome_id_t genome_id);
	novelgenome(genome_id_t genome_id, pos_t pos);
	novelgenome(const novelgenome<pos_t>& other) = default;
	novelgenome& operator=(const novelgenome<pos_t>& other) = default;
	novelgenome(novelgenome<pos_t>&& other) noexcept;
	novelgenome& operator=(novelgenome<pos_t>&& other) noexcept;

	void set_pos(pos_t pos) { m_pos = std::move(pos); }
	const pos_t& get_pos() const { return m_pos; }
	pos_t& get_pos() { return m_pos; }

	genome_id_t get_genome_id() const { return m_genome_id; }

	double distance(const novelgenome<pos_t>& other) const;

  private:
	genome_id_t m_genome_id;
	pos_t m_pos;

	template<typename pos_t_>
	friend serializer& operator<<(serializer& ser, const novelgenome<pos_t_>& rg);
	template<typename pos_t_>
	friend deserializer& operator>>(deserializer& des, novelgenome<pos_t_>& rg);
};

template<typename pos_t>
serializer& operator<<(serializer& ser, const novelgenome<pos_t>& ng);
template<typename pos_t>
deserializer& operator>>(deserializer& des, novelgenome<pos_t>& ng);
}

#include "impl/novelgenome.tpp"
