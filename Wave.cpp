//
// Created by Egor Dubrouski on 07.03.23.
//

#include "WaveFunctionCollapse.h"
#pragma GCC target("avx2")
#pragma GCC optimize("O3")


WaveFunctionCollapse::Wave::Wave(size_t n, size_t m, const std::map<Tile, int> &tiles, const Patterns &patterns,
                                 size_t tile_size, std::string name) {
    TILE_SIZE = tile_size;
    data_.resize(n - TILE_SIZE + 1, std::vector<Superposition>(m - TILE_SIZE + 1));
    for (auto &i: data_) {
        for (auto &j: i) {
            j = Superposition(tiles);
        }
    }
    backups.push_back(Backup(data_, (n - TILE_SIZE + 1) * (m - TILE_SIZE + 1)));
    backup_cycle = (n - TILE_SIZE + 1) * (m - TILE_SIZE + 1) / 30;
    patterns_ = patterns;
    image_name = name;
    std::filesystem::remove_all("../images/output/" + image_name);
    std::filesystem::create_directories("../images/output/" + image_name);
    mng.ReadImage(image_name);
}

int WaveFunctionCollapse::Wave::CollapseLowestEntropy() {
    const long double eps = 1e-6;
    std::vector<std::pair<size_t, size_t>> candidates;
    long double min_entropy = 1e10;
    bool collapsed = true;
    int cnt = 0;
    int idy = 0, idx = 0;
    for (const auto &i: data_) {
        idx = 0;
        for (const auto &j: i) {
            if (!j.IsCollapsed()) {
                min_entropy = std::min(min_entropy, j.GetEntropy());
                cnt++;
                collapsed = false;
            }
            idx++;
        }
        idy++;
    }

    if (collapsed) {
        return 0;
    }
    for (size_t i = 0; i < data_.size(); i++) {
        for (size_t j = 0; j < data_[0].size(); j++) {
            if (!data_[i][j].IsCollapsed() && std::abs(min_entropy - data_[i][j].GetEntropy()) < eps) {
                candidates.emplace_back(i, j);
            }
        }
    }

    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    size_t roll = dist(rng);
    std::pair<size_t, size_t> cords = candidates[roll];
    //std::cerr << cords.first << ' ' << cords.second << std::endl;
    Collapse(cords.first, cords.second);
    if (revert_to_backup) {
        revert_to_backup = false;
        size_t backup_id = backups.size() - 1;
        data_ = backups[backup_id].data;
        int ret = -backups[backup_id].backup_time;
        if (backups.size() > 1) {
            backups.pop_back();
        }
        return ret;
    }
    if (std::abs(backups.back().backup_time - cnt) >= backup_cycle) {
        backups.push_back(Backup(data_, cnt));
    }
    WriteImage();
    return cnt;
}

void WaveFunctionCollapse::Wave::Collapse(size_t y, size_t x) {
    last_upd = std::vector<std::vector<bool>>(data_.size(), std::vector<bool>(data_[0].size(), false));
    std::deque<std::pair<size_t, size_t>> modified;
    modified.emplace_back(y, x);
    data_[y][x].Collapse();
    //PrintPreCollapsedState();
    while (!modified.empty()) {
        if (revert_to_backup) {
            return;
        }
        std::pair<size_t, size_t> cords = modified.front();
        modified.pop_front();
        if (cords.first > 0) {
            if (Update(cords.first - 1, cords.second)) {
                modified.emplace_back(cords.first - 1, cords.second);
            }
        }
        if (cords.second > 0) {
            if (Update(cords.first, cords.second - 1)) {
                modified.emplace_back(cords.first, cords.second - 1);
            }
        }
        if (cords.first < data_.size() - 1) {
            if (Update(cords.first + 1, cords.second)) {
                modified.emplace_back(cords.first + 1, cords.second);
            }
        }
        if (cords.second < data_[0].size() - 1) {
            if (Update(cords.first, cords.second + 1)) {
                modified.emplace_back(cords.first, cords.second + 1);
            }
        }
    }
}

bool WaveFunctionCollapse::Wave::Update(size_t y, size_t x) {
    if (data_[y][x].IsCollapsed() || last_upd[y][x]) {
        return false;
    }
    //last_upd[y][x] = true;
    auto y_ = static_cast<ssize_t>(y);
    auto x_ = static_cast<ssize_t>(x);
    std::unordered_map<Tile, long double, Tile::TileHash> new_coefficients;
    for (const auto &[item, _]: data_[y][x].GetOptions()) {
        long double probability = 1;
        for (auto dir: Adjacent) {

            ssize_t ny_ = y_ + CordChange.at(dir).first;
            ssize_t nx_ = x_ + CordChange.at(dir).second;
            if (ny_ >= 0 && ny_ < data_.size() && nx_ >= 0 && nx_ < data_[0].size()) {
                long double probability_ = 0;
                const Superposition &Neighbor = data_[ny_][nx_];
                for (const auto &i: patterns_.GetOptions(item, dir)) {
                    if (Neighbor.IsPossible(i)) {
                        probability_ = 1;
                        break;
                    }
                }
                probability *= probability_;
            } else {
                long double probability_ = 0;
                for (const auto &i: patterns_.GetOptions(item, dir)) {
                    if (i == Tile()) {
                        probability_ = 1;
                        break;
                    }
                }
                probability *= probability_;
            }

        }
        if (probability > 1e-8) {
            new_coefficients[item] = data_[y][x].GetProbability(item);
        }
    }
    long double sum = 0;
    for (const auto &i: new_coefficients) {
        sum += i.second;
    }
    std::vector<Tile> del;
    for (const auto &i: new_coefficients) {
        new_coefficients[i.first] = static_cast<long double>(i.second) / sum;
        if (new_coefficients[i.first] < 1e-8) {
            del.push_back(i.first);
        }
    }
    for (const auto &i: del) {
        new_coefficients.erase(i);
    }
    if (new_coefficients.empty()) {
        revert_to_backup = true;
    }
    size_t prev_size = data_[y][x].size();
    data_[y][x] = Superposition(new_coefficients);
    /*if(data_[y][x].size() == 1) {
        Collapse(y, x);
    }*/
    return (prev_size != data_[y][x].size());
}

std::vector<std::vector<int>> WaveFunctionCollapse::Wave::GetCollapsedState() {
    std::vector<std::vector<int>> ret(data_.size() + TILE_SIZE - 1,
                                      std::vector<int>(data_[0].size() + TILE_SIZE - 1, -1));
    for (size_t i = 0; i < data_.size(); i++) {
        for (size_t j = 0; j < data_[0].size(); j++) {
            auto buf = data_[i][j].Collapsed().GetData();
            for (size_t i_ = 0; i_ < TILE_SIZE; i_++) {
                for (size_t j_ = 0; j_ < TILE_SIZE; j_++) {
                    ret[i + i_][j + j_] = buf[i_][j_];
                }
            }
        }
    }
    return ret;
}


void WaveFunctionCollapse::Wave::PrintPreCollapsedState() {
    std::vector<std::vector<int>> ret(data_.size() + TILE_SIZE - 1,
                                      std::vector<int>(data_[0].size() + TILE_SIZE - 1, -1));
    for (size_t i = 0; i < data_.size(); i++) {
        for (size_t j = 0; j < data_[0].size(); j++) {
            auto buf = data_[i][j].Collapsed().GetData();
            if (buf.size() > 0) {
                for (size_t i_ = 0; i_ < TILE_SIZE; i_++) {
                    for (size_t j_ = 0; j_ < TILE_SIZE; j_++) {
                        ret[i + i_][j + j_] = buf[i_][j_];
                    }
                }
            }
        }
    }
    std::cerr << std::endl;
    for (auto &i: ret) {
        for (auto &j: i) {
            if (j == -1) {
                std::cerr << '.';
            } else {
                std::cerr << j;
            }
        }
        std::cerr << '\n';
    }
    std::cerr << std::endl;
}


std::vector<std::vector<MixedColors>> WaveFunctionCollapse::Wave::GetPreCollapsedState() {
    std::vector<std::vector<MixedColors>> ret(data_.size() + TILE_SIZE - 1,
                                              std::vector<MixedColors>(data_[0].size() + TILE_SIZE - 1));
    for (size_t i = 0; i < data_.size(); i++) {
        for (size_t j = 0; j < data_[0].size(); j++) {
            for (auto &[tile, prob]: data_[i][j].GetOptions()) {
                auto buf = tile.GetData();
                for (size_t i_ = 0; i_ < TILE_SIZE; i_++) {
                    for (size_t j_ = 0; j_ < TILE_SIZE; j_++) {
                        ret[i + i_][j + j_].AddColor(buf[i_][j_], prob);
                    }
                }
            }
        }
    }
    return ret;
}

void WaveFunctionCollapse::Wave::WriteImage() {

    auto buf = GetPreCollapsedState();
    mng.SetMixedImage(buf);
    //mng.WriteImage(image_name + "/frame" + std::to_string(image_id));
    std::thread write_thread(&ImageManager::WriteImage, mng, image_name + "/frame" + std::to_string(image_id));
    write_thread.detach();
    image_id++;
}

WaveFunctionCollapse::Wave::~Wave() {
    mng.GenerateVideo(image_name, image_id);
}
