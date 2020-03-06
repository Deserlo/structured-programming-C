#include <stdio.h>
#define ADDR(var) &var
#include "./TAXES.h"

void CalculateTaxes(float gross, float deferred, 
                    float *ft, 
					float *st,
					float *ssit); //3.5
float CalcFedtax(float gross, float deferred); //3.5.1
float CalcStatetax(float fedtax); //3.5.2
float CalcSSItax(float gross, float deferred); //3.5.3

void CalculateTaxes(float gross, float deferred, float *ft, float *st, float *ssit) //3.5
{
  *ft = CalcFedtax(gross, deferred);
  *st = CalcStatetax(*ft);
  *ssit = CalcSSItax(gross, deferred);
}

float CalcFedtax(float gross, float deferred) //3.5.1
{
	return (gross - deferred) * FEDTAXRATE; //15% tax
}

float CalcStatetax(float fedtax) //3.5.2
{
	return (fedtax * STATETAXRATE);  //7% tax
}

float CalcSSItax(float gross, float deferred) //3.5.3
{
	return (gross - deferred) * SSITAXRATE; //7.75% tax
}
