#include "websock.hpp"

std::shared_mutex WebSock::MarketDataMutex;

// Sends a WebSocket message and prints the response
void WebSock::Connect(std::string apiKey, std::string accessKey, std::function<void(MarketData&&)> callback)
{
    // TODO: what if connect fails ???

    this->apiKey = apiKey;
    this->accessKey = accessKey;

    try
    {
        boost::asio::io_context ioc;

        // The SSL context is required, and holds certificates
        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.set_verify_mode(boost::asio::ssl::verify_peer);

        ctx.set_options(boost::asio::ssl::context::default_workarounds
                        | boost::asio::ssl::context::no_sslv2
                        | boost::asio::ssl::context::no_sslv3); 

        ctx.set_default_verify_paths();

        load_root_certificates(ctx);

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<beast::ssl_stream<tcp::socket>> websock{ioc, ctx};
        this->ws = std::unique_ptr<websocket::stream<beast::ssl_stream<tcp::socket>>>(new websocket::stream<beast::ssl_stream<tcp::socket>>(ioc, ctx));

        // Look up the domain name
        auto const results = resolver.resolve(this->host, this->port);

        // Make the connection on the IP address we get from a lookup
        net::connect(this->ws->next_layer().next_layer(), results.begin(), results.end());
        
        // Perform the websocket handshake
        this->ws->next_layer().handshake(ssl::stream_base::client);

        this->ws->set_option(websocket::stream_base::decorator([](websocket::request_type& req)
        {
            req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
            req.set("X-Kite-Version", "3");
        }));

        this->ws->handshake(host, ("/?api_key=" + apiKey + "&" + "access_token=" + accessKey));
        
        this->IsConnected = true;
        if(callback != nullptr)
        {
            this->callback = std::move(callback);
        }

        std::thread readThread(&WebSock::ReadThread, this);
        readThread.detach();
    }
    catch(std::exception const& e)
    {
        // TODO:
    }
    catch(...)
    {
        // TODO:
    }
}

void WebSock::Send(std::string&& msg)
{
    this->ws->write(boost::asio::buffer(std::string(msg)));
}

void WebSock::Close()
{
    this->ws->close(websocket::close_code::normal);
}

void WebSock::ReadThread()
{
    boost::beast::multi_buffer buffer;

    for(;;)
    {
        buffer.consume(std::numeric_limits<int>::max());

        this->ws->read(buffer);
        std::string data = boost::beast::buffers_to_string(buffer.data()); // TODO: use pointer returned by data()            
            
        if (data.size() == 0) continue;

        if(data.rfind("{", 0) == 0) 
        {
            // TODO: any possiblity trade data ?? as of now order details are sent which is considered not that useful.
            std::cout << data << std::endl;
            continue; 
        }
            
        {
            std::unique_lock lock(WebSock::MarketDataMutex); // exclusive lock

            short byte = 0;
            short packets = short((unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));

            for(short i = 0; i < packets; i++)
            {
                MarketData marketData;

                short size = short((unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));

                if(size >= 4) marketData.InstrumentToken = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 8) marketData.LastTradedPrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 12) marketData.LastTradedQuantity = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 16) marketData.AverageTradedPrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 20) marketData.Volume = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 24) marketData.TotalBuyQuantity = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 28) marketData.TotalSellQuantity = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 32) marketData.OpenPrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 36) marketData.HighPrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 40) marketData.LowPrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 44) marketData.ClosePrice = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 48) marketData.LastTradedTimestamp = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 52) marketData.OpenInterest = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                if(size >= 56) marketData.OpenInterestHigh = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));                
                if(size >= 60) marketData.OpenInterestLow = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));                
                if(size >= 64) marketData.ExchangeTimestamp = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));                
            
                marketData.LastTradedPrice /= 100;
                marketData.AverageTradedPrice /= 100;
                marketData.OpenPrice /= 100;
                marketData.HighPrice /= 100;
                marketData.LowPrice /= 100;
                marketData.ClosePrice /= 100;

                // Market depth items
                if(size == 184)
                {
                    for(int i = 0; i < 5; i++)
                    {
                        MarketDataDepthItem item;

                        item.Quantity = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        item.Price = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        item.Orders = short((unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        byte += 2; // two bytes padding as per specification

                        item.Price /= 100;

                        marketData.Buy.push_back(std::move(item));
                    }

                    for(int i = 0; i < 5; i++)
                    {
                        MarketDataDepthItem item;

                        item.Quantity = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        item.Price = int((unsigned char)(data[byte++]) << 24 | (unsigned char)(data[byte++]) << 16 | (unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        item.Orders = short((unsigned char)(data[byte++]) << 8 | (unsigned char)(data[byte++]));
                        byte += 2; // two bytes padding as per specification

                        item.Price /= 100;

                        marketData.Sell.push_back(std::move(item));
                    }
                }

                this->callback(std::move(marketData));
            }
        }        
    }
}