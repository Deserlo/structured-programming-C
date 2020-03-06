calcTax.o:	calcTaxes.cpp TAXES.h 
	g++ -c calcTaxes.cpp -o calcTax.o
SortbyLastName.o:	SortbyLastName.cpp EMPLOYEES.h
	g++ -c SortbyLastName.cpp -o SortbyLastName.o
main.exe:	main.cpp calcTax.o SortbyLastName.o
	g++ main.cpp calcTax.o SortbyLastName.o -o main.exe
