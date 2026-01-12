#include "Image_test_helpers.hpp"
#include "Matrix.hpp"
#include "unit_test_framework.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Here's a free test for you! Model yours after this one.
// Test functions have no interface and thus no RMEs, but
// add a comment like the one here to say what it is testing.
// -----
// Sets various pixels in a 2x2 Image and checks
// that Image_print produces the correct output.
TEST(test_print_basic_demo) {
  Image img;
  const Pixel red = {255, 0, 0};
  const Pixel green = {0, 255, 0};
  const Pixel blue = {0, 0, 255};
  const Pixel white = {255, 255, 255};

  Image_init(&img, 2, 2);
  Image_set_pixel(&img, 0, 0, red);
  Image_set_pixel(&img, 0, 1, green);
  Image_set_pixel(&img, 1, 0, blue);
  Image_set_pixel(&img, 1, 1, white);

  // Capture our output
  ostringstream s;
  Image_print(&img, s);

  // Correct output
  ostringstream correct;
  correct << "P3\n2 2\n255\n";
  correct << "255 0 0 0 255 0 \n";
  correct << "0 0 255 255 255 255 \n";
  ASSERT_EQUAL(s.str(), correct.str());
}

// IMPLEMENT YOUR TEST FUNCTIONS HERE
// You are encouraged to use any functions from Image_test_helpers.hpp as
// needed.

// Tests that Image_init sets all pixels to (0,0,0)
TEST(test_image_init_zero_pixels) {
  Image img;
  Image_init(&img, 3, 2);
  ASSERT_EQUAL(Image_width(&img), 3);
  ASSERT_EQUAL(Image_height(&img), 2);

  for (int r = 0; r < 2; ++r) {
    for (int c = 0; c < 3; ++c) {
      Pixel p = Image_get_pixel(&img, r, c);
      ASSERT_EQUAL(p.r, 0);
      ASSERT_EQUAL(p.g, 0);
      ASSERT_EQUAL(p.b, 0);
    }
  }
}

// Tests that Image_set_pixel and Image_get_pixel work correctly
TEST(test_image_set_get_pixel) {
  Image img;
  Pixel p1 = {10, 20, 30};
  Pixel p2 = {255, 255, 255};

  Image_init(&img, 2, 2);
  Image_set_pixel(&img, 0, 0, p1);
  Image_set_pixel(&img, 1, 1, p2);

  Pixel got1 = Image_get_pixel(&img, 0, 0);
  Pixel got2 = Image_get_pixel(&img, 1, 1);

  ASSERT_EQUAL(got1.r, 10);
  ASSERT_EQUAL(got1.g, 20);
  ASSERT_EQUAL(got1.b, 30);

  ASSERT_EQUAL(got2.r, 255);
  ASSERT_EQUAL(got2.g, 255);
  ASSERT_EQUAL(got2.b, 255);
}

// Tests that Image_fill fills the entire image with the same color
TEST(test_image_fill_color) {
  Image img;
  Pixel yellow = {255, 255, 0};
  Image_init(&img, 3, 3);
  Image_fill(&img, yellow);

  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      Pixel p = Image_get_pixel(&img, r, c);
      ASSERT_EQUAL(p.r, 255);
      ASSERT_EQUAL(p.g, 255);
      ASSERT_EQUAL(p.b, 0);
    }
  }
}

// Tests Image_width and Image_height return correct values
TEST(test_image_dimensions) {
  Image img;
  Image_init(&img, 7, 4);
  ASSERT_EQUAL(Image_width(&img), 7);
  ASSERT_EQUAL(Image_height(&img), 4);
}

// Tests reading a PPM-formatted string into an Image
TEST(test_image_init_from_stream) {
  std::istringstream input(
      "P3\n2 2\n255\n255 0 0 0 255 0 0 0 255 255 255 255\n");

  Image img;
  Image_init(&img, input);

  ASSERT_EQUAL(Image_width(&img), 2);
  ASSERT_EQUAL(Image_height(&img), 2);

  Pixel p00 = Image_get_pixel(&img, 0, 0); // 255 0 0
  Pixel p01 = Image_get_pixel(&img, 0, 1); // 0 255 0
  Pixel p10 = Image_get_pixel(&img, 1, 0); // 0 0 255
  Pixel p11 = Image_get_pixel(&img, 1, 1); // 255 255 255

  ASSERT_EQUAL(p00.r, 255);
  ASSERT_EQUAL(p00.g, 0);
  ASSERT_EQUAL(p00.b, 0);
  ASSERT_EQUAL(p01.r, 0);
  ASSERT_EQUAL(p01.g, 255);
  ASSERT_EQUAL(p01.b, 0);
  ASSERT_EQUAL(p10.r, 0);
  ASSERT_EQUAL(p10.g, 0);
  ASSERT_EQUAL(p10.b, 255);
  ASSERT_EQUAL(p11.r, 255);
  ASSERT_EQUAL(p11.g, 255);
  ASSERT_EQUAL(p11.b, 255);
}

// Already provided: Tests Image_print on a 2x2 image with distinct pixels
TEST(test_print_basic) {
  Image img;
  const Pixel red = {255, 0, 0};
  const Pixel green = {0, 255, 0};
  const Pixel blue = {0, 0, 255};
  const Pixel white = {255, 255, 255};

  Image_init(&img, 2, 2);
  Image_set_pixel(&img, 0, 0, red);
  Image_set_pixel(&img, 0, 1, green);
  Image_set_pixel(&img, 1, 0, blue);
  Image_set_pixel(&img, 1, 1, white);

  // Capture our output
  std::ostringstream s;
  Image_print(&img, s);

  // Correct output
  std::ostringstream correct;
  correct << "P3\n2 2\n255\n";
  correct << "255 0 0 0 255 0 \n";
  correct << "0 0 255 255 255 255 \n";
  ASSERT_EQUAL(s.str(), correct.str());
}

TEST_MAIN() // Do NOT put a semicolon here
