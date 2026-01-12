#include "Card.hpp"
#include "Player.hpp"
#include "unit_test_framework.hpp"

#include <iostream>

using namespace std;

TEST(test_simple_player_make_trump_round1_orderup) {
  Player *bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(QUEEN, SPADES));
  bob->add_card(Card(KING, SPADES));
  bob->add_card(Card(ACE, HEARTS));
  bob->add_card(Card(NINE, DIAMONDS));
  bob->add_card(Card(TEN, CLUBS));

  Card up(NINE, SPADES);
  Suit trump;
  bool orderup = bob->make_trump(up, false, 1, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, SPADES);

  delete bob;
}

TEST(test_simple_player_make_trump_round1_exactly_two_trump_face_cards) {
  Player *alice = Player_factory("Alice", "Simple");
  alice->add_card(Card(QUEEN, HEARTS));
  alice->add_card(Card(KING, HEARTS));
  alice->add_card(Card(NINE, CLUBS));
  alice->add_card(Card(TEN, DIAMONDS));
  alice->add_card(Card(ACE, SPADES));

  Card up(TEN, HEARTS);
  Suit trump;
  bool orderup = alice->make_trump(up, false, 1, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, HEARTS);

  delete alice;
}

TEST(test_simple_player_make_trump_round1_pass) {
  Player *alice = Player_factory("Alice", "Simple");
  alice->add_card(Card(NINE, SPADES));
  alice->add_card(Card(TEN, HEARTS));
  alice->add_card(Card(TEN, DIAMONDS));
  alice->add_card(Card(NINE, CLUBS));
  alice->add_card(Card(ACE, HEARTS));

  Card up(TEN, SPADES);
  Suit trump;
  bool orderup = alice->make_trump(up, false, 1, trump);

  ASSERT_FALSE(orderup);

  delete alice;
}

TEST(test_simple_player_make_trump_round2_non_dealer_orderup) {
  Player *carol = Player_factory("Carol", "Simple");
  carol->add_card(Card(KING, DIAMONDS));
  carol->add_card(Card(TEN, HEARTS));
  carol->add_card(Card(ACE, CLUBS));
  carol->add_card(Card(NINE, CLUBS));
  carol->add_card(Card(JACK, SPADES));

  Card up(NINE, HEARTS);
  Suit trump;
  bool orderup = carol->make_trump(up, false, 2, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, DIAMONDS); // same color as HEARTS

  delete carol;
}

TEST(test_simple_player_make_trump_round2_non_dealer_pass) {
  Player *dave = Player_factory("Dave", "Simple");
  dave->add_card(Card(NINE, CLUBS));
  dave->add_card(Card(TEN, SPADES));
  dave->add_card(Card(TEN, DIAMONDS));
  dave->add_card(Card(NINE, HEARTS));
  dave->add_card(Card(TEN, CLUBS));

  Card up(JACK, HEARTS);
  Suit trump;
  bool orderup = dave->make_trump(up, false, 2, trump);

  ASSERT_FALSE(orderup);

  delete dave;
}

TEST(test_simple_player_make_trump_round2_dealer_forced) {
  Player *eve = Player_factory("Eve", "Simple");
  eve->add_card(Card(NINE, CLUBS));
  eve->add_card(Card(TEN, SPADES));
  eve->add_card(Card(TEN, DIAMONDS));
  eve->add_card(Card(NINE, HEARTS));
  eve->add_card(Card(TEN, CLUBS));

  Card up(JACK, HEARTS);
  Suit trump;
  bool orderup = eve->make_trump(up, true, 2, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, DIAMONDS);

  delete eve;
}

TEST(test_simple_player_make_trump_round1_two_low_trump) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(NINE, SPADES));
  p->add_card(Card(TEN, SPADES));
  p->add_card(Card(NINE, CLUBS));
  p->add_card(Card(TEN, HEARTS));
  p->add_card(Card(NINE, DIAMONDS));

  Card up(QUEEN, SPADES);
  Suit trump;
  bool orderup = p->make_trump(up, false, 1, trump);

  ASSERT_FALSE(orderup); // only low trumps, no face/ace

  delete p;
}

TEST(test_simple_player_make_trump_round1_left_bower_counts) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(JACK, DIAMONDS)); // left bower if Hearts trump
  p->add_card(Card(ACE, HEARTS));
  p->add_card(Card(TEN, SPADES));
  p->add_card(Card(NINE, CLUBS));
  p->add_card(Card(TEN, DIAMONDS));

  Card up(KING, HEARTS);
  Suit trump;
  bool orderup = p->make_trump(up, false, 1, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, HEARTS);

  delete p;
}

TEST(test_simple_player_make_trump_round1_dealer_not_special) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(NINE, SPADES));
  p->add_card(Card(TEN, DIAMONDS));
  p->add_card(Card(NINE, CLUBS));
  p->add_card(Card(TEN, HEARTS));
  p->add_card(Card(NINE, DIAMONDS));

  Card up(KING, HEARTS);
  Suit trump;
  bool orderup = p->make_trump(up, true, 1, trump);

  ASSERT_FALSE(orderup); // being dealer doesn’t change round 1 rule

  delete p;
}

TEST(test_simple_player_make_trump_round1_one_trump_face_card) {
  Player *bob = Player_factory("Bob", "Simple");
  bob->add_card(Card(QUEEN, SPADES));
  bob->add_card(Card(TEN, HEARTS));
  bob->add_card(Card(NINE, DIAMONDS));
  bob->add_card(Card(TEN, CLUBS));
  bob->add_card(Card(ACE, HEARTS));

  Card up(NINE, SPADES);
  Suit trump;
  bool orderup = bob->make_trump(up, false, 1, trump);

  ASSERT_FALSE(orderup);

  delete bob;
}

TEST(test_simple_player_make_trump_round2_bower) {
  Player *frank = Player_factory("Frank", "Simple");
  frank->add_card(Card(JACK, DIAMONDS)); // Left bower for HEARTS
  frank->add_card(Card(TEN, SPADES));
  frank->add_card(Card(NINE, CLUBS));
  frank->add_card(Card(TEN, DIAMONDS));
  frank->add_card(Card(ACE, CLUBS));

  Card up(NINE, SPADES);
  Suit trump;
  bool orderup = frank->make_trump(up, false, 2, trump);

  ASSERT_TRUE(orderup);
  ASSERT_EQUAL(trump, CLUBS);

  delete frank;
}

//
// --- add_and_discard tests ---
//

TEST(test_simple_player_add_and_discard_lowest_removed) {
  Player *frank = Player_factory("Frank", "Simple");
  frank->add_card(Card(NINE, SPADES));
  frank->add_card(Card(TEN, HEARTS));
  frank->add_card(Card(JACK, DIAMONDS));
  frank->add_card(Card(QUEEN, CLUBS));
  frank->add_card(Card(KING, SPADES));

  Card up(ACE, SPADES); // trump = SPADES
  frank->add_and_discard(up);

  // TEN of HEARTS is weakest → should be discarded
  for (int i = 0; i < 5; i++) {
    Card c = frank->lead_card(SPADES);
    ASSERT_NOT_EQUAL(c, Card(TEN, HEARTS));
  }

  delete frank;
}

TEST(test_simple_player_add_and_discard_upcard_removed) {
  Player *frank = Player_factory("Frank", "Simple");
  frank->add_card(Card(JACK, SPADES));
  frank->add_card(Card(TEN, CLUBS));
  frank->add_card(Card(JACK, CLUBS));
  frank->add_card(Card(QUEEN, CLUBS));
  frank->add_card(Card(KING, CLUBS));

  Card up(NINE, CLUBS); // upcard is weakest club
  frank->add_and_discard(up);

  for (int i = 0; i < 5; ++i) {
    Card c = frank->lead_card(HEARTS);      // pull cards one by one
    ASSERT_NOT_EQUAL(c, Card(NINE, CLUBS)); // should not have left bower
  }

  delete frank;
}

TEST(test_simple_player_add_and_discard_left_bower_not_discarded) {
  Player *p = Player_factory("P", "Simple");

  // HEARTS is trump (upcard KING of HEARTS)
  p->add_card(Card(JACK, DIAMONDS)); // left bower (strongest after right)
  p->add_card(Card(NINE, CLUBS));    // weak
  p->add_card(Card(TEN, CLUBS));     // weak
  p->add_card(Card(ACE, SPADES));    // decent
  p->add_card(Card(KING, CLUBS));    // decent

  Card up(KING, HEARTS);
  p->add_and_discard(up);

  // After discarding, hand should still contain the left bower
  bool has_left_bower = false;
  for (int i = 0; i < 5; ++i) {
    Card c = p->lead_card(HEARTS); // pull cards one by one
    if (c == Card(JACK, DIAMONDS)) {
      has_left_bower = true;
    }
  }

  ASSERT_TRUE(has_left_bower);

  delete p;
}

//
// --- lead_card tests ---
//

TEST(test_simple_player_lead_card_non_trump) {
  Player *gina = Player_factory("Gina", "Simple");
  gina->add_card(Card(ACE, HEARTS));
  gina->add_card(Card(KING, CLUBS));
  gina->add_card(Card(JACK, DIAMONDS));
  gina->add_card(Card(TEN, SPADES));
  gina->add_card(Card(NINE, SPADES));

  Suit trump = SPADES;
  Card led = gina->lead_card(trump);

  ASSERT_EQUAL(led, Card(ACE, HEARTS));

  delete gina;
}

TEST(test_simple_player_lead_card_all_trump) {
  Player *hank = Player_factory("Hank", "Simple");
  hank->add_card(Card(NINE, SPADES));
  hank->add_card(Card(TEN, SPADES));
  hank->add_card(Card(JACK, SPADES));
  hank->add_card(Card(QUEEN, SPADES));
  hank->add_card(Card(KING, SPADES));

  Suit trump = SPADES;
  Card led = hank->lead_card(trump);

  ASSERT_EQUAL(led, Card(JACK, SPADES)); // right bower

  delete hank;
}

TEST(test_simple_player_lead_card_highest_non_trump) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(KING, HEARTS));
  p->add_card(Card(ACE, DIAMONDS));
  p->add_card(Card(QUEEN, CLUBS));
  p->add_card(Card(JACK, SPADES));
  p->add_card(Card(NINE, SPADES));

  Suit trump = SPADES;
  Card led = p->lead_card(trump);

  ASSERT_EQUAL(led, Card(ACE, DIAMONDS)); // highest non-trump

  delete p;
}

TEST(test_simple_player_lead_card_bower_priority) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(JACK, DIAMONDS)); // left bower (Hearts trump)
  p->add_card(Card(JACK, HEARTS));   // right bower
  p->add_card(Card(ACE, HEARTS));
  p->add_card(Card(KING, HEARTS));
  p->add_card(Card(QUEEN, HEARTS));

  Suit trump = HEARTS;
  Card led = p->lead_card(trump);

  ASSERT_EQUAL(led, Card(JACK, HEARTS)); // right bower must lead

  delete p;
}

//
// --- play_card tests ---
//

TEST(test_simple_player_play_card_follow_suit) {
  Player *ivan = Player_factory("Ivan", "Simple");
  ivan->add_card(Card(TEN, HEARTS));
  ivan->add_card(Card(KING, HEARTS));
  ivan->add_card(Card(ACE, CLUBS));
  ivan->add_card(Card(NINE, SPADES));
  ivan->add_card(Card(TEN, DIAMONDS));

  Suit trump = SPADES;
  Card led = Card(QUEEN, HEARTS);
  Card played = ivan->play_card(led, trump);

  ASSERT_EQUAL(played, Card(KING, HEARTS));

  delete ivan;
}

TEST(test_simple_player_play_card_cannot_follow) {
  Player *jill = Player_factory("Jill", "Simple");
  jill->add_card(Card(NINE, SPADES));
  jill->add_card(Card(TEN, DIAMONDS));
  jill->add_card(Card(JACK, CLUBS));
  jill->add_card(Card(KING, SPADES));
  jill->add_card(Card(ACE, DIAMONDS));

  Suit trump = SPADES;
  Card led = Card(QUEEN, HEARTS);
  Card played = jill->play_card(led, trump);

  // No HEARTS, so play lowest card overall.
  // TEN of DIAMONDS is lowest here.
  ASSERT_EQUAL(played, Card(TEN, DIAMONDS));

  delete jill;
}

TEST(test_simple_player_play_card_follow_suit_highest) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(NINE, HEARTS));
  p->add_card(Card(KING, HEARTS));
  p->add_card(Card(ACE, HEARTS));
  p->add_card(Card(TEN, CLUBS));
  p->add_card(Card(JACK, DIAMONDS));

  Suit trump = SPADES;
  Card led = Card(QUEEN, HEARTS);
  Card played = p->play_card(led, trump);

  ASSERT_EQUAL(played, Card(ACE, HEARTS)); // strongest HEART

  delete p;
}

TEST(test_simple_player_play_card_trump_led) {
  Player *p = Player_factory("P", "Simple");
  p->add_card(Card(NINE, SPADES));
  p->add_card(Card(JACK, CLUBS));  // left bower if spades trump
  p->add_card(Card(JACK, SPADES)); // right bower
  p->add_card(Card(ACE, CLUBS));
  p->add_card(Card(TEN, DIAMONDS));

  Suit trump = SPADES;
  Card led = Card(QUEEN, SPADES); // trump led
  Card played = p->play_card(led, trump);

  ASSERT_EQUAL(played, Card(JACK, SPADES)); // right bower wins

  delete p;
}

TEST(test_simple_player_play_card_left_bower) {
  Player *lisa = Player_factory("Lisa", "Simple");
  lisa->add_card(Card(JACK, DIAMONDS)); // Left bower for HEARTS
  lisa->add_card(Card(TEN, CLUBS));
  lisa->add_card(Card(NINE, SPADES));
  lisa->add_card(Card(ACE, CLUBS));
  lisa->add_card(Card(KING, HEARTS));

  Suit trump = HEARTS;
  Card led = Card(TEN, HEARTS);
  Card played = lisa->play_card(led, trump);

  ASSERT_EQUAL(played, Card(JACK, DIAMONDS)); // Left bower

  delete lisa;
}

// --- get_name test ---

TEST(test_simple_player_get_name) {
  Player *mike = Player_factory("Mike", "Simple");
  ASSERT_EQUAL(mike->get_name(), "Mike");
  delete mike;
}

TEST_MAIN()
