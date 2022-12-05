/*
 *	ntp.h
 */

void setupTime(void);

struct Time_Record getTimeUpdated(void);

struct Time_Record {
    int hours;
    int minutes;
    int seconds;
};
