#include<string>
#include"search_server.h"

using namespace std;

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words.size();
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    document_ids_.push_back(document_id);
}

tuple<vector<string>, DocumentStatus>SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);

    vector<string> matched_words;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Word "s + word + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

 int  SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

 SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const {
     if (text.empty()) {
         throw invalid_argument("Query word is empty"s);
     }
     string word = text;
     bool is_minus = false;
     if (word[0] == '-') {
         is_minus = true;
         word = word.substr(1);
     }
     if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
         throw invalid_argument("Query word "s + text + " is invalid");
     }

     return { word, is_minus, IsStopWord(word) };
 }

 SearchServer::Query SearchServer::ParseQuery(const string& text) const {
     Query result;
     for (const string& word : SplitIntoWords(text)) {
         const auto query_word = ParseQueryWord(word);
         if (!query_word.is_stop) {
             if (query_word.is_minus) {
                 result.minus_words.insert(query_word.data);
             }
             else {
                 result.plus_words.insert(query_word.data);
             }
         }
     }
     return result;
 }

