#include "Card.hpp"
#include "Pack.hpp"
#include "Player.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <utility>
#include <vector>
using namespace std;

enum Team { EVEN, ODD };

class Game {
public:
  Game(istream &fin, bool is_shuffle, int pts,
       const vector<pair<string, string>> &players)
      : pack(fin), points_to_win(pts), is_shuffle(is_shuffle) {
    for (int i = 0; i < players.size(); i++) {
      this->players.push_back(
          Player_factory(players[i].first, players[i].second));
    }
  }
  void play() {
    int pts_even = 0;
    int pts_odd = 0;

    int dealer_index = 0;
    int hand_count = 0;
    while (pts_even < points_to_win && pts_odd < points_to_win) {
      pair<int, int> hand_result = play_hand(dealer_index, hand_count);
      hand_count++;

      pts_even += hand_result.first;
      pts_odd += hand_result.second;

      cout << players.at(0)->get_name() << " and " << players.at(2)->get_name()
           << " have " << pts_even << " points" << endl;
      cout << players.at(1)->get_name() << " and " << players.at(3)->get_name()
           << " have " << pts_odd << " points\n"
           << endl;
      dealer_index = (dealer_index + 1) % 4;
    }

    if (pts_even >= points_to_win) {
      cout << players.at(0)->get_name() << " and " << players.at(2)->get_name()
           << " win!" << endl;
    } else {
      cout << players.at(1)->get_name() << " and " << players.at(3)->get_name()
           << " win!" << endl;
    }

    clean_up();
  }

private:
  vector<Player *> players;
  Pack pack;
  int points_to_win;
  bool is_shuffle;

  void shuffle_pack() {
    if (is_shuffle) {
      pack.shuffle();
    } else {
      pack.reset();
    }
  }

  void deal_cards(int dealer_index) {
    // First pass of dealing: 3 cards, then 2, then 3, then 2
    deal_pass(dealer_index, true);

    // Second pass of dealing: 2 cards, then 3, then 2, then 3
    deal_pass(dealer_index, false);
  }

  void deal_pass(int dealer_index, bool start_with_three) {
    bool deal_three_cards = start_with_three;
    for (int i = 0; i < 4; ++i) {
      int player_index = (dealer_index + 1 + i) % 4;
      int num_to_deal = deal_three_cards ? 3 : 2;

      for (int j = 0; j < num_to_deal; ++j) {
        players[player_index]->add_card(pack.deal_one());
      }

      deal_three_cards = !deal_three_cards;
    }
  }

  Team make_trump(int dealer_index, Suit &ordered_up_suit) {
    Card upcard = pack.deal_one();

    cout << upcard << " turned up" << endl;

    for (int i = 0; i < 4; ++i) {
      int player_index = (dealer_index + 1 + i) % 4;
      if (players.at(player_index)
              ->make_trump(upcard, player_index == dealer_index, 1,
                           ordered_up_suit)) {
        cout << players.at(player_index)->get_name() << " orders up "
             << ordered_up_suit << endl;
        players.at(dealer_index)->add_and_discard(upcard);

        return player_index % 2 == 0 ? EVEN : ODD;
      } else {
        cout << players.at(player_index)->get_name() << " passes" << endl;
      }
    }

    for (int i = 0; i < 4; ++i) {
      int player_index = (dealer_index + 1 + i) % 4;
      if (players.at(player_index)
              ->make_trump(upcard, player_index == dealer_index, 2,
                           ordered_up_suit)) {
        cout << players.at(player_index)->get_name() << " orders up "
             << ordered_up_suit << "\n"
             << endl;
        return player_index % 2 == 0 ? EVEN : ODD;
      } else {
        cout << players.at(player_index)->get_name() << " passes" << endl;
      }
    }

    assert(false); // Should never reach here
  }

  int play_trick(int leader_index, Suit trump) {
    Card led_card = players.at(leader_index)->lead_card(trump);

    cout << led_card << " led by " << players.at(leader_index)->get_name()
         << endl;

    int winning_index = leader_index;
    Card winning_card = led_card;

    for (int i = 1; i < 4; ++i) {
      int player_index = (leader_index + i) % 4;
      Card played_card = players.at(player_index)->play_card(led_card, trump);

      cout << played_card << " played by "
           << players.at(player_index)->get_name() << endl;

      if (Card_less(winning_card, played_card, led_card, trump)) {
        winning_card = played_card;
        winning_index = player_index;
      }
    }

    cout << players.at(winning_index)->get_name() << " takes the trick\n"
         << endl;
    return winning_index;
  }

  pair<int, int> play_hand(int dealer_index, int hand_count) {
    cout << "Hand " << hand_count << endl;
    cout << players.at(dealer_index)->get_name() << " deals" << endl;

    shuffle_pack();
    deal_cards(dealer_index);
    Suit trump;
    Team ordered_up_team = make_trump(dealer_index, trump);
    int leader_index = (dealer_index + 1) % 4;
    int team_even_tricks = 0;
    int team_odd_tricks = 0;
    for (int i = 0; i < 5; ++i) {
      leader_index = play_trick(leader_index, trump);
      if (leader_index % 2 == 0) {
        team_even_tricks++;
      } else {
        team_odd_tricks++;
      }
    }

    // Update points based on tricks won
    if (ordered_up_team == EVEN) {
      if (team_even_tricks >= 3) {
        cout << players.at(0)->get_name() << " and "
             << players.at(2)->get_name() << " win the hand" << endl;
        if (team_even_tricks == 5) {
          cout << "march!" << endl;
          return {2, 0}; // March
        } else {
          return {1, 0}; // Normal win
        }
      } else {
        cout << players[1]->get_name() << " and " << players[3]->get_name()
             << " win the hand" << endl;
        cout << "euchred!" << endl;
        return {0, 2}; // Euchred
      }
    } else {
      if (team_odd_tricks >= 3) {
        cout << players.at(1)->get_name() << " and "
             << players.at(3)->get_name() << " win the hand" << endl;
        if (team_odd_tricks == 5) {
          cout << "march!" << endl;
          return {0, 2}; // March
        } else {
          return {0, 1}; // Normal win
        }
      } else {
        cout << players.at(0)->get_name() << " and "
             << players.at(2)->get_name() << " win the hand" << endl;
        cout << "euchred!" << endl;
        return {2, 0}; // Euchred
      }
    }
  }

  void clean_up() {
    for (Player *p : players) {
      delete p;
    }
  }
};

int main(int argc, char *argv[]) {
  bool valid = true;

  if (argc != 12) {
    valid = false;
  }

  int pts = stoi(string(argv[3]));
  if (pts < 1 || pts > 100) {
    valid = false;
  }

  vector<pair<string, string>> players;
  for (int i = 4; i < argc; i += 2) {
    string player_name = argv[i];
    string player_type = argv[i + 1];
    if (player_type != "Human" && player_type != "Simple") {
      valid = false;
      break;
    }
    players.emplace_back(argv[i], argv[i + 1]);
  }

  string is_shuffle = argv[2];
  if (is_shuffle != "shuffle" && is_shuffle != "noshuffle") {
    valid = false;
  }

  if (!valid) {
    cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
         << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
         << "NAME4 TYPE4" << endl;
    return 1;
  }

  string pack_filename = argv[1];
  ifstream fin(pack_filename);
  if (!fin.is_open()) {
    cout << "Error opening " << pack_filename << endl;
    return 1;
  }

  // print all args
  for (int i = 0; i < argc; i++) {
    cout << argv[i] << " ";
  }
  cout << endl;

  Game game(fin, is_shuffle == "shuffle", pts, players);
  game.play();
  return 0;
}
