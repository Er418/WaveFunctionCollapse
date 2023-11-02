#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Egor Dubrouski on 06.03.23.
//

#include "WaveFunctionCollapse.h"


const std::vector<WaveFunctionCollapse::Direction> WaveFunctionCollapse::Adjacent = {Up, Left, Right, Down};
const std::map<WaveFunctionCollapse::Direction, std::pair<int, int>>
        WaveFunctionCollapse::CordChange = {{Up,    {-1, 0}},
                                            {Left,  {0,  -1}},
                                            {Right, {0,  1}},
                                            {Down,  {1,  0}}};
std::mt19937 WaveFunctionCollapse::rng = std::mt19937(34562357890);

WaveFunctionCollapse::WaveFunctionCollapse(const std::vector<std::vector<int>> &pattern, size_t n, size_t m, size_t tile_size, bool borders, std::string name) {
    TILE_SIZE = tile_size;
    Tiles tiles = Tiles(pattern, TILE_SIZE);
    Patterns patterns = Patterns(tiles, borders);
    progress_bar_ = ProgressBar((n - TILE_SIZE + 1) * (m - TILE_SIZE + 1), 0);
    wave_ = Wave(n, m, tiles.GetTileCount(), patterns, TILE_SIZE, name);
}

std::vector<std::vector<int>> WaveFunctionCollapse::Collapse() {
    bool check = true;
    while(check) {
        int buf = wave_.CollapseLowestEntropy();
        if(buf < 0) {
            progress_bar_.AddRevert();
            buf *= -1;
        }
        progress_bar_.Update(buf, true);
        check = bool(buf);
    }
    progress_bar_.Finish(true);
    return wave_.GetCollapsedState();
}


#pragma clang diagnostic pop