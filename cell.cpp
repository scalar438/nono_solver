#include "cell.hpp"

namespace
{

// Calculate position of enabled bit.
constexpr int get_bit_number(uint32_t val)
{
	constexpr int MultiplyDeBruijnBitPosition[32] = {0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
	                                                 15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
	                                                 16, 7,  26, 12, 18, 6,  11, 5,  10, 9};
	return MultiplyDeBruijnBitPosition[(val * 0x077CB531U) >> 27u];
}

constexpr bool test_get_bit_number()
{
	for (uint32_t i = 0; i < 32; ++i)
	{
		if (get_bit_number(1u << i) != i) return false;
	}
	return true;
}

} // namespace

Cell::Cell(int max_colors) : m_max_colors(max_colors)
{
	// TODO: replace it to actual exception
	if (max_colors > MAX_COLORS) throw 42;
	m_data = (1u << (max_colors + 1)) - 1u;
}

std::optional<int> Cell::get_color() const
{
	if (m_data == 0 || (m_data & (m_data - 1)) != 0) return std::nullopt;
	static_assert(test_get_bit_number());
	return get_bit_number(m_data);
}

bool Cell::is_color_possible(int color_number) const
{
	return (m_data & (1u << color_number)) != 0;
}

bool Cell::is_impossible() const
{
	return m_data == 0;
}

void Cell::set_color_possible(int color_number, bool possible)
{
	if (possible)
		m_data |= (1u << color_number);
	else
		m_data &= ~(1u << color_number);
}

int Cell::max_colors() const
{
	return m_max_colors;
}
