#include "Matrix.hpp"
#include "Matrix_test_helpers.hpp"
#include "unit_test_framework.hpp"
#include <vector>

using namespace std;

// Here's a free test for you! Model yours after this one.
// Test functions have no interface and thus no RMEs, but
// add a comment like the one here to say what it is testing.
// -----
// Fills a 3x5 Matrix with a value and checks
// that Matrix_at returns that value for each element.
TEST(test_fill_basici_demo) {
  Matrix mat;
  const int width = 3;
  const int height = 5;
  const int value = 42;
  Matrix_init(&mat, 3, 5);
  Matrix_fill(&mat, value);

  for(int r = 0; r < height; ++r){
    for(int c = 0; c < width; ++c){
      ASSERT_EQUAL(*Matrix_at(&mat, r, c), value);
    }
  }
}

// ADD YOUR TESTS HERE
// You are encouraged to use any functions from Matrix_test_helpers.hpp as needed.

TEST(test_matrix_init) {
    Matrix mat;
    Matrix_init(&mat, 4, 3);

    ASSERT_EQUAL(Matrix_width(&mat), 4);
    ASSERT_EQUAL(Matrix_height(&mat), 3);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 4; ++c) {
            ASSERT_EQUAL(*Matrix_at(&mat, r, c), 0);
        }
    }
}

TEST(test_matrix_dimensions) {
    Matrix mat;
    Matrix_init(&mat, 7, 9);
    ASSERT_EQUAL(Matrix_width(&mat), 7);
    ASSERT_EQUAL(Matrix_height(&mat), 9);
}

TEST(test_matrix_at_access) {
    Matrix mat;
    Matrix_init(&mat, 2, 2);
    *Matrix_at(&mat, 0, 0) = 10;
    *Matrix_at(&mat, 1, 1) = 20;

    ASSERT_EQUAL(*Matrix_at(&mat, 0, 0), 10);
    ASSERT_EQUAL(*Matrix_at(&mat, 1, 1), 20);
}

TEST(test_fill_basic) {
    Matrix mat;
    const int width = 3;
    const int height = 5;
    const int value = 42;
    Matrix_init(&mat, width, height);
    Matrix_fill(&mat, value);

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            ASSERT_EQUAL(*Matrix_at(&mat, r, c), value);
        }
    }
}

TEST(test_matrix_fill_border) {
    Matrix mat;
    Matrix_init(&mat, 4, 4);
    Matrix_fill(&mat, 0);
    Matrix_fill_border(&mat, 1);

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (r == 0 || r == 3 || c == 0 || c == 3) {
                ASSERT_EQUAL(*Matrix_at(&mat, r, c), 1);
            } else {
                ASSERT_EQUAL(*Matrix_at(&mat, r, c), 0);
            }
        }
    }
}

TEST(test_matrix_max) {
    Matrix mat;
    Matrix_init(&mat, 3, 3);
    Matrix_fill(&mat, 5);
    *Matrix_at(&mat, 1, 1) = 99;

    ASSERT_EQUAL(Matrix_max(&mat), 99);
}

TEST(test_column_of_min_value_in_row) {
    Matrix mat;
    Matrix_init(&mat, 5, 1); // One row, 5 columns
    *Matrix_at(&mat, 0, 0) = 9;
    *Matrix_at(&mat, 0, 1) = 4;
    *Matrix_at(&mat, 0, 2) = 4; // duplicate min
    *Matrix_at(&mat, 0, 3) = 7;
    *Matrix_at(&mat, 0, 4) = 5;

    int col = Matrix_column_of_min_value_in_row(&mat, 0, 0, 5);
    ASSERT_EQUAL(col, 1); // leftmost min value
}

TEST(test_min_value_in_row) {
    Matrix mat;
    Matrix_init(&mat, 4, 1);
    *Matrix_at(&mat, 0, 0) = 8;
    *Matrix_at(&mat, 0, 1) = 2;
    *Matrix_at(&mat, 0, 2) = 6;
    *Matrix_at(&mat, 0, 3) = 3;

    ASSERT_EQUAL(Matrix_min_value_in_row(&mat, 0, 0, 4), 2);
}

TEST(test_matrix_print) {
    Matrix mat;
    Matrix_init(&mat, 2, 2);
    *Matrix_at(&mat, 0, 0) = 1;
    *Matrix_at(&mat, 0, 1) = 2;
    *Matrix_at(&mat, 1, 0) = 3;
    *Matrix_at(&mat, 1, 1) = 4;

    std::ostringstream oss;
    Matrix_print(&mat, oss);

    std::string expected = "2 2\n1 2 \n3 4 \n";
    ASSERT_EQUAL(oss.str(), expected);
}


TEST_MAIN() // Do NOT put a semicolon here
