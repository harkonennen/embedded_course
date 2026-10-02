
//Contains the actual parser implementation


#include <stdlib.h>
#include <string.h>
#include "TimeParser.h"
#include <cstdio>
#include <ctype.h>

// time format: HHMMSS (6 characters)
int time_parse(char *time) {
	// how many seconds, default returns error
	int seconds = TIME_LEN_ERROR;

	// TODO: Check that string is not null
	if (time == NULL)
	{
		return TIME_NULL_ERROR;
	}
	
	if (strlen(time) != 6)
	{
		return TIME_LEN_ERROR;
	}

	for(int i = 0; i < 6; i++){
		if (!isdigit(time[i])){
			return TIME_DIGIT_ERROR;
		}
	}
	

    int values[3];
	values[2] = atoi(time+4); // seconds
	time[4] = 0;
	values[1] = atoi(time+2); // minutes
	time[2] = 0;
	values[0] = atoi(time); // hours

	if (values[0] < 0 || values[0] > 23 ||
    	values[1] < 0 || values[1] > 59 ||
    	values[2] < 0 || values[2] > 59) {

    return TIME_VALUE_ERROR;
	}

	seconds = values[1] * 60 + values[2];
	if (seconds == 0)
	{
		return TIME_VALUE_ERROR;
	}
	
	return seconds;
}
