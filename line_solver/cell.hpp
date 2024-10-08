#pragma once
#include <cstdint>
#include <optional>

const int MAX_COLORS = 31;

class Cell
{
public:
	explicit Cell();

	/// Return 0 (background color) or color number (number from 1 to MAX_COLORS) if cell state is
	/// determined. If not, return nullopt
	std::optional<int> get_color() const;

	bool is_color_possible(uint32_t color_number) const;

	bool is_impossible() const;

	void set_impossible();

	void set_color_possible(uint32_t color_number, bool possible);

	bool operator!=(const Cell &a) const;

	bool operator==(const Cell &a) const;

	void operator&=(const Cell &a);

private:
	// Bitmask with possible colors. 0 - "impossible" cell, it doesn't appear in normal puzzles
	// Lowest bit is background (i.e. if m_data == 1 then cell must be background), all other bits
	// are corresponding colors
	uint32_t m_data;
};
