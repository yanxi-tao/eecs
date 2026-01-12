#include <cassert>
#include "Image.hpp"
#include "Matrix.hpp"

// REQUIRES: img points to an Image
//           0 < width && 0 < height
// MODIFIES: *img
// EFFECTS:  Initializes the Image with the given width and height, with
//           all pixels initialized to RGB values of 0.
void Image_init(Image* img, int width, int height) {
    img->height = height;
    img->width = width;

    Matrix red_matrix;
    Matrix green_matrix;
    Matrix blue_matrix;

    Matrix_init(&red_matrix, width, height);
    Matrix_init(&green_matrix, width, height);
    Matrix_init(&blue_matrix, width, height);

    img->red_channel = red_matrix;
    img->green_channel = green_matrix;
    img->blue_channel = blue_matrix;
}

// REQUIRES: img points to an Image
//           is contains an image in PPM format without comments
//           (any kind of whitespace is ok)
// MODIFIES: *img, is
// EFFECTS:  Initializes the Image by reading in an image in PPM format
//           from the given input stream.
// NOTE:     See the project spec for a discussion of PPM format.
void Image_init(Image* img, std::istream& is) {
    std::string gb_val;
    is >> gb_val;
    assert(gb_val == "P3");

    int h, w;
    is >> w >> h >> gb_val;

    img->height = h;
    img->width = w;

    Matrix red_matrix;
    Matrix green_matrix;
    Matrix blue_matrix;

    Matrix_init(&red_matrix, w, h);
    Matrix_init(&green_matrix, w, h);
    Matrix_init(&blue_matrix, w, h);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int r, g, b;
            is >> r >> g >> b;
            *Matrix_at(&red_matrix, i, j) = r;
            *Matrix_at(&green_matrix, i, j) = g;
            *Matrix_at(&blue_matrix, i, j) = b;
        }
    }

    img->red_channel = red_matrix;
    img->green_channel = green_matrix;
    img->blue_channel = blue_matrix;
}

// REQUIRES: img points to a valid Image
// MODIFIES: os
// EFFECTS:  Writes the image to the given output stream in PPM format.
//           You must use the kind of whitespace specified here.
//           First, prints out the header for the image like this:
//             P3 [newline]
//             WIDTH [space] HEIGHT [newline]
//             255 [newline]
//           Next, prints out the rows of the image, each followed by a
//           newline. Each pixel in a row is printed as three ints
//           for its red, green, and blue components, in that order. Each
//           int is followed by a space. This means that there will be an
//           "extra" space at the end of each line. See the project spec
//           for an example.
void Image_print(const Image* img, std::ostream& os) {
    os << "P3\n" << img->width << " " << img->height << "\n" << "255\n";
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            os << *Matrix_at(&(img->red_channel), i, j) << " ";
            os << *Matrix_at(&(img->green_channel), i, j) << " ";
            os << *Matrix_at(&(img->blue_channel), i, j) << " ";
        }
        os << "\n";
    }
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the width of the Image.
int Image_width(const Image* img) {
    return img->width;
}

// REQUIRES: img points to a valid Image
// EFFECTS:  Returns the height of the Image.
int Image_height(const Image* img) {
    return img->height;
}

// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// EFFECTS:  Returns the pixel in the Image at the given row and column.
Pixel Image_get_pixel(const Image* img, int row, int column) {
    return Pixel{
        *Matrix_at(&(img->red_channel), row, column),
        *Matrix_at(&(img->green_channel), row, column),
        *Matrix_at(&(img->blue_channel), row, column)
    };
}

// REQUIRES: img points to a valid Image
//           0 <= row && row < Image_height(img)
//           0 <= column && column < Image_width(img)
// MODIFIES: *img
// EFFECTS:  Sets the pixel in the Image at the given row and column
//           to the given color.
void Image_set_pixel(Image* img, int row, int column, Pixel color) {
    *Matrix_at(&(img->red_channel), row, column) = color.r;
    *Matrix_at(&(img->green_channel), row, column) = color.g;
    *Matrix_at(&(img->blue_channel), row, column) = color.b;
}

// REQUIRES: img points to a valid Image
// MODIFIES: *img
// EFFECTS:  Sets each pixel in the image to the given color.
void Image_fill(Image* img, Pixel color) {
    Matrix_fill(&(img->red_channel), color.r);
    Matrix_fill(&(img->green_channel), color.g);
    Matrix_fill(&(img->blue_channel), color.b);
}
