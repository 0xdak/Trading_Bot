//============================================================================
// Name        : Trading_Bot.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


/* FOREGROUND */
#define RST  "\x1B[0m"
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
int RSI_PERIOD = 14;

TA_RetCode rc;

int closed_candle_len = 0;

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

	TA_RetCode rc = TA_RSI(startIdx, endIDx, array, RSI_PERIOD, &outBegIdx, &outNBElement, outReal);
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];
}

double calc_ema(int startIdx, int endIDx, double* array) {
	int outBegIdx;
	int outNBElement;
	double outReal[endIDx-startIdx-RSI_PERIOD+1] = {0,};
	double *inReal = &closed_prices[0];

	TA_RetCode rc = TA_EMA(startIdx, endIDx, array, RSI_PERIOD, &outBegIdx, &outNBElement, outReal);
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];
}

std::vector<double> calc_macd(int startIdx, int endIDx, double* array) {
	int outBegIdx;
	int outNBElement;
	std::vector<double> outMACD(closed_candle_len);
	std::vector<double> outMACDSignal(closed_candle_len);
	std::vector<double> outMACDHist(closed_candle_len);
	TA_RetCode rc = TA_MACD(startIdx, endIDx, array, 12, 26, 9, &outBegIdx, &outNBElement, outMACD.data(), outMACDSignal.data(), outMACDHist.data());
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return {-1, -1, -1};
	}

	std::vector<double> outReal(3);
	outReal[0] = outMACD[outNBElement-1];
	outReal[1] = outMACDSignal[outNBElement-1];
	outReal[2] = outMACDHist[outNBElement-1];
	return outReal;
}

double calc_sma(int startIdx, int endIDx, double* array) {
	int outBegIdx;
	int outNBElement;
	std::vector<double> outReal(closed_candle_len);

	TA_RetCode rc = TA_SMA(startIdx, endIDx, array, 5, &outBegIdx, &outNBElement, outReal.data());
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];
}


// 2 sn'de bir?? candle gelir, bu candle kapanmışsa kapanan candle'lara ekler (RSI hesaplanması için)
int ws_klines_onData( Json::Value &json_result ) {

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

		if (closed_candle_len > RSI_PERIOD){
			rsi = calc_rsi(0, closed_candle_len-1, closed_prices);
			sma = calc_sma(0, closed_candle_len-1, closed_prices);
			ema = calc_ema(0, closed_candle_len-1, closed_prices);
			macd_all = calc_macd(0, closed_candle_len-1, closed_prices);
		}


	}

	system("CLS");


	//{ PRINT TITLES | CÜZDAN
	std::cout << setfill('-') << setw(55) << " " << std::endl << setfill(' ');
	std::cout << setw(6)  << "asset";
	std::cout << setw(11) << "price";
	std::cout << setw(11) << "free";
	std::cout << setw(7)  << "locked";
	std::cout << std::endl;
	//}

	//{ PRINT VALUES | CÜZDAN
	for ( int i  = 0 ; i < account_result["balances"].size() ; i++ ) {
		Balance balance = Balance(account_result["balances"][i]);

		std::cout << setw(6) << fixed << setprecision(3) << balance.Asset;
		std::cout << setw(11) << balance.Free + balance.Locked;
		std::cout << setw(11) << balance.Free;
		std::cout << setw(7) << balance.Locked;
		std::cout <<std::endl;
	}
	//}

	//{ PRINT DASHES
	std::cout << setfill('-') << setw(55) << " " << std::endl << setfill(' ');
	//}


	//{ PRINT TITLES | VARLIK VE STRATEJİLER
	std::cout << setw(7) << candle.Symbol << std::endl;
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

	std::cout << setw(10) << candle.ClosePrice;
	std::cout << setw(10) << rsi;
	std::cout << setw(10) << sma;
	std::cout << setw(10) << ema;
	std::cout << setw(10) << macd_all[0];
	std::cout << setw(10) << macd_all[1];
	std::cout << setw(10) << macd_all[2];

	std::cout <<std::endl;
	//}


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
	BinaCPP_websocket::connect_endpoint( ws_klines_onData ,"/ws/btcusdt@kline_1s");
	BinaCPP_websocket::enter_event_loop();


	return 0;
}
