#pragma once
#include <string>
#include <vector>

template <class T> class ReversedContainer
{
public:
	ReversedContainer(const T &data) : m_data(data), m_size(data.size()) {}

	typename T::value_type operator[](size_t i) const { return m_data[m_size - i - 1]; }

	size_t size() const { return m_size; }

	size_t find(typename T::value_type c) const
	{
		auto rfind_res = m_data.rfind(c);
		if (rfind_res == std::string::npos) return std::string::npos;
		return m_size - rfind_res - 1;
	}

private:
	const T &m_data;
	const size_t m_size;
};