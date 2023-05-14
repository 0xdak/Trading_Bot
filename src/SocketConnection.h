/*
 * SocketConnection.h
 *
 *  Created on: 14 May 2023
 *      Author: alida
 */

#ifndef SOCKETCONNECTION_H_
#define SOCKETCONNECTION_H_

#include <iostream>


#include "iomanip"
#include <unistd.h>

#include "winsock2.h"
#include "windows.h"
#include "ws2tcpip.h"

#include <thread>

#include "string.h"
#include "Candle.h"

#include <vector>
#include "indicators.h"

#include "ta_libc.h"

class SocketConnection {
public:
	SocketConnection();
	void run();
	std::string closed_candles_to_json(const std::map<int, Candle>& myMap, const std::vector<Balance> balance_list);
	void talk_with_client(SOCKET socket, int new_client);
	virtual ~SocketConnection();
	SOCKET s;
	WSADATA w;
};

#endif /* SOCKETCONNECTION_H_ */
