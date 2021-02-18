#pragma once
#include <vector>

template <class T> class ReversedVector
{
public:
	explicit ReversedVector(std::vector<T> &data) : m_data_ptr(&data) {}

	ReversedVector() { m_data_ptr = &m_data; }

	ReversedVector(ReversedVector<T> &&old) noexcept : m_data(std::move(old.m_data))
	{
		if (&old.m_data == old.m_data_ptr)
		{
			m_data_ptr = &m_data;
		}
		else
		{
			m_data_ptr = old.m_data_ptr;
		}
	}

	T &operator[](size_t index) { return *(m_data_ptr->rbegin() + index); }

	const T &operator[](size_t index) const { return *(m_data_ptr->rbegin() + index); }

	size_t size() const { return m_data_ptr->size(); }

	void resize(size_t new_size) { m_data_ptr->resize(new_size); }

private:
	std::vector<T> m_data;

	std::vector<T> *m_data_ptr;
};