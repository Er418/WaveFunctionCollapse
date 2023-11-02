#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Egor Dubrouski on 06.03.23.
//

#ifndef SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_WAVEFUNCTIONCOLLAPSE_H
#define SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_WAVEFUNCTIONCOLLAPSE_H

#include <cassert>
#include <cmath>
#include <deque>
#include <limits>
#include <utility>
#include <vector>
#include <thread>
#include <map>
#include <unordered_map>
#include <set>
#include <random>
#include <iostream>
#include "ProgressBar.h"
#include "ImageManager.h"





class WaveFunctionCollapse {
#TODO: add comments,
private:
    size_t TILE_SIZE = 3;
    static std::mt19937 rng;
    enum Direction {
        Up, Left, Right, Down
    };
    static const std::vector<Direction> Adjacent;
    static const std::map<Direction, std::pair<int, int>> CordChange;
    ProgressBar progress_bar_;

    class Tile {
    private:
        size_t TILE_SIZE = 3;
        std::vector<std::vector<int>> data_;
        int tile_id = -1;
    public:
        struct TileHash {
            std::size_t operator()(const Tile &k) const {
                return k.tile_id + 1;
            }
        };

        explicit Tile(size_t tile_size = 0);

        Tile(const Tile &other);

        Tile(const std::vector<std::vector<int>> &pattern, size_t y, size_t x, size_t tile_size);

        void SetId(int id);

        bool operator==(const Tile &other) const;

        bool operator<(const Tile &other) const;

        std::vector<std::vector<int>> GetData() const;
    };

    class Tiles {
    private:
        size_t TILE_SIZE;
        std::vector<std::vector<Tile>> data_;
        std::map<Tile, int> tile_ids;
    public:
        Tiles(const std::vector<std::vector<int>> &pattern, size_t tile_size);

        Tile GetTile(size_t y, size_t x) const;

        std::map<Direction, Tile> AdjacentTiles(size_t y, size_t x, bool borders) const;

        size_t GetHeight() const;

        size_t GetWidth() const;

        std::map<Tile, int> GetTileCount() const;
    };

    class Superposition {
    private:
        std::unordered_map<Tile, long double, Tile::TileHash> coefficients_;
        Tile collapsed_tile_;
        bool collapsed_ = false;
    public:
        Superposition() = default;

        Superposition(const Superposition &other);

        explicit Superposition(const std::map<Tile, int> &tiles);

        explicit Superposition(const std::unordered_map<Tile, long double, Tile::TileHash> &coefficients);

        Tile Collapse();

        Tile Collapsed() const;

        void Delete(const Tile &tile);

        long double GetEntropy() const;

        long double GetProbability(const Tile &tile) const;

        const std::unordered_map<Tile, long double, Tile::TileHash> &GetOptions() const;

        size_t size() const;

        bool IsCollapsed() const;

        bool IsPossible(const WaveFunctionCollapse::Tile &tile) const;
    };

    class Patterns {
    private:
        std::unordered_map<Tile, std::map<Direction, std::set<Tile>>, Tile::TileHash> data_;
    public:
        Patterns() = default;

        explicit Patterns(const Tiles &tiles, bool border = false);

        bool IsValid(const Tile &self, Direction dir, const Tile &adj);

        const std::set<Tile> &GetOptions(const Tile &self, Direction dir);
    };

    class Wave {
    private:
        class Backup {
        public:
            std::vector<std::vector<Superposition>> data;
            int backup_time = 0;

            Backup(std::vector<std::vector<Superposition>> data_, int time) {
                data = std::move(data_);
                backup_time = time;
            }
        };

        std::vector<std::vector<Superposition>> data_;
        std::vector<Backup> backups;
        Patterns patterns_;
        int backup_cycle = 1000;
        int image_id = 0;
        ImageManager mng;
        std::string image_name;

        int backup_2_score = 0;
        bool revert_to_backup = false;
        size_t TILE_SIZE;

        void Collapse(size_t y, size_t x);

        bool Update(size_t y, size_t x);

        std::vector<std::vector<bool>> last_upd;

    public:


        Wave() = default;

        Wave(size_t n, size_t m, const std::map<Tile, int> &tiles, const Patterns &patterns, size_t tile_size, std::string name);

        int CollapseLowestEntropy();

        std::vector<std::vector<int>> GetCollapsedState();

        void PrintPreCollapsedState();

        std::vector<std::vector<MixedColors>> GetPreCollapsedState();

        void WriteImage();

        ~Wave();
    };

    Wave wave_;
public:
    WaveFunctionCollapse(const std::vector<std::vector<int>> &pattern, size_t n, size_t m, size_t tile_size,
                         bool borders, std::string name);

    std::vector<std::vector<int>> Collapse();
};


#endif //SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_WAVEFUNCTIONCOLLAPSE_H

#pragma clang diagnostic pop