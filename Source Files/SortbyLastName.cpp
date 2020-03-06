#include <stdio.h>
#include "./EMPLOYEES.h"
#include <string.h>
#define MAXEMPS 5
void SortbyLastName(EmployeeRecord *record, int start, int end);


void SortbyLastName(EmployeeRecord *record, int start, int end)
{
	char temp[16];
	int left = start,
	    right = end,
	    pivot = (start+end)/2;
	EmployeeRecord temprecord[MAXEMPS];
	while (left < right) {
		//find left candidate
		while (strcmp(record[left].lastname, record[pivot].lastname) < 0) left++;	
		//find right candidate */
		while (strcmp(record[right].lastname, record[pivot].lastname) > 0) right--;
				if (left <= right) {
					temprecord[0] = record[left];
					record[left] = record[right];
					record[right] = temprecord[0];	
					left ++;
					right --;
				}	
		}
		if (start < end) SortbyLastName(record, start, right);
		if (left < end) SortbyLastName(record, left, end);
}
