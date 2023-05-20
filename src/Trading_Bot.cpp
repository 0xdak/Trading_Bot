//============================================================================
// Name        : Trading_Bot.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


/* FOREGROUND */
#define RST   "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST


#include <iostream>
#include "ta_libc.h"

#include "binacpp.h"
#include "binacpp_websocket.h"
#include <json/json.h>

#include "iomanip"
#include <unistd.h>

#include "winsock2.h"
#include "windows.h"
#include "ws2tcpip.h"

#include <thread>

#include "string.h"
#include "Candle.h"

#include "indicators.h"
#include "SocketConnection.h"

#include <ctime>

#define TESTNET 1 // testnet api key'leri ve endpoint'leri farklı oluyor

#if TESTNET
	#define API_KEY "t5MLiI2oktjavIswBqEDj7lBVaOoWvMwqTAL3GB6FX4GnG6zGof0n9xvJVk7HfQ4"
	#define SEC_KEY "z2FiYxCNRueHbtvrUBHLMGTqTMfY2g2zdXBMLsnt8N5pnZvEgBqzauvJ0Sremngf"
#else
	#define API_KEY "t5MLiI2oktjavIswBqEDj7lBVaOoWvMwqTAL3GB6FX4GnG6zGof0n9xvJVk7HfQ4"
	#define SEC_KEY "z2FiYxCNRueHbtvrUBHLMGTqTMfY2g2zdXBMLsnt8N5pnZvEgBqzauvJ0Sremngf"
#endif

#define RSI_OVERBOUGHT 70
#define RSI_OVERSOLD 30

extern std::vector<Balance> balance_list;
extern map<int,Candle> closed_candles; // kapanan mumlar
extern bool is_someone_connected;
double closed_prices[200];

TA_RetCode rc;

int closed_candle_len = 0;

const char* ws_interval = "/ws/btcusdt@kline_1m";
bool close_websocket = false;

void printTime() {
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
    std::cout << std::setfill('0');
	std::cout << "\n " << (now->tm_year + 1900) << '-'
		 << std::setw(2) << (now->tm_mon + 1) << '-'
		 << std::setw(2) << now->tm_mday
		 << " ";
    std::cout << std::setfill('0');
    std::cout << std::setw(2) << now->tm_hour << ":";
    std::cout << std::setw(2) << now->tm_min << ":";
    std::cout << std::setw(2) << now->tm_sec;
}

// 1 sn'de bir candle gelir, bu candle kapanmışsa kapanan candle'lara ekler (RSI hesaplanması için)
int ws_klines_onData(Json::Value &json_result) {
	if (close_websocket) {
		std::cout << close_websocket <<"\n";
		return -1;
	}

	// TODO her saniye account bilgisini çekmektense en başta,işlem yaptıktan sonra ve belli periyotta çekmek yeterli olur mu
	Json::Value account_result;
	long recvWindow = 10000;
	BinaCPP::get_account(recvWindow, account_result);
//	std::cout << result << std::endl;


	double rsi = -1;
	double sma = -1;
	double ema = -1;
	std::vector<double> macd_all = {-1, -1, -1};
	Candle candle = Candle(json_result["k"]);
//	std::cout << candle << std::endl;
	if (candle.IsCandleClosed == 1) { // MUM KAPANDIYSA
//
		closed_prices[closed_candle_len] = candle.ClosePrice;
		closed_candles[closed_candle_len++] = candle;

	}

//	system("chcp 65001"); //ekranda türkçe karakterlerin doğru gözükmesi için
	system("CLS");


	printTime();

	if (is_someone_connected)
		std::cout << setfill(' ') << setw(65) << FGRN("Arayüze aktarılıyor\n");
	else
		std::cout << setfill(' ') << setw(65) << FRED("Arayüze aktarılmıyor\n");

	//{ PRINT TITLES | CÜZDAN
	std::cout << setfill('-') << setw(75) << " " << std::endl << setfill(' ');
	std::cout << setw(6)  << KYEL << "asset" << RST;
	std::cout << setw(11) << KCYN << "price" <<  RST;
	std::cout << setw(12) << KGRN << "free" << RST;
	std::cout << setw(8)  << KRED << "locked" << RST;
	std::cout << std::endl;
	//}

	//{ PRINT VALUES | CÜZDAN
	for ( int i  = 0 ; i < account_result["balances"].size() ; i++ ) {
		Balance balance = Balance(account_result["balances"][i]);
		balance_list[i] = balance;
		std::cout << fixed << setprecision(3) << KYEL << setw(6) << balance.Asset << RST;
		std::cout <<  KCYN << setw(11) << balance.Free + balance.Locked << RST;
		std::cout << KGRN << setw(11) << balance.Free << RST;
		std::cout << KRED << setw(9) << balance.Locked << RST;
		std::cout <<std::endl;
	}
	//}

	//{ PRINT DASHES
	std::cout << setfill('-') << setw(75) << " " << std::endl << setfill(' ');
	//}


	//{ PRINT TITLES | VARLIK VE STRATEJİLER
	std::cout << " " << KCYN << setw(7) << candle.Symbol << RST;
	std::cout << "  | " << KBLU << closed_candle_len << RST " adet mum verisi alındı." << std::endl << std::endl;
	std::cout << setw(10) << "price";
	std::cout << setw(10) << "RSI";
	std::cout << setw(10) << "SMA";
	std::cout << setw(10) << "EMA";
	std::cout << setw(10) << "MACD";
	std::cout << setw(10) << "MACD_S";
	std::cout << setw(10) << "MACD_H";


	std::cout <<std::endl;
	//}


	//{ PRINT VALUES | VARLIK VE STRATEJİLER

	for (int i = closed_candle_len-1; i > closed_candle_len-4; i--) {
		if (i == closed_candle_len-1)
			std::cout << KYEL;


		if (closed_candle_len > RSI_PERIOD){
			rsi = calc_rsi(0, i, closed_prices);
			sma = calc_sma(0, i, closed_prices);
			ema = calc_ema(0, i, closed_prices);
			macd_all = calc_macd(0, i, closed_prices);
		}

		std::cout << setw(10) << closed_prices[i];
		if (RSI_OVERSOLD < rsi && RSI_OVERBOUGHT > rsi)
			std::cout << KGRN << setw(10) << rsi << RST;
		else
			std::cout << setw(10) << rsi;

		if (i == closed_candle_len-1)
			std::cout << KYEL;
		std::cout << setw(10) << sma;
		std::cout << setw(10) << ema;
		std::cout << setw(10) << macd_all[0];
		std::cout << setw(10) << macd_all[1];
		std::cout << setw(10) << macd_all[2];

		std::cout <<std::endl << RST;

	}

	//}


//	std::cout << candle << std::endl;
}

void init_websocket(){
	BinaCPP_websocket::init();
	BinaCPP_websocket::connect_endpoint(ws_klines_onData, ws_interval);
	BinaCPP_websocket::enter_event_loop();
}
int main() {
	system("chcp 65001");
    std::thread myThread(&SocketConnection::run, SocketConnection());
//	if (ret == false){
//		std::cout << "Failed when trying to listening port" << std::endl;
//	}

	std::string api_key = API_KEY;
	std::string secret_key = SEC_KEY;

	BinaCPP::init(api_key,secret_key);



	TA_RetCode rc = TA_Initialize();
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_Initialize" << std::endl;
		return 0;
	}

	init_websocket();
//    websocket_w.join();

	return 0;
}
