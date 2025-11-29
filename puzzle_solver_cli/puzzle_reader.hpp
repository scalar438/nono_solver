#pragma once
#include <exception>
#include <istream>
#include <puzzle_solver/puzzle_data.hpp>

class PuzzleReadException : public std::exception
{
public:
	PuzzleReadException(const std::string &arg) {}
};

PuzzleData read_puzzle(std::istream &is);