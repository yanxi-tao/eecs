#include "Player.hpp"
#include "Card.hpp"
#include <algorithm>
#include <cassert>

class SimplePlayer : public Player {
public:
  SimplePlayer(const std::string &name) : name(name) {}

  const std::string &get_name() const override { return name; }

  void add_card(const Card &c) override {
    assert(hand.size() < 5);
    hand.push_back(c);
  }

  bool make_trump(const Card &upcard, bool is_dealer, int round,
                  Suit &order_up_suit) const override {
    if (round == 1) {
      int trump_count = 0;
      for (const Card &c : hand) {
        if (c.is_trump(upcard.get_suit()) && c.is_face_or_ace()) {
          trump_count++;
        }
      }

      if (trump_count >= 2) {
        order_up_suit = upcard.get_suit();
        return true;
      } else {
        return false;
      }
    } else if (round == 2) {
      Suit next_suit = Suit_next(upcard.get_suit());
      if (is_dealer) {
        order_up_suit = next_suit;
        return true;
      } else {
        for (const Card &c : hand) {
          if (c.is_trump(next_suit) && c.is_face_or_ace()) {
            order_up_suit = next_suit;
            return true;
          }
        }
        return false;
      }
    }

    assert(false);
  }

  void add_and_discard(const Card &upcard) override {
    Suit trump = upcard.get_suit();
    hand.push_back(upcard);
    int discard_index = 0;
    for (int i = 1; i < hand.size(); i++) {
      if (Card_less(hand[i], hand[discard_index], trump)) {
        discard_index = i;
      }
    }
    hand.erase(hand.begin() + discard_index);
    assert(hand.size() == 5);
  }

  Card lead_card(Suit trump) override {
    assert(!hand.empty());
    int highest_non_trump_index = -1;
    for (int i = 0; i < hand.size(); i++) {
      if (!hand[i].is_trump(trump)) {
        if (highest_non_trump_index == -1 ||
            hand[i] > hand[highest_non_trump_index]) {
          highest_non_trump_index = i;
        }
      }
    }

    if (highest_non_trump_index != -1) {
      Card lead_card = hand[highest_non_trump_index];
      hand.erase(hand.begin() + highest_non_trump_index);
      return lead_card;
    } else {
      int highest_trump_index = 0;
      for (int i = 1; i < hand.size(); i++) {
        if (hand[i].is_trump(trump) &&
            Card_less(hand[highest_trump_index], hand[i], trump)) {
          highest_trump_index = i;
        }
      }
      Card lead_card = hand[highest_trump_index];
      hand.erase(hand.begin() + highest_trump_index);
      return lead_card;
    }
  }

  Card play_card(const Card &led_card, Suit trump) override {
    assert(!hand.empty());
    Suit led_suit = led_card.get_suit(trump);
    int follow_suit_index = -1;
    for (int i = 0; i < hand.size(); i++) {
      if (hand[i].get_suit(trump) == led_suit) {
        if (follow_suit_index == -1 ||
            Card_less(hand[follow_suit_index], hand[i], led_card, trump)) {
          follow_suit_index = i;
        }
      }
    }

    if (follow_suit_index != -1) {
      Card play_card = hand[follow_suit_index];
      hand.erase(hand.begin() + follow_suit_index);
      return play_card;
    } else {
      int lowest_card_index = 0;
      for (int i = 1; i < hand.size(); i++) {
        if (Card_less(hand[i], hand[lowest_card_index], led_card, trump)) {
          lowest_card_index = i;
        }
      }
      Card play_card = hand[lowest_card_index];
      hand.erase(hand.begin() + lowest_card_index);
      return play_card;
    }
  }

private:
  std::string name;
  std::vector<Card> hand;
};

class HumanPlayer : public Player {
public:
  HumanPlayer(const std::string &name) : name(name) {}

  const std::string &get_name() const override { return name; }

  void add_card(const Card &c) override {
    assert(hand.size() < 5);
    hand.push_back(c);
    std::sort(hand.begin(), hand.end());
  }

  bool make_trump(const Card &upcard, bool is_dealer, int round,
                  Suit &order_up_suit) const override {
    print_hand();
    std::cout << "Human player " << name
              << ", please enter a suit, or \"pass\":\n";

    std::string input;
    std::cin >> input;
    if (input != "pass") {
      order_up_suit = string_to_suit(input);
      return true;
    } else {
      return false;
    }
  }

  void add_and_discard(const Card &upcard) override {
    print_hand();
    std::cout << "Discard upcard: [-1]\n";
    std::cout << "Human player " << name
              << ", please select a card to discard:\n"
              << std::endl;
    int index;
    std::cin >> index;
    if (index == -1) {
      return;
    } else {
      assert(index >= 0 && index < hand.size());
      hand[index] = upcard;
      return;
    }
  }

  Card lead_card(Suit trump) override {
    print_hand();
    std::cout << "Human player " << name << ", please select a card:\n";

    int index;
    std::cin >> index;
    assert(index >= 0 && index < hand.size());
    Card lead_card = hand[index];
    hand.erase(hand.begin() + index);
    return lead_card;
  }

  Card play_card(const Card &led_card, Suit trump) override {
    print_hand();
    std::cout << "Human player " << name << ", please select a card:\n";

    int index;
    std::cin >> index;
    assert(index >= 0 && index < hand.size());
    Card play_card = hand[index];
    hand.erase(hand.begin() + index);
    return play_card;
  }

private:
  std::string name;
  std::vector<Card> hand;
  void print_hand() const {
    for (size_t i = 0; i < hand.size(); ++i)
      std::cout << "Human player " << name << "'s hand: "
                << "[" << i << "] " << hand[i] << "\n";
  }
};

Player *Player_factory(const std::string &name, const std::string &strategy) {
  if (strategy == "Simple") {
    return new SimplePlayer(name);
  } else if (strategy == "Human") {
    return new HumanPlayer(name);
  } else {
    assert(false);
    return nullptr;
  }
}

std::ostream &operator<<(std::ostream &os, const Player &p) {
  os << p.get_name();
  return os;
}
