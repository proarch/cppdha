#include <iostream>
#include "kite.hpp"
#include "models.hpp"

void MarketDataCallback(MarketData&& data);
void LogToken(Token token);

template<typename T> void Log(T&& t) {  }
template<> void Log(Margin&& margin);
template<> void Log(Order&& order);
template<> void Log(Trade&& trade);
template<> void Log(Holding&& holding);
template<> void Log(Position&& position);
template<> void Log(Quote&& quote);
template<> void Log(MarketData&& marketData);

template<typename T>
void LogResponse(KiteResponse<std::vector<T>>&& response)
{
    if(response.Status == "success")
    {
        for(auto it = response.Data.begin(); it != response.Data.end(); it++)
        {
            Log<T>(std::move(*it));
        }
    }
    else
    {
        std::cout << response.Message << std::endl;
    }    
}

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cout << "Invalid number of arguments. Sample commands..." << std::endl 
                  << "./cppdha xxxx_apikey_xxxxx xxxxxx_apisecret_xxxxx" << std::endl;
        return 0;        
    }

    std::string apiKey {argv[1]};
    std::string apiSecret {argv[2]};
    std::string redirectedUrl {};

    std::string loginUrl =  "https://kite.trade/connect/login?api_key=" + apiKey + "&v=3";
    system(("wsl-open " + loginUrl).c_str());

    std::cout << "Enter redirected URL after authentication: ";
    std::cin >> redirectedUrl;

    Kite kite(std::move(apiKey), std::move(apiSecret));

    kite.Authenticate(std::move(redirectedUrl));

    // get margins
    KiteResponse<std::vector<Margin>> margins = kite.GetMargins();
    LogResponse<Margin>(std::move(margins));

    // Get holdings
    KiteResponse<std::vector<Holding>> holdings = kite.GetHoldings();
    LogResponse<Holding>(std::move(holdings));

    // Get positions
    KiteResponse<std::vector<Position>> positions = kite.GetPositions();
    LogResponse<Position>(std::move(positions));

    std::string input;
    std::cout << "Do you want to place a new order (y/n) ? : ";
    std::cin >> input;
    if(input == "y")
    {
        std::string symbol, exchange;
        int quantity;
        float price;

        std::cout << "Enter Symbol : ";
        std::cin >> symbol;
        std::cout << "Enter Exchange : ";
        std::cin >> exchange;
        std::cout << "Enter quantity : ";
        std::cin >> quantity;
        std::cout << "Enter price : ";
        std::cin >> price;

        KiteResponse<std::string> response = kite.NewOrder(symbol, exchange, "BUY", "LIMIT", quantity, price, 0, "MIS", "DAY");
        std::cout << "New order: " << response.Data << std::endl;
    }

    std::cout << "Do you want to modify an existing order (y/n) ? : ";
    std::cin >> input;
    if(input == "y")
    {
        std::string orderId;
        int quantity;
        double price;

        std::cout << "Enter Order ID : ";
        std::cin >> orderId;
        std::cout << "Enter quantity : ";
        std::cin >> quantity;
        std::cout << "Enter price : ";
        std::cin >> price;

        KiteResponse<std::string> response = kite.ModifyOrder(orderId, "LIMIT", quantity, price, 0, "DAY");
        std::cout << "Modify order: " << response.Data << std::endl;
    }

    std::cout << "Do you want to cancel an order (y/n) ? : ";
    std::cin >> input;
    if(input == "y")
    {
        std::string orderId;

        std::cout << "Enter Order ID : ";
        std::cin >> orderId;

        KiteResponse<std::string> response = kite.CancelOrder(orderId);
        std::cout << "Cancel order: " << response.Data << std::endl;
    }

    // Get all orders
    KiteResponse<std::vector<Order>> orders = kite.GetOrders();
    LogResponse<Order>(std::move(orders));

    // Get all trades
    KiteResponse<std::vector<Trade>> trades = kite.GetTrades();
    LogResponse<Trade>(std::move(trades));

    std::cout << "Do you want to get order history and trades for the given order (y/n) ? : ";
    std::cin >> input;
    if(input == "y")
    {
        std::string orderId;
        std::cout << "Enter Order ID: ";
        std::cin >> orderId;

        KiteResponse<std::vector<Order>> orders = kite.GetOrderHistory(orderId);
        LogResponse<Order>(std::move(orders));

        KiteResponse<std::vector<Trade>> trades = kite.GetTradesForOrder(orderId);
        LogResponse<Trade>(std::move(trades));
    }

    // Get quotes
    KiteResponse<std::vector<Quote>> quotes = kite.GetQuotes("NSE", "INFY");
    LogResponse<Quote>(std::move(quotes));

    std::function<void(MarketData&&)> callback = MarketDataCallback;
    kite.SubscribeMarketData(408065, callback);

    std::cout << "Press any key to exit this sample application" << std::endl;
    std::cin >> input;
}

void MarketDataCallback(MarketData&& data)
{
    std::cout << "Market Data Callback invoked" << std::endl;

    Log<MarketData>(std::move(data));
}

void LogToken(Token token)
{
    std::cout   << "Token Log: "
                << " UserName: " << token.UserName
                << " UserShortname: " << token.UserShortname
                << " Email: " << token.Email
                << " UserType: " << token.UserType
                //<< "Broker: " << Broker
                //<< "Exchanges: " << Exchanges
                //<< "OrderTypes: " << OrderTypes
                << " Broker: " << token.Broker
                << " ApiKey: " << token.ApiKey
                << " AccessToken: " << token.AccessToken
                << " PublicToken: " << token.PublicToken
                << " RefreshToken: " << token.RefreshToken
                << " LoginTime: " << (1900 + token.LoginTime.tm_year) << "-" << (token.LoginTime.tm_mon + 1) << "-" << token.LoginTime.tm_mday 
                                 << " " << token.LoginTime.tm_hour << ":" << token.LoginTime.tm_min << ":" << token.LoginTime.tm_sec
                << " AvatarUrl: " << token.AvatarUrl
                << std::endl;
};

template<>
void Log(Margin&& margin)
{
    std::cout   << "Margin Log: "
                << " Enabled: " << margin.Enabled
                << " Net: " << margin.Net
                << " AvailableAdhocMargin: " << margin.AvailableAdhocMargin
                << " AvailableCash: " << margin.AvailableCash
                << " AvailableCollateral: " << margin.AvailableCollateral
                << " AvailableIntradayPayIn: " << margin.AvailableIntradayPayIn
                << " UtilizedDebits: " << margin.UtilizedDebits
                << " UtilizedExposure: " << margin.UtilizedExposure
                << " UtilizedM2MRealised: " << margin.UtilizedM2MRealised
                << " UtilizedM2MUnrealised: " << margin.UtilizedM2MUnrealised
                << " UtilizeOptionPremium: " << margin.UtilizeOptionPremium
                << " UtilizedPayout: " << margin.UtilizedPayout
                << " UtilizedSpan: " << margin.UtilizedSpan
                << " UtilizedHoldingSales: " << margin.UtilizedHoldingSales
                << " UtilizedTurnover: " << margin.UtilizedTurnover
                << std::endl;
};

template<>
void Log(Order&& order)
{
    std::cout   << "Order Log: "
                << " OrderId: " << order.OrderId
                << " ParentOrderId: " << order.ParentOrderId
                << " ExchangeOrderId: " << order.ExchangeOrderId
                << " PlacedBy: " << order.PlacedBy
                << " Variety: " << order.Variety
                << " Status: " << order.Status
                << " Symbol: " << order.Symbol
                << " Exchange: " << order.Exchange
                << " InstrumentToken: " << order.InstrumentToken
                << " TransactionType: " << order.TransactionType
                << " OrderType: " << order.OrderType
                << " Product: " << order.Product
                << " Validity: " << order.Validity
                << " Price: " << order.Price
                << " Quantity: " << order.Quantity
                << " TriggerPrice: " << order.TriggerPrice
                << " AveragePrice: " << order.AveragePrice
                << " PendingQuantity: " << order.PendingQuantity
                << " FilledQuantity: " << order.FilledQuantity
                << " CancelledQuantity: " << order.CancelledQuantity
                << " DisclosedQuantity: " << order.DisclosedQuantity
                << " MarketProtection: " << order.MarketProtection
                << " OrderTimestamp: " << (1900 + order.OrderTimestamp.tm_year) << "-" << (order.OrderTimestamp.tm_mon + 1) << "-" << order.OrderTimestamp.tm_mday 
                                      << " " << order.OrderTimestamp.tm_hour << ":" << order.OrderTimestamp.tm_min << ":" << order.OrderTimestamp.tm_sec
                << " ExchangeTimestamp: " << (1900 + order.ExchangeTimestamp.tm_year) << "-" << (order.ExchangeTimestamp.tm_mon + 1) << "-" << order.ExchangeTimestamp.tm_mday 
                                         << " " << order.ExchangeTimestamp.tm_hour << ":" << order.ExchangeTimestamp.tm_min << ":" << order.ExchangeTimestamp.tm_sec
                << " ExchangeUpdateTimestamp: " << (1900 + order.ExchangeUpdateTimestamp.tm_year) << "-" << (order.ExchangeUpdateTimestamp.tm_mon + 1) << "-" << order.ExchangeUpdateTimestamp.tm_mday 
                                         << " " << order.ExchangeUpdateTimestamp.tm_hour << ":" << order.ExchangeUpdateTimestamp.tm_min << ":" << order.ExchangeUpdateTimestamp.tm_sec
                << " StatusMessage: " << order.StatusMessage
                << std::endl;
};

template<>
void Log(Trade&& trade)
{
    std::cout   << "Trade Log: "
                << " TradeId: " << trade.TradeId
                << " OrderId: " << trade.OrderId
                << " ExchangeOrderId: " << trade.ExchangeOrderId
                << " Symbol: " << trade.Symbol
                << " Exchange: " << trade.Exchange
                << " InstrumentToken: " << trade.InstrumentToken
                << " TransactionType: " << trade.TransactionType
                << " Product: " << trade.Product
                << " AveragePrice: " << trade.AveragePrice
                << " Quantity: " << trade.Quantity
                << " FillTimestamp: " << (1900 + trade.FillTimestamp.tm_year) << "-" << (trade.FillTimestamp.tm_mon + 1) << "-" << trade.FillTimestamp.tm_mday 
                                     << " " << trade.FillTimestamp.tm_hour << ":" << trade.FillTimestamp.tm_min << ":" << trade.FillTimestamp.tm_sec
                << " ExchangeTimestamp: " << (1900 + trade.ExchangeTimestamp.tm_year) << "-" << (trade.ExchangeTimestamp.tm_mon + 1) << "-" << trade.ExchangeTimestamp.tm_mday 
                                         << " " << trade.ExchangeTimestamp.tm_hour << ":" << trade.ExchangeTimestamp.tm_min << ":" << trade.ExchangeTimestamp.tm_sec
                << std::endl;
};

template<>
void Log(Holding&& holding)
{
    std::cout   << "Holding Log: "
                << " Symbol: " << holding.Symbol
                << " Exchange: " << holding.Exchange
                << " ISIN: " << holding.ISIN
                << " Quantity: " << holding.Quantity
                << " T1Quantity: " << holding.T1Quantity
                << " AveragePrice: " << holding.AveragePrice
                << " LastPrice: " << holding.LastPrice
                << " PNL: " << holding.PNL
                << " Product: " << holding.Product
                << " CollateralQuantity: " << holding.CollateralQuantity
                << " CollateralType: " << holding.CollateralType
                << std::endl;
};

template<>
void Log(Position&& position)
{
    std::cout   << "Position Log: "
                << " Symbol: " << position.Symbol
                << " Exchange: " << position.Exchange
                << " InstrumentToken: " << position.InstrumentToken
                << " Product: " << position.Product
                << " Quantity: " << position.Quantity
                << " OvernightQuantity: " << position.OvernightQuantity
                << " Multiplier: " << position.Multiplier
                << " AveragePrice: " << position.AveragePrice
                << " ClosePrice: " << position.ClosePrice
                << " LastPrice: " << position.LastPrice
                << " Value: " << position.Value
                << " PNL: " << position.PNL
                << " M2M: " << position.M2M
                << " Unrealized: " << position.Unrealized
                << " Realized: " << position.Realized
                << " BuyQuantity: " << position.BuyQuantity
                << " BuyPrice: " << position.BuyPrice
                << " BuyValue: " << position.BuyValue
                << " BuyM2M: " << position.BuyM2M
                << " DayBuyQuantity: " << position.DayBuyQuantity
                << " DayBuyPrice: " << position.DayBuyPrice
                << " DayBuyValue: " << position.DayBuyValue
                << " DaySellQuantity: " << position.DaySellQuantity
                << " DaySellPrice: " << position.DaySellPrice
                << " DaySellValue: " << position.DaySellValue
                << " SellQuantity: " << position.SellQuantity
                << " SellPrice: " << position.SellPrice
                << " SellValue: " << position.SellValue
                << " SellM2M: " << position.SellM2M
                << std::endl;
};

template<>
void Log(Quote&& quote)
{
    std::cout   << "Quote Log: "
                << "InstrumentToken: " << quote.InstrumentToken
                << " Timestamp: " << (1900 + quote.Timestamp.tm_year) << "-" << (quote.Timestamp.tm_mon + 1) << "-" << quote.Timestamp.tm_mday 
                                 << " " << quote.Timestamp.tm_hour << ":" << quote.Timestamp.tm_min << ":" << quote.Timestamp.tm_sec
                << " LastTradeTime: " << (1900 + quote.LastTradeTime.tm_year) << "-" << (quote.LastTradeTime.tm_mon + 1) << "-" << quote.LastTradeTime.tm_mday 
                                     << " " << quote.LastTradeTime.tm_hour << ":" << quote.LastTradeTime.tm_min << ":" << quote.LastTradeTime.tm_sec
                << " LastPrice: " << quote.LastPrice
                << " LastQuantity: " << quote.LastQuantity
                << " BuyQuantity: " << quote.BuyQuantity
                << " SellQuantity: " << quote.SellQuantity
                << " Volume: " << quote.Volume
                << " AveragePrice: " << quote.AveragePrice
                << " NetChange: " << quote.NetChange
                << " LowerCiruitLimit: " << quote.LowerCiruitLimit
                << " UpperCircuitLimit: " << quote.UpperCircuitLimit
                << " Open: " << quote.Open
                << " High: " << quote.High
                << " Low: " << quote.Low
                << " Close: " << quote.Close
                << " Depth Buy Size: " << quote.Buy.size()
                << " Depth Sell Size: " << quote.Sell.size()
                << std::endl;
};

template<>
void Log(MarketData&& marketData)
{
    std::cout   << "Market Data Log: "
                << " InstrumentToken: " << marketData.InstrumentToken
                << " LastTradedPrice: " << marketData.LastTradedPrice
                << " LastTradedQuantity: " << marketData.LastTradedQuantity
                << " AverageTradedPrice: " << marketData.AverageTradedPrice
                << " Volume: " << marketData.Volume
                << " TotalBuyQuantity: " << marketData.TotalBuyQuantity
                << " TotalSellQuantity: " << marketData.TotalSellQuantity
                << " OpenPrice: " << marketData.OpenPrice
                << " HighPrice: " << marketData.HighPrice
                << " LowPrice: " << marketData.LowPrice
                << " ClosePrice: " << marketData.ClosePrice
                << " LastTradedTimestamp: " << marketData.LastTradedTimestamp
                << " OpenInterest: " << marketData.OpenInterest
                << " OpenInterestHigh: " << marketData.OpenInterestHigh
                << " OpenInterestLow: " << marketData.OpenInterestLow
                << " ExchangeTimestamp: " << marketData.ExchangeTimestamp
                << std::endl;
                        
    for(auto& item : marketData.Buy)
    {
        std::cout << "Price: " << item.Price << " Quantity: " << item.Quantity << " Orders: " << item.Orders << std::endl;
    }
    for(auto& item : marketData.Sell)
    {
        std::cout << "Price: " << item.Price << " Quantity: " << item.Quantity << " Orders: " << item.Orders << std::endl;
    }
};