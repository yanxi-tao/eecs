#include "csvstream.hpp"
#include <cmath>
#include <iostream>
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
  void train(csvstream &train_file, bool train_mode = true) {
    if (train_mode) {
      cout << "training data:" << endl;
    }
    map<string, string> row;
    while (train_file >> row) {
      string label = row["tag"];
      string content = row["content"];

      if (train_mode) {
        cout << "  label = " << label << ", content = " << content << endl;
      }

      total_posts++;
      labels.insert(label);
      label_counts[label]++;

      set<string> words = unique_words(content);
      for (const string &word : words) {
        vocabulary.insert(word);
        words_counts[word]++;
        label_word_counts[{label, word}]++;
      }
    }
    if (train_mode) {
      cout << "trained on " << total_posts << " examples" << endl;
      cout << "vocabulary size = " << vocabulary.size() << endl;
      cout << endl;
    }
  }

  void predict(csvstream &test_file) const {
    cout << "trained on " << total_posts << " examples" << endl;
    cout << endl;
    cout << "test data:" << endl;
    map<string, string> row;
    int num_correct = 0;
    int num_test_posts = 0;
    while (test_file >> row) {
      num_test_posts++;
      string correct_label = row["tag"];
      string content = row["content"];
      set<string> words = unique_words(content);

      string best_label = *labels.begin();
      double best_log_prob = log_prob_score(words, *labels.begin());

      for (const string &label : labels) {
        double log_prob = log_prob_score(words, label);
        if (log_prob > best_log_prob) {
          best_log_prob = log_prob;
          best_label = label;
        }
      }
      if (best_label == correct_label) {
        num_correct++;
      }
      print_test_summary(correct_label, best_label, best_log_prob, content);
    }
    cout << "performance: " << num_correct << " / " << num_test_posts
         << " posts predicted correctly" << endl;
  }

  void print_classifier_summary() const {
    cout << "classes:" << endl;
    for (const string &label : labels) {
      double log_prior = log_prior_score(label);
      cout << "  " << label << ", " << label_counts.at(label)
           << " examples, log-prior = " << log_prior << endl;
    }
    cout << "classifier parameters:" << endl;
    for (const auto &pair : label_word_counts) {
      const string &label = pair.first.first;
      const string &word = pair.first.second;
      int count = pair.second;
      double log_likelihood = log_likelihood_score(label, word);

      cout << "  " << label << ":" << word << ", count = " << count
           << ", log-likelihood = " << log_likelihood << endl;
    }
    cout << endl;
  }

  void print_test_summary(const string &correct_label,
                          const string &predicted_label, double log_prob_score,
                          const string &content) const {
    cout << "  correct = " << correct_label
         << ", predicted = " << predicted_label
         << ", log-probability score = " << log_prob_score << endl;
    cout << "  content = " << content << endl;
    cout << endl; // Blank line after each post
  }

private:
  double log_prior_score(const string &label) const {
    return log((double)label_counts.at(label) / total_posts);
  }

  double log_likelihood_score(const string &label, const string &word) const {

    // Case 1: Word w seen in posts with label C
    if (label_word_counts.count({label, word})) {
      int num_label_word_posts = label_word_counts.at({label, word});
      int num_label_posts = label_counts.at(label);
      return log((double)num_label_word_posts / num_label_posts);
    }
    // Case 2: Word w not seen in label C, but seen in training data
    else if (words_counts.count(word)) {
      int num_word_posts = words_counts.at(word);
      return log((double)num_word_posts / total_posts);
    }
    // Case 3: Word w not seen anywhere in training data
    else {
      return log(1.0 / total_posts);
    }
  }

  double log_prob_score(const set<string> &words, const string &label) const {
    double log_prob = log_prior_score(label);
    for (const string &word : words) {
      log_prob += log_likelihood_score(label, word);
    }
    return log_prob;
  }

  int total_posts = 0;
  set<string> labels;
  set<string> vocabulary;

  map<string, int> words_counts;
  map<string, int> label_counts;
  map<pair<string, string>, int> label_word_counts;
};

int main(int argc, char *argv[]) {
  // Set precision for all floating-point output
  cout.precision(3);

  if (argc < 2 || argc > 3) {
    cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
    return 1;
  }

  string train_filename = argv[1];
  string test_filename = (argc == 3) ? argv[2] : "";
  bool train_mode = test_filename.empty();

  Classifier classifier;
  try {
    csvstream train_csv(train_filename);
    classifier.train(train_csv, train_mode);
    if (train_mode) {
      classifier.print_classifier_summary();
    }
  } catch (...) {
    cout << "Error opening file: " << train_filename << endl;
  }
  if (!test_filename.empty()) {
    try {
      csvstream test_csv(test_filename);
      classifier.predict(test_csv);
    } catch (...) {
      cout << "Error opening file: " << test_filename << endl;
    }
  }
  return 0;
}
