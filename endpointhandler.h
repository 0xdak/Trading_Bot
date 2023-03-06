#ifndef ENDPOINTHANDLER_H
#define ENDPOINTHANDLER_H

#include <curl/curl.h>
#include <iostream>
#include <vector>


class TicketPrice {
public:
    std::string symbol;
    std::string price;

    friend std::ostream& operator<<(std::ostream& os, const TicketPrice& ticketPrice) {
        os << "TicketPrice { " << std::endl;
        os << "\tSymbol\t: " << ticketPrice.symbol << std::endl;
        os << "\tPrice\t: " << ticketPrice.price << std::endl;
        os << "}" << std::endl;
        return os;
    }
};




typedef std::vector<TicketPrice> TicketPrices;





class EndpointHandler
{
public:

    class Endpoints
    {
        public:
            const std::string BaseUrl     = "https://api.binance.com";

            const std::string TickerPrise = BaseUrl + "/api/v3/ticker/price";
    };

    EndpointHandler();

    TicketPrice GetTickerPrice(std::string symbol);
    TicketPrices GetTickerPrices();

private:
   Endpoints endpoints;
   std::string CURL_GET_REQUEST(std::string url);
};


#endif // ENDPOINTHANDLER_H
