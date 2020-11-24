#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include <vector>
#include <ctime>

template<typename T>
class KiteResponse
{
    public:
        std::string Status;
        std::string Message;
        T Data;
};

class Token
{
    public:
        std::string UserId;
        std::string UserName;
        std::string UserShortname;
        std::string Email;
        std::string UserType;
        std::string Broker;
        std::vector<std::string> Exchanges;
        std::vector<std::string> Products;
        std::vector<std::string> OrderTypes;
        std::string ApiKey;
        std::string AccessToken;
        std::string PublicToken;
        std::string RefreshToken;
        std::tm LoginTime;
        std::string AvatarUrl;

        Token()
        {
            this->LoginTime.tm_year = this->LoginTime.tm_mon = this->LoginTime.tm_mday = 0;
            this->LoginTime.tm_hour = this->LoginTime.tm_min = this->LoginTime.tm_sec = 0;
        }

        Token(const Token&) = delete;
        Token& operator=(const Token&) = delete;
        Token(Token&&) = default;
        Token& operator=(Token&&) = default;
        virtual ~Token() = default;
};

class  Margin
{
    public:
        bool Enabled;
        float Net;

        float AvailableAdhocMargin;
        float AvailableCash;
        float AvailableCollateral;
        float AvailableIntradayPayIn;

        float UtilizedDebits;
        float UtilizedExposure;
        float UtilizedM2MRealised;
        float UtilizedM2MUnrealised;
        float UtilizeOptionPremium;
        float UtilizedPayout;
        float UtilizedSpan;
        float UtilizedHoldingSales;
        float UtilizedTurnover;

        Margin() = default;
        Margin(const Margin&) = delete;
        Margin& operator=(const Margin&) = delete;
        Margin(Margin&&) = default;
        Margin& operator=(Margin&&) = default;
        virtual ~Margin() = default;
};

class Order
{
    public:
        std::string OrderId;
        std::string ParentOrderId;
        std::string ExchangeOrderId;
        std::string PlacedBy;
        std::string Variety;
        std::string Status; // TODO: It can be in string format!
        std::string Symbol;
        std::string Exchange;
        int InstrumentToken;
        std::string TransactionType;
        std::string OrderType;
        std::string Product; // TODO: use enum
        std::string Validity;
        float Price;
        int Quantity;
        float TriggerPrice;
        float AveragePrice;
        int PendingQuantity;
        int FilledQuantity;
        int CancelledQuantity;
        int DisclosedQuantity;
        int MarketProtection;
        std::tm OrderTimestamp;
        std::tm ExchangeTimestamp;
        std::tm ExchangeUpdateTimestamp;
        std::string StatusMessage;
        // TODO: tag
        // TODO: meta

        Order()
        {
            this->OrderTimestamp.tm_year = this->OrderTimestamp.tm_mon = this->OrderTimestamp.tm_mday = 0;
            this->OrderTimestamp.tm_hour = this->OrderTimestamp.tm_min = this->OrderTimestamp.tm_sec = 0;
            this->ExchangeTimestamp.tm_year = this->ExchangeTimestamp.tm_mon = this->ExchangeTimestamp.tm_mday = 0;
            this->ExchangeTimestamp.tm_hour = this->ExchangeTimestamp.tm_min = this->ExchangeTimestamp.tm_sec = 0;
            this->ExchangeUpdateTimestamp.tm_year = this->ExchangeUpdateTimestamp.tm_mon = this->ExchangeUpdateTimestamp.tm_mday = 0;
            this->ExchangeUpdateTimestamp.tm_hour = this->ExchangeUpdateTimestamp.tm_min = this->ExchangeUpdateTimestamp.tm_sec = 0;
        }

        Order(const Order&) = delete;
        Order& operator=(const Order&) = delete;
        Order(Order&&) = default;
        Order& operator=(Order&&) = default;
        virtual ~Order() = default;
};

class Trade 
{
    public:
        std::string TradeId;
        std::string OrderId;
        std::string ExchangeOrderId;
        std::string Symbol;
        std::string Exchange;
        int InstrumentToken;
        std::string TransactionType;
        std::string Product; // TODO: use enum
        float AveragePrice;
        int Quantity;
        std::tm FillTimestamp;
        std::tm ExchangeTimestamp;

        Trade()
        {
            this->FillTimestamp.tm_year = this->FillTimestamp.tm_mon = this->FillTimestamp.tm_mday = 0;
            this->FillTimestamp.tm_hour = this->FillTimestamp.tm_min = this->FillTimestamp.tm_sec = 0;
            this->ExchangeTimestamp.tm_year = this->ExchangeTimestamp.tm_mon = this->ExchangeTimestamp.tm_mday = 0;
            this->ExchangeTimestamp.tm_hour = this->ExchangeTimestamp.tm_min = this->ExchangeTimestamp.tm_sec = 0;
        }

        Trade(const Trade&) = delete;
        Trade& operator=(const Trade&) = delete;
        Trade(Trade&&) = default;
        Trade& operator=(Trade&&) = default;
        virtual ~Trade() = default;
};

struct Holding
{
    public:
        std::string Symbol;
        std::string Exchange;
        std::string ISIN;
        int Quantity;
        int T1Quantity;
        float AveragePrice;
        float LastPrice;
        float PNL;
        std::string Product; // TODO: use enum
        int CollateralQuantity;
        std::string CollateralType;

        Holding() = default;
        Holding(const Holding&) = delete;
        Holding& operator=(const Holding&) = delete;
        Holding(Holding&&) = default;
        Holding& operator=(Holding&&) = default;
        virtual ~Holding() = default;
};

struct Position
{
    public:
        std::string Symbol;
        std::string Exchange;
        int InstrumentToken;
        std::string Product; // TODO: use enum
        int Quantity;
        int OvernightQuantity;
        short Multiplier;
        float AveragePrice;
        float ClosePrice;
        float LastPrice;
        float Value;
        float PNL;
        float M2M;
        float Unrealized;
        float Realized;
        int BuyQuantity;
        float BuyPrice;
        float BuyValue;
        float BuyM2M;
        int DayBuyQuantity;
        float DayBuyPrice;
        float DayBuyValue;
        int DaySellQuantity;
        float DaySellPrice;
        float DaySellValue;
        int SellQuantity;
        float SellPrice;
        float SellValue;
        float SellM2M;

        Position() = default;
        Position(const Position&) = delete;
        Position& operator=(const Position&) = delete;
        Position(Position&&) = default;
        Position& operator=(Position&&) = default;
        virtual ~Position() = default;
};

class QuoteDepthItem
{
    public:
        float Price;
        int Quantity;
        int Orders;

        QuoteDepthItem() = default;
        QuoteDepthItem(const QuoteDepthItem&) = delete;
        QuoteDepthItem& operator=(const QuoteDepthItem&) = delete;
        QuoteDepthItem(QuoteDepthItem&&) = default;
        QuoteDepthItem& operator=(QuoteDepthItem&&) = default;
        virtual ~QuoteDepthItem() = default;
};

class Quote
{
    public:
        std::string Symbol;
        std::string Exchange;
        int InstrumentToken;
        std::tm Timestamp; 
        std::tm LastTradeTime;
        float LastPrice;
        int LastQuantity;
        int BuyQuantity;
        int SellQuantity;
        long Volume;
        float AveragePrice;
        // oi
        // oi_day_high
        // oi_day_low
        float NetChange;
        float LowerCiruitLimit;
        float UpperCircuitLimit;
        float Open;
        float High;
        float Low;
        float Close;

        std::vector<QuoteDepthItem> Buy;
        std::vector<QuoteDepthItem> Sell;

        Quote()
        {
            this->Timestamp.tm_year = this->Timestamp.tm_mon = this->Timestamp.tm_mday = 0;
            this->Timestamp.tm_hour = this->Timestamp.tm_min = this->Timestamp.tm_sec = 0;
            this->LastTradeTime.tm_year = this->LastTradeTime.tm_mon = this->LastTradeTime.tm_mday = 0;
            this->LastTradeTime.tm_hour = this->LastTradeTime.tm_min = this->LastTradeTime.tm_sec = 0;
        }

        Quote(const Quote&) = delete;
        Quote& operator=(const Quote&) = delete;
        Quote(Quote&&) = default;
        Quote& operator=(Quote&&) = default;
        virtual ~Quote() = default;
};

class MarketDataDepthItem
{
    public:
        int Quantity;
        float Price;
        short Orders;

        MarketDataDepthItem() = default;
        MarketDataDepthItem(const MarketDataDepthItem&) = delete;
        MarketDataDepthItem& operator=(const MarketDataDepthItem&) = delete;
        MarketDataDepthItem(MarketDataDepthItem&&) = default;
        MarketDataDepthItem& operator=(MarketDataDepthItem&&) = default;
        virtual ~MarketDataDepthItem() = default;
};

class MarketData
{
    public:
        int InstrumentToken;
        int LastTradedPrice;
        int LastTradedQuantity;
        int AverageTradedPrice;
        int Volume;
        int TotalBuyQuantity;
        int TotalSellQuantity;
        int OpenPrice;
        int HighPrice;
        int LowPrice;
        int ClosePrice;
        int LastTradedTimestamp;
        int OpenInterest;
        int OpenInterestHigh;
        int OpenInterestLow;
        int ExchangeTimestamp;
        std::vector<MarketDataDepthItem> Buy;
        std::vector<MarketDataDepthItem> Sell;

        MarketData() = default;
        MarketData(const MarketData&) = delete;
        MarketData& operator=(const MarketData&) = delete;
        MarketData(MarketData&&) = default;
        MarketData& operator=(MarketData&&) = default;
        virtual ~MarketData() = default;
};

#endif // STRUCTURES_H