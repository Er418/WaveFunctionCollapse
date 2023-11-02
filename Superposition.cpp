//
// Created by Egor Dubrouski on 07.03.23.
//
#include "WaveFunctionCollapse.h"
#pragma GCC target("avx2")
#pragma GCC optimize("O3")


WaveFunctionCollapse::Superposition::Superposition(const WaveFunctionCollapse::Superposition &other) {
    coefficients_ = other.coefficients_;
    collapsed_tile_ = other.collapsed_tile_;
}

WaveFunctionCollapse::Superposition::Superposition(const std::map<Tile, int> &tiles) {
    long double sum = 0;
    for (const auto &i: tiles) {
        sum += i.second;
    }
    for (const auto &i: tiles) {
        coefficients_[i.first] = static_cast<long double>(i.second) / sum;
    }
}

WaveFunctionCollapse::Superposition::Superposition(
        const std::unordered_map<Tile, long double, Tile::TileHash> &coefficients) {
    coefficients_ = coefficients;
}

WaveFunctionCollapse::Tile WaveFunctionCollapse::Superposition::Collapse() {
    std::uniform_real_distribution<long double> dist(0, 0.9999999999);
    long double roll = dist(rng);
    long double cumulative_sum = 0;
    Tile ret;
    for (const auto &i: coefficients_) {
        cumulative_sum += i.second;
        if (cumulative_sum >= roll) {
            collapsed_tile_ = i.first;
            ret = collapsed_tile_;
            break;
        }
    }
    coefficients_.clear();
    coefficients_[ret] = 1;
    collapsed_ = true;
    return ret;
}

void WaveFunctionCollapse::Superposition::Delete(const WaveFunctionCollapse::Tile &tile) {
    if (coefficients_.contains(tile)) {
        long double sum = 1 - coefficients_[tile];
        coefficients_.erase(tile);
        for (auto &i: coefficients_) {
            i.second /= sum;
        }
    }
}

long double WaveFunctionCollapse::Superposition::GetEntropy() const {
    long double entropy = 0;
    for (const auto &i: coefficients_) {
        if (i.second > 1e-8) {
            entropy += i.second * std::log2(i.second);
        }
    }
    return -entropy;
}

long double WaveFunctionCollapse::Superposition::GetProbability(const WaveFunctionCollapse::Tile &tile) const {
    if (coefficients_.contains(tile)) {
        return coefficients_.at(tile);
    } else {
        return 0;
    }
}

bool WaveFunctionCollapse::Superposition::IsPossible(const WaveFunctionCollapse::Tile &tile) const {
    return coefficients_.contains(tile);
}

const std::unordered_map<WaveFunctionCollapse::Tile, long double, WaveFunctionCollapse::Tile::TileHash> &
WaveFunctionCollapse::Superposition::GetOptions() const {
    return coefficients_;
}

size_t WaveFunctionCollapse::Superposition::size() const {
    return coefficients_.size();
}

bool WaveFunctionCollapse::Superposition::IsCollapsed() const {
    return collapsed_;
}

WaveFunctionCollapse::Tile WaveFunctionCollapse::Superposition::Collapsed() const {
    return collapsed_tile_;
}



