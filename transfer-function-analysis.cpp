#include "llvm/ADT/APInt.h"
#include "llvm/IR/ConstantRange.h"

#include <iostream>
#include <set>
#include <vector>

using namespace llvm;

/// Custom comparator for APInt
struct APIntCompare {
    bool operator()(const APInt& lhs, const APInt& rhs) const {
        return lhs.slt(rhs);
    }
};

using APIntSet = std::set<APInt, APIntCompare>;

/// Generates all possible `ConstantRange`s for the given bitwidth
std::vector<ConstantRange> enumerateAbstractValues(size_t bitwidth) {
    std::vector<ConstantRange> ranges;

    const int64_t signed_min
            = APInt::getSignedMinValue(bitwidth).getSExtValue();
    const int64_t signed_max
            = APInt::getSignedMaxValue(bitwidth).getSExtValue();

    for (int64_t low = signed_min; low < signed_max; ++low) {
        const APInt lower_bound(bitwidth, low, true);

        for (int64_t high = low; high < signed_max; ++high) {
            const APInt upper_bound(bitwidth, high + 1, true);

            ranges.emplace_back(lower_bound, upper_bound);
        }
    }
    ranges.emplace_back(bitwidth, false);

    return ranges;
}

/// Converts an abstract `ConstantRange` of integers to a set of concrete values
APIntSet concretize(const ConstantRange& range) {
    APIntSet result;

    if (range.isEmptySet()) return result;

    for (APInt current = range.getLower(); current != range.getUpper();
         ++current) {
        result.insert(current);
    }

    return result;
}

/// Converts a concrete set of integers to an abstract `ConstantRange`
ConstantRange abstractize(const APIntSet& values, size_t bitwidth) {
    if (values.empty()) return ConstantRange(bitwidth, false);

    const APInt min = *values.begin();
    APInt max = *values.rbegin();

    if (max.isMaxSignedValue()) --max;

    if (max.getSExtValue() < min.getSExtValue())
        return ConstantRange(bitwidth, false);
    return ConstantRange(min, max + 1);
}

/// Composite (built-in) absolute-value transfer function
ConstantRange computeAbsoluteRange(const ConstantRange& range) {
    return range.abs();
}

/// Decomposed absolute-value transfer function by calling `abs` on each element
ConstantRange computeAbsoluteRangeDecomposed(const ConstantRange& range) {
    const size_t bitwidth = range.getBitWidth();

    if (range.isEmptySet()) return ConstantRange(bitwidth, false);

    APIntSet set;
    for (const auto& value : concretize(range)) {
        // If `value` is equal to -2^(bitwidth-1)
        // (i.e., minimum possible value for this bitwidth),
        // then the absolute value is not in the valid range for the bitwidth
        if (!value.isMinSignedValue()) set.insert(value.abs());
    }

    return abstractize(set, bitwidth);
}

/// Reports whether `ConstantRange` `a` is more precise than `b`
bool isMorePrecise(const ConstantRange& a, const ConstantRange& b) {
    if (a.isEmptySet() && !b.isEmptySet()) return true;
    if (!a.isEmptySet() && b.isEmptySet()) return false;

    return a.contains(b) && !b.contains(a);
}

/// Reports the results of running the tests
struct AbstractValueComparisonResult {
    unsigned int composeiteMorePreciseCount = 0;
    unsigned int decomposedMorePreciseCount = 0;
    unsigned int incomparableCount = 0;
    unsigned int total = 0;

    void print() const {
        std::cout << "Total abstract values: " << this->total << "\n"
                  << "Tests where composite fn was more precise: "
                  << this->composeiteMorePreciseCount << "\n"
                  << "Tests where decomposed fn was more precise: "
                  << this->decomposedMorePreciseCount << "\n"
                  << "Incomparable results: " << this->incomparableCount
                  << "\n";
    }
};

/// Main testing routine
AbstractValueComparisonResult runTests(const size_t bitwidth) {
    AbstractValueComparisonResult result;
    const std::vector<ConstantRange> all_ranges = enumerateAbstractValues(
            bitwidth);
    result.total = all_ranges.size();

    for (const ConstantRange& range : all_ranges) {
        const ConstantRange composite_result = computeAbsoluteRange(range);
        const ConstantRange decomposed_result = computeAbsoluteRangeDecomposed(
                range);

        // Compare results
        const APIntSet concrete_composite = concretize(composite_result);
        const APIntSet concrete_decomposed = concretize(decomposed_result);
        if (isMorePrecise(composite_result, decomposed_result))
            result.composeiteMorePreciseCount++;
        else if (isMorePrecise(decomposed_result, composite_result))
            result.decomposedMorePreciseCount++;
        else if (!composite_result.contains(decomposed_result)
                 && !decomposed_result.contains(composite_result))
            result.incomparableCount++;
    }

    return result;
}

/// Main program entry point
int main() {
    const size_t bitwidth = 5;
    const AbstractValueComparisonResult result = runTests(bitwidth);

    std::cout << "Analysis for signed integers with bitwidth " << bitwidth
              << ":\n";
    result.print();

    return 0;
}
