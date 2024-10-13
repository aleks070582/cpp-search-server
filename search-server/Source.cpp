#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include<map>
#include<cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
struct Query {
    set<string> plus;
    set<string> minus;
};
string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document)
    {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words)
        {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        ++document_count;


    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    int document_count = 0;
    map<string, map<int, double>> word_to_document_freqs_;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const
    {
        Query query;
        for (const string& word : SplitIntoWordsNoStop(text))
        {
            if (word.at(0) != '-')
            {

                query.plus.insert(word);
            }
            else
            {
                string temp = word.substr(1);
                if (!IsStopWord(temp)) {
                    query.minus.insert(temp);
                }
            }

        }
        return query;
    }
    double ComputeWordInverseDocumentFreq(const string& word) const
    {
        if (!word_to_document_freqs_.contains(word)) return 0;
        if (word_to_document_freqs_.at(word).size() == 0) return 0;
        return log(document_count * 1.0 / word_to_document_freqs_.at(word).size());
    }


    vector<Document> FindAllDocuments(const Query query_words) const
    {
        vector<Document> matched_documents;
        map<int, double> document_to_relevance;
        for (const auto& word : query_words.plus)
        {
            double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            if (!word_to_document_freqs_.contains(word)) continue;
            for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word))
            {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }

        }
        for (const auto& word : query_words.minus)
        {
            if (!word_to_document_freqs_.contains(word)) continue;
            for (auto& [document_id, term_freq] : word_to_document_freqs_.at(word))
            {
                document_to_relevance.erase(document_id);
            }
        }


        for (const auto ch : document_to_relevance)
        {
            matched_documents.push_back({ ch.first, ch.second });
        }
        return matched_documents;
    }

};

SearchServer CreateSearchServer()
{
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main()
{
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query))
    {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}