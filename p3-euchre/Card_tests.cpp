#include "Card.hpp"
#include "unit_test_framework.hpp"
#include <iostream>
#include <sstream>

using namespace std;

// Test default constructor
TEST(test_card_default_ctor) {
  Card c;
  ASSERT_EQUAL(TWO, c.get_rank());
  ASSERT_EQUAL(SPADES, c.get_suit());
}

// Test parameterized constructor
TEST(test_card_ctor) {
  Card c(ACE, HEARTS);
  ASSERT_EQUAL(ACE, c.get_rank());
  ASSERT_EQUAL(HEARTS, c.get_suit());

  Card c2(NINE, CLUBS);
  ASSERT_EQUAL(NINE, c2.get_rank());
  ASSERT_EQUAL(CLUBS, c2.get_suit());

  Card c3(JACK, DIAMONDS);
  ASSERT_EQUAL(JACK, c3.get_rank());
  ASSERT_EQUAL(DIAMONDS, c3.get_suit());
}

// Test get_suit with trump
TEST(test_get_suit_with_trump) {
  Card c(TEN, CLUBS);
  ASSERT_EQUAL(CLUBS, c.get_suit(SPADES));
  ASSERT_EQUAL(CLUBS, c.get_suit(HEARTS));
  ASSERT_EQUAL(CLUBS, c.get_suit(CLUBS));
  ASSERT_EQUAL(CLUBS, c.get_suit(DIAMONDS));

  Card c2(QUEEN, HEARTS);
  ASSERT_EQUAL(HEARTS, c2.get_suit(SPADES));
  ASSERT_EQUAL(HEARTS, c2.get_suit(CLUBS));
  ASSERT_EQUAL(HEARTS, c2.get_suit(DIAMONDS));

  // Jack of Hearts is left bower when trump is Diamonds
  Card c3(JACK, HEARTS);
  ASSERT_EQUAL(DIAMONDS, c3.get_suit(DIAMONDS));
  ASSERT_EQUAL(HEARTS, c3.get_suit(SPADES));
  ASSERT_EQUAL(HEARTS, c3.get_suit(CLUBS));

  // Jack of Diamonds is left bower when trump is Hearts
  Card c4(JACK, DIAMONDS);
  ASSERT_EQUAL(HEARTS, c4.get_suit(HEARTS));
  ASSERT_EQUAL(DIAMONDS, c4.get_suit(SPADES));
  ASSERT_EQUAL(DIAMONDS, c4.get_suit(CLUBS));
}

// Test is_face_or_ace
TEST(test_face_or_ace) {
  // Non-face cards
  ASSERT_FALSE(Card(NINE, HEARTS).is_face_or_ace());
  ASSERT_FALSE(Card(TEN, HEARTS).is_face_or_ace());

  // Face cards and Ace
  ASSERT_TRUE(Card(JACK, SPADES).is_face_or_ace());
  ASSERT_TRUE(Card(QUEEN, HEARTS).is_face_or_ace());
  ASSERT_TRUE(Card(KING, CLUBS).is_face_or_ace());
  ASSERT_TRUE(Card(ACE, DIAMONDS).is_face_or_ace());
}

// Test right bower
TEST(test_right_bower) {
  // Jack of trump suit is right bower
  Card right_spades(JACK, SPADES);
  ASSERT_TRUE(right_spades.is_right_bower(SPADES));
  ASSERT_FALSE(right_spades.is_right_bower(HEARTS));
  ASSERT_FALSE(right_spades.is_right_bower(CLUBS));
  ASSERT_FALSE(right_spades.is_right_bower(DIAMONDS));

  Card right_hearts(JACK, HEARTS);
  ASSERT_TRUE(right_hearts.is_right_bower(HEARTS));
  ASSERT_FALSE(right_hearts.is_right_bower(SPADES));
  ASSERT_FALSE(right_hearts.is_right_bower(CLUBS));
  ASSERT_FALSE(right_hearts.is_right_bower(DIAMONDS));

  Card right_clubs(JACK, CLUBS);
  ASSERT_TRUE(right_clubs.is_right_bower(CLUBS));
  ASSERT_FALSE(right_clubs.is_right_bower(SPADES));
  ASSERT_FALSE(right_clubs.is_right_bower(HEARTS));
  ASSERT_FALSE(right_clubs.is_right_bower(DIAMONDS));

  Card right_diamonds(JACK, DIAMONDS);
  ASSERT_TRUE(right_diamonds.is_right_bower(DIAMONDS));
  ASSERT_FALSE(right_diamonds.is_right_bower(SPADES));
  ASSERT_FALSE(right_diamonds.is_right_bower(HEARTS));
  ASSERT_FALSE(right_diamonds.is_right_bower(CLUBS));

  // Non-Jacks are not right bowers
  ASSERT_FALSE(Card(QUEEN, SPADES).is_right_bower(SPADES));
  ASSERT_FALSE(Card(KING, HEARTS).is_right_bower(HEARTS));
  ASSERT_FALSE(Card(ACE, CLUBS).is_right_bower(CLUBS));
}

// Test left bower
TEST(test_left_bower) {
  // Jack of next suit is left bower
  Card left_spades(JACK, CLUBS); // Clubs is next suit of Spades
  ASSERT_TRUE(left_spades.is_left_bower(SPADES));
  ASSERT_FALSE(left_spades.is_left_bower(HEARTS));
  ASSERT_FALSE(left_spades.is_left_bower(CLUBS));
  ASSERT_FALSE(left_spades.is_left_bower(DIAMONDS));

  Card left_hearts(JACK, DIAMONDS); // Diamonds is next suit of Hearts
  ASSERT_TRUE(left_hearts.is_left_bower(HEARTS));
  ASSERT_FALSE(left_hearts.is_left_bower(SPADES));
  ASSERT_FALSE(left_hearts.is_left_bower(CLUBS));
  ASSERT_FALSE(left_hearts.is_left_bower(DIAMONDS));

  Card left_clubs(JACK, SPADES); // Spades is next suit of Clubs
  ASSERT_TRUE(left_clubs.is_left_bower(CLUBS));
  ASSERT_FALSE(left_clubs.is_left_bower(SPADES));
  ASSERT_FALSE(left_clubs.is_left_bower(HEARTS));
  ASSERT_FALSE(left_clubs.is_left_bower(DIAMONDS));

  Card left_diamonds(JACK, HEARTS); // Hearts is next suit of Diamonds
  ASSERT_TRUE(left_diamonds.is_left_bower(DIAMONDS));
  ASSERT_FALSE(left_diamonds.is_left_bower(SPADES));
  ASSERT_FALSE(left_diamonds.is_left_bower(HEARTS));
  ASSERT_FALSE(left_diamonds.is_left_bower(CLUBS));

  // Non-Jacks are not left bowers
  ASSERT_FALSE(Card(QUEEN, CLUBS).is_left_bower(SPADES));
  ASSERT_FALSE(Card(KING, DIAMONDS).is_left_bower(HEARTS));
}

// Test right and left bower mutual exclusivity
TEST(test_right_and_left_bower_exclusive) {
  Card right(JACK, SPADES);
  ASSERT_TRUE(right.is_right_bower(SPADES));
  ASSERT_FALSE(right.is_left_bower(SPADES));

  Card left(JACK, CLUBS);
  ASSERT_TRUE(left.is_left_bower(SPADES));
  ASSERT_FALSE(left.is_right_bower(SPADES));

  // Same card can be left bower for one trump and right bower for another
  Card jack_hearts(JACK, HEARTS);
  ASSERT_TRUE(jack_hearts.is_right_bower(HEARTS));
  ASSERT_TRUE(jack_hearts.is_left_bower(DIAMONDS));
  ASSERT_FALSE(jack_hearts.is_left_bower(HEARTS));
  ASSERT_FALSE(jack_hearts.is_right_bower(DIAMONDS));
}

// Test is_trump
TEST(test_is_trump) {
  // Regular trump cards
  Card t1(NINE, HEARTS);
  ASSERT_TRUE(t1.is_trump(HEARTS));
  ASSERT_FALSE(t1.is_trump(SPADES));
  ASSERT_FALSE(t1.is_trump(CLUBS));
  ASSERT_FALSE(t1.is_trump(DIAMONDS));

  // Right bower is trump
  Card right(JACK, SPADES);
  ASSERT_TRUE(right.is_trump(SPADES));
  ASSERT_FALSE(right.is_trump(HEARTS));
  ASSERT_TRUE(right.is_trump(CLUBS)); // Jack of Spades is left bower for Clubs
  ASSERT_FALSE(right.is_trump(DIAMONDS));

  // Left bower is trump
  Card left(JACK, CLUBS); // Left bower when trump is Spades
  ASSERT_TRUE(left.is_trump(SPADES));
  ASSERT_FALSE(left.is_trump(HEARTS));
  ASSERT_TRUE(left.is_trump(CLUBS)); // Also trump when Clubs is trump
  ASSERT_FALSE(left.is_trump(DIAMONDS));

  // All trump suits test
  Card hearts_card(QUEEN, HEARTS);
  ASSERT_TRUE(hearts_card.is_trump(HEARTS));
  ASSERT_FALSE(hearts_card.is_trump(SPADES));
  ASSERT_FALSE(hearts_card.is_trump(CLUBS));
  ASSERT_FALSE(hearts_card.is_trump(DIAMONDS));
}

// Test stream output operator
TEST(test_operator_output) {
  Card c1(QUEEN, DIAMONDS);
  ostringstream oss1;
  oss1 << c1;
  ASSERT_EQUAL("Queen of Diamonds", oss1.str());

  Card c2(TWO, SPADES);
  ostringstream oss2;
  oss2 << c2;
  ASSERT_EQUAL("Two of Spades", oss2.str());

  Card c3(ACE, HEARTS);
  ostringstream oss3;
  oss3 << c3;
  ASSERT_EQUAL("Ace of Hearts", oss3.str());

  Card c4(JACK, CLUBS);
  ostringstream oss4;
  oss4 << c4;
  ASSERT_EQUAL("Jack of Clubs", oss4.str());
}

// Test stream input operator
TEST(test_operator_input) {
  istringstream iss1("King of Clubs");
  Card c1;
  iss1 >> c1;
  ASSERT_EQUAL(KING, c1.get_rank());
  ASSERT_EQUAL(CLUBS, c1.get_suit());

  istringstream iss2("Two of Hearts");
  Card c2;
  iss2 >> c2;
  ASSERT_EQUAL(TWO, c2.get_rank());
  ASSERT_EQUAL(HEARTS, c2.get_suit());

  istringstream iss3("Ace of Spades");
  Card c3;
  iss3 >> c3;
  ASSERT_EQUAL(ACE, c3.get_rank());
  ASSERT_EQUAL(SPADES, c3.get_suit());
}

// Test equality operator
TEST(test_operator_equality) {
  Card c1(TEN, HEARTS);
  Card c2(TEN, HEARTS);
  Card c3(JACK, HEARTS);
  Card c4(TEN, SPADES);

  ASSERT_TRUE(c1 == c2);
  ASSERT_FALSE(c1 == c3);
  ASSERT_FALSE(c1 == c4);
  ASSERT_TRUE(c1 == c1);
}

// Test inequality operator
TEST(test_operator_inequality) {
  Card c1(TEN, HEARTS);
  Card c2(TEN, HEARTS);
  Card c3(JACK, HEARTS);
  Card c4(TEN, SPADES);

  ASSERT_FALSE(c1 != c2);
  ASSERT_TRUE(c1 != c3);
  ASSERT_TRUE(c1 != c4);
  ASSERT_FALSE(c1 != c1);
}

// Test less than operator
TEST(test_operator_less_than) {
  Card c1(TEN, HEARTS);
  Card c2(JACK, HEARTS);
  Card c3(TEN, SPADES);
  Card c4(NINE, HEARTS);

  // Test rank comparison
  ASSERT_TRUE(c1 < c2);  // TEN < JACK
  ASSERT_FALSE(c2 < c1); // JACK > TEN
  ASSERT_TRUE(c4 < c1);  // NINE < TEN

  // Test suit comparison when ranks are equal
  ASSERT_TRUE(c3 < c1);  // TEN of SPADES < TEN of HEARTS (SPADES < HEARTS)
  ASSERT_FALSE(c1 < c3); // TEN of HEARTS > TEN of SPADES

  // Test with different ranks and suits
  Card two_diamonds(TWO, DIAMONDS);
  Card ace_spades(ACE, SPADES);
  ASSERT_TRUE(two_diamonds < ace_spades);
  ASSERT_FALSE(ace_spades < two_diamonds);
}

// Test less than or equal operator
TEST(test_operator_less_equal) {
  Card c1(TEN, HEARTS);
  Card c2(JACK, HEARTS);
  Card c3(TEN, HEARTS);

  ASSERT_TRUE(c1 <= c2);  // TEN <= JACK
  ASSERT_FALSE(c2 <= c1); // JACK > TEN
  ASSERT_TRUE(c1 <= c3);  // TEN <= TEN (equal)
  ASSERT_TRUE(c3 <= c1);  // TEN <= TEN (equal)
}

// Test greater than operator
TEST(test_operator_greater_than) {
  Card c1(TEN, HEARTS);
  Card c2(JACK, HEARTS);
  Card c3(NINE, HEARTS);

  ASSERT_FALSE(c1 > c2); // TEN < JACK
  ASSERT_TRUE(c2 > c1);  // JACK > TEN
  ASSERT_TRUE(c1 > c3);  // TEN > NINE
  ASSERT_FALSE(c3 > c1); // NINE < TEN
}

// Test greater than or equal operator
TEST(test_operator_greater_equal) {
  Card c1(TEN, HEARTS);
  Card c2(JACK, HEARTS);
  Card c3(TEN, HEARTS);

  ASSERT_FALSE(c1 >= c2); // TEN < JACK
  ASSERT_TRUE(c2 >= c1);  // JACK > TEN
  ASSERT_TRUE(c1 >= c3);  // TEN >= TEN (equal)
  ASSERT_TRUE(c3 >= c1);  // TEN >= TEN (equal)
}

// Test Suit_next function
TEST(test_suit_next) {
  ASSERT_EQUAL(CLUBS, Suit_next(SPADES));
  ASSERT_EQUAL(DIAMONDS, Suit_next(HEARTS));
  ASSERT_EQUAL(SPADES, Suit_next(CLUBS));
  ASSERT_EQUAL(HEARTS, Suit_next(DIAMONDS));
}

// Test Card_less with trump (2-parameter version)
TEST(test_card_less_trump_basic) {
  Suit trump = HEARTS;

  // Right bower beats everything
  Card right_bower(JACK, HEARTS);
  Card ace(ACE, SPADES);
  ASSERT_FALSE(Card_less(right_bower, ace, trump));
  ASSERT_TRUE(Card_less(ace, right_bower, trump));

  // Left bower beats everything except right bower
  Card left_bower(JACK, DIAMONDS);
  ASSERT_FALSE(Card_less(left_bower, ace, trump));
  ASSERT_TRUE(Card_less(ace, left_bower, trump));
  ASSERT_TRUE(Card_less(left_bower, right_bower, trump));
  ASSERT_FALSE(Card_less(right_bower, left_bower, trump));
}

// Test Card_less with trump hierarchy
TEST(test_card_less_trump_hierarchy) {
  Suit trump = SPADES;

  Card right_bower(JACK, SPADES);  // Highest
  Card left_bower(JACK, CLUBS);    // Second highest
  Card ace_trump(ACE, SPADES);     // Third highest (trump)
  Card king_trump(KING, SPADES);   // Fourth highest (trump)
  Card ace_non_trump(ACE, HEARTS); // Lower (non-trump)
  Card nine_trump(NINE, SPADES);   // Lowest trump

  // Right bower > left bower
  ASSERT_TRUE(Card_less(left_bower, right_bower, trump));
  ASSERT_FALSE(Card_less(right_bower, left_bower, trump));

  // Left bower > trump ace
  ASSERT_TRUE(Card_less(ace_trump, left_bower, trump));
  ASSERT_FALSE(Card_less(left_bower, ace_trump, trump));

  // Trump ace > trump king
  ASSERT_TRUE(Card_less(king_trump, ace_trump, trump));
  ASSERT_FALSE(Card_less(ace_trump, king_trump, trump));

  // Any trump > any non-trump
  ASSERT_TRUE(Card_less(ace_non_trump, nine_trump, trump));
  ASSERT_FALSE(Card_less(nine_trump, ace_non_trump, trump));
}

// Test Card_less with non-trump cards
TEST(test_card_less_non_trump) {
  Suit trump = HEARTS;

  Card ace_spades(ACE, SPADES);
  Card king_spades(KING, SPADES);
  Card ace_clubs(ACE, CLUBS);

  // Among non-trump cards, normal comparison applies
  ASSERT_TRUE(Card_less(king_spades, ace_spades, trump));
  ASSERT_FALSE(Card_less(ace_spades, king_spades, trump));
  ASSERT_FALSE(
      Card_less(ace_clubs, ace_spades,
                trump)); // ACE > ACE, then CLUBS < SPADES, but ACE rank is same
}

// Test Card_less with led card (3-parameter version)
TEST(test_card_less_with_led_card) {
  Suit trump = HEARTS;
  Card led_card(NINE, SPADES);

  // Cards that follow suit
  Card ten_spades(TEN, SPADES);
  Card queen_spades(QUEEN, SPADES);

  // Cards that don't follow suit
  Card ace_clubs(ACE, CLUBS);
  Card king_diamonds(KING, DIAMONDS);
  Card trump_nine(NINE, HEARTS);

  // Following suit beats not following suit (unless trump)
  ASSERT_FALSE(Card_less(ten_spades, ace_clubs, led_card, trump));
  ASSERT_TRUE(Card_less(ace_clubs, ten_spades, led_card, trump));

  // Trump beats following suit
  ASSERT_FALSE(Card_less(trump_nine, queen_spades, led_card, trump));
  ASSERT_TRUE(Card_less(queen_spades, trump_nine, led_card, trump));

  // Among cards that follow suit, higher rank wins
  ASSERT_TRUE(Card_less(ten_spades, queen_spades, led_card, trump));
  ASSERT_FALSE(Card_less(queen_spades, ten_spades, led_card, trump));
}

// Test Card_less with trump led
TEST(test_card_less_trump_led) {
  Suit trump = HEARTS;
  Card led_card(NINE, HEARTS); // Trump is led

  Card right_bower(JACK, HEARTS);
  Card left_bower(JACK, DIAMONDS);
  Card ace_trump(ACE, HEARTS);
  Card off_suit(ACE, SPADES);

  // When trump is led, trump hierarchy applies
  ASSERT_TRUE(Card_less(ace_trump, right_bower, led_card, trump));
  ASSERT_TRUE(Card_less(left_bower, right_bower, led_card, trump));
  ASSERT_TRUE(Card_less(off_suit, ace_trump, led_card, trump));
}

// Test Card_less with left bower considerations
TEST(test_card_less_left_bower_complex) {
  Suit trump = SPADES;
  Card led_card(TEN, CLUBS);

  Card left_bower(JACK, CLUBS); // This is left bower, which means it's trump
  Card ace_clubs(ACE, CLUBS);
  Card trump_nine(NINE, SPADES);

  // Left bower should beat ace of clubs when clubs is led (bower is trump)
  ASSERT_TRUE(Card_less(ace_clubs, left_bower, led_card, trump));

  // Left bower beats regular trump since left bower is second highest trump
  ASSERT_FALSE(Card_less(left_bower, trump_nine, led_card, trump));
}

// Test string_to_rank function
TEST(test_string_to_rank) {
  ASSERT_EQUAL(TWO, string_to_rank("Two"));
  ASSERT_EQUAL(THREE, string_to_rank("Three"));
  ASSERT_EQUAL(FOUR, string_to_rank("Four"));
  ASSERT_EQUAL(FIVE, string_to_rank("Five"));
  ASSERT_EQUAL(SIX, string_to_rank("Six"));
  ASSERT_EQUAL(SEVEN, string_to_rank("Seven"));
  ASSERT_EQUAL(EIGHT, string_to_rank("Eight"));
  ASSERT_EQUAL(NINE, string_to_rank("Nine"));
  ASSERT_EQUAL(TEN, string_to_rank("Ten"));
  ASSERT_EQUAL(JACK, string_to_rank("Jack"));
  ASSERT_EQUAL(QUEEN, string_to_rank("Queen"));
  ASSERT_EQUAL(KING, string_to_rank("King"));
  ASSERT_EQUAL(ACE, string_to_rank("Ace"));
}

// Test string_to_suit function
TEST(test_string_to_suit) {
  ASSERT_EQUAL(SPADES, string_to_suit("Spades"));
  ASSERT_EQUAL(HEARTS, string_to_suit("Hearts"));
  ASSERT_EQUAL(CLUBS, string_to_suit("Clubs"));
  ASSERT_EQUAL(DIAMONDS, string_to_suit("Diamonds"));
}

// Test rank stream operators
TEST(test_rank_stream_operators) {
  // Test output
  ostringstream oss;
  oss << ACE;
  ASSERT_EQUAL("Ace", oss.str());

  oss.str("");
  oss << KING;
  ASSERT_EQUAL("King", oss.str());

  oss.str("");
  oss << TWO;
  ASSERT_EQUAL("Two", oss.str());

  // Test input
  istringstream iss("Queen");
  Rank r;
  iss >> r;
  ASSERT_EQUAL(QUEEN, r);
}

// Test suit stream operators
TEST(test_suit_stream_operators) {
  // Test output
  ostringstream oss;
  oss << SPADES;
  ASSERT_EQUAL("Spades", oss.str());

  oss.str("");
  oss << HEARTS;
  ASSERT_EQUAL("Hearts", oss.str());

  oss.str("");
  oss << CLUBS;
  ASSERT_EQUAL("Clubs", oss.str());

  oss.str("");
  oss << DIAMONDS;
  ASSERT_EQUAL("Diamonds", oss.str());

  // Test input
  istringstream iss("Hearts");
  Suit s;
  iss >> s;
  ASSERT_EQUAL(HEARTS, s);
}

TEST_MAIN()
