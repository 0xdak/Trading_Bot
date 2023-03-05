#include <iostream>
#include <string>
#include "endpointhandler.h"


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
//    CURL *curl;
//    CURLcode res;
//    std::string readBuffer;

//    curl = curl_easy_init();

//    std::string url = "https://api.binance.com/api/v3/ticker/price";
//    if(curl) {
//        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // VERIFICATION problemini çözdü
//        res = curl_easy_perform(curl);
//        curl_easy_cleanup(curl);

//        std::cout << "OUT: " << readBuffer << std::endl;
//      }
//    else
//        std::cout << "Curl'e girmedi" << std::endl;

    EndpointHandler ep_handler;
    std::string ret = ep_handler.GetTickerPrice();
    std::cout << ret << std::endl;
    return 0;
}
