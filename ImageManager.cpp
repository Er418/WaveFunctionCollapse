#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
//
// Created by Egor Dubrouski on 06.03.23.
//

#include "ImageManager.h"
#pragma GCC target("avx2")
#pragma GCC optimize("O3")


void ImageManager::writeBmpFile(const std::string& filename, const std::vector<std::vector<Color>>& pixels) {
    // Create a new image
    cv::Mat image(pixels.size(), pixels[0].size(), CV_8UC3);

    // Copy the pixel data to the image
    for (int i = 0; i < pixels.size(); ++i) {
        for (int j = 0; j < pixels[0].size(); ++j) {
            cv::Vec3b& pixel = image.at<cv::Vec3b>(i, j);
            pixel[2] = pixels[i][j].getR();
            pixel[1] = pixels[i][j].getG();
            pixel[0] = pixels[i][j].getB();
        }
    }

    // Write the image to a BMP file
    cv::imwrite(filename, image);
}


ImageManager::ImageType ImageManager::readBmpFile(const std::string &fileName) {
    cv::Mat image = cv::imread(fileName, cv::IMREAD_COLOR);

    // Create 2D vector to store image pixels as Color objects
    std::vector<std::vector<Color>> pixels(image.rows, std::vector<Color>(image.cols));

    // Convert pixel data from OpenCV format to Color objects
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            cv::Vec3b bgr = image.at<cv::Vec3b>(i, j);
            pixels[i][j] = Color(bgr[2], bgr[1], bgr[0]);
        }
    }

    return pixels;
}


void ImageManager::ReadImage(const std::string &filename) {

    image = readBmpFile("../images/input/" + filename + ".bmp");

    for (const auto &i: image) {
        for (const auto &j: i) {
            if (!color_map.contains(j)) {
                pattern_map[(int) color_map.size()] = j;
                color_map[j] = (int) color_map.size();
            }
        }
    }
}


void ImageManager::WriteImage(const std::string &filename) {
    writeBmpFile(std::string("../images/output/" + filename + ".bmp"), image);
}


std::vector<std::vector<int>> ImageManager::GetPattern() {
    std::vector<std::vector<int>> pattern(image.size(), std::vector<int>(image[0].size()));
    for (size_t i = 0; i < image.size(); i++) {
        for (size_t j = 0; j < image[0].size(); j++) {
            pattern[i][j] = color_map[image[i][j]];
        }
    }
    return pattern;
}


void ImageManager::SetPattern(const std::vector<std::vector<int>> &pattern) {
    image = ImageType(pattern.size(), std::vector<Color>(pattern[0].size()));
    for (size_t i = 0; i < pattern.size(); i++) {
        for (size_t j = 0; j < pattern[0].size(); j++) {
            image[i][j] = pattern_map[pattern[i][j]];
        }
    }
}

void ImageManager::SetMixedImage(std::vector<std::vector<MixedColors>> &image_) {
    image = ImageType(image_.size(), std::vector<Color>(image_[0].size()));
    for (size_t i = 0; i < image_.size(); i++) {
        for (size_t j = 0; j < image_[0].size(); j++) {
            for (auto k: pattern_map) {
                image_[i][j].SetColor(k.first, k.second);
            }
            image[i][j] = image_[i][j].GetAverageColor();
        }
    }
}

void ImageManager::GenerateVideo(std::string filename_, int numFrames) {
    if(numFrames == 0) {
        return;
    }
    sleep(1);
    std::cout << std::endl << "Generating video" << std::endl;
    std::string filename = "../images/output/" + filename_ + "/frame";
    // Open the BMP files
    cv::Mat frame = cv::imread(filename + "0.bmp");
    cv::Mat new_frame = frame;
    double mult = 2000.0 / frame.size().height;
    cv::resize(frame, new_frame, cv::Size(), mult, mult, cv::INTER_NEAREST);
    auto writer = cv::VideoWriter("../images/output/videos/" + filename_ + ".avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                  60, new_frame.size());
    writer << new_frame;
    cv::Mat prev = frame;
    ProgressBar bar(0, numFrames + 119);
    for (int i = 1; i < numFrames; i++) {
        bar.Update(i);
        frame = cv::imread(filename + std::to_string(i) + ".bmp");
        if(std::equal(frame.begin<uchar>(), frame.end<uchar>(), prev.begin<uchar>())) {
            continue;
        } else {
            prev = frame;
        }
        cv::resize(frame, new_frame, cv::Size(), mult, mult, cv::INTER_NEAREST);
        writer << new_frame;
    }
    for(int i = 0; i < 120; i++) {
        bar.Update(numFrames + i);
        writer << new_frame;
    }
    bar.Finish();
    writer.release();
}

#pragma clang diagnostic pop