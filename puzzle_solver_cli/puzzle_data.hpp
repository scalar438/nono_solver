#pragma once
#include <cstdint>
#include <exception>
#include <optional>
#include <vector>

class Color
{
public:
	Color(uint_fast8_t r, uint_fast8_t g, uint_fast8_t b);

	uint_fast8_t R() const;
	uint_fast8_t G() const;
	uint_fast8_t B() const;

	std::strong_ordering operator<=>(const Color &) const = default;

private:
	uint_fast32_t m_color;
};

class BlockData
{
public:
	BlockData(uint_fast32_t len, Color col) : m_len(len), m_color(col) {}

	uint_fast32_t len() const
	{
		return m_len;
	}

	Color color() const
	{
		return m_color;
	}

private:
	uint_fast32_t m_len;
	Color m_color;
};

class PuzzleData
{
public:
	PuzzleData(int r, int c, std::vector<std::vector<BlockData>> row_clues,
	           std::vector<std::vector<BlockData>> col_clues,
	           std::optional<Color> background_color);

	int rows() const;
	int cols() const;

	const std::vector<BlockData> &row_clue(int index) const;
	const std::vector<BlockData> &col_clue(int index) const;

	Color get_color(int index) const;

	std::optional<Color> background_color(int index) const;

private:
	int m_rows;
	int m_cols;
	std::vector<std::vector<BlockData>> m_row_clues;
	std::vector<std::vector<BlockData>> m_col_clues;

	std::vector<Color> m_colors;
	std::optional<Color> m_background_color;
};

class InvalidPuzzleException : public std::exception
{
public:
	InvalidPuzzleException() {}
};