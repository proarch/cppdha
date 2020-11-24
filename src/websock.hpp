#ifndef WEB_SOCK_H
#define WEB_SOCK_H

#include <iostream>
// standard header includes
#include <functional>
#include <cstdlib>
#include <cmath>
#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <limits>

// boost header includes
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

// project header includes
#include "root_certificates.hpp"
#include "models.hpp"

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>

using namespace std::literals::chrono_literals;

class WebSock
{
    private:
        const std::string host = "ws.kite.trade";
        const std::string port = "443";

        std::unique_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>> ws;

        std::string apiKey; 
        std::string accessKey;
        std::function<void(MarketData&&)> callback;

        void ReadThread();

    public:
        bool IsConnected { false };
        void Connect(std::string apiKey, std::string accessKey, std::function<void(MarketData&&)> callback);
        void Send(std::string&& msg);
        void Close();

        static std::shared_mutex MarketDataMutex;
};

//g++ -std=c++17 websock.cpp -o websock.exe -Wall -pthread -lssl -lcrypto -I/usr/local/include/

#endif //WEB_SOCK_H