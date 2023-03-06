#include "endpointhandler.h"
#include "rapidjson/document.h"

EndpointHandler::EndpointHandler()
{
    std::cout << "EndpointHandler()" << std::endl;
}


// CURL_GET_REQUEST()'ten dönen json response'unu Deserialize ederek döndürür.
TicketPrice EndpointHandler::GetTickerPrice(std::string symbol) {
    TicketPrice ticketPrice;

    std::string response_json = CURL_GET_REQUEST(endpoints.TickerPrise + "?symbol=" + symbol);

    rapidjson::Document doc;
    doc.Parse(response_json.c_str());

    if (!doc.IsObject()) {
        std::cerr << "Belge bir JSON nesnesi değil!" << std::endl;
        return ticketPrice;
    }

    ticketPrice.symbol = doc["symbol"].GetString();
    ticketPrice.price = doc["price"].GetString();
    return ticketPrice;
}

TicketPrices EndpointHandler::GetTickerPrices() {
    TicketPrices ticketPrices;
    std::string response_json = CURL_GET_REQUEST(endpoints.TickerPrise);

    rapidjson::Document doc;
    doc.Parse(response_json.c_str());

    // Check for parse errors
  if (doc.HasParseError()) {
      std::cerr << "Error parsing JSON: "
                << doc.GetParseError() << std::endl;
      return ticketPrices;
  }

    rapidjson::Value::ConstValueIterator itr;

    ticketPrices.resize(1);
    int i = 0;
    for (itr = doc.Begin(); itr != doc.End(); ++itr) {
        std::cerr << i << std::endl;
        TicketPrice ticketPrice;
        ticketPrice.symbol = itr->GetObj()["symbol"].GetString();
        ticketPrice.price = itr->GetObj()["price"].GetString();
        ticketPrices[i] = ticketPrice;
        i++;
        ticketPrices.resize(i+1);
    }


//    ticketPrice.symbol = doc["symbol"].GetString();
//    ticketPrice.price = doc["price"].GetString();
    return ticketPrices;
}


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
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

//        std::cout << "OUT: " << readBuffer << std::endl;
      }
    else
        std::cout << "Curl'e girmedi" << std::endl;

    return readBuffer;
}

