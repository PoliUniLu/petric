#ifndef __CORA_PETRIC__
#define __CORA_PETRIC__

#include <set>
#include <vector>

namespace cora {
namespace petric {

struct PetricResult {
    // Indices of essential implicants.
    std::set<int> essential_implicant_idx;
    // All minimal sum-of-product solutions.
    // Each sum is represented by set of indices of prime implicants.
    std::vector<std::set<int>> sums_of_products;
};

PetricResult petric(const std::vector<std::set<int>>& pi_coverage,
                    const bool single_result);

} // namespace petric
} // namespace cora

#endif // __CORA_PETRIC__