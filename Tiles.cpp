//
// Created by Egor Dubrouski on 07.03.23.
//
#include "WaveFunctionCollapse.h"


WaveFunctionCollapse::Tiles::Tiles(const std::vector<std::vector<int>> &pattern, size_t tile_size) {
    TILE_SIZE = tile_size;
    assert(pattern.size() >= TILE_SIZE);
    assert(pattern[0].size() >= TILE_SIZE);
    size_t height = pattern.size() - TILE_SIZE + 1;
    size_t width = pattern[0].size() - TILE_SIZE + 1;

    data_.resize(height, std::vector<Tile>(width));
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            data_[i][j] = Tile(pattern, i, j, TILE_SIZE);
            if (tile_ids.contains(Tile(pattern, i, j, TILE_SIZE))) {
                data_[i][j].SetId(tile_ids[Tile(pattern, i, j, TILE_SIZE)]);
            } else {
                data_[i][j].SetId((int) tile_ids.size());
                tile_ids[Tile(pattern, i, j, TILE_SIZE)] = (int) tile_ids.size();
            }
        }
    }
}

WaveFunctionCollapse::Tile WaveFunctionCollapse::Tiles::GetTile(size_t y, size_t x) const {
    return data_[y][x];
}

std::map<WaveFunctionCollapse::Direction, WaveFunctionCollapse::Tile>
WaveFunctionCollapse::Tiles::AdjacentTiles(size_t y, size_t x, bool borders) const {
    std::map<WaveFunctionCollapse::Direction, WaveFunctionCollapse::Tile> ret;
    if (y > 0) {
        ret[Direction::Up] = data_[y - 1][x];
    } else {
        ret[Direction::Up] = Tile();
    }

    if (x > 0) {
        ret[Direction::Left] = data_[y][x - 1];
    } else {
        ret[Direction::Left] = Tile();
    }

    if (y < data_.size() - 1) {
        ret[Direction::Down] = data_[y + 1][x];
    } else {
        ret[Direction::Down] = Tile();
    }

    if (x < data_[0].size() - 1) {
        ret[Direction::Right] = data_[y][x + 1];
    } else {
        ret[Direction::Right] = Tile();
    }

    return ret;
}

size_t WaveFunctionCollapse::Tiles::GetHeight() const {
    return data_.size();
}

size_t WaveFunctionCollapse::Tiles::GetWidth() const {
    return data_[0].size();
}

std::map<WaveFunctionCollapse::Tile, int> WaveFunctionCollapse::Tiles::GetTileCount() const {
    std::map<Tile, int> tile_count;
    for (const auto &i: data_) {
        for (const auto &j: i) {
            tile_count[j]++;
        }
    }
    return tile_count;
}
