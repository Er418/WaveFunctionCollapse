//
// Created by Egor Dubrouski on 07.03.23.
//

#ifndef SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_PROGRESSBAR_H
#define SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_PROGRESSBAR_H
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <climits>


class ProgressBar {
private:
    double start_value_ = 0;
    double end_value_ = 0;
    double current_value_ = 0;
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
    size_t bar_length = 50;
    size_t reverts = 0;
    std::chrono::steady_clock::duration  ElapsedTime();
    int64_t EstimatedTimeLeft();
    std::string GetTimeLeft();
    std::string GetTimeUsed();
    std::string GetBar();
    std::string GetProgressBar(bool backups);
public:
    ProgressBar();
    explicit ProgressBar(double end_value);
    ProgressBar(double start_value, double end_value);

    void Update(double new_value, bool backups=false);

    void AddRevert();

    void Finish(bool backups=false);
};


#endif //SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_PROGRESSBAR_H
