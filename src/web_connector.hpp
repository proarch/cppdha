#ifndef WEB_CONNECTOR_H
#define WEB_CONNECTOR_H

#include <string>
#include <sstream>
#include <curl/curl.h>

enum class HttpVerb
{
    GET,
    POST,
    PUT,
    DELETE
};

class WebConnector
{
    private:

    public:
        std::string Call(HttpVerb verb, std::string&& url);
        std::string Call(HttpVerb verb, std::string&& url, std::string&& authorization);
        std::string Call(HttpVerb verb, std::string&& url, std::string&& authorization, std::string&& data);
};

#endif