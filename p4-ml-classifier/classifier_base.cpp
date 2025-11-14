#include "csvstream.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

// EFFECTS: Return a set of unique whitespace delimited words
set<string> unique_words(const string &str) {
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) {
    words.insert(word);
  }
  return words;
}

class Classifier {
public:
  // MODIFIES: cout
  // EFFECTS:  Trains the classifier on the given training data.
  //           Prints "training data:" and each post if in debug mode.
  void train(csvstream &train_file, bool debug_mode) {
    if (debug_mode) {
      cout << "training data:" << endl;
    }
    map<string, string> row;

    while (train_file >> row) {
      string label = row["tag"];
      string content = row["content"];

      if (debug_mode) {
        cout << "  label = " << label << ", content = " << content << endl;
      }

      total_posts++;
      all_labels.insert(label);
      labels_to_posts_count[label]++;

      set<string> words = unique_words(content);
      for (const string &word : words) {
        all_words.insert(word);
        words_to_posts_count[word]++;
        label_word_to_posts_count[{label, word}]++;
      }
    }
    cout << "trained on " << total_posts << " examples" << endl;
    if (debug_mode) {
      cout << "vocabulary size = " << all_words.size() << endl;
    }
    cout << endl;
  }

  // MODIFIES: cout
  // EFFECTS:  Prints the classifier's learned parameters, including
  //           class log-priors and word log-likelihoods.
  void print_classifier_summary() const {
    cout << "classes:" << endl;
    for (const string &label : all_labels) {
      double log_prior =
          log((double)labels_to_posts_count.at(label) / total_posts);
      cout << "  " << label << ", " << labels_to_posts_count.at(label)
           << " examples, log-prior = " << log_prior << endl;
    }

    cout << "classifier parameters:" << endl;
    for (const string &label : all_labels) {
      for (const string &word : all_words) {
        if (label_word_to_posts_count.count({label, word})) {
          int count = label_word_to_posts_count.at({label, word});
          double log_likelihood = get_log_likelihood(label, word);
          cout << "  " << label << ":" << word << ", count = " << count
               << ", log-likelihood = " << log_likelihood << endl;
        }
      }
    }
    cout << endl;
  }

  // EFFECTS:  Predicts the label for a post represented by a set of unique
  //           words. Returns a pair containing the predicted label and its
  //           log-probability score.
  pair<string, double> predict(const set<string> &words) const {
    string best_label = "";
    double max_log_prob = -numeric_limits<double>::infinity();

    for (const string &label : all_labels) {
      double current_log_prob =
          log((double)labels_to_posts_count.at(label) / total_posts);

      for (const string &word : words) {
        current_log_prob += get_log_likelihood(label, word);
      }

      if (current_log_prob > max_log_prob) {
        max_log_prob = current_log_prob;
        best_label = label;
      } else if (current_log_prob == max_log_prob) {
        if (label < best_label) {
          best_label = label;
        }
      }
    }
    return {best_label, max_log_prob};
  }

private:
  // EFFECTS:  Calculates the log-likelihood of a word given a label,
  //           using the formulas from the specification.
  double get_log_likelihood(const string &label, const string &word) const {
    int num_label_posts = labels_to_posts_count.at(label);

    // Case 1: Word w seen in posts with label C
    if (label_word_to_posts_count.count({label, word})) {
      int num_label_word_posts = label_word_to_posts_count.at({label, word});
      return log((double)num_label_word_posts / num_label_posts);
    }
    // Case 2: Word w not seen in label C, but seen in training data
    else if (words_to_posts_count.count(word)) {
      int num_word_posts = words_to_posts_count.at(word);
      return log((double)num_word_posts / total_posts);
    }
    // Case 3: Word w not seen anywhere in training data
    else {
      return log(1.0 / total_posts);
    }
  }

  int total_posts = 0;
  set<string> all_labels;
  set<string> all_words;                  // The vocabulary
  map<string, int> labels_to_posts_count; // Posts per label C
  map<string, int> words_to_posts_count;  // Posts containing word w

  map<pair<string, string>, int> label_word_to_posts_count;
};

// MODIFIES: cout
// EFFECTS:  Reads posts from test_csv, predicts their labels using
//           classifier, and prints results and performance summary.
void run_tests(const Classifier &classifier, csvstream &test_csv) {
  cout << "test data:" << endl;

  int correct_predictions = 0;
  int total_test_posts = 0;
  map<string, string> row;

  while (test_csv >> row) {
    total_test_posts++;
    string correct_label = row["tag"];
    string content = row["content"];

    set<string> words = unique_words(content);
    pair<string, double> prediction = classifier.predict(words);
    string predicted_label = prediction.first;
    double log_prob_score = prediction.second;

    if (predicted_label == correct_label) {
      correct_predictions++;
    }

    cout << "  correct = " << correct_label
         << ", predicted = " << predicted_label
         << ", log-probability score = " << log_prob_score << endl;
    cout << "  content = " << content << endl;
    cout << endl; // Blank line after each post
  }

  cout << "performance: " << correct_predictions << " / " << total_test_posts
       << " posts predicted correctly" << endl;
}

int main(int argc, char *argv[]) {
  // Set precision for all floating-point output
  cout.precision(3);

  if (argc < 2 || argc > 3) {
    cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
    return 1;
  }

  string train_filename = argv[1];
  string test_filename = (argc == 3) ? argv[2] : "";
  bool debug_mode = (argc == 2); // Train-only mode

  csvstream *train_csv = nullptr;
  try {
    train_csv = new csvstream(train_filename);
  } catch (const csvstream_exception &e) {
    cout << "Error opening file: " << train_filename << endl;
    return 1;
  }

  Classifier classifier;
  classifier.train(*train_csv, debug_mode);
  delete train_csv;
  train_csv = nullptr;

  if (debug_mode) {
    classifier.print_classifier_summary();
  } else {
    csvstream *test_csv = nullptr;
    try {
      test_csv = new csvstream(test_filename);
    } catch (const csvstream_exception &e) {
      cout << "Error opening file: " << test_filename << endl;
      return 1;
    }

    run_tests(classifier, *test_csv);

    delete test_csv;
    test_csv = nullptr;
  }

  return 0;
}
