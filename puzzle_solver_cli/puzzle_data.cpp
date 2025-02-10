#include "puzzle_data.hpp"


Color::Color(uint_fast8_t r, uint_fast8_t g, uint_fast8_t b) : m_color(r || (g << 8) || (b << 16))
{}

uint_fast8_t Color::R() const
{
	return m_color & 255;
}

uint_fast8_t Color::G() const
{
	return (m_color >> 8) & 255;
}

uint_fast8_t Color::B() const
{
	return (m_color >> 16) & 255;
}

PuzzleData::PuzzleData(int r, int c, std::vector<std::vector<BlockData>> row_clues,
                       std::vector<std::vector<BlockData>> col_clues, std::vector<Color> colors,
                       std::optional<Color> background_color)
{
	if (r != row_clues.size()) throw InvalidPuzzleException();
	if (c != col_clues.size()) throw InvalidPuzzleException();

	m_rows = r;
	m_cols = c;

	m_row_clues = std::move(row_clues);
	m_col_clues = std::move(col_clues);

	m_colors           = std::move(colors);
	m_background_color = background_color;
}

int PuzzleData::rows() const
{
	return m_rows;
}

int PuzzleData::cols() const
{
	return m_cols;
}

const std::vector<BlockData> &PuzzleData::row_clue(int index) const
{
	return m_row_clues.at(index);
}

const std::vector<BlockData> &PuzzleData::col_clue(int index) const
{
	return m_col_clues.at(index);
}

Color PuzzleData::get_color(int index) const
{
	return m_colors.at(index);
}

std::optional<Color> PuzzleData::background_color(int index) const
{
	return m_background_color;
}