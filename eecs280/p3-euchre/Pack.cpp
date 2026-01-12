#include "Pack.hpp"
#include "Card.hpp"

Pack::Pack() : next(0) {
  // Initialize the pack in standard order
  int index = 0;
  for (int suit = SPADES; suit <= DIAMONDS; suit++) {
    for (int rank = NINE; rank <= ACE; rank++) {
      cards[index++] = Card(static_cast<Rank>(rank), static_cast<Suit>(suit));
    }
  }
}

Pack::Pack(std::istream &pack_input) : next(0) {
  int index = 0;
  while (pack_input >> cards[index]) {
    index++;
    if (index >= PACK_SIZE) {
      break; // Prevent overflow if more than PACK_SIZE cards are provided
    }
  }
}

Card Pack::deal_one() { return cards[next++]; }

void Pack::reset() { next = 0; }

void Pack::shuffle() {
  // Perform an in shuffle seven times
  for (int shuffle_count = 0; shuffle_count < 7; shuffle_count++) {
    std::array<Card, PACK_SIZE> shuffled;
    int mid = PACK_SIZE / 2;
    int index = 0;
    for (int i = 0; i < mid; i++) {
      shuffled[index++] = cards[i + mid]; // Take from the second half
      shuffled[index++] = cards[i];       // Then take from the first half
    }
    cards.swap(shuffled); // Update the pack with the shuffled cards
  }
  reset(); // Reset next index after shuffling
}

bool Pack::empty() const { return next >= PACK_SIZE; }
