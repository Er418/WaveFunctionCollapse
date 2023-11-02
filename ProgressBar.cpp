//
// Created by Egor Dubrouski on 07.03.23.
//

#include "ProgressBar.h"

ProgressBar::ProgressBar() {
    start_time_ = std::chrono::steady_clock::now();
}

ProgressBar::ProgressBar(double end_value) : ProgressBar() {
    end_value_ = end_value;
}

ProgressBar::ProgressBar(double start_value, double end_value) : ProgressBar(end_value) {
    start_value_ = start_value;
    current_value_ = start_value;
}

void ProgressBar::Update(double new_value, bool backups) {
    current_value_ = new_value;
    std::string ProgressBarText = GetProgressBar(backups);
    std::cout << "\r" << ProgressBarText;
    std::cout.flush();
}

std::chrono::steady_clock::duration ProgressBar::ElapsedTime() {
    return std::chrono::steady_clock::now() - start_time_;
}

int64_t ProgressBar::EstimatedTimeLeft() {
    double progress = (current_value_ - start_value_) / (end_value_ - current_value_);
    int64_t milliseconds_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(ElapsedTime()).count();
    if (progress > 0.000001) {
        return std::round(milliseconds_elapsed / progress);
    } else {
        return LLONG_MAX;
    }
}

std::string ProgressBar::GetTimeLeft() {
    int64_t seconds_left = EstimatedTimeLeft() / 1000;
    std::string ret;
    if (seconds_left >= 3600) {
        ret += std::to_string(seconds_left / 3600) + "h ";
        seconds_left %= 3600;
    }
    if (seconds_left >= 60) {
        ret += std::to_string(seconds_left / 60) + "m ";
        seconds_left %= 60;
    }
    ret += std::to_string(seconds_left) + "s";
    return ret;
}

std::string ProgressBar::GetTimeUsed() {
    int64_t seconds_used = std::chrono::duration_cast<std::chrono::seconds>(ElapsedTime()).count();
    std::string ret;
    if (seconds_used >= 3600) {
        ret += std::to_string(seconds_used / 3600) + "h ";
        seconds_used %= 3600;
    }
    if (seconds_used >= 60) {
        ret += std::to_string(seconds_used / 60) + "m ";
        seconds_used %= 60;
    }
    ret += std::to_string(seconds_used) + "s";
    return ret;
}

std::string ProgressBar::GetBar() {
    double progress = (current_value_ - start_value_) / (end_value_ - start_value_);
    size_t filled_segments = std::floor(progress * bar_length);
    std::string ret;
    for (size_t i = 0; i < filled_segments; i++) {
        ret += "|";
    }
    for (size_t i = filled_segments; i < bar_length; i++) {
        ret += ".";
    }
    return ret;
}

std::string ProgressBar::GetProgressBar(bool backups) {
    std::string ret;
    ret += "[" + GetBar() + "]";
    double progress = (current_value_ - start_value_) / (end_value_ - start_value_);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << progress * 100;
    std::string prog_str = oss.str();
    ret += " " + prog_str + "% | Time left: " + GetTimeLeft();
    if (backups) {
        ret += " | Reverted to backup " + std::to_string(reverts) + " time(s)";
    }
    return ret;
}

void ProgressBar::AddRevert() {
    reverts++;
}

void ProgressBar::Finish(bool backups) {
    std::string progerss_bar_text;
    progerss_bar_text += "[" + GetBar() + "]";
    double progress = 1;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << progress * 100;
    std::string prog_str = oss.str();
    progerss_bar_text += " " + prog_str + "% | Time : " + GetTimeUsed();
    if (backups) {
        progerss_bar_text += " | Reverted to backup " + std::to_string(reverts) + " time(s)";
    }
    std::cout << "\r" << progerss_bar_text;
    std::cout.flush();
    std::cout << std::endl;
}




