#include "CBruteForceSolver.hpp"

#include <algorithm>
#include <numeric>

using namespace TSP;

std::vector<size_t> CBruteForceSolver::solve(const Matrix& matrix, size_t root)
{
	std::vector<size_t> idxes(matrix.size());
	std::iota(idxes.begin(), idxes.end(), 0);

	auto permutation_begin_it = idxes.begin();
	auto permutation_end_it = idxes.end();

	size_t min_length{0xFFFFFFFF};
	std::vector<size_t> min_sequence;
	min_sequence.reserve(idxes.size());

	static auto calculate_length = [&matrix](const std::vector<size_t>& idxes)
	{
		size_t length{0};

		for(size_t i = 0; i < idxes.size()-1; ++i)
		{
			length += matrix[idxes[i]][idxes[i+1]];
		}

		return length;
	};

	do
	{
		size_t length = calculate_length(idxes);
		if (length < min_length)
		{
			min_length = length;
			min_sequence = idxes;
		}
	} while (std::next_permutation(permutation_begin_it, permutation_end_it));

	return min_sequence;
}