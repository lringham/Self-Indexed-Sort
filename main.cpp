#include <algorithm>
#include <vector>
#include <random>
#include <iterator>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <assert.h> 

using std::vector;
using milli = std::chrono::milliseconds;
using timer = std::chrono::high_resolution_clock;

void print(const vector<int>& numbers)
{
	std::copy(begin(numbers), end(numbers), std::ostream_iterator<int>(std::cout, " "));
	std::cout << '\n';
}

vector<int> generateNumbers(const unsigned COUNT, const int RANGE_START, const int RANGE_END)
{
	std::cout << "Generating " << COUNT << " random numbers from [" << RANGE_START << ", " << RANGE_END << "]...\n";

	vector<int> numbers(COUNT);
	std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<int> dist(RANGE_START, RANGE_END);
	std::generate(begin(numbers), end(numbers), [&]()
	{
		return dist(gen);
	});

	return numbers;
}

vector<int> quickSort(vector<int> numbers)
{
	auto start = timer::now();
	std::sort(begin(numbers), end(numbers));
	std::cout << std::setfill(' ') << std::setw(10)
		<< std::chrono::duration_cast<milli>(timer::now() - start).count()
		<< " ms - quickSort\n";
	return numbers;
}

vector<int> selfIndexSort(vector<int> numbers)
{
	auto start = timer::now();

	// Initialize Sort Space
	int biggest_num = std::numeric_limits<int>::min(), smallest_num = std::numeric_limits<int>::max();
	for (int num : numbers)
	{
		if (num < smallest_num)
			smallest_num = num;
		if (num > biggest_num)
			biggest_num = num;
	}
	const unsigned SS_COUNT = biggest_num - smallest_num + 1;
	auto start_prefactored = timer::now();
	vector<int> sort_space(SS_COUNT, 0);

	// Self-indexed arrangement	
	for (int num : numbers)
		sort_space[num - smallest_num]++;

	// Order-preserved compression
	{
		unsigned i = 0;
		for (unsigned j = 0; j < SS_COUNT; ++j)
			while (sort_space[j] > 0)
			{
				numbers[i++] = j + smallest_num;
				sort_space[j]--;
			}
	}

	// Print out timing information
	auto end = timer::now();
	std::cout << std::setfill(' ') << std::setw(10)
		<< std::chrono::duration_cast<milli>(end - start).count()
		<< " ms - selfIndexSort\n";

	std::cout << std::setfill(' ') << std::setw(10)
		<< std::chrono::duration_cast<milli>(end - start_prefactored).count()
		<< " ms - selfIndexSort (prefactored)\n";

	return numbers;
}

void test(const unsigned COUNT, const int RANGE_START, const int RANGE_END)
{
	vector<int> unsorted_numbers = generateNumbers(COUNT, RANGE_START, RANGE_END);
	int biggest_num = std::numeric_limits<int>::min(), smallest_num = std::numeric_limits<int>::max();
	for (int num : unsorted_numbers)
	{
		if (num < smallest_num)
			smallest_num = num;
		if (num > biggest_num)
			biggest_num = num;
	}

	std::cout << "Sorting...\n";
	selfIndexSort(unsorted_numbers);
	quickSort(unsorted_numbers);

	// Sanity check
	//vector<int> sorted_sis_numbers = selfIndexSort(unsorted_numbers);
	//vector<int> sorted_qs_numbers = quickSort(unsorted_numbers);

	//assert(std::is_sorted(begin(sorted_sis_numbers), end(sorted_sis_numbers)));
	//assert(std::is_sorted(begin(sorted_sis_numbers2), end(sorted_sis_numbers2)));
	//for (unsigned i = 0; i < COUNT; ++i)
	//	assert(sorted_sis_numbers[i] == sorted_qs_numbers[i]);
}

int main()
{
	// Test increasing number count
	test(1'000'000, 0, 1'000'000);
	test(2'000'000, 0, 1'000'000);
	test(3'000'000, 0, 1'000'000);
	test(4'000'000, 0, 1'000'000);
	test(5'000'000, 0, 1'000'000);

	// Test performance with small gaps
	test(100'000'000, 0, 50'000);
	test(200'000'000, 0, 50'000);
	test(300'000'000, 0, 50'000);
	test(400'000'000, 0, 50'000);
	test(500'000'000, 0, 50'000);

	// Test gap size inceasing
	test(10'000, 0, 1'000'000);
	test(10'000, 0, 10'000'000);
	test(10'000, 0, 100'000'000);
	test(10'000, 0, 250'000'000);
	test(10'000, 0, 500'000'000);

	// Test a very large array and range
	test(2'000'000'000, 0, 2'000'000'000);

	std::cout << "Press enter to exit...";
	std::cin.get();
}
