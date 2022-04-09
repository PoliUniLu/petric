#include "petric.h"

#include <algorithm>
#include <cassert>
//#include <iostream>
#include <map>
#include <numeric>

namespace cora {
namespace petric {
namespace {

/*std::ostream& operator <<(std::ostream& os, const std::vector<int>& v) {
    for (int x : v) {
        os << x;
    }
    return os;
}

std::ostream& operator <<(std::ostream& os, const std::set<std::vector<int>>& s) {
    bool first = true;
    os << "(";
    for (const auto& v : s) {
        if (!first) {
            os << "+";
        } else {
            first = false;
        }
        os << v;
    }
    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, 
    const std::vector<std::set<std::vector<int>>>& i) {
    for (const auto& s : i) {
        os << s;
    }
    return os;
}*/


std::map<int, std::set<int>> createRowIdxToImplIdxMap(
    const std::vector<std::set<int>>& imp_coverage) {
    // Map mapping covered row index to set of indices of 
    // implicants covering it.
    std::map<int, std::set<int>> row_to_implicant_idx;
    for (unsigned int imp_idx = 0; imp_idx < imp_coverage.size(); ++imp_idx ) {
        for (const auto cov_idx : imp_coverage[imp_idx]) {
            row_to_implicant_idx[cov_idx].insert(imp_idx);
        }
    }
    return row_to_implicant_idx;
}

std::set<int> findEssentialImplicants(
    const std::map<int, std::set<int>>& row_to_impl_map) {  
    std::set<int> essential_impl_idx;
    for (const auto& row_with_coverage : row_to_impl_map) {
        if (row_with_coverage.second.size() == 1) {
            essential_impl_idx.insert((*row_with_coverage.second.begin()));
        }
    }
    return essential_impl_idx;
}

std::set<std::vector<int>> booleanMultiply(const std::set<std::vector<int>> &x,
                                        const std::set<std::vector<int>> &y) {
    std::set<std::vector<int>> res;
    //std::cout << "Multiplying " << x << y << "  = ";

    for (const std::vector<int>& x_i : x) {
        for (const std::vector<int>& y_i : y) {
            std::vector<int> tmp;
            std::set_union(x_i.begin(), x_i.end(),
                           y_i.begin(), y_i.end(),
                           std::back_inserter(tmp));

            // X+XY=X
            // If there's a superset of tmp in result,
            // we want to remove it.
            // std::remove_if(res.begin(), res.end(),
            //     [&](const std::vector<int>& z) {
            //         return std::includes(z.begin(), z.end(),
            //                              tmp.begin(), tmp.end());
            //     }
            // );
            for (auto it = res.begin(); it != res.end(); ) {
                if (std::includes(it->begin(), it->end(),
                                  tmp.begin(), tmp.end())) {
                    it = res.erase(it);
                } else {
                    ++it;
                }
            }

            // If tmp is superset of existing term in res,
            // we don't add it.
            if (std::all_of(res.begin(), res.end(), 
                    [&](const std::vector<int>& z) {
                        return !std::includes(tmp.begin(), tmp.end(),
                                                z.begin(), z.end());
                    })) {
                        res.insert(tmp);
                    }            
        }
    }
    //std::cout << res << std::endl;
    return res;
}

std::vector<std::set<std::vector<int>>> createMultiplicationInput(
    const std::map<int, std::set<int>>& row_to_impl_map) {
    std::vector<std::set<std::vector<int>>> res;

    for (const auto& row_item : row_to_impl_map) {
        std::set<std::vector<int>> tmp;
        std::transform(row_item.second.begin(),
                       row_item.second.end(),
                       std::inserter(tmp,tmp.end()),
                        [] (const int x) -> std::vector<int> {
                            return std::vector<int>{x};
                       });
        assert(res.size() > 0);
        res.emplace_back(std::move(tmp));
    }


    return res;
}

} // namespace

PetricResult petric(const std::vector<std::set<int>>& pi_coverage) {
    std::map<int, std::set<int>> row_to_impl_map = 
        createRowIdxToImplIdxMap(pi_coverage);

    PetricResult result;

    // Essential implicants
    result.essential_implicant_idx = 
        findEssentialImplicants(row_to_impl_map);

    // Petric multiplication.
    std::vector<std::set<std::vector<int>>> multIn = 
        createMultiplicationInput(row_to_impl_map);

    if (multIn.empty()) {
        return result;
    }

    //std::cout << "About to reduce " <<  multIn << std::endl;

    std::set<std::vector<int>> res = 
        multIn.size() > 1 ? std::reduce(++multIn.begin(), multIn.end(),
                                        multIn[0], booleanMultiply) : multIn[0];
    
    //std::cout << "Reduced to " << res.size() << std::endl; 

    for (const auto& pi : res) {
        result.sums_of_products.emplace_back(pi.begin(), pi.end());
    }
    return result;
}

} // namespace petric
} // namespace cora
