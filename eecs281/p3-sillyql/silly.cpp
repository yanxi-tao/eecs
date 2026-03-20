// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797926254D

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Field.h"

using namespace std;

enum class IndexType : uint8_t { Bst, Hash, None };
enum class CondType  : uint8_t { Gt, Lt, Eq };

struct Comp {
    CondType cond_;

    explicit Comp(const string& cond) : cond_(CondType::Eq) {
        if      (cond == "<") cond_ = CondType::Lt;
        else if (cond == ">") cond_ = CondType::Gt;
        // "=" and anything else → Eq (safe default)
    }

    bool operator()(const Field& lhs, const Field& rhs) const {
        switch (cond_) {
            case CondType::Gt: return rhs < lhs;
            case CondType::Eq: return lhs == rhs;
            case CondType::Lt: return lhs < rhs;
        }
        return false; // unreachable
    }
};

// =================== Table ===================
class Table {
private:
    vector<ColumnType>            col_types;
    unordered_map<string, size_t> col_map;
    vector<vector<Field>>         columns;

    unordered_map<Field, vector<size_t>> hash_idx;
    map<Field,           vector<size_t>> bst_idx;
    IndexType index_type  = IndexType::None;
    size_t    indexed_col = 0;

    // helper: print one data row
    void printRow(const vector<size_t>& col_index, size_t r) const {
        for (size_t c : col_index) cout << columns[c][r] << " ";
        cout << "\n";
    }

public:
    Table(vector<string> names, vector<ColumnType> types)
        : col_types(std::move(types)) {

        col_map.reserve(names.size());

        for (size_t i = 0; i < names.size(); ++i)
            col_map.emplace(std::move(names[i]), i);
        columns.resize(names.size());
    }

    // ------------------------- INSERT -------------------------
    void insertRows(size_t num_new_rows, const string& table_name) {
        size_t num_cols  = columns.size();
        size_t start_row = columns.empty() ? 0 : columns[0].size();
        size_t end_row   = start_row + num_new_rows - 1;

        for (size_t c = 0; c < num_cols; ++c)
            columns[c].reserve(end_row + 1);

        for (size_t r = 0; r < num_new_rows; ++r) {
            for (size_t c = 0; c < num_cols; ++c) {
                switch (col_types[c]) {
                    case ColumnType::Double: { double v; cin >> v; columns[c].emplace_back(v); break; }
                    case ColumnType::Int:    { int    v; cin >> v; columns[c].emplace_back(v); break; }
                    case ColumnType::Bool:   { bool   v; cin >> v; columns[c].emplace_back(v); break; }
                    case ColumnType::String: { string v; cin >> v; columns[c].emplace_back(v); break; }
                }
            }
        }

        // update index if needed with newly inserted rows
        if (index_type == IndexType::Hash) {
            for (size_t r = start_row; r <= end_row; ++r)
                hash_idx[columns[indexed_col][r]].push_back(r);
        } else if (index_type == IndexType::Bst) {
            for (size_t r = start_row; r <= end_row; ++r)
                bst_idx[columns[indexed_col][r]].push_back(r);
        }

        cout << "Added " << num_new_rows << " rows to " << table_name
             << " from position " << start_row << " to " << end_row << "\n";
    }

    // ------------------------- PRINT (internal) -------------------------
    void print(bool quiet_mode, bool is_cond, const string& table_name,
               const vector<string>& print_names,
               size_t cond_col_index    = 0,
               const Field& value       = Field(0),
               const Comp&  comp        = Comp("=")) {

        size_t num_rows = columns.empty() ? 0 : columns[0].size();

        // validate every print-column name before emitting anything
        vector<size_t> col_index;
        col_index.reserve(print_names.size());
        for (const auto& name : print_names) {
            auto it = col_map.find(name);
            if (it == col_map.end()) {
                cout << "Error during PRINT: " << name
                     << " does not name a column in " << table_name << "\n";
                return;
            }
            col_index.push_back(it->second);
        }

        // print column-name header (suppressed in quiet mode)
        if (!quiet_mode) {
            for (const auto& name : print_names) cout << name << " ";
            cout << "\n";
        }

        // iterate and count (output suppressed in quiet mode)
        size_t count = 0;

        if (is_cond) {
            if (index_type == IndexType::Bst && indexed_col == cond_col_index) {
                // BST index → sorted key order; within each key (vector), insertion order
                for (const auto& [key, rows] : bst_idx) {
                    if (comp(key, value)) {
                        for (auto r : rows) {
                            if (!quiet_mode) printRow(col_index, r);
                            ++count;
                        }
                    }
                }
            } else if (index_type == IndexType::Hash && indexed_col == cond_col_index
                       && comp.cond_ == CondType::Eq) {
                // hash index used for O(1) equality lookup; results in insertion order
                auto it = hash_idx.find(value);
                if (it != hash_idx.end()) {
                    for (auto r : it->second) {
                        if (!quiet_mode) printRow(col_index, r);
                        ++count;
                    }
                }
            } else {
                // simple linear scan in insertion order
                for (size_t r = 0; r < num_rows; ++r) {
                    if (comp(columns[cond_col_index][r], value)) {
                        if (!quiet_mode) printRow(col_index, r);
                        ++count;
                    }
                }
            }
        } else {
            // PRINT...ALL
            count = num_rows;
            if (!quiet_mode) {
                for (size_t r = 0; r < num_rows; ++r) printRow(col_index, r);
            }
        }

        cout << "Printed " << count << " matching rows from " << table_name << "\n";
    }

    void printAll(bool quiet_mode, const string& table_name,
                  const vector<string>& print_names) {
        print(quiet_mode, false, table_name, print_names);
    }

    void printCond(bool quiet_mode, const string& table_name,
                   const vector<string>& print_names) {
        string col_name, op;
        cin >> col_name >> op;

        auto it = col_map.find(col_name);
        if (it == col_map.end()) {
            string junk; getline(cin, junk);
            cout << "Error during PRINT: " << col_name
                 << " does not name a column in " << table_name << "\n";
            return;
        }

        size_t col_idx = it->second;
        switch (col_types[col_idx]) {
            case ColumnType::Double: { double v; cin >> v; print(quiet_mode, true, table_name, print_names, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::Int:    { int    v; cin >> v; print(quiet_mode, true, table_name, print_names, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::Bool:   { bool   v; cin >> v; print(quiet_mode, true, table_name, print_names, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::String: { string v; cin >> v; print(quiet_mode, true, table_name, print_names, col_idx, Field(v), Comp(op)); break; }
        }
    }

    // ------------------------- DELETE -------------------------
    void delete_(const string& table_name, size_t cond_col_index,
                 const Field& value, const Comp& comp) {
        size_t num_rows = columns.empty() ? 0 : columns[0].size();

        vector<bool> keep(num_rows, true);
        size_t deleted_count = 0;
        for (size_t r = 0; r < num_rows; ++r) {
            if (comp(columns[cond_col_index][r], value)) {
                keep[r] = false;
                ++deleted_count;
            }
        }

        if (deleted_count > 0) {
            size_t kept_count = num_rows - deleted_count;
            for (size_t c = 0; c < columns.size(); ++c) {
                vector<Field> new_col;
                new_col.reserve(kept_count);

                for (size_t r = 0; r < num_rows; ++r) {
                    if (keep[r]) {
                        new_col.push_back(std::move(columns[c][r]));
                    }
                }
                columns[c] = std::move(new_col);
            }
        }

        // rebuilt/rehash active index
        size_t kept = num_rows - deleted_count;
        if (index_type == IndexType::Bst) {
            bst_idx.clear();
            for (size_t r = 0; r < kept; ++r)
                bst_idx[columns[indexed_col][r]].push_back(r);
        } else if (index_type == IndexType::Hash) {
            hash_idx.clear();
            for (size_t r = 0; r < kept; ++r)
                hash_idx[columns[indexed_col][r]].push_back(r);
        }

        cout << "Deleted " << deleted_count << " rows from " << table_name << "\n";
    }

    void deleteRows(const string& table_name, const string& col_name) {
        auto it = col_map.find(col_name);
        if (it == col_map.end()) {
            string junk; getline(cin, junk);
            cout << "Error during DELETE: " << col_name
                 << " does not name a column in " << table_name << "\n";
            return;
        }
        string op; cin >> op;
        size_t col_idx = it->second;
        switch (col_types[col_idx]) {
            case ColumnType::Double: { double v; cin >> v; delete_(table_name, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::Int:    { int    v; cin >> v; delete_(table_name, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::Bool:   { bool   v; cin >> v; delete_(table_name, col_idx, Field(v), Comp(op)); break; }
            case ColumnType::String: { string v; cin >> v; delete_(table_name, col_idx, Field(v), Comp(op)); break; }
        }
    }

    // ------------------------- JOIN -------------------------
    void joinTable(bool quiet_mode,
                   const string& table_name1, const string& table_name2,
                   const Table& table2,
                   const string& col_name1,   const string& col_name2,
                   const vector<pair<string, int>>& print_names) {

        // validate the two join-condition columns
        auto it_c1 = col_map.find(col_name1);
        if (it_c1 == col_map.end()) {
            cout << "Error during JOIN: " << col_name1
                 << " does not name a column in " << table_name1 << "\n";
            return;
        }
        auto it_c2 = table2.col_map.find(col_name2);
        if (it_c2 == table2.col_map.end()) {
            cout << "Error during JOIN: " << col_name2
                 << " does not name a column in " << table_name2 << "\n";
            return;
        }
        size_t c1_index = it_c1->second;
        size_t c2_index = it_c2->second;

        // validate and resolve print columns from their respective tables
        vector<size_t> col_index;
        col_index.reserve(print_names.size());
        for (const auto& [name, tbl] : print_names) {
            if (tbl == 1) {
                auto it = col_map.find(name);
                if (it == col_map.end()) {
                    cout << "Error during JOIN: " << name
                         << " does not name a column in " << table_name1 << "\n";
                    return;
                }
                col_index.push_back(it->second);
            } else {
                auto it = table2.col_map.find(name);
                if (it == table2.col_map.end()) {
                    cout << "Error during JOIN: " << name
                         << " does not name a column in " << table_name2 << "\n";
                    return;
                }
                col_index.push_back(it->second);
            }
        }

        // print column-name header (suppressed in quiet mode)
        if (!quiet_mode) {
            for (const auto& [name, tbl] : print_names) cout << name << " ";
            cout << "\n";
        }

        size_t num_rows1    = columns.empty() ? 0 : columns[0].size();
        size_t joined_count = 0;

        // emit one matched pair; always increments count (output suppressed in quiet mode)
        auto emit = [&](size_t r1, size_t r2) {
            if (!quiet_mode) {
                for (size_t i = 0; i < print_names.size(); ++i) {
                    if (print_names[i].second == 1)
                        cout << columns[col_index[i]][r1] << " ";
                    else
                        cout << table2.columns[col_index[i]][r2] << " ";
                }
                cout << "\n";
            }
            ++joined_count;
        };

        // use table2's index if available
        if (table2.index_type == IndexType::Hash && table2.indexed_col == c2_index) {
            for (size_t r1 = 0; r1 < num_rows1; ++r1) {
                auto it = table2.hash_idx.find(columns[c1_index][r1]);
                if (it != table2.hash_idx.end())
                    for (auto r2 : it->second) emit(r1, r2);
            }
        } else if (table2.index_type == IndexType::Bst && table2.indexed_col == c2_index) {
            for (size_t r1 = 0; r1 < num_rows1; ++r1) {
                auto it = table2.bst_idx.find(columns[c1_index][r1]);
                if (it != table2.bst_idx.end())
                    for (auto r2 : it->second) emit(r1, r2);
            }
        } else {
            size_t num_rows2 = table2.columns.empty() ? 0 : table2.columns[0].size();
            unordered_map<Field, vector<size_t>> tmp;
            tmp.reserve(num_rows2);
            for (size_t r2 = 0; r2 < num_rows2; ++r2)
                tmp[table2.columns[c2_index][r2]].push_back(r2);
            for (size_t r1 = 0; r1 < num_rows1; ++r1) {
                auto hit = tmp.find(columns[c1_index][r1]);
                if (hit != tmp.end())
                    for (size_t r2 : hit->second) emit(r1, r2);
            }
        }

        cout << "Printed " << joined_count << " rows from joining "
             << table_name1 << " to " << table_name2 << "\n";
    }

    // ------------------------- GENERATE -------------------------
    void generateIndex(const string& table_name, const string& type,
                       const string& col_name) {
        auto it = col_map.find(col_name);
        if (it == col_map.end()) {
            cout << "Error during GENERATE: " << col_name
                 << " does not name a column in " << table_name << "\n";
            return;
        }

        size_t num_rows = columns.empty() ? 0 : columns[0].size();
        indexed_col = it->second;

        if (type == "hash") {
            index_type = IndexType::Hash;
            bst_idx.clear();
            hash_idx.clear();
            for (size_t r = 0; r < num_rows; ++r)
                hash_idx[columns[indexed_col][r]].push_back(r);
            cout << "Generated hash index for table " << table_name
                 << " on column " << col_name
                 << ", with " << hash_idx.size() << " distinct keys\n";
        } else if (type == "bst") {
            index_type = IndexType::Bst;
            hash_idx.clear();
            bst_idx.clear();
            for (size_t r = 0; r < num_rows; ++r)
                bst_idx[columns[indexed_col][r]].push_back(r);
            cout << "Generated bst index for table " << table_name
                 << " on column " << col_name
                 << ", with " << bst_idx.size() << " distinct keys\n";
        }
    }
};

// =================== DB ===================
class DB {
private:
    unordered_map<string, Table> tables;

public:
    // ------------------------- CREATE -------------------------
    void execCreate() {
        string table_name; size_t num_cols;
        cin >> table_name >> num_cols;

        if (tables.count(table_name)) {
            string junk; getline(cin, junk);
            cout << "Error during CREATE: Cannot create already existing table "
                 << table_name << "\n";
            return;
        }

        vector<ColumnType> tmp_types;
        vector<string>     tmp_names;
        tmp_types.reserve(num_cols);
        tmp_names.reserve(num_cols);

        for (size_t i = 0; i < num_cols; ++i) {
            string type; cin >> type;
            switch (type[0]) {
                case 'd': tmp_types.push_back(ColumnType::Double); break;
                case 'i': tmp_types.push_back(ColumnType::Int);    break;
                case 'b': tmp_types.push_back(ColumnType::Bool);   break;
                case 's': tmp_types.push_back(ColumnType::String); break;
            }
        }

        cout << "New table " << table_name << " with column(s) ";
        for (size_t i = 0; i < num_cols; ++i) {
            string col; cin >> col;
            tmp_names.push_back(col);
            cout << col << " ";
        }
        cout << "created\n";

        tables.emplace(table_name,
                       Table(std::move(tmp_names), std::move(tmp_types)));
    }

    // ------------------------- REMOVE -------------------------
    void execRemove() {
        string table_name; cin >> table_name;
        auto tb = tables.find(table_name);
        if (tb != tables.end()) {
            tables.erase(tb);
            cout << "Table " << table_name << " removed\n";
        } else {
            cout << "Error during REMOVE: " << table_name
                 << " does not name a table in the database\n";
        }
    }

    // ------------------------- INSERT -------------------------
    void execInsert() {
        string junk, table_name; size_t num_rows;
        cin >> junk >> table_name >> num_rows >> junk;

        auto it = tables.find(table_name);
        if (it != tables.end()) {
            it->second.insertRows(num_rows, table_name);
        } else {
            string junk2; getline(cin, junk2);
            cout << "Error during INSERT: " << table_name
                 << " does not name a table in the database\n";
        }
    }

    // ------------------------- PRINT -------------------------
    void execPrint(bool quiet_mode) {
        string junk, table_name, cond, col;
        size_t num_cols;
        cin >> junk >> table_name >> num_cols;

        auto it = tables.find(table_name);
        if (it == tables.end()) {
            string junk2; getline(cin, junk2);
            cout << "Error during PRINT: " << table_name
                 << " does not name a table in the database\n";
            return;
        }

        vector<string> print_names;
        print_names.reserve(num_cols);
        for (size_t c = 0; c < num_cols; ++c) { cin >> col; print_names.push_back(col); }
        cin >> cond;

        if (cond == "ALL")
            it->second.printAll(quiet_mode, table_name, print_names);
        else  // WHERE
            it->second.printCond(quiet_mode, table_name, print_names);
    }

    // ------------------------- DELETE -------------------------
    void execDelete() {
        string junk, table_name, col_name;
        cin >> junk >> table_name >> junk >> col_name;

        auto it = tables.find(table_name);
        if (it != tables.end()) {
            it->second.deleteRows(table_name, col_name);
        } else {
            string junk2; getline(cin, junk2);
            cout << "Error during DELETE: " << table_name
                 << " does not name a table in the database\n";
        }
    }

    // ------------------------- JOIN -------------------------
    void execJoin(bool quiet_mode) {
        string table_name1, table_name2, junk, col_name1, col_name2, print_name;
        size_t num_cols; int selected_table;

        cin >> table_name1 >> junk >> table_name2
            >> junk >> col_name1 >> junk >> col_name2
            >> junk >> junk >> num_cols;

        vector<pair<string, int>> print_names;
        print_names.reserve(num_cols);
        for (size_t c = 0; c < num_cols; ++c) {
            cin >> print_name >> selected_table;
            print_names.push_back({print_name, selected_table});
        }

        auto it1 = tables.find(table_name1);
        if (it1 == tables.end()) {
            cout << "Error during JOIN: " << table_name1
                 << " does not name a table in the database\n";
            return;
        }
        auto it2 = tables.find(table_name2);
        if (it2 == tables.end()) {
            cout << "Error during JOIN: " << table_name2
                 << " does not name a table in the database\n";
            return;
        }

        it1->second.joinTable(quiet_mode, table_name1, table_name2,
                              it2->second, col_name1, col_name2, print_names);
    }

    // ------------------------- GENERATE -------------------------
    void execGenerate() {
        string junk, table_name, type, col_name;
        cin >> junk >> table_name >> type >> junk >> junk >> col_name;

        auto it = tables.find(table_name);
        if (it != tables.end()) {
            it->second.generateIndex(table_name, type, col_name);
        } else {
            cout << "Error during GENERATE: " << table_name
                 << " does not name a table in the database\n";
        }
    }
};

// =================== MAIN ===================
int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cin  >> boolalpha;
    cout << boolalpha;

    bool quiet_mode = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            cout << "Usage: silly [-h] [-q]\n";
            return 0;
        } else if (arg == "-q" || arg == "--quiet") {
            quiet_mode = true;
        } else {
            cerr << "Unrecognized command line argument: " << arg << "\n";
            return 1;
        }
    }

    DB db;
    string cmd;

    do {
        cout << "% ";
        cin >> cmd;

        if (cin.fail()) {
            cerr << "Error: Reading from cin has failed\n";
            exit(1);
        }

        switch (cmd[0]) {
            case 'C': db.execCreate();              break;
            case 'Q':                               break;
            case '#': { string d; getline(cin, d);  break; }
            case 'R': db.execRemove();              break;
            case 'I': db.execInsert();              break;
            case 'P': db.execPrint(quiet_mode);     break;
            case 'D': db.execDelete();              break;
            case 'J': db.execJoin(quiet_mode);      break;
            case 'G': db.execGenerate();            break;
            default: {
                string d; getline(cin, d);
                cout << "Error: unrecognized command\n";
                break;
            }
        }
    } while (cmd != "QUIT");

    cout << "Thanks for being silly!\n";
    return 0;
}
