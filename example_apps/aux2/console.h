#pragma once
#include <auxe/auxe.h>

void read_auxenv(int& fs0, vector<string>& auxpathfromenv, int& precision, const string& envfilename);
void save_auxenv(auxContext* ctx, int precision, const string& envfilename); // auxenv.cpp

#define AUX2_VERSION "1.01"
