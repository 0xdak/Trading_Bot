#include <iostream>
#include <string>
#include <vector>
#include "endpointhandler.h"

int main()
{
    std::string varlik = "EURUSDT";
    EndpointHandler ep_handler;
    TicketPrice ticketPrice = ep_handler.GetTickerPrice(varlik);
    TicketPrices ticketPrices = ep_handler.GetTickerPrices();

    std::cout << "Exchange: " << varlik << std::endl;
    std::cout << ticketPrice << std::endl;

    std::cout << "Tum varliklar"<< std::endl;
    for (int i = 0; i < ticketPrices.size(); ++i) {
        std::cout << ticketPrices[i] << std::endl;
    }

    return 0;
}
