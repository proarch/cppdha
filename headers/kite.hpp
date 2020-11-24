#ifndef KITE_H
#define KITE_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

// headers for sha encryption
#include "modes.h"
#include "sha.h"
#include "filters.h"
#include "hex.h"

#include "web_connector.hpp"
#include "websock.hpp"
#include "models.hpp"

#include <json.hpp>
using json = nlohmann::json;

class Kite
{
    private:
        std::string apiKey;
        std::string apiSecret;

        Token token; // holds all user information and api_key, access, public token.

        WebConnector webConnector;
        WebSock websock;        

        std::string GetChecksum(std::string& requestToken);

    public:
        Kite(std::string apiKey, std::string apiSecret) : apiKey(std::move(apiKey)), apiSecret(std::move(apiSecret))
        {

        };
        void Authenticate(const std::string& redirectedUrl);
        void Logout();
        KiteResponse<std::vector<Margin>> GetMargins();
        KiteResponse<std::vector<Holding>> GetHoldings();
        KiteResponse<std::vector<Position>> GetPositions(); // TODO: currently it pulls only net positions
        KiteResponse<std::vector<Quote>> GetQuotes(std::string exchange, std::string symbol);
        KiteResponse<std::string> NewOrder(std::string symbol, std::string exchange, std::string transactionType, std::string orderType, int qty, float price, float triggerPrice, std::string product, std::string validity);
        KiteResponse<std::string> ModifyOrder(std::string orderId, std::string orderType, int qty, float price, float triggerPrice, std::string validity);
        KiteResponse<std::string> CancelOrder(std::string orderId);
        KiteResponse<std::vector<Order>> GetOrders();
        KiteResponse<std::vector<Order>> GetOrderHistory(std::string& orderId);
        KiteResponse<std::vector<Trade>> GetTrades();
        KiteResponse<std::vector<Trade>> GetTradesForOrder(std::string& orderId);

        // web socket streaming
        void SubscribeMarketData(int instrumentToken, std::function<void(MarketData&&)> callback);
        void SubscribeMarketData(std::string instruments, std::function<void(MarketData&&)> callback);
        void UnsubscribeMarketData(int instrumentTokens);        
};

#endif // KITE_H