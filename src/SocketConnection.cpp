/*
 * SocketConnection.cpp
 *
 *  Created on: 14 May 2023
 *      Author: alida
 */

#include "SocketConnection.h"

#include <thread>

std::vector<Balance> balance_list(8);
std::map<int,Candle> closed_candles;

#define PORTNO 5000

SocketConnection::SocketConnection() {
	    int error = WSAStartup(0x0202, &w);  // Fill in WSA info

	    if (error)
	    {
	        return;                     //For some reason we couldn't start Winsock
	    }

	    if (w.wVersion != 0x0202)             //Wrong Winsock version?
	    {
	        WSACleanup ();
	        return;
	    }

	    SOCKADDR_IN addr;                     // The address structure for a TCP socket

	    addr.sin_family = AF_INET;            // Address family
	    addr.sin_port = htons (PORTNO);       // Assign port to this socket

	    addr.sin_addr.s_addr = htonl (INADDR_ANY);

	    s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket

	    if (s == INVALID_SOCKET)
	    {
	        return;                     //Don't continue if we couldn't create a //socket!!
	    }

	    if (bind(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	    {
	        return;
	    }

	//    listen(s, SOMAXCONN);
	    if (listen(s, SOMAXCONN) < 0){
	      std::cout << "Listen Failed\n";
	      exit(EXIT_FAILURE);
	    }

}


void SocketConnection::run() {
    // istekler kabul ediliyor.
	struct sockaddr_in client_addr;
	socklen_t boyut = sizeof(client_addr);
	int new_client = accept(s, (struct sockaddr*)&client_addr, &boyut);
	if(new_client < 0){
		fprintf(stderr, "İsteklerin kabulü sırasında hata oluştu.");
		return;
	}
//    std::thread myThreadds(talk_with_client, s, new_client);
	talk_with_client(s, new_client);
	run();
}

void SocketConnection::talk_with_client(SOCKET socket, int new_client) {
	while(1) {
		std::vector<char> buf(256);
		int veri_boyutu = recv(new_client, buf.data(), buf.size(), 0);
		if(veri_boyutu > 0) {
		    if (strcmp(buf.data(),"getTickets")==0){
		    	std::string jsonStr = closed_candles_to_json(closed_candles, balance_list);
		    	send(new_client, jsonStr.c_str(), jsonStr.size(), 0);
		    }
		} else {
		    close(new_client);
		    close(socket);
		    break;
		}
	}

}


std::string SocketConnection::closed_candles_to_json(const std::map<int, Candle>& myMap, const std::vector<Balance> balance_list) {
	// myMap veri yapısını dönüştürmek istiyoruz
	Json::Value candle_json_list(Json::arrayValue);
	Json::Value balance_json_list(Json::arrayValue);
	for (const auto& [key, value] : myMap)
	    {
	    	Json::Value candle_json;
	    	candle_json["Symbol"] = value.Symbol;
	        candle_json["OpenPrice"] = value.OpenPrice;
	        candle_json["ClosePrice"] = value.ClosePrice;
	        candle_json["HighPrice"] = value.HighPrice;
	        candle_json["LowPrice"] = value.LowPrice;
	        candle_json["Volume"] = value.Volume;
	        candle_json["CloseTime"] = value.CloseTime;
	        candle_json["IsCandleClosed"] = value.IsCandleClosed;
	        candle_json_list.append(candle_json);
	    }

	for (const auto& balance : balance_list) {
		Json::Value balance_json;
	    balance_json["Asset"] = balance.Asset;
	    balance_json["Free"] = balance.Free;
	    balance_json["Locked"] = balance.Locked;
	    balance_json_list.append(balance_json);
	}
    Json::Value mainJson(Json::objectValue);
    mainJson["balances"] = balance_json_list;
    mainJson["candles"] = candle_json_list;

	Json::StreamWriterBuilder builder;
	builder["indentation"] = "    "; // Opsiyonel: Girinti seviyesi
	std::string jsonStr = Json::writeString(builder, mainJson);
//	std::cout << jsonStr << "\n";
	return jsonStr;
}


SocketConnection::~SocketConnection() {
	// TODO Auto-generated destructor stub
}

