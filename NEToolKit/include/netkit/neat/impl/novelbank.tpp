#include <queue>
#include <algorithm> // std::min

#include "netkit/neat/novelbank.h"

namespace {
	template<typename pos_t>
	class novel_distance_comparator {
	public:
		explicit novel_distance_comparator(pos_t reference, const bool &greater_first = false)
		  : m_greater(greater_first), m_reference(std::move(reference)) {}

		bool operator()(const pos_t& lhs, const pos_t& rhs) const {
			if (m_greater) return (m_reference.novelty_distance(lhs) < m_reference.novelty_distance(rhs));
			else return (m_reference.novelty_distance(lhs) > m_reference.novelty_distance(rhs));
		}

	private:
		bool m_greater;
		pos_t m_reference;
	};
}

template<typename pos_t>
netkit::novelbank<pos_t>::novelbank(size_t max_size, double min_threshold, unsigned int nb_neighbours)
	: m_max_size(max_size)
	, m_min_threshold(min_threshold)
	, m_nb_neighbours(nb_neighbours)
	, m_bank()
    , m_pop() {}

template<typename pos_t>
netkit::novelbank<pos_t>::novelbank(novelbank<pos_t>&& other) noexcept
	: m_max_size(other.m_max_size)
	, m_min_threshold(other.m_min_threshold)
	, m_nb_neighbours(other.m_nb_neighbours)
	, m_bank(std::move(other.m_bank))
	, m_pop(std::move(other.m_pop)) {}

template<typename pos_t>
netkit::novelbank<pos_t>& netkit::novelbank<pos_t>::operator=(novelbank<pos_t>&& other) noexcept {
	m_max_size = other.m_max_size;
	m_min_threshold = other.m_min_threshold;
	m_nb_neighbours = other.m_nb_neighbours;
	m_bank = std::move(other.m_bank);
	m_pop = std::move(other.m_pop);

	return *this;
}

template<typename pos_t>
double netkit::novelbank<pos_t>::evaluate(const novelgenome<pos_t>& ng) {
	std::priority_queue<pos_t, std::vector<pos_t>, novel_distance_comparator<pos_t>>
		pq{novel_distance_comparator<pos_t>(ng.get_pos(), true)};

	for (pos_t& pos : m_bank) {
		pq.push(pos);

		if (pq.size() > m_nb_neighbours) {
			pq.pop();
		}
	}

	for (novelgenome<pos_t>& other : m_pop) {
		if (ng.get_genome_id() != other.get_genome_id()) {
			pq.push(other.get_pos());

			if (pq.size() > m_nb_neighbours) {
				pq.pop();
			}
		}
	}

	double sparseness = 0.0;
	while (!pq.empty()) {
		sparseness += ng.get_pos().novelty_distance(pq.top());
		pq.pop();
	}
	sparseness /= m_nb_neighbours;

	// add highly novel elements to the novelty bank buffer for later usage.
	if (sparseness >= m_min_threshold) {
		m_bank_buffer.push_back(ng.get_pos());
	}

	return sparseness;
}

template<typename pos_t>
void netkit::novelbank<pos_t>::pop_register(novelgenome<pos_t> ng) {
	m_pop.push_back(std::move(ng));
}

template<typename pos_t>
void netkit::novelbank<pos_t>::pop_clear() {
	m_pop.clear();
}

template<typename pos_t>
void netkit::novelbank<pos_t>::bank_update() {
	if (m_bank.size() + m_bank_buffer.size() > m_max_size) {
		// get rid of the oldest records.
		size_t excess = m_bank.size() + m_bank_buffer.size() - m_max_size;
		for (size_t i = excess; i--;) {
			m_bank.pop_front();
		}
	}

	// update the bank from the buffer.
	for (pos_t& p : m_bank_buffer) {
		m_bank.push_back(std::move(p));
	}
	m_bank_buffer.clear();
}

template<typename pos_t>
void netkit::novelbank<pos_t>::bank_clear() {
	m_bank.clear();
}

template<typename pos_t>
netkit::serializer& netkit::operator<<(serializer& ser, const novelbank<pos_t>& g) {
	ser.append(g.m_max_size);
	ser.append(g.m_min_threshold);
	ser.append(g.m_nb_neighbours);
	ser.new_line();

	ser.append(g.m_bank.size());
	ser.new_line();
	for (const pos_t& p : g.m_bank) {
		ser << p;
	}

	ser.append(g.m_bank_buffer.size());
	ser.new_line();
	for (const pos_t& p : g.m_bank_buffer) {
		ser << p;
	}

	ser.append(g.m_pop.size());
	ser.new_line();
	for (const auto& ng : g.m_pop) {
		ser << ng;
	}

	return ser;
}

template<typename pos_t>
netkit::deserializer& netkit::operator>>(netkit::deserializer& des, novelbank<pos_t>& g) {
	g.m_bank.clear();
	g.m_bank_buffer.clear();
	g.m_pop.clear();

	des.get_next(g.m_max_size);
	des.get_next(g.m_min_threshold);
	des.get_next(g.m_nb_neighbours);

	size_t bank_size;
	des.get_next(bank_size);
	for (size_t i = 0; i < bank_size; ++i) {
		pos_t p;
		des >> p;
		g.m_bank.push_back(std::move(p));
	}

	size_t bank_buffer_size;
	des.get_next(bank_buffer_size);
	for (size_t i = 0; i < bank_buffer_size; ++i) {
		pos_t p;
		des >> p;
		g.m_bank_buffer.push_back(std::move(p));
	}

	size_t pop_size;
	des.get_next(pop_size);
	for (size_t i = 0; i < pop_size; ++i) {
		novelgenome<pos_t> ng(0);
		des >> ng;
		g.m_pop.push_back(std::move(ng));
	}

	return des;
}
