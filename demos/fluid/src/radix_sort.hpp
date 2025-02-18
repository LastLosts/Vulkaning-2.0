#include "glm/exponential.hpp"
#include <cstdint>
#include <vector>

constexpr uint32_t max_digits = 2;


static uint32_t get_digit(uint32_t val, uint32_t dig)
{
    uint32_t powten = glm::pow(10, dig);
    return (val % powten) / (powten / 10);
}

// Used as refernece
static void radix_sort(std::vector<uint32_t> &arr)
{
    for (uint32_t j = 1; j <= max_digits; ++j)
    {
        uint32_t digits[10]{};

        // Count digits
        for (size_t i = 0; i < arr.size(); ++i)
        {
            digits[get_digit(arr[i], j)]++;
        }
        // Accumulate
        for (uint32_t i = 1; i < 10; ++i)
        {
            digits[i] = digits[i] + digits[i - 1];
        }
        // Shift
        for (uint32_t i = 9; i > 0; --i)
        {
            digits[i] = digits[i - 1];
        }
        digits[0] = 0;

        std::vector<uint32_t> copy{arr};
        for (size_t i = 0; i < arr.size(); ++i)
        {
            copy[digits[get_digit(arr[i], j)]] = arr[i];
            digits[get_digit(arr[i], j)]++;
        }
        arr = std::move(copy);
    }
}

