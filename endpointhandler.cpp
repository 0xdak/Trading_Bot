#include "endpointhandler.h"
#include <curl/curl.h>

EndpointHandler::EndpointHandler()
{


    std::cout << "EndpointHandler()" << std::endl;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// CURL_GET_REQUEST()'ten dönen json response'unu Deserialize ederek döndürür.
std::string EndpointHandler::GetTickerPrice() {
    return CURL_GET_REQUEST(endpoints.TickerPrise);
}


// parametre olarak verilen url'ye GET request'i atar ve dönen json response'unu döndürür.
std::string EndpointHandler::CURL_GET_REQUEST(std::string url) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    //curl_easy_init() fonksiyonu, bir CURL öğesi oluşturur ve bu öğe, bir HTTP isteği yapmak veya bir FTP dosyası indirmek gibi işlemler gerçekleştirmek için kullanılır. İşlemler tamamlandıktan sonra, curl_easy_cleanup() fonksiyonu kullanılarak oluşturulan CURL öğesi bellekten silinmelidir.
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // VERIFICATION problemini çözdü
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        std::cout << "OUT: " << readBuffer << std::endl;
      }
    else
        std::cout << "Curl'e girmedi" << std::endl;

    return readBuffer;
}

