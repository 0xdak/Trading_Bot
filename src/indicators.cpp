/*
 * indicators.cpp
 *
 *  Created on: 7 May 2023
 *      Author: alida
 */
#include "indicators.h"


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

	TA_RetCode rc = TA_EMA(startIdx, endIDx, array, RSI_PERIOD, &outBegIdx, &outNBElement, outReal);
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];
}

// {macd, macdsignal, macdhist}
std::vector<double> calc_macd(int startIdx, int endIDx, double* array) {
	int outBegIdx;
	int outNBElement;
	std::vector<double> outMACD(endIDx+1);
	std::vector<double> outMACDSignal(endIDx+1);
	std::vector<double> outMACDHist(endIDx+1);
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
	std::vector<double> outReal(endIDx+1);

	TA_RetCode rc = TA_SMA(startIdx, endIDx, array, 5, &outBegIdx, &outNBElement, outReal.data());
	if (rc != TA_SUCCESS){
		std::cerr << "RSI::RSI: error on TA_RSI" << std::endl;
		return -1;
	}

	return outReal[outNBElement-1];
}
