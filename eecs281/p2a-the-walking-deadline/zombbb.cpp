// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA262043

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <vector>
#include "getopt.h"
#include "P2random.h"

using namespace std;

struct Zombie {
    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t created_round;
    uint32_t rounds_active;
};
ostream &operator<<(ostream& os, const Zombie &z) {
    return os << z.name << " (distance: " << z.distance
        << ", speed: " << z.speed << ", health: " << z.health << ")";
}
struct ZombieComp {
    bool operator()(const Zombie* a, const Zombie* b) {
        uint32_t eta_a = a->distance / a->speed;
        uint32_t eta_b = b->distance / b->speed;

        if (eta_a != eta_b) return eta_a > eta_b;

        if (a->health != b->health) return a->health > b->health;

        return a->name > b->name;
    }
};

class RunningMedian {
private:
    priority_queue<uint32_t> lower;
    priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> upper;

public:
    void insert(uint32_t num) {
        lower.push(num);

        upper.push(lower.top());
        lower.pop();

        if (lower.size() < upper.size()) {
            lower.push(upper.top());
            upper.pop();
        }
    }

    uint32_t median() {
        if (lower.size() > upper.size()) {
            return lower.top();
        }
        else {
            return (lower.top() + upper.top()) / 2;
        }
    }
};

class ZomBBB {
private:
    // flags
    bool verbose = false;
    bool median_mode = false;
    bool stats_mode = false;
    uint32_t num_stats = 0;

    // header info
    uint32_t quiver_capacity = 0;
    uint32_t random_seed = 0;
    uint32_t max_rand_dist = 0;
    uint32_t max_rand_speed = 0;
    uint32_t max_rand_health = 0;

    // game state
    uint32_t current_round = 0;
    uint32_t arrows = 0;
    bool alive = true;
    string kname;

    // round inputs
    uint32_t next_input_round = 0;
    uint32_t next_rand_count = 0;
    uint32_t next_named_count = 0;

    // containers
    deque<Zombie> zbs;
    priority_queue<Zombie*, vector<Zombie*>, ZombieComp> szbs;
    vector<Zombie*> kzbs;
    RunningMedian rm;

public:
    void getOpts(int argc, char** argv) {
        int choice;
        int option_index = 0;
        option long_options[] = {
            {"verbose",    no_argument,       nullptr, 'v'},
            {"statistics", required_argument, nullptr, 's'},
            {"median",     no_argument,       nullptr, 'm'},
            {"help",       no_argument,       nullptr, 'h'},
            {nullptr,      0,                 nullptr, '\0'}
        };

        while ((choice = getopt_long(argc, argv, "vs:mh", long_options, &option_index)) != -1) {
            switch (choice) {
            case 'v':
                verbose = true;
                break;
            case 'm':
                median_mode = true;
                break;
            case 's':
                stats_mode = true;
                num_stats = static_cast<uint32_t>(stoul(optarg));
                break;
            case 'h':
                cout << "Help message...\n";
                exit(0);
            default:
                exit(1);
            }
        }
    }

    void play() {
        readHeader();
        current_round = 1;

        bool more_input = readRound();

        while (more_input || !szbs.empty()) {
            if (verbose) cout << "Round: " << current_round << "\n";

            arrows = quiver_capacity;

            moveZombies();

            if (!alive) break;

            if (more_input && current_round == next_input_round) {
                spawnZombies();
                more_input = readRound();
            }

            shootZombies();

            ++current_round;
        }

        if (alive) {
            cout << "VICTORY IN ROUND " << current_round - 1 << "! "
                 << kzbs.back()->name << " was the last zombie.\n";
        } else {
            cout << "DEFEAT IN ROUND " << current_round << "! " << kname << " ate your brains!\n";
        }

        if (stats_mode) printStats();
    }
private:
    void readHeader() {
        string junk;
        getline(cin, junk);

        string key;
        cin >> key >> quiver_capacity >> key >> random_seed >> key
            >> max_rand_dist >> key >> max_rand_speed >> key >> max_rand_health;

        P2random::initialize(random_seed, max_rand_dist, max_rand_speed, max_rand_health);
    }

    // EOF ? false : true
    bool readRound() {
        string junk;
        if (cin >> junk) {
            cin >> junk >> next_input_round;
            cin >> junk >> next_rand_count;
            cin >> junk >> next_named_count;
            return true;
        }
        return false;
    }

    void moveZombies() {
        for (auto& z : zbs) {
            if (z.health != 0) {
                z.distance -= min(z.distance, z.speed);
                if (alive && z.distance == 0) {
                    kname = z.name;
                    alive = false;
                }
                if (verbose) cout << "Moved: " << z << '\n';
            }
        }
    }

    void spawnZombies() {
        for (uint32_t i = 0; i < next_rand_count; ++i) {
            string name  = P2random::getNextZombieName();
            uint32_t distance = P2random::getNextZombieDistance();
            uint32_t speed    = P2random::getNextZombieSpeed();
            uint32_t health   = P2random::getNextZombieHealth();

            zbs.push_back({name, distance, speed, health, current_round, 0});
            szbs.push(&zbs.back());

            if (verbose) cout << "Created: " << zbs.back() << "\n";
        }

        for (uint32_t i = 0; i < next_named_count; ++i) {
            string junk;
            Zombie z;
            cin >> z.name >> junk >> z.distance >> junk >> z.speed >> junk >> z.health;
            z.created_round = current_round;
            z.rounds_active = 0;

            zbs.push_back(std::move(z));
            szbs.push(&zbs.back());

            if (verbose) cout << "Created: " << zbs.back() << "\n";
        }
    }

    void shootZombies() {
        while (arrows > 0 and !szbs.empty()) {
            Zombie* z = szbs.top();
            uint32_t dmg = min(arrows, z->health);
            z->health -= dmg;
            arrows -= dmg;

            if (z->health == 0) {
                z->rounds_active = current_round - z->created_round + 1;
                kzbs.push_back(z);
                szbs.pop();
                if (verbose) cout << "Destroyed: " << *z << "\n";

                if (median_mode) rm.insert(z->rounds_active);
            }
        }
        if (median_mode && !kzbs.empty()) {
            cout << "At the end of round " << current_round
                 << ", the median zombie lifetime is " << rm.median() << "\n";
        }
    }

    void printStats() {
        uint32_t active_zombies = 0;
        for (auto &z : zbs) {
            if (z.health != 0) ++active_zombies;
        }
        cout << "Zombies still active: " << active_zombies << "\n";

        uint32_t iter_cnt = min((uint32_t)kzbs.size(), num_stats);
        cout << "First zombies killed:\n";
        for (uint32_t i = 0; i < iter_cnt; ++i) {
            cout << kzbs[i]->name << " " << i+1 << "\n";
        }
        cout << "Last zombies killed:\n";
        for (uint32_t i = 0; i < iter_cnt; ++i) {
            cout << kzbs[kzbs.size() - 1 - i]->name << " "
                 << iter_cnt - i << "\n";
        }

        vector<Zombie*> all = kzbs;
        for (auto &z : zbs) {
            if (z.health != 0) {
                z.rounds_active = current_round - z.created_round + (alive ? 0 : 1);
                all.push_back(&z);
            }
        }
        iter_cnt = min(num_stats, static_cast<uint32_t>(all.size()));
        partial_sort(all.begin(), all.begin() + iter_cnt, all.end(),
            [](const Zombie* a, const Zombie* b) {
            if (a->rounds_active != b->rounds_active) {
                return a->rounds_active > b->rounds_active;
            }
            return a->name < b->name;
        });
        cout << "Most active zombies:\n";
        for (uint32_t i = 0; i < iter_cnt; ++i) {
            cout << all[i]->name << " " << all[i]->rounds_active << "\n";
        }

        partial_sort(all.begin(), all.begin() + iter_cnt, all.end(),
            [](const Zombie* a, const Zombie* b) {
            if (a->rounds_active != b->rounds_active) {
                return a->rounds_active < b->rounds_active;
            }
            return a->name < b->name;
        });
        cout << "Least active zombies:\n";
        for (uint32_t i = 0; i < iter_cnt; ++i) {
            cout << all[i]->name << " " << all[i]->rounds_active << "\n";
        }
    }
};

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    ZomBBB game;
    game.getOpts(argc, argv);
    game.play();

    return 0;
}