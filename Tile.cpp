//
// Created by Egor Dubrouski on 07.03.23.
//
#include "WaveFunctionCollapse.h"


WaveFunctionCollapse::Tile::Tile(size_t tile_size) {
    TILE_SIZE = tile_size;
}

WaveFunctionCollapse::Tile::Tile(const WaveFunctionCollapse::Tile &other) {
    TILE_SIZE = other.TILE_SIZE;
    data_ = other.data_;
    tile_id = other.tile_id;
}

WaveFunctionCollapse::Tile::Tile(const std::vector<std::vector<int>> &pattern, size_t y, size_t x, size_t tile_size) {
    TILE_SIZE = tile_size;
    data_.resize(TILE_SIZE, std::vector<int>(TILE_SIZE));
    assert(y + TILE_SIZE <= pattern.size());
    assert(x + TILE_SIZE <= pattern[0].size());
    for (size_t i = y; i < y + TILE_SIZE; i++) {
        for (size_t j = x; j < x + TILE_SIZE; j++) {
            data_[i - y][j - x] = pattern[i][j];
        }
    }
}

bool WaveFunctionCollapse::Tile::operator==(const WaveFunctionCollapse::Tile &other) const {
    if (tile_id == -1 || other.tile_id == -1) {
        return data_ == other.data_;
    } else {
        return tile_id == other.tile_id;
    }
}

bool WaveFunctionCollapse::Tile::operator<(const WaveFunctionCollapse::Tile &other) const {
    if (tile_id == -1 || other.tile_id == -1) {
        return std::lexicographical_compare(data_.begin(), data_.end(),
                                            other.data_.begin(), other.data_.end());
    } else {
        return tile_id < other.tile_id;
    }
}

std::vector<std::vector<int>> WaveFunctionCollapse::Tile::GetData() const {
    return data_;
}

void WaveFunctionCollapse::Tile::SetId(int id) {
    tile_id = id;
}

