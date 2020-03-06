/*
Program: main.cpp
Author: Eva Najera
Due Date: May 17, 2015
Description: This program computes gross pay, taxes,
              and net pay for each employee using number of hours worked,
              and hourly pay rate and displays totals and averages for each
              numeric column, sorts employee in alphabetical order, and prints
	      a check for each employee.

*/

#include "./EMPLOYEES.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "./TAXES.h"
#define ADDR(var) &var
#include <stdlib.h> 
#define MAXEMPS 5 
#define REG_HOURS 40
#define OVTRATE 1.5
#define MAXDESCS 9
#define TRUE 1
#define FALSE 0
typedef short BOOLEAN;

/*Function Prototypes*/
void PrintReportHeadings(FILE *reportfile); 
void InitializeAccumulators(int *numemps, float *totreg, float *totovt, float *totrate,
			   float *totgross, float *totfed, float *totstate,
			   float *totSSI, float *totdefr, float *totnet);						
void InputEmployeeData (EmployeeRecord *record, int i);
char *join_lastfirst(char * lastname,  char * firstname);
char *join_firstlast(char * lastname, char * firstname);
void split_hours(float hours, float * regularHours, float * ovtHours);
float CalculateGross(float hours, float payrate); 
extern void CalculateTaxes(float gross, float deferred, float *ft, float *st, float *ssit); 
void AddDetailToAccumulators(EmployeeRecord record[], int i, int *numemps, float *totrate,
				float *totreg, float *totovt, float *totgross,float *totfed,
				float *totstate, float *totSSI,
				float *totdefr, float *totnet);
void PrintDetailReport(FILE *reportfile, EmployeeRecord record[], int i);
void PrintSummaryReport(float totreg, float totovt, float totrate,
				float totgross, float totfed, float totstate,
				float totSSI, float totdefr, float totnet, 
				int numemps, FILE *reportfile);
extern void SortbyLastName(EmployeeRecord * record, int start, int end);
float calcOvtPay(float payrate, float ovtHours);
float calcRegPay(float payrate, float regularHours);
float calcTaxTotal(float fedtax, float statetax, float ssitax);
float calcNetPay(EmployeeRecord record[], int i);
void PrintCheck(FILE *CheckFile, EmployeeRecord record[], int i, int CheckNo, char *CompanyName, char *CompanyAddress,
 		char *CompanyCityStateZip, char *EmpAddress, char *EmpCityState, char *EmpZip);
void PrintCheckHeadings(FILE *CheckFile, char *CompanyName, char *CompanyAddress, char *CompanyCityStateZip, int CheckNo);
void PrintCheckBody(FILE *CheckFile, EmployeeRecord record[], int CheckNo, int i);
void PrintCheckStub(FILE *CheckFile, EmployeeRecord record[], char *EmpAddress, char *EmpCityState, char *EmpZip, int CheckNo, int i);
void getEmpResidence(char *EmpAddress, char *EmpCityState, char *Zip);
void getCompanyLocation(char * CompanyName, char *CompanyAddress, char *CompanyCityStateZip);
char * convertNetPay(float netpay);
char * generateRefCode(char * lastname, int chkNo);
BOOLEAN isVowel(char aChar);

int main(void)
{
	FILE *reportfile;  
	FILE *CheckFile;
	float totreg, totovt, totrate, totgross, totfed, totstate, totSSI, totdefr, totnet;
	int numemps, i = 0, CheckNo = 100;
	const int SIZE = MAXEMPS;
	EmployeeRecord record[SIZE];
	char CompanyName[20], CompanyAddress[30], CompanyCityStateZip[40];
	char EmpAddress[25], EmpCityState[30], EmpZip[7];
		
	reportfile = fopen("report.txt", "wt");
	if (reportfile == NULL)  //if open error, report error and exit
	{
		printf(" Cannot open data file\n");
		while (getchar() != '\n'); 
		printf(" Key press to exit \n");
		getchar();
		exit(-100); // requires <stdlib.h> 
	}
	
	CheckFile = fopen("EmployeeChecks.txt", "wt");
	if(CheckFile == NULL)
	{
		printf("Cannot open output file\n");
		while(getchar() != '\n');
		printf("Key press to exit\n");
		getchar();
		exit(-100);
	} 

	InitializeAccumulators(ADDR(numemps),ADDR(totreg),ADDR(totovt),ADDR(totrate),ADDR(totgross),ADDR(totfed),ADDR
	(totstate),ADDR(totSSI),ADDR(totdefr),ADDR(totnet));
	getCompanyLocation(CompanyName, CompanyAddress, CompanyCityStateZip);
	getEmpResidence(EmpAddress, EmpCityState, EmpZip);

	while (numemps < MAXEMPS) {  
		InputEmployeeData(record, i);
		record[i].gross = CalculateGross(record[i].hours, record[i].payrate);
		CalculateTaxes(record[i].gross,record[i].defr,ADDR(record[i].fedtax),ADDR(record[i].statetax),ADDR
		(record[i].ssitax));
		split_hours(record[i].hours, ADDR(record[i].reghrs), ADDR(record[i].ovthrs));
		AddDetailToAccumulators(record, i, &numemps, &totrate,&totreg, &totovt,&totgross, &totfed,&totstate,
		&totSSI, &totdefr, &totnet);
		i++;
	}
	SortbyLastName(record,0, SIZE-1);
	PrintReportHeadings(reportfile);
	for (i = 0; i < SIZE; i++)
	{
		PrintDetailReport(reportfile, record, i);
		PrintCheck(CheckFile, record, i, CheckNo, CompanyName, CompanyAddress, CompanyCityStateZip, EmpAddress,
	        EmpCityState,EmpZip);
		CheckNo++;	
    	}
	PrintSummaryReport(totreg, totovt, totrate, totgross, totfed, totstate,
	                   totSSI, totdefr, totnet, numemps, reportfile);
	fclose(reportfile);
	fclose(CheckFile);
	while (getchar() != '\n');
	return 0;			
}

//function definitions
void PrintReportHeadings(FILE *reportfile)
{
	
	fprintf(reportfile,"            Employee             Pay     Reg Hrs    Gross     Fed       SSI     Net\n");
	fprintf(reportfile,"            Name                 Rate    Ovt Hrs    Pay       State     Defr    Pay\n"); 
	fprintf(reportfile,"            ========             =====   =======    =======   =======   ======  =======  \n");
}

void InitializeAccumulators(int *numemps, float *totreg, float *totovt, float *totrate,
							float *totgross, float *totfed, float *totstate,
							float *totSSI, float *totdefr, float *totnet)
{
	*numemps = 0;
	*totreg = 0.0;
	*totovt = 0.0;
	*totrate = 0.0;
	*totgross = 0.0;
	*totfed = 0.0;
	*totstate = 0.0;
	*totSSI = 0.0;
	*totdefr = 0.0;
	*totnet = 0.0;		
}						
void InputEmployeeData (EmployeeRecord *record, int i)
{	
	
	printf("    Please enter employee's last name: "); 
	scanf("%s", record[i].lastname);
	printf("    Please enter employee's first name: ");
	scanf("%s", record[i].firstname);
	printf("    Please enter the employee's total hours worked for this pay period: ");
	scanf("%f", &record[i].hours);
        printf("    Please enter employee's hourly rate of pay: "); 
	scanf("%f", &record[i].payrate); 
	printf("    Please enter employee's amount of deferred earnings: ");
	scanf("%f", &record[i].defr); 	
}

char *join_lastfirst(char * lastname,  char * firstname)
{
	char first[30], last[30],lastfirst[60];
	strcpy(first, firstname);
	strcpy(last, lastname);
	strcat(last, ", ");
	strcpy(lastfirst, last);
	return strcat(lastfirst, first);
	
}
char *join_firstlast(char * lastname, char * firstname)
{
	char last[30],first[30],firstLast[60];	
	strcpy(last, lastname);
	strcpy(first, firstname);
	strcat(first, " ");
	strcpy(firstLast, first);
	return strcat(firstLast, last);
}

void split_hours(float hours, float * regularHours, float * ovtHours)
{	
	if (hours <= 40) {
        *regularHours = hours;
        *ovtHours = 0.0;
    }
    else {
        *regularHours = 40;
        *ovtHours = hours - 40;
    }
}	

float calcRegPay(float payrate, float regularHours)
{	
	return regularHours * payrate;
}

float calcOvtPay(float payrate, float ovtHours)
{	
	return ovtHours * OVTRATE * payrate;
}

float CalculateGross(float hours, float payrate)
{
	if (hours <= REG_HOURS)
		return hours * payrate;
	else
		return payrate * REG_HOURS + (hours - REG_HOURS) * OVTRATE * payrate;	
}
float calcTaxTotal(float fedtax, float statetax, float ssitax)
{
	return fedtax + statetax + ssitax;
}
float calcNetPay(EmployeeRecord record[], int i)
{

	record[i].netpay = record[i].gross - (record[i].fedtax + record[i].statetax + record[i].ssitax + record
	[i].defr);
	return record[i].netpay;
}

void AddDetailToAccumulators(EmployeeRecord record[], int i, int *numemps, float *totrate,float *totreg, float *totovt, 			     	     float *totgross,float *totfed, float *totstate, float *totSSI,float *totdefr, float 				     *totnet)
{
	*numemps = *numemps + 1;
	*totrate = *totrate + record[i].payrate;
	*totreg = *totreg + record[i].reghrs;
	*totovt = *totovt + record[i].ovthrs;
	*totgross = *totgross + record[i].gross;
	*totfed = *totfed + record[i].fedtax;
	*totstate = *totstate + record[i].statetax;
	*totSSI = *totSSI + record[i].ssitax;
	*totdefr = *totdefr + record[i].defr;
	*totnet = *totnet + calcNetPay(record, i);			
}

void PrintSummaryReport(float totreg, float totovt, float totrate, float totgross, float totfed, 
			float totstate, float totSSI, float totdefr, float totnet, int numemps, FILE *reportfile)
{
	fprintf(reportfile,"            Totals               %5.2f   %7.2f    %7.2f   %7.2f   %6.2f  %5.2f\n",
	        totrate, totreg, totgross, totfed, totSSI, totnet);
	fprintf(reportfile,"                                         %7.2f              %7.2f %8.2f\n", 
	totovt,totstate, totdefr);
	fprintf(reportfile,"            Averages             %5.2f   %7.2f    %7.2f   %7.2f   %6.2f   %5.2f\n",
	        totrate/numemps, totreg/numemps, totgross/numemps, totfed/numemps, totSSI/numemps, totnet/numemps);
	fprintf(reportfile,"                                         %7.2f              %7.2f %8.2f\n", 
	totovt/numemps,totstate/numemps, totdefr/numemps);
}

void PrintDetailReport(FILE *reportfile, EmployeeRecord record[], int i)
{
	char fullname[50];
	strcpy(fullname,join_lastfirst(record[i].lastname, record[i].firstname));
	fprintf(reportfile,"            %-21s%5.2f   %7.2f    %7.2f   %7.2f   %6.2f   %5.2f\n",
	        fullname,record[i].payrate, record[i].reghrs, record[i].gross, record[i].fedtax, record[i].ssitax,
	 	record[i].netpay);
	fprintf(reportfile,"                                         %7.2f              %7.2f %8.2f\n",
        	record[i].ovthrs,record[i].statetax, record[i].defr);
}

void getCompanyLocation(char * CompanyName, char *CompanyAddress, char *CompanyCityStateZip)
{
	FILE *CompanyDataFile;	
	CompanyDataFile = fopen("CompanyInfo.dat", "r");
	if(CompanyDataFile == NULL)
	{
		printf(" Cannot open data file\n");
	  	while (getchar() != '\n');
	  	printf("Key press to exit\n");
	  	getchar();
	  	exit(-100);
	}	
	char *str;
	fgets(CompanyName, 20, CompanyDataFile);
	if (( str = strchr(CompanyName, '\n')) != NULL)//(1) gets rid of newline (1-3)
		*str = '\0';
	fgets(CompanyAddress, 30, CompanyDataFile);
	if (( str = strchr(CompanyAddress, '\n')) != NULL)//(2)
		*str = '\0';
	fgets(CompanyCityStateZip, 40, CompanyDataFile);
	if (( str = strchr(CompanyCityStateZip, '\n')) != NULL)//(3)
		*str = '\0';
	fclose(CompanyDataFile);
}
void PrintCheck(FILE *CheckFile, EmployeeRecord record[], int i, int CheckNo, char *CompanyName, char *CompanyAddress,
	 	char *CompanyCityStateZip, char *EmpAddress, char *EmpCityState, char *EmpZip)
{

	PrintCheckHeadings(CheckFile, CompanyName, CompanyAddress, CompanyCityStateZip,
	CheckNo);
	PrintCheckBody(CheckFile, record, CheckNo, i);
	PrintCheckStub(CheckFile, record, EmpAddress, EmpCityState, EmpZip, CheckNo, i);
}

void PrintCheckHeadings(FILE *CheckFile, char *CompanyName, char *CompanyAddress, char *CompanyCityStateZip,int CheckNo)
{
	
	fprintf(CheckFile,"*********************************************************************\n");
	fprintf(CheckFile,"*                                        Check No %d               *\n",CheckNo);
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*         %s                                         *\n",CompanyName);                                                                     
	fprintf(CheckFile,"*         %s                                   *\n",CompanyAddress);
	fprintf(CheckFile,"*         %s                               *\n", CompanyCityStateZip);
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*                                                                   *\n");
}

void PrintCheckBody(FILE *CheckFile, EmployeeRecord record[], int CheckNo, int i)
{

	fprintf(CheckFile,"*         Pay to the Order of                                       *\n");
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*         %-20s                                      *\n",join_firstlast(record[i].lastname,
	record[i].firstname));
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*         %-58s*\n",convertNetPay(record[i].netpay));
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*         Reference: %-10s                                     *\n",generateRefCode(record
	[i].lastname,CheckNo));
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*********************************************************************\n");
}

void PrintCheckStub(FILE *CheckFile, EmployeeRecord record[], char *EmpAddress, char *EmpCityState, char *EmpZip, int CheckNo, int i)
{
	char firstLast[55];	
	sprintf(firstLast, "%s %s", record[i].firstname, record[i].lastname);
	fprintf(CheckFile,"*         Check No %d                                              *\n", CheckNo);
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*         %-15s  Reg Pay:   %6.2f  Fed Tax:   %6.2f     *\n", firstLast, calcRegPay(record
	[i].payrate, record[i].reghrs), record[i].fedtax); 	
	fprintf(CheckFile,"*         %-15s  Ovt Pay:   %6.2f  State Tax: %6.2f     *\n", EmpAddress, calcOvtPay(record
	[i].payrate, record[i].ovthrs), record[i].statetax);
	fprintf(CheckFile,"*         %-15s  Gross Pay: %6.2f  SSI Tax:   %6.2f     *\n", EmpCityState, record
	[i].gross, record[i].ssitax);
	fprintf(CheckFile,"*         %-15s  Net Pay:   %6.2f  Tax Total: %6.2f     *\n", EmpZip, calcNetPay(record, i),
	calcTaxTotal(record[i].fedtax, record[i].statetax, record[i].ssitax));
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*                                                                   *\n");
	fprintf(CheckFile,"*********************************************************************\n");

}

void getEmpResidence(char *EmpAddress, char *EmpCityState, char *Zip)
{
	FILE *EmpDataFile;	
	EmpDataFile = fopen("EmpData.txt", "r");
	if(EmpDataFile == NULL)
	{
		printf(" Cannot open data file\n");
	  	while (getchar() != '\n');
	  	printf("Key press to exit\n");
	  	getchar();
	  	exit(-100);
	}
	char EmpAddress1[8], EmpAddress2[8], EmpAddress3[8], EmpCityState1[15], EmpCityState2[8];
	fscanf(EmpDataFile, "%s %s %s", EmpAddress1,EmpAddress2,EmpAddress3);
	fscanf(EmpDataFile, "%s %s", EmpCityState1, EmpCityState2);
	fscanf(EmpDataFile, "%s", Zip);
	sprintf(EmpAddress, "%s %s %s", EmpAddress1, EmpAddress2, EmpAddress3);
	sprintf(EmpCityState, "%s %s", EmpCityState1, EmpCityState2);
	fclose(EmpDataFile);
}


char * convertNetPay(float netpay)
{
	char Ones[9][9+1];
	char Tens[9][9+1];
	char Teens[9][9+1];
	FILE * inputFile;
	
	float cents;
	int numHuns, numTens, numOnes;
	cents = netpay - int(netpay);
	numHuns = int(netpay)/100;
	char netPayString[100];

	inputFile = fopen("./numStrings.dat","rt");
	if (inputFile == NULL)
	{
		printf(" File open failed ...\n");
		printf("   Press any to terminate ...");
		while (getchar() != '\n'); 
		exit(-17); 
	}

	int i;
	for (i=0; i < MAXDESCS; i++)
		fscanf(inputFile,"%s",Ones[i]);
	for (i=0; i < MAXDESCS; i++)
		fscanf(inputFile,"%s",Tens[i]);
	for (i=0; i < MAXDESCS; i++)
		fscanf(inputFile,"%s",Teens[i]);
	fclose(inputFile);	
	
	strcpy(netPayString, "The sum ");
	if (numHuns > 0)
	{
		strcat(netPayString, Ones[numHuns-1]);
		strcat(netPayString, " Hundred ");
	}
	int remainder = int(netpay)%100;
	if ((remainder >= 11) && (remainder <=19))
		strcat(netPayString, Teens[remainder - 11]);
	else if (remainder >=20)
	{
		numTens = int(netpay) %100/10;
		numOnes = int(netpay) %100%10;
		if (numTens > 0)
		{
			strcat(netPayString, Tens[numTens-1]);
			strcat(netPayString, "-");
		}
		if (numOnes > 0)
		{
			strcat(netPayString, Ones[numOnes-1]);
		}
	}
	strcat(netPayString, " and ");
	cents = cents + .005;
	cents = cents * 100;
	cents = int(cents);
	char ResultString[10];
	sprintf(ResultString, "%d",int(cents));
	strcat(netPayString, ResultString);
	return strcat(netPayString, "/100 Dollars");	
}
char * generateRefCode(char * lastname, int chkNo)
{
	static char tempStr[30+1];
	int i,j = 1;
	char ChkNum[10];

	tempStr[0] = lastname[0];
	for (i=1; i < strlen(lastname);i++)
	{
	  if (!isVowel(lastname[i])) 
	  tempStr[j++] = toupper(lastname[i]);
	}
	tempStr[j] = 0; // add string terminator
	sprintf(ChkNum, "%d", chkNo); 
	return strcat(tempStr, ChkNum);	
}

BOOLEAN isVowel(char aChar)
{
	switch (toupper(aChar)) //<ctype.h>
	{
	  case 'A':
	  case 'E':
	  case 'I':
	  case 'O':
	  case 'U' : return TRUE;break;
	  default: return FALSE;
	}
}
