/*
 * Candle.cpp
 *
 *  Created on: 11 Mar 2023
 *      Author: alida
 */

#include "Candle.h"

Candle::Candle() {

}

// kline json'unun geldigi varsayiliyor
Candle::Candle(Json::Value &result) {
//	std::cout << "Candle Constructor(Json::Value &result)" << std::endl;

	this->Symbol         = result["s"].asString().c_str();
	this->OpenPrice   	 = std::stod(result["o"].asString().c_str());
	this->ClosePrice     = std::stod(result["c"].asString().c_str());
	this->HighPrice 	 = std::stod(result["h"].asString().c_str());
	this->LowPrice 	     = std::stod(result["l"].asString().c_str());
	this->Volume 		 = std::stod(result["v"].asString().c_str());
	this->IsCandleClosed = result["x"].asBool();

}

Candle::~Candle() {
	// TODO Auto-generated destructor stub
}

