//============================================================================
// Name        : Trading_Bot.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include "ta_libc.h"

#include "binacpp.h"
#include "binacpp_websocket.h"
#include <json/json.h>

#include "iomanip"
#include <unistd.h>

#include "Candle.h"

#define TESTNET 1 // testnet api key'leri ve endpoint'leri farklı oluyor

#if TESTNET
	#define API_KEY "t5MLiI2oktjavIswBqEDj7lBVaOoWvMwqTAL3GB6FX4GnG6zGof0n9xvJVk7HfQ4"
	#define SEC_KEY "z2FiYxCNRueHbtvrUBHLMGTqTMfY2g2zdXBMLsnt8N5pnZvEgBqzauvJ0Sremngf"
#else
	#define API_KEY "t5MLiI2oktjavIswBqEDj7lBVaOoWvMwqTAL3GB6FX4GnG6zGof0n9xvJVk7HfQ4"
	#define SEC_KEY "z2FiYxCNRueHbtvrUBHLMGTqTMfY2g2zdXBMLsnt8N5pnZvEgBqzauvJ0Sremngf"
#endif

map<long,map<string,double>> klinesCache; // key,value

map<int,Candle> closed_candles; // kapanan mumlar
double closed_prices[100];
//double def_closed_prices[30] = {18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.759, 18.759, 18.756, 18.746, 18.747, 18.744, 18.743, 18.742, 18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.759, 18.759, 18.756, 18.746, 18.747, 18.744, 18.743, 18.742};
//double def_closed_prices[30] = {1.18676, 1.18618, 1.18712, 1.18791, 18.756, 18.756, 18.756, 18.759, 18.759, 18.756, 18.746, 18.747, 18.744, 18.743, 18.742, 18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.756, 18.759, 18.759, 18.756, 18.746, 18.747, 18.744, 18.743, 18.742};
double def_closed_prices[23] = {1.18676,
								1.18618,
								1.18712,
								1.18791,

								1.18578,
								1.18484,
								1.18566,
								1.18535,

								1.1858,
								1.18452,
								1.1846,
								1.18476,

								1.18508,
								1.1844,
								1.18382,
								1.18548,

								1.18648,
								1.18574,
								1.1854,
								1.18575,

								1.18806,
								1.18961,
								1.19055,};
int RSI_PERIOD = 14;

TA_RetCode rc;


double new_closed_prices[23];

int counter = 0;
void price_event(double closed_price) {
	new_closed_prices[counter++] = closed_price;
	if (counter > RSI_PERIOD) {
		int outBegIdx;
		int outNBElement;
		double outReal[100] = {0,};

		TA_RetCode rc = TA_RSI(0, counter-1, &new_closed_prices[0], RSI_PERIOD, &outBegIdx, &outNBElement, outReal);
		if (rc != TA_SUCCESS){
			std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		}
	}
}

void send_price_event() {
	for (int var = 0; var < sizeof(def_closed_prices)/sizeof(double); ++var) {
		price_event(def_closed_prices[var]);

	}
}

void print_closed_candles(){
	std::cout << "==================================" << endl;

	map<int,Candle>::iterator it_i;

	for (it_i = closed_candles.begin() ; it_i != closed_candles.end() ; it_i++ ) {
		int index = (*it_i).first;
		Candle candleObj = (*it_i).second;
		std::cout << candleObj << std::endl;
	}
}


double calc_rsi(int startIdx, int endIDx, double* array) {
	int outBegIdx;
	int outNBElement;
	double outReal[endIDx-startIdx-RSI_PERIOD+1] = {0,};
	double *inReal = &closed_prices[0];

	TA_RetCode rc = TA_RSI(startIdx, endIDx, array, RSI_PERIOD, &outBegIdx, &outNBElement, outReal);
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];

}


int closed_candle_len = 0;

// 2 sn'de bir?? candle gelir, bu candle kapanmışsa kapanan candle'lara ekler (RSI hesaplanması için)
int ws_klines_onData( Json::Value &json_result ) {

	system("CLS");

	Candle candle = Candle(json_result["k"]);
//	std::cout << candle << std::endl;
	if (candle.IsCandleClosed == 1) { // MUM KAPANDIYSA
//
		closed_prices[closed_candle_len] = candle.ClosePrice;
		closed_candles[closed_candle_len++] = candle;

		for (int i = 0; i < closed_candle_len; ++i) {
			std::cout << closed_prices[i] << ", ";
		}
		std::cout << std::endl;
		if (closed_candle_len > RSI_PERIOD){
			double rsi = calc_rsi(0, closed_candle_len-1, closed_prices);
			std::cout << "RSI: " << rsi << std::endl;
		}
	}

	// TODO her saniye account bilgisini çekmektense en başta,işlem yaptıktan sonra ve belli periyotta çekmek yeterli olur mu
	Json::Value result;
	long recvWindow = 10000;
	BinaCPP::get_account(recvWindow, result);
//	std::cout << result << std::endl;


	std::cout << setfill('-') << setw(55) << " " << std::endl << setfill(' ');
	std::cout << setw(6) << "asset";
	std::cout << setw(11) << "price";
	std::cout << setw(11) << "free";
	std::cout << setw(7) << "locked";
	std::cout << std::endl;

	for ( int i  = 0 ; i < result["balances"].size() ; i++ ) {
		Balance balance = Balance(result["balances"][i]);

		std::cout << setw(6) << fixed << setprecision(3) << balance.Asset;
		std::cout << setw(11) << balance.Free + balance.Locked;
		std::cout << setw(11) << balance.Free;
		std::cout << setw(7) << balance.Locked;
		std::cout <<std::endl;
	}
	std::cout << setfill('-') << setw(55) << " " << std::endl << setfill(' ');
//	std::cout << candle << std::endl;
}
int ws_userStream_OnData( Json::Value &json_result ) {
	std::cout << json_result << std::endl;
}

int main() {

	std::string api_key = API_KEY;
	std::string secret_key = SEC_KEY;

	BinaCPP::init(api_key,secret_key);



	TA_RetCode rc = TA_Initialize();
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_Initialize" << std::endl;
		return 0;
	}


	BinaCPP_websocket::init();
	BinaCPP_websocket::connect_endpoint( ws_klines_onData ,"/ws/btceur@kline_1s");
	BinaCPP_websocket::enter_event_loop();


	return 0;
}
