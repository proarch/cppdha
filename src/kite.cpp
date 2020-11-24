#include "kite.hpp"

std::string Kite::GetChecksum(std::string& requestToken)
{
    CryptoPP::SHA256 hash;
    byte digest[ CryptoPP::SHA256::DIGESTSIZE ];
    std::string message = this->apiKey + requestToken + this->apiSecret;

    hash.CalculateDigest(digest, (byte*) message.c_str(), message.length());

    CryptoPP::HexEncoder encoder;
    std::string checksum;
    encoder.Attach( new CryptoPP::StringSink( checksum ) );
    encoder.Put( digest, sizeof(digest) );
    encoder.MessageEnd();

    transform(checksum.begin(), checksum.end(), checksum.begin(), ::tolower);
    
    return checksum;
}

void Kite::Authenticate(const std::string& redirectedUrl)
{
    std::string requestTokenField {"request_token="};
    std::string requestToken;

    std::size_t startpos = redirectedUrl.find(requestTokenField);
    if(startpos != std::string::npos)
    {
        std::size_t endpos = redirectedUrl.find("&", startpos);
        if(endpos == std::string::npos)
        {
            endpos = redirectedUrl.size();
        }

        requestToken = redirectedUrl.substr(startpos + requestTokenField.size(), endpos - startpos - requestTokenField.size());
    }

    std::string checksum = GetChecksum(requestToken);

    std::string data = "api_key=" + this->apiKey + "&" + "request_token=" + requestToken + "&" + "checksum=" + checksum;
    std::string output = webConnector.Call(HttpVerb::POST, "https://api.kite.trade/session/token", "", std::move(data));

    auto jsonToken = json::parse(output);
    if(jsonToken["data"].find("user_id") != jsonToken["data"].end() && jsonToken["data"]["user_id"] != nullptr)
        this->token.UserId = jsonToken["data"]["user_id"].get<std::string>();
    if(jsonToken["data"].find("user_name") != jsonToken["data"].end() && jsonToken["data"]["user_name"] != nullptr)
        this->token.UserName = jsonToken["data"]["user_name"].get<std::string>();
    if(jsonToken["data"].find("user_shortname") != jsonToken["data"].end() && jsonToken["data"]["user_shortname"] != nullptr)
        this->token.UserShortname = jsonToken["data"]["user_shortname"].get<std::string>();
    if(jsonToken["data"].find("email") != jsonToken["data"].end() && jsonToken["data"]["email"] != nullptr)
        this->token.Email = jsonToken["data"]["email"].get<std::string>();
    if(jsonToken["data"].find("user_type") != jsonToken["data"].end() && jsonToken["data"]["user_type"] != nullptr)
        this->token.UserType = jsonToken["data"]["user_type"].get<std::string>();
    if(jsonToken["data"].find("broker") != jsonToken["data"].end() && jsonToken["data"]["broker"] != nullptr)
        this->token.Broker = jsonToken["data"]["broker"].get<std::string>();
    if(jsonToken["data"].find("exchanges") != jsonToken["data"].end())
        this->token.Exchanges = jsonToken["data"]["exchanges"].get<std::vector<std::string>>();
    if(jsonToken["data"].find("products") != jsonToken["data"].end())
        this->token.Products = jsonToken["data"]["products"].get<std::vector<std::string>>();
    if(jsonToken["data"].find("order_types") != jsonToken["data"].end())
        this->token.OrderTypes = jsonToken["data"]["order_types"].get<std::vector<std::string>>();
    if(jsonToken["data"].find("api_key") != jsonToken["data"].end() && jsonToken["data"]["api_key"] != nullptr)
        this->token.ApiKey = jsonToken["data"]["api_key"].get<std::string>();
    if(jsonToken["data"].find("access_token") != jsonToken["data"].end() && jsonToken["data"]["access_token"] != nullptr)
        this->token.AccessToken = jsonToken["data"]["access_token"].get<std::string>();
    if(jsonToken["data"].find("public_token") != jsonToken["data"].end() && jsonToken["data"]["public_token"] != nullptr)
        this->token.PublicToken = jsonToken["data"]["public_token"].get<std::string>();
    if(jsonToken["data"].find("refresh_token") != jsonToken["data"].end() && jsonToken["data"]["refresh_token"] != nullptr)
        this->token.RefreshToken = jsonToken["data"]["refresh_token"].get<std::string>();
    if(jsonToken["data"].find("login_time") != jsonToken["data"].end() && jsonToken["data"]["login_time"] != nullptr)
    {
        strptime(jsonToken["data"]["login_time"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &this->token.LoginTime);
    }
    if(jsonToken["data"].find("avatar_url") != jsonToken["data"].end() && jsonToken["data"]["avatar_url"] != nullptr)
        this->token.AvatarUrl = jsonToken["data"]["avatar_url"].get<std::string>();
}

void Kite::Logout()
{
    std::string url  = "https://api.kite.trade/session/token?api_key=" + this->token.ApiKey + "&access_token=" + this->token.AccessToken;
    std::string output = webConnector.Call(HttpVerb::DELETE, std::move(url));
}

KiteResponse<std::vector<Margin>> Kite::GetMargins()
{
    KiteResponse<std::vector<Margin>> response;
    std::vector<Margin> margins;
    margins.reserve(2);

    std::string url = "https://api.kite.trade/user/margins";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));
    
    auto jsonMargins = json::parse(output);
    response.Status = jsonMargins["status"].get<std::string>();
    
    if(response.Status == "success")
    {
        
        if(jsonMargins["data"].find("equity") != jsonMargins["data"].end())
        {   
            // Equity margin details
            Margin equityMargin;

            if(jsonMargins["data"]["equity"].find("enabled") != jsonMargins["data"]["equity"].end())
                equityMargin.Enabled = jsonMargins["data"]["equity"]["enabled"].get<bool>();
            if(jsonMargins["data"]["equity"].find("net") != jsonMargins["data"]["equity"].end())
                equityMargin.Net = jsonMargins["data"]["equity"]["net"].get<float>();

            if(jsonMargins["data"]["equity"].find("available") != jsonMargins["data"]["equity"].end())
            {
                if(jsonMargins["data"]["equity"]["available"].find("adhoc_margin") != jsonMargins["data"]["equity"]["available"].end())
                    equityMargin.AvailableAdhocMargin = jsonMargins["data"]["equity"]["available"]["adhoc_margin"].get<float>();
                if(jsonMargins["data"]["equity"]["available"].find("cash") != jsonMargins["data"]["equity"]["available"].end())
                    equityMargin.AvailableCash = jsonMargins["data"]["equity"]["available"]["cash"].get<float>();
                if(jsonMargins["data"]["equity"]["available"].find("collateral") != jsonMargins["data"]["equity"]["available"].end())
                    equityMargin.AvailableCollateral = jsonMargins["data"]["equity"]["available"]["collateral"].get<float>();
                if(jsonMargins["data"]["equity"]["available"].find("intraday_payin") != jsonMargins["data"]["equity"]["available"].end())
                    equityMargin.AvailableIntradayPayIn = jsonMargins["data"]["equity"]["available"]["intraday_payin"].get<float>();
            }
            if(jsonMargins["data"]["equity"].find("utilised") != jsonMargins["data"]["equity"].end())
            {
                if(jsonMargins["data"]["equity"]["utilised"].find("debits") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedDebits = jsonMargins["data"]["equity"]["utilised"]["debits"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("exposure") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedExposure = jsonMargins["data"]["equity"]["utilised"]["exposure"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("m2m_realised") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedM2MRealised = jsonMargins["data"]["equity"]["utilised"]["m2m_realised"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("m2m_unrealised") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedM2MUnrealised = jsonMargins["data"]["equity"]["utilised"]["m2m_unrealised"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("option_premium") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizeOptionPremium = jsonMargins["data"]["equity"]["utilised"]["option_premium"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("payout") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedPayout = jsonMargins["data"]["equity"]["utilised"]["payout"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("span") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedSpan = jsonMargins["data"]["equity"]["utilised"]["span"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("holding_sales") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedHoldingSales = jsonMargins["data"]["equity"]["utilised"]["holding_sales"].get<float>();
                if(jsonMargins["data"]["equity"]["utilised"].find("turnover") != jsonMargins["data"]["equity"]["utilised"].end())
                    equityMargin.UtilizedTurnover = jsonMargins["data"]["equity"]["utilised"]["turnover"].get<float>();
            }

            response.Data.push_back(std::move(equityMargin));
        }

        if(jsonMargins["data"].find("commodity") != jsonMargins["data"].end())
        {
            // Commodity margin details
            Margin commodityMargin;
    
            if(jsonMargins["data"]["commodity"].find("enabled") != jsonMargins["data"]["commodity"].end())
                commodityMargin.Enabled = jsonMargins["data"]["commodity"]["enabled"].get<bool>();
            if(jsonMargins["data"]["commodity"].find("net") != jsonMargins["data"]["commodity"].end())
                commodityMargin.Net = jsonMargins["data"]["commodity"]["net"].get<float>();

            if(jsonMargins["data"]["commodity"].find("available") != jsonMargins["data"]["commodity"].end())
            {
                if(jsonMargins["data"]["commodity"]["available"].find("adhoc_margin") != jsonMargins["data"]["commodity"]["available"].end())
                    commodityMargin.AvailableAdhocMargin = jsonMargins["data"]["commodity"]["available"]["adhoc_margin"].get<float>();
                if(jsonMargins["data"]["commodity"]["available"].find("cash") != jsonMargins["data"]["commodity"]["available"].end())
                    commodityMargin.AvailableCash = jsonMargins["data"]["commodity"]["available"]["cash"].get<float>();
                if(jsonMargins["data"]["commodity"]["available"].find("collateral") != jsonMargins["data"]["commodity"]["available"].end())
                    commodityMargin.AvailableCollateral = jsonMargins["data"]["commodity"]["available"]["collateral"].get<float>();
                if(jsonMargins["data"]["commodity"]["available"].find("intraday_payin") != jsonMargins["data"]["commodity"]["available"].end())
                    commodityMargin.AvailableIntradayPayIn = jsonMargins["data"]["commodity"]["available"]["intraday_payin"].get<float>();
            }

            if(jsonMargins["data"]["commodity"].find("utilised") != jsonMargins["data"]["commodity"].end())
            {
                if(jsonMargins["data"]["commodity"]["utilised"].find("debits") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedDebits = jsonMargins["data"]["commodity"]["utilised"]["debits"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("exposure") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedExposure = jsonMargins["data"]["commodity"]["utilised"]["exposure"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("m2m_realised") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedM2MRealised = jsonMargins["data"]["commodity"]["utilised"]["m2m_realised"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("m2m_unrealised") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedM2MUnrealised = jsonMargins["data"]["commodity"]["utilised"]["m2m_unrealised"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("option_premium") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizeOptionPremium = jsonMargins["data"]["commodity"]["utilised"]["option_premium"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("payout") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedPayout = jsonMargins["data"]["commodity"]["utilised"]["payout"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("span") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedSpan = jsonMargins["data"]["commodity"]["utilised"]["span"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("holding_sales") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedHoldingSales = jsonMargins["data"]["commodity"]["utilised"]["holding_sales"].get<float>();
                if(jsonMargins["data"]["commodity"]["utilised"].find("turnover") != jsonMargins["data"]["commodity"]["utilised"].end())
                    commodityMargin.UtilizedTurnover = jsonMargins["data"]["commodity"]["utilised"]["turnover"].get<float>();
            }

            response.Data.push_back(std::move(commodityMargin));
        }
    }
    else
    {
        response.Message = jsonMargins["message"].get<std::string>();
    }
        
    return response;
}

KiteResponse<std::vector<Holding>> Kite::GetHoldings()
{
    KiteResponse<std::vector<Holding>> response;

    std::string url  = "https://api.kite.trade/portfolio/holdings";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        auto holdingList = jsonOutput["data"].get<std::vector<std::string>>();
        for(auto& holdingItem : holdingList)
        {
            auto jsonHolding = json::parse(holdingItem);
            Holding holding;

            if(jsonHolding.find("tradingsymbol") != jsonHolding.end() && jsonHolding["tradingsymbol"] != nullptr)
                holding.Symbol = jsonHolding["tradingsymbol"].get<std::string>();
            if(jsonHolding.find("exchange") != jsonHolding.end() && jsonHolding["exchange"] != nullptr)
                holding.Exchange = jsonHolding["exchange"].get<std::string>();
            if(jsonHolding.find("isin") != jsonHolding.end() && jsonHolding["isin"] != nullptr)
                holding.ISIN = jsonHolding["isin"].get<std::string>();
            if(jsonHolding.find("quantity") != jsonHolding.end())
                holding.Quantity = jsonHolding["quantity"].get<int>();
            if(jsonHolding.find("t1_quantity") != jsonHolding.end())
                holding.T1Quantity = jsonHolding["t1_quantity"].get<int>();
            if(jsonHolding.find("average_price") != jsonHolding.end())
                holding.AveragePrice = jsonHolding["average_price"].get<float>();
            if(jsonHolding.find("last_price") != jsonHolding.end())
                holding.LastPrice = jsonHolding["last_price"].get<float>();
            if(jsonHolding.find("pnl") != jsonHolding.end())
                holding.PNL = jsonHolding["pnl"].get<float>();
            if(jsonHolding.find("product") != jsonHolding.end() && jsonHolding["product"] != nullptr)
                holding.Product = jsonHolding["product"].get<std::string>();
            if(jsonHolding.find("collateral_quantity") != jsonHolding.end())
                holding.CollateralQuantity = jsonHolding["collateral_quantity"].get<int>();
            if(jsonHolding.find("collateral_type") != jsonHolding.end() && jsonHolding["collateral_type"] != nullptr)
                holding.CollateralType = jsonHolding["collateral_type"].get<std::string>();

            response.Data.push_back(std::move(holding));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }    

    return response;
}

KiteResponse<std::vector<Position>> Kite::GetPositions()
{
    KiteResponse<std::vector<Position>> response;

    std::string url  = "https://api.kite.trade/portfolio/positions";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        if(jsonOutput["data"].find("net") != jsonOutput["data"].end())
        {
            for(auto& positionItem : jsonOutput["data"]["net"].items())
            {
                auto& jsonPosition = positionItem.value();
                Position position;

                if(jsonPosition.find("tradingsymbol") != jsonPosition.end() && jsonPosition["tradingsymbol"] != nullptr)
                    position.Symbol = jsonPosition["tradingsymbol"].get<std::string>();
                if(jsonPosition.find("exchange") != jsonPosition.end() && jsonPosition["exchange"] != nullptr)
                    position.Exchange = jsonPosition["exchange"].get<std::string>();
                if(jsonPosition.find("instrument_token") != jsonPosition.end())
                    position.InstrumentToken = jsonPosition["instrument_token"].get<int>();
                if(jsonPosition.find("product") != jsonPosition.end() && jsonPosition["product"] != nullptr)
                    position.Product = jsonPosition["product"].get<std::string>();
                if(jsonPosition.find("quantity") != jsonPosition.end())
                    position.Quantity = jsonPosition["quantity"].get<int>();
                if(jsonPosition.find("overnight_quantity") != jsonPosition.end())
                    position.OvernightQuantity = jsonPosition["overnight_quantity"].get<int>();
                if(jsonPosition.find("multiplier") != jsonPosition.end())
                    position.Multiplier = jsonPosition["multiplier"].get<short>();
                if(jsonPosition.find("average_price") != jsonPosition.end())
                    position.AveragePrice = jsonPosition["average_price"].get<float>();
                if(jsonPosition.find("close_price") != jsonPosition.end())
                    position.ClosePrice = jsonPosition["close_price"].get<float>();
                if(jsonPosition.find("last_price") != jsonPosition.end())
                    position.LastPrice = jsonPosition["last_price"].get<float>();
                if(jsonPosition.find("value") != jsonPosition.end())
                    position.Value = jsonPosition["value"].get<float>();
                if(jsonPosition.find("pnl") != jsonPosition.end())
                    position.PNL = jsonPosition["pnl"].get<float>();
                if(jsonPosition.find("m2m") != jsonPosition.end())
                    position.M2M = jsonPosition["m2m"].get<float>();
                if(jsonPosition.find("unrealised") != jsonPosition.end())
                    position.Unrealized = jsonPosition["unrealised"].get<float>();
                if(jsonPosition.find("realised") != jsonPosition.end())
                    position.Realized = jsonPosition["realised"].get<float>();
                if(jsonPosition.find("buy_quantity") != jsonPosition.end())
                    position.BuyQuantity = jsonPosition["buy_quantity"].get<int>();
                if(jsonPosition.find("buy_price") != jsonPosition.end())
                    position.BuyPrice = jsonPosition["buy_price"].get<float>();
                if(jsonPosition.find("buy_value") != jsonPosition.end())
                    position.BuyValue = jsonPosition["buy_value"].get<float>();
                if(jsonPosition.find("buy_m2m") != jsonPosition.end())
                    position.BuyM2M = jsonPosition["buy_m2m"].get<float>();
                if(jsonPosition.find("day_buy_quantity") != jsonPosition.end())
                    position.DayBuyQuantity = jsonPosition["day_buy_quantity"].get<int>();
                if(jsonPosition.find("day_buy_price") != jsonPosition.end())
                    position.DayBuyPrice = jsonPosition["day_buy_price"].get<float>();
                if(jsonPosition.find("day_buy_value") != jsonPosition.end())
                    position.DayBuyValue = jsonPosition["day_buy_value"].get<float>();
                if(jsonPosition.find("day_sell_quantity") != jsonPosition.end())
                    position.DaySellQuantity = jsonPosition["day_sell_quantity"].get<int>();
                if(jsonPosition.find("day_sell_price") != jsonPosition.end())
                    position.DaySellPrice = jsonPosition["day_sell_price"].get<float>();
                if(jsonPosition.find("day_sell_value") != jsonPosition.end())
                    position.DaySellValue = jsonPosition["day_sell_value"].get<float>();
                if(jsonPosition.find("sell_quantity") != jsonPosition.end())
                    position.SellQuantity = jsonPosition["sell_quantity"].get<int>();
                if(jsonPosition.find("sell_price") != jsonPosition.end())
                    position.SellPrice = jsonPosition["sell_price"].get<float>();
                if(jsonPosition.find("sell_value") != jsonPosition.end())
                    position.SellValue = jsonPosition["sell_value"].get<float>();
                if(jsonPosition.find("sell_m2m") != jsonPosition.end())
                    position.SellM2M = jsonPosition["sell_m2m"].get<float>();

                response.Data.push_back(std::move(position));
            }
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    } 

    return response;
}

KiteResponse<std::string> Kite::NewOrder(std::string symbol, std::string exchange, std::string transactionType, std::string orderType, int qty, float price, float triggerPrice, std::string product, std::string validity)
{
    KiteResponse<std::string> response;

    std::string url  = "https://api.kite.trade/orders/regular";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;
    
    std::string data;

    data += "tradingsymbol=" + symbol;
    data += "&";
    data += "exchange=" + exchange;
    data += "&";
    data += "transaction_type=" + transactionType;
    data += "&";
    data += "order_type=" + orderType;
    data += "&";
    data += "quantity=" + std::to_string(qty);
    data += "&";
    data += "product=" + product;
    data += "&";
    data += "validity=" + validity;
    if(orderType == "LIMIT")
    {
        data += "&";
        data += "price=" + std::to_string(price);
    }
    if(orderType == "SL" || orderType == "SLM")
    {
        data += "&";
        data += "trigger_price=" + std::to_string(triggerPrice);
    }

    std::string output = webConnector.Call(HttpVerb::POST, std::move(url), std::move(authorization), std::move(data));
std::cout << "New Order: " << output << std::endl;
    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        response.Data = jsonOutput["data"]["order_id"].get<std::string>();
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }
    
    return response;
}

KiteResponse<std::string> Kite::ModifyOrder(std::string orderId, std::string orderType, int qty, float price, float triggerPrice, std::string validity)
{
    KiteResponse<std::string> response;

    std::string url  = "https://api.kite.trade/orders/regular/" + orderId;
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;
    
    std::string data;
    data += "order_type=" + orderType;
    data += "&";
    data += "quantity=" + qty;
    data += "&";
    data += "validity=" + validity;
    if(orderType == "LIMIT")
    {
        data += "&";
        data += "price=" + std::to_string(price);
    }
    if(orderType == "SL" || orderType == "SLM")
    {
        data += "&";
        data += "trigger_price=" + std::to_string(triggerPrice);
    }

    std::string output = webConnector.Call(HttpVerb::PUT, std::move(url), std::move(authorization), std::move(data));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        response.Data = jsonOutput["data"]["order_id"].get<std::string>();
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }

    return response;
}

KiteResponse<std::string> Kite::CancelOrder(std::string orderId)
{
    KiteResponse<std::string> response;

    std::string url  = "https://api.kite.trade/orders/regular/" + orderId;
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::DELETE, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        response.Data = jsonOutput["data"]["order_id"].get<std::string>();
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }

    return response;
}

KiteResponse<std::vector<Order>> Kite::GetOrders()
{
    KiteResponse<std::vector<Order>> response;

    std::string url  = "https://api.kite.trade/orders";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        for(auto& orderItem : jsonOutput["data"].items())
        {   
            auto& jsonOrder = orderItem.value();
            Order order;
        
            if(jsonOrder.find("order_id") != jsonOrder.end() && jsonOrder["order_id"] != nullptr)
                order.OrderId = jsonOrder["order_id"].get<std::string>();
            if(jsonOrder.find("parent_order_id") != jsonOrder.end() && jsonOrder["parent_order_id"] != nullptr)
                order.ParentOrderId = jsonOrder["parent_order_id"].get<std::string>();
            if(jsonOrder.find("exchange_order_id") != jsonOrder.end() && jsonOrder["exchange_order_id"] != nullptr)
                order.ExchangeOrderId = jsonOrder["exchange_order_id"].get<std::string>();
            if(jsonOrder.find("placed_by") != jsonOrder.end() && jsonOrder["placed_by"] != nullptr)
                order.PlacedBy = jsonOrder["placed_by"].get<std::string>();
            if(jsonOrder.find("variety") != jsonOrder.end() && jsonOrder["variety"] != nullptr)
                order.Variety = jsonOrder["variety"].get<std::string>();
            if(jsonOrder.find("status") != jsonOrder.end() && jsonOrder["status"] != nullptr)
                order.Status = jsonOrder["status"].get<std::string>();
            if(jsonOrder.find("tradingsymbol") != jsonOrder.end() && jsonOrder["tradingsymbol"] != nullptr)
                order.Symbol = jsonOrder["tradingsymbol"].get<std::string>();
            if(jsonOrder.find("exchange") != jsonOrder.end() && jsonOrder["exchange"] != nullptr)
                order.Exchange = jsonOrder["exchange"].get<std::string>();
            if(jsonOrder.find("instrument_token") != jsonOrder.end())
                order.InstrumentToken = jsonOrder["instrument_token"].get<int>();
            if(jsonOrder.find("transaction_type") != jsonOrder.end() && jsonOrder["transaction_type"] != nullptr)
                order.TransactionType = jsonOrder["transaction_type"].get<std::string>();
            if(jsonOrder.find("order_type") != jsonOrder.end() && jsonOrder["order_type"] != nullptr)
                order.OrderType = jsonOrder["order_type"].get<std::string>();
            if(jsonOrder.find("product") != jsonOrder.end() && jsonOrder["product"] != nullptr)
                order.Product = jsonOrder["product"].get<std::string>();
            if(jsonOrder.find("validity") != jsonOrder.end() && jsonOrder["validity"] != nullptr)
                order.Validity = jsonOrder["validity"].get<std::string>();
            if(jsonOrder.find("price") != jsonOrder.end())
                order.Price = jsonOrder["price"].get<float>();
            if(jsonOrder.find("quantity") != jsonOrder.end())
                order.Quantity = jsonOrder["quantity"].get<int>();
            if(jsonOrder.find("trigger_price") != jsonOrder.end())
                order.TriggerPrice = jsonOrder["trigger_price"].get<float>();
            if(jsonOrder.find("average_price") != jsonOrder.end())
                order.AveragePrice = jsonOrder["average_price"].get<float>();
            if(jsonOrder.find("pending_quantity") != jsonOrder.end())
                order.PendingQuantity = jsonOrder["pending_quantity"].get<int>();
            if(jsonOrder.find("filled_quantity") != jsonOrder.end())
                order.FilledQuantity = jsonOrder["filled_quantity"].get<int>();
            //order.CancelledQuantity = jsonOrder[""].get<int>();
            if(jsonOrder.find("disclosed_quantity") != jsonOrder.end())
                order.DisclosedQuantity = jsonOrder["disclosed_quantity"].get<int>();
            if(jsonOrder.find("market_protection") != jsonOrder.end())
                order.MarketProtection = jsonOrder["market_protection"].get<int>();
            if(jsonOrder.find("order_timestamp") != jsonOrder.end() && jsonOrder["order_timestamp"] != nullptr)
                strptime(jsonOrder["order_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.OrderTimestamp);
            if(jsonOrder.find("exchange_timestamp") != jsonOrder.end() && jsonOrder["exchange_timestamp"] != nullptr)
                strptime(jsonOrder["exchange_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.ExchangeTimestamp);
            if(jsonOrder.find("exchange_update_timestamp") != jsonOrder.end() && jsonOrder["exchange_update_timestamp"] != nullptr)
                strptime(jsonOrder["exchange_update_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.ExchangeUpdateTimestamp);
            if(jsonOrder.find("status_message") != jsonOrder.end() && jsonOrder["status_message"] != nullptr)
                order.StatusMessage = jsonOrder["status_message"].get<std::string>();

            response.Data.push_back(std::move(order));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }

    return response;
}

KiteResponse<std::vector<Order>> Kite::GetOrderHistory(std::string& orderId)
{
    KiteResponse<std::vector<Order>> response;    

    std::string url  = "https://api.kite.trade/orders/" + orderId;
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        for(auto& orderItem : jsonOutput["data"].items())
        {
            auto& jsonOrder = orderItem.value();
            Order order;

            if(jsonOrder.find("average_price") != jsonOrder.end())
                order.AveragePrice = jsonOrder["average_price"].get<float>();
            if(jsonOrder.find("cancelled_quantity") != jsonOrder.end())
                order.CancelledQuantity = jsonOrder["cancelled_quantity"].get<int>();
            if(jsonOrder.find("disclosed_quantity") != jsonOrder.end())
                order.DisclosedQuantity = jsonOrder["disclosed_quantity"].get<int>();
            if(jsonOrder.find("exchange") != jsonOrder.end() && jsonOrder["exchange"] != nullptr)
                order.Exchange = jsonOrder["exchange"].get<std::string>();
            if(jsonOrder.find("exchange_order_id") != jsonOrder.end() && jsonOrder["exchange_order_id"] != nullptr)
                order.ExchangeOrderId = jsonOrder["exchange_order_id"].get<std::string>();
            if(jsonOrder.find("exchange_timestamp") != jsonOrder.end() && jsonOrder["exchange_timestamp"] != nullptr)
                strptime(jsonOrder["exchange_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.ExchangeTimestamp);
            if(jsonOrder.find("exchange_update_timestamp") != jsonOrder.end() && jsonOrder["exchange_update_timestamp"] != nullptr)
                strptime(jsonOrder["exchange_update_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.ExchangeUpdateTimestamp);
            if(jsonOrder.find("filled_quantity") != jsonOrder.end())
                order.FilledQuantity = jsonOrder["filled_quantity"].get<int>();
            if(jsonOrder.find("instrument_token") != jsonOrder.end())
                order.InstrumentToken = jsonOrder["instrument_token"].get<int>();
            if(jsonOrder.find("market_protection") != jsonOrder.end())
                order.MarketProtection = jsonOrder["market_protection"].get<int>();
            if(jsonOrder.find("order_id") != jsonOrder.end() && jsonOrder["order_id"] != nullptr)
                order.OrderId = jsonOrder["order_id"].get<std::string>();
            if(jsonOrder.find("order_timestamp") != jsonOrder.end() && jsonOrder["order_timestamp"] != nullptr)
                strptime(jsonOrder["order_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &order.OrderTimestamp);
            if(jsonOrder.find("order_type") != jsonOrder.end() && jsonOrder["order_type"] != nullptr)
                order.OrderType = jsonOrder["order_type"].get<std::string>();
            if(jsonOrder.find("parent_order_id") != jsonOrder.end() && jsonOrder["parent_order_id"] != nullptr)
                order.ParentOrderId = jsonOrder["parent_order_id"].get<std::string>();
            if(jsonOrder.find("pending_quantity") != jsonOrder.end())
                order.PendingQuantity = jsonOrder["pending_quantity"].get<int>();
            if(jsonOrder.find("placed_by") != jsonOrder.end() && jsonOrder["placed_by"] != nullptr)
                order.PlacedBy = jsonOrder["placed_by"].get<std::string>();
            if(jsonOrder.find("price") != jsonOrder.end())
                order.Price = jsonOrder["price"].get<float>();
            if(jsonOrder.find("product") != jsonOrder.end() && jsonOrder["product"] != nullptr)
                order.Product = jsonOrder["product"].get<std::string>();
            if(jsonOrder.find("quantity") != jsonOrder.end())
                order.Quantity = jsonOrder["quantity"].get<int>();
            if(jsonOrder.find("status") != jsonOrder.end() && jsonOrder["status"] != nullptr)
                order.Status = jsonOrder["status"].get<std::string>();
            if(jsonOrder.find("status_message") != jsonOrder.end() && jsonOrder["status_message"] != nullptr)
                order.StatusMessage = jsonOrder["status_message"].get<std::string>();
            if(jsonOrder.find("tradingsymbol") != jsonOrder.end() && jsonOrder["tradingsymbol"] != nullptr)
                order.Symbol = jsonOrder["tradingsymbol"].get<std::string>();
            if(jsonOrder.find("transaction_type") != jsonOrder.end() && jsonOrder["transaction_type"] != nullptr)
                order.TransactionType = jsonOrder["transaction_type"].get<std::string>();
            if(jsonOrder.find("trigger_price") != jsonOrder.end())
                order.TriggerPrice = jsonOrder["trigger_price"].get<float>();
            if(jsonOrder.find("validity") != jsonOrder.end() && jsonOrder["validity"] != nullptr)
                order.Validity = jsonOrder["validity"].get<std::string>();
            if(jsonOrder.find("variety") != jsonOrder.end() && jsonOrder["variety"] != nullptr)
                order.Variety = jsonOrder["variety"].get<std::string>();
        
            response.Data.push_back(std::move(order));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }
    
    return response;
}

KiteResponse<std::vector<Trade>> Kite::GetTrades()
{
    KiteResponse<std::vector<Trade>> response;

    std::string url  = "https://api.kite.trade/trades";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        for(auto& tradeItem : jsonOutput["data"].items())
        {
            auto& jsonTrade = tradeItem.value();
            Trade trade;

            if(jsonTrade.find("trade_id") != jsonTrade.end() && jsonTrade["trade_id"] != nullptr)
                trade.TradeId = jsonTrade["trade_id"].get<std::string>();
            if(jsonTrade.find("order_id") != jsonTrade.end() && jsonTrade["order_id"] != nullptr)
                trade.OrderId = jsonTrade["order_id"].get<std::string>();
            if(jsonTrade.find("exchange_order_id") != jsonTrade.end() && jsonTrade["exchange_order_id"] != nullptr)
                trade.ExchangeOrderId = jsonTrade["exchange_order_id"].get<std::string>();
            if(jsonTrade.find("tradingsymbol") != jsonTrade.end() && jsonTrade["tradingsymbol"] != nullptr)
                trade.Symbol = jsonTrade["tradingsymbol"].get<std::string>();
            if(jsonTrade.find("exchange") != jsonTrade.end() && jsonTrade["exchange"] != nullptr)
                trade.Exchange = jsonTrade["exchange"].get<std::string>();
            if(jsonTrade.find("instrument_token") != jsonTrade.end())
                trade.InstrumentToken = jsonTrade["instrument_token"].get<int>();
            if(jsonTrade.find("transaction_type") != jsonTrade.end() && jsonTrade["transaction_type"] != nullptr)
                trade.TransactionType = jsonTrade["transaction_type"].get<std::string>();
            if(jsonTrade.find("product") != jsonTrade.end() && jsonTrade["product"] != nullptr)
                trade.Product = jsonTrade["product"].get<std::string>();
            if(jsonTrade.find("average_price") != jsonTrade.end())
                trade.AveragePrice = jsonTrade["average_price"].get<float>();
            if(jsonTrade.find("quantity") != jsonTrade.end())
                trade.Quantity = jsonTrade["quantity"].get<int>();
            if(jsonTrade.find("fill_timestamp") != jsonTrade.end() && jsonTrade["fill_timestamp"] != nullptr)
                strptime(jsonTrade["fill_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &trade.FillTimestamp);
            if(jsonTrade.find("exchange_timestamp") != jsonTrade.end() && jsonTrade["exchange_timestamp"] != nullptr)
                strptime(jsonTrade["exchange_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &trade.ExchangeTimestamp);

            response.Data.push_back(std::move(trade));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }
    
    return response;
}

KiteResponse<std::vector<Trade>> Kite::GetTradesForOrder(std::string& orderId)
{
    KiteResponse<std::vector<Trade>> response;

    std::string url  = "https://api.kite.trade/orders/" + orderId + "/trades";
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        for(auto& tradeItem : jsonOutput["data"].items())
        {
            auto& jsonTrade = tradeItem.value();
            Trade trade;

            if(jsonTrade.find("trade_id") != jsonTrade.end() && jsonTrade["trade_id"] != nullptr)
                trade.TradeId = jsonTrade["trade_id"].get<std::string>();
            if(jsonTrade.find("order_id") != jsonTrade.end() && jsonTrade["order_id"] != nullptr)
                trade.OrderId = jsonTrade["order_id"].get<std::string>();
            if(jsonTrade.find("exchange_order_id") != jsonTrade.end() && jsonTrade["exchange_order_id"] != nullptr)
                trade.ExchangeOrderId = jsonTrade["exchange_order_id"].get<std::string>();
            if(jsonTrade.find("tradingsymbol") != jsonTrade.end() && jsonTrade["tradingsymbol"] != nullptr)
                trade.Symbol = jsonTrade["tradingsymbol"].get<std::string>();
            if(jsonTrade.find("exchange") != jsonTrade.end() && jsonTrade["exchange"] != nullptr)
                trade.Exchange = jsonTrade["exchange"].get<std::string>();
            if(jsonTrade.find("instrument_token") != jsonTrade.end())
                trade.InstrumentToken = jsonTrade["instrument_token"].get<int>();
            if(jsonTrade.find("transaction_type") != jsonTrade.end() && jsonTrade["transaction_type"] != nullptr)
                trade.TransactionType = jsonTrade["transaction_type"].get<std::string>();
            if(jsonTrade.find("product") != jsonTrade.end() && jsonTrade["product"] != nullptr)
                trade.Product = jsonTrade["product"].get<std::string>();
            if(jsonTrade.find("average_price") != jsonTrade.end())
                trade.AveragePrice = jsonTrade["average_price"].get<float>();
            if(jsonTrade.find("quantity") != jsonTrade.end())
                trade.Quantity = jsonTrade["quantity"].get<int>();
            if(jsonTrade.find("fill_timestamp") != jsonTrade.end() && jsonTrade["fill_timestamp"] != nullptr)
                strptime(jsonTrade["fill_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &trade.FillTimestamp);
            if(jsonTrade.find("exchange_timestamp") != jsonTrade.end() && jsonTrade["exchange_timestamp"] != nullptr)
                strptime(jsonTrade["exchange_timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &trade.ExchangeTimestamp);

            response.Data.push_back(std::move(trade));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }
    
    return response;
}

KiteResponse<std::vector<Quote>> Kite::GetQuotes(std::string exchange, std::string symbol)
{
    KiteResponse<std::vector<Quote>> response;

    std::string url  = "https://api.kite.trade/quote?i=" + (exchange + ":" + symbol);
    std::string authorization = "Authorization: token " + this->token.ApiKey + ":" + this->token.AccessToken;

    std::string output = webConnector.Call(HttpVerb::GET, std::move(url), std::move(authorization));

    auto jsonOutput = json::parse(output);
    response.Status = jsonOutput["status"].get<std::string>();

    if(response.Status == "success")
    {
        auto quoteList = jsonOutput["data"];
        for(auto& quoteItem : quoteList.items())
        {
            Quote quote;
            auto& value = quoteItem.value();

            quote.Symbol = quoteItem.key();
            if(value.find("instrument_token") != value.end())
                quote.InstrumentToken = value["instrument_token"].get<int>();
            if(value.find("timestamp") != value.end() && value["timestamp"] != nullptr)
                strptime(value["timestamp"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &quote.Timestamp);
            if(value.find("last_trade_time") != value.end() && value["last_trade_time"] != nullptr)
                strptime(value["last_trade_time"].get<std::string>().c_str(), "%Y-%m-%d %H:%M:%S", &quote.LastTradeTime);
            if(value.find("last_price") != value.end())
                quote.LastPrice = value["last_price"].get<float>();
            if(value.find("last_quantity") != value.end())
                quote.LastQuantity = value["last_quantity"].get<int>();
            if(value.find("buy_quantity") != value.end())
                quote.BuyQuantity = value["buy_quantity"].get<int>();
            if(value.find("sell_quantity") != value.end())
                quote.SellQuantity = value["sell_quantity"].get<int>();
            if(value.find("volume") != value.end())
                quote.Volume = value["volume"].get<long>();
            if(value.find("average_price") != value.end())
                quote.AveragePrice = value["average_price"].get<float>();
            if(value.find("net_change") != value.end())
                quote.NetChange = value["net_change"].get<float>();
            if(value.find("lower_circuit_limit") != value.end())
                quote.LowerCiruitLimit = value["lower_circuit_limit"].get<float>();
            if(value.find("upper_circuit_limit") != value.end())
                quote.UpperCircuitLimit = value["upper_circuit_limit"].get<float>();
            if(value.find("open") != value.end())
                quote.Open = value["open"].get<float>();
            if(value.find("high") != value.end())
                quote.High = value["high"].get<float>();
            if(value.find("low") != value.end())
                quote.Low = value["low"].get<float>();
            if(value.find("close") != value.end())
                quote.Close = value["close"].get<float>();

            if(value["depth"].find("buy") != value["depth"].end())
            {
                for(auto& buyItem : value["depth"]["buy"].items())
                {
                    QuoteDepthItem quoteDepthItem;

                    auto& buyValue = buyItem.value();
                    quoteDepthItem.Price = buyValue["price"].get<float>();
                    quoteDepthItem.Quantity = buyValue["quantity"].get<int>();
                    quoteDepthItem.Orders = buyValue["orders"].get<int>();

                    quote.Buy.push_back(std::move(quoteDepthItem));
                }
            }

            if(value["depth"].find("sell") != value["depth"].end())
            {
                for(auto& sellItem : value["depth"]["sell"].items())
                {
                    QuoteDepthItem quoteDepthItem;

                    auto& sellValue = sellItem.value();
                    quoteDepthItem.Price = sellValue["price"].get<float>();
                    quoteDepthItem.Quantity = sellValue["quantity"].get<int>();
                    quoteDepthItem.Orders = sellValue["orders"].get<int>();

                    quote.Sell.push_back(std::move(quoteDepthItem));
                }
            }

            response.Data.push_back(std::move(quote));
        }
    }
    else
    {
        response.Message = jsonOutput["message"].get<std::string>();
    }
    
    return response;
}

void Kite::SubscribeMarketData(int instrumentToken, std::function<void(MarketData&&)> callback)
{
    if(this->websock.IsConnected == false)
    {
        this->websock.Connect(this->apiKey, this->token.AccessToken, callback);
    }

    std::string message = R"({"a": "subscribe", "v": [)" + std::to_string(instrumentToken) + R"(]})";
    this->websock.Send(std::move(message));

    message = R"({"a": "mode", "v": ["full", [)" + std::to_string(instrumentToken) + R"(]]})";
    this->websock.Send(std::move(message));
}

void Kite::SubscribeMarketData(std::string instruments, std::function<void(MarketData&&)> callback)
{
    if(this->websock.IsConnected == false)
    {
        this->websock.Connect(this->apiKey, this->token.AccessToken, callback);
    }

    std::string message = R"({"a": "subscribe", "v": [)" + instruments + R"(]})";
    this->websock.Send(std::move(message));

    message = R"({"a": "mode", "v": ["full", [)" + instruments + R"(]]})";
    this->websock.Send(std::move(message));
}

void Kite::UnsubscribeMarketData(int instrumentToken)
{
    if(this->websock.IsConnected == false)
    {
        this->websock.Connect(this->apiKey, this->token.AccessToken, nullptr);
    }

    std::string message = R"({"a": "unsubscribe", "v": [)" + std::to_string(instrumentToken) + R"(]})";
    this->websock.Send(std::move(message));
}