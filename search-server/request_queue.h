#include "request_queue.h"

using namespace std;

vector<Document> RequestQueue::AddFindRequest(const string & raw_query, DocumentStatus status) {
    vector<Document> document = request_server_.FindTopDocuments(raw_query, status);

    requests_.push_back({ !document.empty() });
  
   
    if (requests_.size() > min_in_day_)
    {
        requests_.pop_front();
    }
    return document;
}

vector<Document> RequestQueue::AddFindRequest(const string & raw_query) {
    vector<Document> document = request_server_.FindTopDocuments(raw_query);
    requests_.push_back({! document.empty() });

    if (requests_.size() > min_in_day_)
    {
        requests_.pop_front();
    }
    return document;
}

int RequestQueue::GetNoResultRequests() const
{
    int result = 0;
    for (const QueryResult& ch : requests_)
    {
        if (ch.result==false)
        {
            ++result;
        }
    }
    return result;
}
