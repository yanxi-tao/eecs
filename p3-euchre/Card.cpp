#include "Card.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

using namespace std;

/////////////// Rank operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const RANK_NAMES[] = {
    "Two",   // TWO
    "Three", // THREE
    "Four",  // FOUR
    "Five",  // FIVE
    "Six",   // SIX
    "Seven", // SEVEN
    "Eight", // EIGHT
    "Nine",  // NINE
    "Ten",   // TEN
    "Jack",  // JACK
    "Queen", // QUEEN
    "King",  // KING
    "Ace"    // ACE
};

// REQUIRES str represents a valid rank ("Two", "Three", ..., "Ace")
// EFFECTS returns the Rank corresponding to str, for example "Two" -> TWO
Rank string_to_rank(const std::string &str) {
  for (int r = TWO; r <= ACE; ++r) {
    if (str == RANK_NAMES[r]) {
      return static_cast<Rank>(r);
    }
  }
  assert(false); // Input string didn't match any rank
  return {};
}

// EFFECTS Prints Rank to stream, for example "Two"
std::ostream &operator<<(std::ostream &os, Rank rank) {
  os << RANK_NAMES[rank];
  return os;
}

// REQUIRES If any input is read, it must be a valid rank
// EFFECTS Reads a Rank from a stream, for example "Two" -> TWO
std::istream &operator>>(std::istream &is, Rank &rank) {
  string str;
  if (is >> str) {
    rank = string_to_rank(str);
  }
  return is;
}

/////////////// Suit operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const SUIT_NAMES[] = {
    "Spades",   // SPADES
    "Hearts",   // HEARTS
    "Clubs",    // CLUBS
    "Diamonds", // DIAMONDS
};

// REQUIRES str represents a valid suit ("Spades", "Hearts", "Clubs", or
// "Diamonds") EFFECTS returns the Suit corresponding to str, for example
// "Clubs" -> CLUBS
Suit string_to_suit(const std::string &str) {
  for (int s = SPADES; s <= DIAMONDS; ++s) {
    if (str == SUIT_NAMES[s]) {
      return static_cast<Suit>(s);
    }
  }
  assert(false); // Input string didn't match any suit
  return {};
}

// EFFECTS Prints Suit to stream, for example "Spades"
std::ostream &operator<<(std::ostream &os, Suit suit) {
  os << SUIT_NAMES[suit];
  return os;
}

// REQUIRES If any input is read, it must be a valid suit
// EFFECTS Reads a Suit from a stream, for example "Spades" -> SPADES
std::istream &operator>>(std::istream &is, Suit &suit) {
  string str;
  if (is >> str) {
    suit = string_to_suit(str);
  }
  return is;
}

/////////////// Write your implementation for Card below ///////////////

// NOTE: We HIGHLY recommend you check out the operator overloading
// tutorial in the project spec before implementing
// the following operator overload functions:
//   operator<<
//   operator>>
//   operator<
//   operator<=
//   operator>
//   operator>=
//   operator==
//   operator!=

Card::Card() : rank(TWO), suit(SPADES) {}

Card::Card(Rank rank_in, Suit suit_in) : rank(rank_in), suit(suit_in) {}

Rank Card::get_rank() const { return rank; }

Suit Card::get_suit() const { return suit; }

Suit Card::get_suit(Suit trump) const {
  if (is_left_bower(trump)) {
    return trump;
  } else {
    return suit;
  }
}

bool Card::is_face_or_ace() const { return rank >= JACK; }

bool Card::is_right_bower(Suit trump) const {
  return rank == JACK && suit == trump;
}

bool Card::is_left_bower(Suit trump) const {
  if (rank != JACK) {
    return false;
  }
  switch (trump) {
  case SPADES:
    return suit == CLUBS;
  case HEARTS:
    return suit == DIAMONDS;
  case CLUBS:
    return suit == SPADES;
  case DIAMONDS:
    return suit == HEARTS;
  default:
    return false; // should never reach here
  }
}

bool Card::is_trump(Suit trump) const {
  return suit == trump || is_left_bower(trump);
}

std::ostream &operator<<(std::ostream &os, const Card &card) {
  os << card.get_rank() << " of " << card.get_suit();
  return os;
}

std::istream &operator>>(std::istream &is, Card &card) {
  Rank rank;
  Suit suit;
  string of;
  if (is >> rank >> of >> suit) {
    card = Card(rank, suit);
  }
  return is;
}

bool operator<(const Card &a, const Card &b) {
  if (a.get_rank() != b.get_rank()) {
    return a.get_rank() < b.get_rank();
  } else {
    return a.get_suit() < b.get_suit();
  }
}

bool operator<=(const Card &a, const Card &b) { return !(a > b); }

bool operator>(const Card &a, const Card &b) { return b < a; }

bool operator>=(const Card &a, const Card &b) { return !(a < b); }

bool operator==(const Card &a, const Card &b) {
  return a.get_rank() == b.get_rank() && a.get_suit() == b.get_suit();
}

bool operator!=(const Card &a, const Card &b) { return !(a == b); }

Suit Suit_next(Suit suit) {
  switch (suit) {
  case SPADES:
    return CLUBS;
  case HEARTS:
    return DIAMONDS;
  case CLUBS:
    return SPADES;
  case DIAMONDS:
    return HEARTS;
  default:
    return SPADES; // should never reach here
  }
}

bool Card_less(const Card &a, const Card &b, Suit trump) {
  bool a_is_trump = a.is_trump(trump);
  bool b_is_trump = b.is_trump(trump);
  if (a_is_trump && !b_is_trump) {
    return false;
  } else if (!a_is_trump && b_is_trump) {
    return true;
  } else if (a_is_trump && b_is_trump) {
    if (a.is_right_bower(trump)) {
      return false;
    } else if (b.is_right_bower(trump)) {
      return true;
    } else if (a.is_left_bower(trump)) {
      return false;
    } else if (b.is_left_bower(trump)) {
      return true;
    } else {
      return a < b; // both are trump but not bowers, compare normally
    }
  } else {
    return a < b; // both are non-trump, compare normally
  }
}

bool Card_less(const Card &a, const Card &b, const Card &led_card, Suit trump) {
  bool a_is_trump = a.is_trump(trump);
  bool b_is_trump = b.is_trump(trump);
  if (a_is_trump && !b_is_trump) {
    return false;
  } else if (!a_is_trump && b_is_trump) {
    return true;
  } else if (a_is_trump && b_is_trump) {
    if (a.is_right_bower(trump)) {
      return false;
    } else if (b.is_right_bower(trump)) {
      return true;
    } else if (a.is_left_bower(trump)) {
      return false;
    } else if (b.is_left_bower(trump)) {
      return true;
    } else {
      return a < b; // both are trump but not bowers, compare normally
    }
  } else { // both are non-trump
    bool a_is_led = a.get_suit() == led_card.get_suit();
    bool b_is_led = b.get_suit() == led_card.get_suit();
    if (a_is_led && !b_is_led) {
      return false;
    } else if (!a_is_led && b_is_led) {
      return true;
    } else if (a_is_led && b_is_led) {
      return a < b; // both are led, compare normally
    } else {
      return a < b; // both are non-trump and non-led, compare normally
    }
  }
}
