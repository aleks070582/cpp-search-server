#pragma once

#include<deque>
#include"search_server.h"
#include"document.h"


class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) :request_server_(search_server)
    {

    }

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;

private:
    struct QueryResult
    {
        bool result;

    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& request_server_;

};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> document = request_server_.FindTopDocuments(raw_query, document_predicate);
    
    
    requests_.push_back({! document.empty() });
    if (requests_.size() > min_in_day_)
    {
        requests_.pop_front();
    }
    return document;
}
