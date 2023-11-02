//
// Created by Egor Dubrouski on 07.03.23.
//
#include "WaveFunctionCollapse.h"
#pragma GCC target("avx2")
#pragma GCC optimize("O3")


WaveFunctionCollapse::Patterns::Patterns(const WaveFunctionCollapse::Tiles &tiles, bool borders) {
    for (size_t i = 0; i < tiles.GetHeight(); i++) {
        for (size_t j = 0; j < tiles.GetWidth(); j++) {
            Tile self = tiles.GetTile(i, j);
            for (const auto &item: tiles.AdjacentTiles(i, j, borders)) {
                data_[self][item.first].insert(item.second);
                if(!borders) data_[self][item.first].insert(Tile());
            }
        }
    }
}

bool
WaveFunctionCollapse::Patterns::IsValid(const WaveFunctionCollapse::Tile &self, WaveFunctionCollapse::Direction dir,
                                        const WaveFunctionCollapse::Tile &adj) {
    if (!data_.contains(self)) {
        return false;
    }
    if (!data_[self].contains(dir)) {
        return false;
    }
    return data_[self][dir].contains(adj);
}

const std::set<WaveFunctionCollapse::Tile>&
WaveFunctionCollapse::Patterns::GetOptions(const WaveFunctionCollapse::Tile &self,
                                           WaveFunctionCollapse::Direction dir) {
    return data_[self][dir];
}