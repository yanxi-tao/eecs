#include "Image.hpp"
#include "processing.hpp"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int load_image(Image *img, const string &filename) {
  ifstream fin(filename);
  if (!fin.is_open()) {
    return 0;
  }

  Image_init(img, fin);
  return 1;
}

int write_image(const Image *img, const string &filename) {
  ofstream fout(filename);
  if (!fout.is_open()) {
    return 0;
  }
  Image_print(img, fout);
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 4 && argc != 5) {
    cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
         << "WIDTH and HEIGHT must be less than or equal to original" << endl;
    return 1;
  }

  string filename = argv[1];

  Image input_img;
  if (!load_image(&input_img, filename)) {
    cout << "Error opening file: " << filename << endl;
    return 1;
  }

  if (argc == 4) {
    string width_str = argv[3];
    int width = stoi(width_str);
    if (width < 0 || width > Image_width(&input_img)) {
      cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
           << "WIDTH and HEIGHT must be less than or equal to original" << endl;
      return 1;
    }
    seam_carve_width(&input_img, width);
    if (!write_image(&input_img, argv[2])) {
      cout << "Error opening file: " << argv[2] << endl;
    }
  } else {
    string width_str = argv[3];
    int width = stoi(width_str);
    string height_str = argv[4];
    int height = stoi(height_str);
    if (width < 0 || width > Image_width(&input_img) || height < 0 ||
        height > Image_height(&input_img)) {
      cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
           << "WIDTH and HEIGHT must be less than or equal to original" << endl;
      return 1;
    }
    seam_carve(&input_img, width, height);
    if (!write_image(&input_img, argv[2])) {
      cout << "Error opening file: " << argv[2] << endl;
    }
  }
}
