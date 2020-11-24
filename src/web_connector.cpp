#include "web_connector.hpp"

size_t curl_callback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e)
    {
        return 0; //handle memory problem
    }
    return newLength;
}

std::string WebConnector::Call(HttpVerb verb, std::string&& url)
{
    return this->Call(verb, std::move(url), "", "");
}

std::string WebConnector::Call(HttpVerb verb, std::string&& url, std::string&& authorization)
{
    return this->Call(verb, std::move(url), std::move(authorization), "");
}

std::string WebConnector::Call(HttpVerb verb, std::string&& url, std::string&& authorization, std::string&& data)
{   
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL; // init to NULL is important 
    std::ostringstream oss;
    std::string output;
    
    headers = curl_slist_append(headers, "X-Kite-Version: 3"); 
    if(authorization.size() > 0)
    {
        headers = curl_slist_append(headers, authorization.c_str());
    }

    curl = curl_easy_init();

    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        switch(verb)
        {
            /*
            case HttpVerb::GET: 
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                break;
            */
            case HttpVerb::POST: 
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                break;
            case HttpVerb::PUT: 
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                break;
            case HttpVerb::DELETE: 
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

        if(data.size() > 0 && verb != HttpVerb::GET)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());
        }

        res = curl_easy_perform(curl);
        /* TODO: handle curl errors
        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        */

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return output;
}