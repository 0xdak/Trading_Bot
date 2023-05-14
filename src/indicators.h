/*
 * indicators.h
 *
 *  Created on: 7 May 2023
 *      Author: alida
 */

#ifndef INDICATORS_H_
#define INDICATORS_H_

#define RSI_PERIOD 14

#include "ta_libc.h"
#include <iostream>
#include <vector>

double calc_rsi(int startIdx, int endIDx, double* array);
double calc_ema(int startIdx, int endIDx, double* array);
std::vector<double> calc_macd(int startIdx, int endIDx, double* array) ;
double calc_sma(int startIdx, int endIDx, double* array);

#endif /* INDICATORS_H_ */
