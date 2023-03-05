#ifndef ENDPOINTHANDLER_H
#define ENDPOINTHANDLER_H

#include <curl/curl.h>
#include <iostream>


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

    std::string GetTickerPrice(); // JSON VERİSİ DÖNDERECEK deserialization

private:
   Endpoints endpoints;
   std::string CURL_GET_REQUEST(std::string url);
};

#endif // ENDPOINTHANDLER_H
