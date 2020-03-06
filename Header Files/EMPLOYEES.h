/*
EMPLOYEES.h
*/
	typedef char STR15[15+1];
	typedef char STR10[10+1];
	typedef struct EmployeeRecord
	{
		STR15 lastname;
		STR10 firstname;
		float hours,
		      reghrs,
		      ovthrs,
			payrate,
			defr,
			gross,
			fedtax,
			statetax,
			ssitax,
			netpay;
	} EmployeeRecord;

	
 
