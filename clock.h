int addAlarm(String name, int hour, int minute, void (*execute)(int));
void alarmLoop();
String showAlarms();
RtcDateTime getCurrentRtcDateTime();
String getCurrentDate();
void setupClock();
int alarmsCount();