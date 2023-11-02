//
// Created by Egor Dubrouski on 06.03.23.
//

#ifndef SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_IMAGEMANAGER_H
#define SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_IMAGEMANAGER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "ProgressBar.h"

class Color {
public:
    explicit Color(char r = 0, char g = 0, char b = 0) : m_r(r), m_g(g), m_b(b) {}

    char getR() const { return m_r; }

    char getG() const { return m_g; }

    char getB() const { return m_b; }

    void setR(char r) { m_r = r; }

    void setG(char g) { m_g = g; }

    void setB(char b) { m_b = b; }

    bool operator==(const Color &other) const {
        return m_r == other.m_r && m_g == other.m_g && m_b == other.m_b;
    }

    bool operator<(const Color &other) const {
        if (m_r != other.m_r) {
            return m_r < other.m_r;
        } else if (m_g != other.m_g) {
            return m_g < other.m_g;
        } else {
            return m_b < other.m_b;
        }
    }

private:
    char m_r, m_g, m_b;
};


class MixedColors {
private:
    std::map<int, double> colors;
    std::map<int, Color> rgb_values;

    void Normalize() {
        double sum = 0;
        for (auto [_, i]: colors) {
            sum += i;
        }
        for (auto &[_, i]: colors) {
            i /= sum;
        }
    }

public:

    void AddColor(int color_id, double color_probability) {
        colors[color_id] += color_probability;
    }

    void SetColor(int color_id, Color color) {
        rgb_values[color_id] = color;
    }

    Color GetAverageColor() {
        Normalize();
        Color ret;
        for (auto &[id, k]: colors) {
            ret.setR(char(ret.getR() + int(k * (unsigned char)rgb_values[id].getR())));
            ret.setG(char(ret.getG() + int(k * (unsigned char)rgb_values[id].getG())));
            ret.setB(char(ret.getB() + int(k * (unsigned char)rgb_values[id].getB())));
        }
        return ret;
    }
};


class ImageManager {
private:


    using ImageType = std::vector<std::vector<Color>>;

    std::map<Color, int> color_map;
    std::map<int, Color> pattern_map;
    ImageType image;

    static void writeBmpFile(const std::string& filename, const std::vector<std::vector<Color>>& pixels);

    static ImageType readBmpFile(const std::string &fileName);

public:


    void ReadImage(const std::string &filename);

    void WriteImage(const std::string &filename);

    void SetMixedImage(std::vector<std::vector<MixedColors>> &image);

    std::vector<std::vector<int>> GetPattern();

    void SetPattern(const std::vector<std::vector<int>> &pattern);

    void GenerateVideo(std::string filename, int numFrames);
};


#endif //SIMPLIFIEDWAVEFUNCTIONCOLLAPSE_IMAGEMANAGER_H
