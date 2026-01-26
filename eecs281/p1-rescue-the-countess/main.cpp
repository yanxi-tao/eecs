// Project Identifier: B99292359FFD910ED13A7E6C7F9705B874262D79

#include <cstdint>
#include <deque>
#include <iostream>
#include <getopt.h>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>

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
    // '\0' means not visited else visited
    // 'n', 'e', 's', 'w' for directions, or '0'-'9' for the room number we warped from.
    char discovered_from = '\0';
};

struct Location {
    uint32_t rm, row, col;
};

struct Atlas {
    vector<Tile> grid;
    uint32_t dim;
    uint64_t sq_dim;

    Atlas(uint32_t rm_cnt, uint32_t d): dim(d), sq_dim(dim * dim) {
        grid.resize(rm_cnt * sq_dim);
    };

    inline Tile& at(uint32_t rm, uint32_t row, uint32_t col) {
        return  grid[rm * sq_dim + row * dim + col];
    }
};

pair<Location, Location> parseMap(Atlas &atlas, uint32_t rm_cnt, uint32_t dim) {
    string line;
    uint32_t rm = 0;
    uint32_t row = 0;

    Location start;
    Location end;

    while(cin >> line) {
        // check for comment, if so, skip by eating rest of line
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
                atlas.at(rm, row, col).type = c;
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

        atlas.at(rm, row, col).type = c;
        if (c == 'S') start = {rm, row, col};
        if (c == 'C') end = {rm, row, col};

    }
    return {start, end};
}

bool solve(Atlas &atlas, Location start, Config &config, uint32_t rm_cnt, uint32_t dim) {
    deque<Location> search_container;

    atlas.at(start.rm, start.row, start.col).discovered_from = 'S';
    search_container.push_back(start);

    // Using static constexpr for directions
    static constexpr int8_t d_row[] = {-1, 0, 1, 0};
    static constexpr int8_t d_col[] = {0, 1, 0, -1};
    static constexpr char dir[] = { 'n', 'e', 's', 'w'};

    while (!search_container.empty()) {
        Location current;
        if (config.scheme == RScheme::Queue) {
            current = search_container.front();
            search_container.pop_front();
        } else {
            current = search_container.back();
            search_container.pop_back();
        }

        Tile &current_t = atlas.at(current.rm, current.row, current.col);

        if (current_t.type >= '0' and current_t.type <= '9') {
            uint32_t next_rm = (uint32_t)(current_t.type - '0');
            if (next_rm < rm_cnt) {
                Tile &next_t = atlas.at(next_rm, current.row, current.col);
                if (next_t.discovered_from == '\0' and next_t.type != '#' and next_t.type != '!') {
                    next_t.discovered_from = (char)(current.rm + '0');
                    if (next_t.type == 'C') return true;
                    search_container.push_back({next_rm, current.row, current.col});
                }
            }
        }
        else {
            for (int8_t i = 0; i < 4; i++) {
                // The wrap-around behavior is intentional for bounds checking.
                uint32_t next_r = current.row + (uint32_t)(d_row[i]);
                uint32_t next_c = current.col + (uint32_t)(d_col[i]);

                if (next_r < dim and next_c < dim) {
                    Tile &neighbor = atlas.at(current.rm, next_r, next_c);
                    if (neighbor.discovered_from == '\0' and neighbor.type != '#' and neighbor.type != '!') {
                        neighbor.discovered_from = dir[i];

                        if (neighbor.type == 'C') return true;
                        search_container.push_back({current.rm, next_r, next_c});
                    }
                }
            }
        }
    }
    return false;
}

void reconstructPath(Atlas &atlas, vector<Location> &path, Location start, Location end) {
    Location current = end;

    static constexpr int8_t d_row[] = {-1, 0, 1, 0};
    static constexpr int8_t d_col[] = {0, 1, 0, -1};
    static constexpr char dir[] = { 'n', 'e', 's', 'w'};

    path.push_back(current);

    while (!(current.rm == start.rm and current.row == start.row and current.col == start.col)) {
        Tile &current_t = atlas.at(current.rm, current.row, current.col);

        if (current_t.discovered_from >= '0' and current_t.discovered_from <= '9') {
            uint32_t prev_rm = (uint32_t)(current_t.discovered_from - '0');
            Tile &prev_t = atlas.at(prev_rm, current.row, current.col);
            prev_t.type = 'p';
            current = {prev_rm, current.row, current.col};
        } else {
            for (int8_t i = 0; i < 4; i++) {
                if (current_t.discovered_from == dir[i]) {
                    // This relies on unsigned overflow arithmetic, which is well-defined.
                    uint32_t prev_r = current.row - (uint32_t)(d_row[i]);
                    uint32_t prev_c = current.col - (uint32_t)(d_col[i]);

                    Tile &prev_t = atlas.at(current.rm, prev_r, prev_c);
                    prev_t.type = current_t.discovered_from;
                    current = {current.rm, prev_r, prev_c};
                    break;
                }
            }
        }
        path.push_back(current);
    }

    std::reverse(path.begin(), path.end());
}

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    Config config = getOpts(argc, argv);

    char in_type;
    uint32_t rm_cnt;
    uint32_t dim;
    cin >> in_type >> rm_cnt >> dim;

    Atlas atlas(rm_cnt, dim);

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
        for (auto &tile : atlas.grid) {
            if (tile.discovered_from != '\0') ++tile_cnt;
        }
        cout << "No solution, " << tile_cnt << " tiles discovered." << endl;
    } else {
        vector<Location> path;
        reconstructPath(atlas, path, start, end);

        if (config.format == OFomat::Map) {
            cout << "Start in room " << start.rm << ", row " << start.row << ", column " << start.col << "\n";
            for (uint32_t rm = 0; rm < rm_cnt; ++rm) {
                cout << "//castle room " << rm << "\n";
                for (uint32_t row = 0; row < dim; ++row) {
                    for (uint32_t col = 0; col < dim; ++col) {
                        cout << atlas.at(rm, row, col).type;
                    }
                    cout << '\n';
                }
            }
        } else {
            cout << "Path taken:\n";
            path.pop_back();
            for (const auto& loc : path) {
                cout << "(" << loc.rm << "," << loc.row << "," << loc.col << "," << atlas.at(loc.rm, loc.row, loc.col).type << ")\n";
            }
        }
    }
    return 0;
}