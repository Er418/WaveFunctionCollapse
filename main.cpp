#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"

#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "ImageManager.h"
#include "WaveFunctionCollapse.h"


int main() {
    std::string name;
    std::cout << "Enter pattern name" << std::endl;
    std::cin >> name;
    ImageManager mng;
    mng.ReadImage(name);

    size_t n, m, TILE_SIZE;
    std::cout << "Enter output image dimensions (height, width)" << std::endl;
    std::cin >> n >> m;
    std::cout << "Enter tile size" << std::endl;
    std::cin >> TILE_SIZE;
    int border = 0;
    std::cout << "Use borders in output image? (Y/N)" << std::endl;
    char sym;
    std::cin >> sym;
    if(sym == 'Y' || sym == 'y') {
        border = 1;
    }
    //mng.GenerateVideo("tree1copy", 68685);/*

    //mng.WriteImage(name + "test");
    auto buf = mng.GetPattern();
    WaveFunctionCollapse wfc(buf, n, m, TILE_SIZE, border, name);
    mng.SetPattern(wfc.Collapse());
    mng.WriteImage(name);
    return 0;
}

#pragma clang diagnostic pop

