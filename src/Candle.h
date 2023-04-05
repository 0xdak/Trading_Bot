/*
 * Candle.h
 *
 *  Created on: 11 Mar 2023
 *      Author: alida
 */

#ifndef CANDLE_H_
#define CANDLE_H_

#include <iostream>
#include <json/json.h>

class Candle {
public:
	std::string Symbol    = "";
	double OpenPrice   	  = -1;
	double ClosePrice     = -1;
	double HighPrice 	  = -1;
	double LowPrice 	  = -1;
	double Volume 		  = -1;
	bool   IsCandleClosed = -1;


	Candle();
	Candle(Json::Value &result);
	virtual ~Candle();


    friend std::ostream& operator<<(std::ostream& os, const Candle& candle) {
        os << "Candle { " << std::endl;
        os << "\tSymbol\t: " << candle.Symbol << std::endl;
        os << "\tOpen Price\t: " << candle.OpenPrice << std::endl;
        os << "\tClose Price\t: " << candle.ClosePrice << std::endl;
        os << "\tHigh Price\t: " << candle.HighPrice << std::endl;
        os << "\tLow Price\t: " << candle.LowPrice << std::endl;
        os << "\tVolume\t: " << candle.Volume << std::endl;
        os << "\tIs Candle Closed\t: " << candle.IsCandleClosed << std::endl;
        os << "}" << std::endl;
        return os;
    }
};

#endif /* CANDLE_H_ */
