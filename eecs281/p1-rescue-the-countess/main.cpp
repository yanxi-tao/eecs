// Project Identifier: B99292359FFD910ED13A7E6C7F9705B874262D79

#include <cstdint>
#include <deque>
#include <ios>
#include <iostream>
#include <getopt.h>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

// CLArgs processing

enum class OFomat {
    Map,
    List,
};

enum class RScheme {
    Stack,
    Queue,
    None
};

struct Config {
    RScheme scheme = RScheme::None;
    OFomat format = OFomat::Map;
};

Config getOpts(int argc, char** argv) {
    int opt, opt_idx = 0;
    opterr = 0;
    Config config;

    const option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"stack", no_argument, nullptr, 's'},
        {"queue", no_argument, nullptr, 'q'},
        {"output", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, '\0'}
    };
    const char* short_opts = "hsqo:";

    while ((opt = getopt_long(argc, argv, short_opts, long_options, &opt_idx)) != -1) {
        switch (opt) {
            case 'h':
                cout << "Print Help MSG!" << endl;
                exit(0);
            case 's':
                if (config.scheme != RScheme::None) {
                    cerr << "Multiple routing modes specified!" << endl;
                    exit(1);
                }
                config.scheme = RScheme::Stack;
                break;
            case 'q':
                if (config.scheme != RScheme::None) {
                    cerr << "Multiple routing modes specified!" << endl;
                    exit(1);
                }
                config.scheme = RScheme::Queue;
                break;
            case 'o': {
                string arg = optarg;
                if (arg != "M" && arg != "L") {
                    cerr << "Invalid Output Format: " << arg << endl;
                    exit(1);
                }
                if (arg == "M") {
                    config.format = OFomat::Map;
                } else {
                    config.format = OFomat::List;
                }
                break;
            }
            default:
                cerr << "Invalid Args/Flgas" << endl;
                exit(1);
        }
    }
    if (config.scheme == RScheme::None) {
        cerr << "Routing Scheme Not set" << endl;
        exit(1);
    }
    return config;
}

// parsing & searching

struct Tile {
    char type = '.';
    bool visited = false;

    // 'n', 'e', 's', 'w' for directions, or '0'-'9' for the room number we warped from.
    char discovered_from = '\0';
};

struct Location {
    uint32_t rm, row, col;
};

using Atlas = vector<vector<vector<Tile>>>;

pair<Location, Location> parseMap(Atlas &atlas, uint32_t rm_cnt, uint32_t dim) {
    string line;
    uint32_t rm = 0;
    uint32_t row = 0;

    Location start;
    Location end;

    while(cin >> line) {
        if (line.substr(0, 2) == "//") {
            string junk;
            getline(cin, junk);
            continue;
        }

        if (rm >= rm_cnt) {
            cerr << "Too many rows in input!" << endl;
            exit(1);
        }

        if (line.length() != dim) {
                cerr << "Map line length does not match dimension!" << endl;
                exit(1);
        }
        uint32_t col = 0;
        for (char c : line) {
            if (c == '.') {
                ++col;
                continue;
            } else {
                if (c != '#' and c != '!' and c != 'S' and c != 'C' and !(c >= '0' and c <= '9')) {
                    cerr << "Invalid Map Pos: " << c << endl;
                    exit(1);
                }
                atlas[rm][row][col] = {c};
                if (c == 'S') start = {rm, row, col};
                if (c == 'C') end = {rm, row, col};
            }
            ++col;
        }
        ++row;
        if (row % dim == 0) {
            row = 0;
            ++rm;
        }
    }
    if (rm != rm_cnt) {
        cerr << "Not enough data provided for all rooms!" << endl;
        exit(1);
    }

    return {start, end};
}

pair<Location, Location> parseList(Atlas &atlas, uint32_t rm_cnt, uint32_t dim) {
    uint32_t rm, row, col;
    char c;
    char open_p, comma1, comma2, comma3, close_p;

    Location start;
    Location end;

    while (cin >> open_p) {
        if (open_p == '/') {
            string junk;
            getline(cin, junk);
            continue;
        }
        // We expect: (rm,row,col,type)
        cin >> rm >> comma1 >> row >> comma2 >> col >> comma3 >> c >> close_p;

        if (cin.fail()) {
            cerr << "Malformed coordinate list input!" << endl;
            exit(1);
        }

        if (rm >= rm_cnt) {
            cerr << "Room " << rm << " does not exist!" << endl;
            exit(1);
        }
        if (row >= dim) {
            cerr << "Row " << row << " does not exist!" << endl;
            exit(1);
        }
        if (col >= dim) {
            cerr << "Col " << col << " does not exist!" << endl;
            exit(1);
        }

        if (c != '.' && c != '#' && c != '!' && c != 'S' && c != 'C' && !(c >= '0' && c <= '9')) {
            cerr << c << " is an invalid map character!" << endl;
            exit(1);
        }

        atlas[rm][row][col] = {c};
        if (c == 'S') start = {rm, row, col};
        if (c == 'C') end = {rm, row, col};

    }
    return {start, end};
}

bool solve(Atlas &atlas, Location start, Config &config, uint32_t rm_cnt, uint32_t dim) {
    deque<Location> search_container;

    atlas[start.rm][start.row][start.col].visited = true;
    search_container.push_back(start);

    while (!search_container.empty()) {
        Location current;
        if (config.scheme == RScheme::Queue) {
            current = search_container.front();
            search_container.pop_front();
        } else {
            current = search_container.back();
            search_container.pop_back();
        }

        Tile &current_t = atlas[current.rm][current.row][current.col];

        if (current_t.type >= '0' and current_t.type <= '9') {
            uint32_t next_rm = (uint32_t)(current_t.type - '0');
            if (next_rm < rm_cnt) {
                Tile &next_t = atlas[next_rm][current.row][current.col];
                if (!next_t.visited and next_t.type != '#' and next_t.type != '!') {
                    Location next = {next_rm, current.row, current.col};
                    next_t.visited = true;
                    next_t.discovered_from = (char)(current.rm + '0');
                    if (next_t.type == 'C') return true;
                    search_container.push_back(next);
                }
            }
        } else {
            // north, east, south, west
            int8_t d_row[] = {-1, 0, 1, 0};
            int8_t d_col[] = {0, 1, 0, -1};
            char dir[] = { 'n', 'e', 's', 'w'};
            for (int8_t i = 0; i < 4; i++) {
                int64_t next_r = (int64_t)current.row + d_row[i];
                int64_t next_c = (int64_t)current.col + d_col[i];
                if (next_r >= 0 and next_r < (int64_t)dim and next_c >= 0 and next_c < (int64_t)dim) {
                    Tile &neighbor = atlas[current.rm][(uint32_t)next_r][(uint32_t)next_c];
                    if (!neighbor.visited and neighbor.type != '#' and neighbor.type != '!') {
                        neighbor.visited = true;
                        neighbor.discovered_from = dir[i];

                        if (neighbor.type == 'C') return true;
                        search_container.push_back({current.rm, (uint32_t)next_r, (uint32_t)next_c});
                    }
                }
            }
        }
    }
    return false;
}

void reconstructPath(Atlas &atlas, Location start, Location end) {
    Location current = end;
    // north, east, south, west
    int8_t d_row[] = {-1, 0, 1, 0};
    int8_t d_col[] = {0, 1, 0, -1};
    char dir[] = { 'n', 'e', 's', 'w'};

    while (!(current.rm == start.rm and current.row == start.row and current.col == start.col)) {
        Tile &current_t = atlas[current.rm][current.row][current.col];
        if (current_t.discovered_from >= '0' and current_t.discovered_from <= '9') {
            uint32_t prev_rm = (uint32_t)(current_t.discovered_from - '0');
            Tile &prev_t = atlas[prev_rm][current.row][current.col];
            prev_t.type = 'p';
            current = {prev_rm, current.row, current.col};
        } else {
            for (int8_t i = 0; i < 4; i++) {
                if (current_t.discovered_from == dir[i]) {
                    int64_t prev_r = (int64_t)current.row - d_row[i];
                    int64_t prev_c = (int64_t)current.col - d_col[i];
                    Tile &prev_t = atlas[current.rm][(uint32_t)prev_r][(uint32_t)prev_c];
                    prev_t.type = current_t.discovered_from;
                    current = {current.rm, (uint32_t)prev_r, (uint32_t)prev_c};
                    break;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);

    Config config = getOpts(argc, argv);

    char in_type;
    uint32_t rm_cnt;
    uint32_t dim;
    cin >> in_type >> rm_cnt >> dim;

    Atlas atlas(rm_cnt, vector<vector<Tile>>(dim, vector<Tile>(dim)));


    Location start, end;

    if (in_type == 'M') {
        tie(start, end) = parseMap(atlas, rm_cnt, dim);
    } else if (in_type == 'L') {
        tie(start, end) = parseList(atlas, rm_cnt, dim);
    } else {
        cerr << "Invalid Input Format" << endl;
        exit(1);
    }

    if (!solve(atlas, start, config, rm_cnt, dim)) {
        uint32_t tile_cnt = 0;
        for (auto &rm : atlas) {
            for (auto &row : rm) {
                for (auto &tile : row) {
                    if (tile.visited) ++tile_cnt;
                }
            }
        }
        cout << "No solution, " << tile_cnt << " tiles discovered." << endl;
    } else {
        reconstructPath(atlas, start, end);
        if (config.format == OFomat::Map) {
            uint32_t rm_iter_cnt = 0;
            cout << "Start in room " << start.rm << ", row " << start.row << ", column " << start.col << "\n";
            for (auto &rm : atlas) {
                cout << "//castle room " << rm_iter_cnt << "\n";
                for (auto &row : rm) {
                    for (auto &c : row) {
                        cout << c.type;
                    }
                    cout << endl;
                }
                ++rm_iter_cnt;
            }
        } else {
            cout << "Path taken:\n";
            Location current = start;
            // north, east, south, west
            int8_t d_row[] = {-1, 0, 1, 0};
            int8_t d_col[] = {0, 1, 0, -1};
            char dir[] = { 'n', 'e', 's', 'w'};

            while (!(current.rm == end.rm and current.row == end.row and current.col == end.col)) {
                Tile &current_t = atlas[current.rm][current.row][current.col];
                cout << "(" << current.rm << "," << current.row << "," << current.col << "," << current_t.type << ")" << endl;

                if (current_t.type == 'p') {
                    for (uint32_t r = 0; r < rm_cnt; ++r) {
                        // Optimization: Skip current room
                        if (r == current.rm) continue;

                        if (atlas[r][current.row][current.col].discovered_from == (char)(current.rm + '0')) {
                            current.rm = r;
                            break;
                        }
                    }
                } else {
                    for (int8_t i = 0; i < 4; i++) {
                        if (current_t.type == dir[i]) {
                            int64_t next_r = (int64_t)current.row + d_row[i];
                            int64_t next_c = (int64_t)current.col + d_col[i];
                            current = {current.rm, (uint32_t)next_r, (uint32_t)next_c};
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}