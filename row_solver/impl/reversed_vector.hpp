#pragma once
#include <type_traits>
#include <vector>

namespace impl
{
template <class T> class ReversedVectorConst
{
public:
	ReversedVectorConst(const std::vector<T> &data) : : m_data_ptr_const(&data) {}
	ReversedVectorConst() {}

	typename std::vector<T>::const_reference operator[](size_t index) const
	{
		return *(m_data_ptr_const->rbegin() + index);
	}

	size_t size() const { return m_data_ptr_const->size(); }

protected:
	const std::vector<T> *m_data_ptr_const;
};

template <class T> class ReversedVector : public ReversedVectorConst<T>
{
public:
	ReversedVector(std::vector<T> &data) : m_data_ptr(&data) {}
	ReversedVector() {}

	typename std::vector<T>::reference operator[](size_t index)
	{
		return *(m_data_ptr->rbegin() + index);
	}

	using ReversedVectorConst::operator[];

	void resize(size_t new_size) { m_data_ptr->resize(new_size); }

	void set_data_ptr(std::vector<T> *data_ptr)
	{
		m_data_ptr       = data_ptr;
		m_data_ptr_const = data_ptr;
	}

protected:
	std::vector<T> *m_data_ptr;
};

} // namespace impl

template <class T>
class ReversedVector : public std::conditional_t<std::is_const_v<T>, impl::ReversedVectorConst<T>,
                                                 impl::ReversedVector<T>>
{
public:
	explicit ReversedVector(std::vector<T> &data) { set_data_ptr(&data); }

	ReversedVector() { set_data_ptr(&m_data); }

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

	std::vector<T> move_out()
	{
		auto old_ptr = m_data_ptr;
		set_data_ptr(&m_data);
		return std::move(*old_ptr);
	}

private:
	std::vector<T> m_data;
};