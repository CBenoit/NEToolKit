#pragma once

#include <vector>
#include <deque>

#include "netkit/csv/serializer.h"
#include "netkit/csv/deserializer.h"
#include "novelgenome.h"

namespace netkit {
class base_population; // forward declaration

// /!\ the algorithm to find the closest elements by novelty distance is naive and quite slow.
// It may be best to implement a specific novelbank for the specific pos_t using, for instance, a quad-tree.
template<typename pos_t>
class novelbank {
  public:
	explicit novelbank(size_t max_size, double min_threshold, unsigned int nb_neighbours);
	novelbank(const novelbank<pos_t>& other) = default;
	novelbank& operator=(const novelbank<pos_t>& other) = default;
	novelbank(novelbank<pos_t>&& other) noexcept;
	novelbank& operator=(novelbank<pos_t>&& other) noexcept;

	double evaluate(const novelgenome<pos_t>& ng);

	void pop_register(novelgenome<pos_t> ng);
	void pop_clear();

	void bank_update();
	void bank_clear();

  private:
	size_t m_max_size;
	double m_min_threshold;
	unsigned int m_nb_neighbours;

	std::deque<pos_t> m_bank;
	std::vector<pos_t> m_bank_buffer;
	std::vector<netkit::novelgenome<pos_t>> m_pop;

	template<typename pos_t_>
	friend serializer& operator<<(serializer& ser, const novelbank<pos_t_>& rg);
	template<typename pos_t_>
	friend deserializer& operator>>(deserializer& des, novelbank<pos_t_>& rg);
};

template<typename pos_t>
serializer& operator<<(serializer& ser, const novelbank<pos_t>& ng);
template<typename pos_t>
deserializer& operator>>(deserializer& des, novelbank<pos_t>& ng);
}

#include "impl/novelbank.tpp"
