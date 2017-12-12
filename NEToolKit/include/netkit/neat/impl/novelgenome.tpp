#include "netkit/neat/novelgenome.h"

template<typename pos_t>
netkit::novelgenome<pos_t>::novelgenome(genome_id_t genome_id)
	: m_genome_id(std::move(genome_id))
	, m_pos() {}

template<typename pos_t>
netkit::novelgenome<pos_t>::novelgenome(genome_id_t genome_id, pos_t pos)
	: m_genome_id(std::move(genome_id))
	, m_pos(pos) {}

template<typename pos_t>
netkit::novelgenome<pos_t>::novelgenome(novelgenome<pos_t>&& other) noexcept
	: m_genome_id(std::move(other.m_genome_id))
	, m_pos(std::move(other.m_pos)) {}

template<typename pos_t>
netkit::novelgenome<pos_t>& netkit::novelgenome<pos_t>::operator=(novelgenome<pos_t>&& other) noexcept {
	m_genome_id = std::move(other.m_genome_id);
	m_pos = std::move(other.m_pos);

	return *this;
}

template<typename pos_t>
double netkit::novelgenome<pos_t>::distance(const novelgenome<pos_t>& other) const {
	return m_pos.novelty_distance(other.m_pos);
}

template<typename pos_t>
netkit::serializer& netkit::operator<<(serializer& ser, const novelgenome<pos_t>& g) {
	ser.append(g.m_genome_id);
	ser.new_line();
	ser << g.m_pos;

	return ser;
}

template<typename pos_t>
netkit::deserializer& netkit::operator>>(netkit::deserializer& des, novelgenome<pos_t>& g) {
	des.get_next(g.m_genome_id);
	des >> g.m_pos;

	return des;
}
