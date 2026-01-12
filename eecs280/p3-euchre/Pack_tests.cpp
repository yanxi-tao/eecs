#include "Card.hpp"
#include "Pack.hpp"
#include "unit_test_framework.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

// Constants from Pack.hpp
static const int PACK_SIZE = 24;

// Helper function to create a pack input file content as a string
string create_pack_input() {
  stringstream ss;
  for (int suit = SPADES; suit <= DIAMONDS; suit++) {
    for (int rank = NINE; rank <= ACE; rank++) {
      Card card(static_cast<Rank>(rank), static_cast<Suit>(suit));
      ss << card << "\n";
    }
  }
  return ss.str();
}

// Helper function to create custom pack input
string create_custom_pack_input() {
  stringstream ss;
  ss << "Nine of Spades\n";
  ss << "Ten of Hearts\n";
  ss << "Jack of Clubs\n";
  ss << "Queen of Diamonds\n";
  ss << "King of Spades\n";
  ss << "Ace of Hearts\n";
  // Add remaining cards to complete pack
  for (int i = 6; i < PACK_SIZE; i++) {
    ss << "Nine of Spades\n"; // Just fill with same card for simplicity
  }
  return ss.str();
}

// Test default constructor: checks if pack is initialized in standard order
TEST(test_pack_default_ctor) {
  Pack pack;
  // Expected standard order: 9S through AS, 9H through AH, 9C through AC, 9D
  // through AD
  Card expected_cards[] = {
      Card(NINE, SPADES),    Card(TEN, SPADES),    Card(JACK, SPADES),
      Card(QUEEN, SPADES),   Card(KING, SPADES),   Card(ACE, SPADES),
      Card(NINE, HEARTS),    Card(TEN, HEARTS),    Card(JACK, HEARTS),
      Card(QUEEN, HEARTS),   Card(KING, HEARTS),   Card(ACE, HEARTS),
      Card(NINE, CLUBS),     Card(TEN, CLUBS),     Card(JACK, CLUBS),
      Card(QUEEN, CLUBS),    Card(KING, CLUBS),    Card(ACE, CLUBS),
      Card(NINE, DIAMONDS),  Card(TEN, DIAMONDS),  Card(JACK, DIAMONDS),
      Card(QUEEN, DIAMONDS), Card(KING, DIAMONDS), Card(ACE, DIAMONDS)};

  for (int i = 0; i < PACK_SIZE; ++i) {
    Card card = pack.deal_one();
    ASSERT_EQUAL(card, expected_cards[i]);
  }
}

// Test that pack is initially not empty
TEST(test_pack_default_not_empty) {
  Pack pack;
  ASSERT_FALSE(pack.empty());
}

// Test istream constructor: reads from valid input stream
TEST(test_pack_istream_ctor_valid) {
  string input = create_pack_input();
  istringstream iss(input);
  Pack pack(iss);
  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(NINE, SPADES));
  // Verify a few more cards
  Card second_card = pack.deal_one();
  ASSERT_EQUAL(second_card, Card(TEN, SPADES));
  Card third_card = pack.deal_one();
  ASSERT_EQUAL(third_card, Card(JACK, SPADES));
}

// Test istream constructor with custom order
TEST(test_pack_istream_ctor_custom) {
  string input = create_custom_pack_input();
  istringstream iss(input);
  Pack pack(iss);

  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(NINE, SPADES));
  Card second_card = pack.deal_one();
  ASSERT_EQUAL(second_card, Card(TEN, HEARTS));
  Card third_card = pack.deal_one();
  ASSERT_EQUAL(third_card, Card(JACK, CLUBS));
  Card fourth_card = pack.deal_one();
  ASSERT_EQUAL(fourth_card, Card(QUEEN, DIAMONDS));
}

// Test istream constructor: handles partial input (less than PACK_SIZE)
TEST(test_pack_istream_ctor_partial) {
  stringstream ss;
  ss << Card(NINE, SPADES) << "\n" << Card(TEN, SPADES) << "\n";
  istringstream iss(ss.str());
  Pack pack(iss);
  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(NINE, SPADES));
  Card second_card = pack.deal_one();
  ASSERT_EQUAL(second_card, Card(TEN, SPADES));
  // Remaining cards are undefined but should not crash
  for (int i = 2; i < PACK_SIZE; ++i) {
    pack.deal_one(); // Should not throw
  }
}

// Test istream constructor is not empty after creation
TEST(test_pack_istream_ctor_not_empty) {
  string input = create_pack_input();
  istringstream iss(input);
  Pack pack(iss);
  ASSERT_FALSE(pack.empty());
}

// Test deal_one: ensures correct card dealing and index increment
TEST(test_pack_deal_one) {
  Pack pack;
  Card first = pack.deal_one();
  ASSERT_EQUAL(first, Card(NINE, SPADES));
  Card second = pack.deal_one();
  ASSERT_EQUAL(second, Card(TEN, SPADES));
  // Deal until one card remains
  for (int i = 2; i < PACK_SIZE - 1; ++i) {
    pack.deal_one();
  }
  Card last = pack.deal_one();
  ASSERT_EQUAL(last, Card(ACE, DIAMONDS));
}

// Test deal_one: dealing all cards in sequence
TEST(test_pack_deal_all_cards) {
  Pack pack;
  Card expected_cards[] = {
      Card(NINE, SPADES),    Card(TEN, SPADES),    Card(JACK, SPADES),
      Card(QUEEN, SPADES),   Card(KING, SPADES),   Card(ACE, SPADES),
      Card(NINE, HEARTS),    Card(TEN, HEARTS),    Card(JACK, HEARTS),
      Card(QUEEN, HEARTS),   Card(KING, HEARTS),   Card(ACE, HEARTS),
      Card(NINE, CLUBS),     Card(TEN, CLUBS),     Card(JACK, CLUBS),
      Card(QUEEN, CLUBS),    Card(KING, CLUBS),    Card(ACE, CLUBS),
      Card(NINE, DIAMONDS),  Card(TEN, DIAMONDS),  Card(JACK, DIAMONDS),
      Card(QUEEN, DIAMONDS), Card(KING, DIAMONDS), Card(ACE, DIAMONDS)};

  for (int i = 0; i < PACK_SIZE; ++i) {
    Card card = pack.deal_one();
    ASSERT_EQUAL(card, expected_cards[i]);
    if (i < PACK_SIZE - 1) {
      ASSERT_FALSE(pack.empty());
    }
  }
  ASSERT_TRUE(pack.empty());
}

// Test reset: ensures next index is set to 0
TEST(test_pack_reset) {
  Pack pack;
  // Deal a few cards
  pack.deal_one();
  pack.deal_one();
  pack.reset();
  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(NINE, SPADES));
  Card second_card = pack.deal_one();
  ASSERT_EQUAL(second_card, Card(TEN, SPADES));
}

// Test reset: pack is not empty after reset (even if was empty before)
TEST(test_pack_reset_from_empty) {
  Pack pack;
  // Deal all cards
  for (int i = 0; i < PACK_SIZE; ++i) {
    pack.deal_one();
  }
  ASSERT_TRUE(pack.empty());

  pack.reset();
  ASSERT_FALSE(pack.empty());

  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(NINE, SPADES));
}

// Test reset multiple times
TEST(test_pack_reset_multiple) {
  Pack pack;
  pack.deal_one(); // Deal Nine of Spades
  pack.deal_one(); // Deal Ten of Spades

  pack.reset();
  Card card1 = pack.deal_one();
  ASSERT_EQUAL(card1, Card(NINE, SPADES));

  pack.reset();
  Card card2 = pack.deal_one();
  ASSERT_EQUAL(card2, Card(NINE, SPADES));

  pack.reset();
  Card card3 = pack.deal_one();
  ASSERT_EQUAL(card3, Card(NINE, SPADES));
}

// Test empty: verifies empty state after dealing all cards
TEST(test_pack_empty) {
  Pack pack;
  ASSERT_FALSE(pack.empty());
  // Deal all but one card
  for (int i = 0; i < PACK_SIZE - 1; ++i) {
    pack.deal_one();
    ASSERT_FALSE(pack.empty());
  }
  // Deal the last card
  pack.deal_one();
  ASSERT_TRUE(pack.empty());
}

// Test empty: pack starts as not empty
TEST(test_pack_empty_initial_state) {
  Pack pack;
  ASSERT_FALSE(pack.empty());
}

// Test empty: after dealing one card
TEST(test_pack_empty_after_one_deal) {
  Pack pack;
  pack.deal_one();
  ASSERT_FALSE(pack.empty());
}

// Test shuffle: verifies in-shuffle behavior after seven shuffles
TEST(test_pack_shuffle) {
  Pack pack;
  pack.shuffle();

  // Expected order after seven in-shuffles (this is the known result for euchre
  // pack) This is based on the mathematical result of in-shuffling a 24-card
  // deck 7 times
  Card expected_cards[] = {
      Card(KING, CLUBS),     Card(JACK, HEARTS),  Card(NINE, SPADES),
      Card(ACE, CLUBS),      Card(QUEEN, HEARTS), Card(TEN, SPADES),
      Card(NINE, DIAMONDS),  Card(KING, HEARTS),  Card(JACK, SPADES),
      Card(TEN, DIAMONDS),   Card(ACE, HEARTS),   Card(QUEEN, SPADES),
      Card(JACK, DIAMONDS),  Card(NINE, CLUBS),   Card(KING, SPADES),
      Card(QUEEN, DIAMONDS), Card(TEN, CLUBS),    Card(ACE, SPADES),
      Card(KING, DIAMONDS),  Card(JACK, CLUBS),   Card(NINE, HEARTS),
      Card(ACE, DIAMONDS),   Card(QUEEN, CLUBS),  Card(TEN, HEARTS)};

  for (int i = 0; i < PACK_SIZE; ++i) {
    Card card = pack.deal_one();
    ASSERT_EQUAL(card, expected_cards[i]);
  }
}

// Test shuffle: ensures reset is called (next index is 0 after shuffle)
TEST(test_pack_shuffle_resets) {
  Pack pack;
  // Deal a few cards
  pack.deal_one();
  pack.deal_one();
  pack.shuffle();

  // Should start from beginning of shuffled deck
  Card first_card = pack.deal_one();
  ASSERT_EQUAL(first_card, Card(KING, CLUBS));

  // Pack should not be empty after shuffle
  ASSERT_FALSE(pack.empty());
}

// Test shuffle: multiple shuffles
TEST(test_pack_shuffle_multiple) {
  Pack pack;
  pack.shuffle();
  Card first_after_one = pack.deal_one();
  ASSERT_EQUAL(first_after_one, Card(KING, CLUBS));

  pack.reset();
  pack.shuffle();
  Card first_after_two = pack.deal_one();
  // After two shuffles (14 total in-shuffles), expect different order
  // The exact card depends on the shuffle algorithm implementation
  // Just verify a card is dealt without crash
  ASSERT_TRUE(first_after_two.get_rank() >= NINE &&
              first_after_two.get_rank() <= ACE);
}

// Test shuffle: pack is not empty after shuffle
TEST(test_pack_shuffle_not_empty) {
  Pack pack;
  pack.shuffle();
  ASSERT_FALSE(pack.empty());
}

// Test multiple operations: deal, shuffle, reset, and deal again
TEST(test_pack_combined_operations) {
  Pack pack;
  // Deal a few cards
  Card first = pack.deal_one();
  ASSERT_EQUAL(first, Card(NINE, SPADES));
  Card second = pack.deal_one();
  ASSERT_EQUAL(second, Card(TEN, SPADES));

  // Shuffle
  pack.shuffle();
  Card after_shuffle = pack.deal_one();
  ASSERT_EQUAL(after_shuffle, Card(KING, CLUBS));

  // Reset
  pack.reset();
  Card after_reset = pack.deal_one();
  ASSERT_EQUAL(after_reset, Card(KING, CLUBS)); // Same as post-shuffle start

  // Deal until empty
  for (int i = 1; i < PACK_SIZE; ++i) {
    pack.deal_one();
  }
  ASSERT_TRUE(pack.empty());
}

// Test complex sequence: reset, deal, shuffle, deal, reset, deal
TEST(test_pack_complex_sequence) {
  Pack pack;

  // Reset (no-op on fresh pack)
  pack.reset();
  Card card1 = pack.deal_one();
  ASSERT_EQUAL(card1, Card(NINE, SPADES));

  // Deal a few more
  Card card2 = pack.deal_one();
  Card card3 = pack.deal_one();
  ASSERT_EQUAL(card2, Card(TEN, SPADES));
  ASSERT_EQUAL(card3, Card(JACK, SPADES));

  // Shuffle
  pack.shuffle();
  Card after_shuffle = pack.deal_one();
  ASSERT_EQUAL(after_shuffle, Card(KING, CLUBS));

  // Deal one more from shuffled pack
  Card shuffled_second = pack.deal_one();
  ASSERT_EQUAL(shuffled_second, Card(JACK, HEARTS));

  // Reset
  pack.reset();
  Card after_reset = pack.deal_one();
  ASSERT_EQUAL(after_reset, Card(KING, CLUBS));
}

// Test with istream pack and operations
TEST(test_pack_istream_with_operations) {
  string input = create_custom_pack_input();
  istringstream iss(input);
  Pack pack(iss);

  // Deal first card
  Card first = pack.deal_one();
  ASSERT_EQUAL(first, Card(NINE, SPADES));

  // Reset
  pack.reset();
  Card after_reset = pack.deal_one();
  ASSERT_EQUAL(after_reset, Card(NINE, SPADES));

  // Deal a few more
  Card second = pack.deal_one();
  Card third = pack.deal_one();
  ASSERT_EQUAL(second, Card(TEN, HEARTS));
  ASSERT_EQUAL(third, Card(JACK, CLUBS));

  // Shuffle and test
  pack.shuffle();
  Card after_shuffle = pack.deal_one();
  // Should be some valid card
  ASSERT_TRUE(after_shuffle.get_rank() >= TWO &&
              after_shuffle.get_rank() <= ACE);
}

// Test edge case: empty and deal all
TEST(test_pack_deal_until_empty) {
  Pack pack;
  int cards_dealt = 0;

  while (!pack.empty()) {
    pack.deal_one();
    cards_dealt++;
  }

  ASSERT_EQUAL(cards_dealt, PACK_SIZE);
  ASSERT_TRUE(pack.empty());
}

// Test consistency: same operations should yield same results
TEST(test_pack_consistency) {
  Pack pack1;
  Pack pack2;

  // Both packs should deal same sequence
  for (int i = 0; i < 5; ++i) {
    Card card1 = pack1.deal_one();
    Card card2 = pack2.deal_one();
    ASSERT_EQUAL(card1, card2);
  }

  // Both packs should shuffle to same result
  pack1.shuffle();
  pack2.shuffle();

  for (int i = 0; i < 5; ++i) {
    Card card1 = pack1.deal_one();
    Card card2 = pack2.deal_one();
    ASSERT_EQUAL(card1, card2);
  }
}

// Test pack with file input simulation
TEST(test_pack_file_like_input) {
  stringstream ss;
  // Write cards in specific order
  ss << "Ace of Spades\n";
  ss << "King of Hearts\n";
  ss << "Queen of Clubs\n";
  ss << "Jack of Diamonds\n";

  // Fill remaining with standard cards
  for (int i = 4; i < PACK_SIZE; ++i) {
    ss << "Nine of Spades\n";
  }

  istringstream iss(ss.str());
  Pack pack(iss);

  Card card1 = pack.deal_one();
  Card card2 = pack.deal_one();
  Card card3 = pack.deal_one();
  Card card4 = pack.deal_one();

  ASSERT_EQUAL(card1, Card(ACE, SPADES));
  ASSERT_EQUAL(card2, Card(KING, HEARTS));
  ASSERT_EQUAL(card3, Card(QUEEN, CLUBS));
  ASSERT_EQUAL(card4, Card(JACK, DIAMONDS));
}

TEST_MAIN()
