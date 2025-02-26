/** @brief Gh control constants, structures, function prototypes.
 *  @file ghcontrol.h
 */
#ifndef GHCONTROL_H
#define GHCONTROL_H

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Constants
#define SEARCHSTR "serial\t\t:"
#define SYSINFOBUFSZ 512
#define GHUPDATE 2000
#define SENSORS 3
#define TEMPERATURE 0
#define HUMIDITY 1
#define PRESSURE 2
#define CTIMESTRSZ 25

//Simulation Constants
#define SIMULATE 1
#define USTEMP 50
#define LSTEMP -10
#define USHUMID 100
#define LSHUMID 0
#define USPRESS 1016
#define LSPRESS 975
#define SIMTEMPERATURE 0
#define SIMHUMIDITY 0
#define SIMPRESSURE 0

//Control Constants
#define STEMP 25.0
#define SHUMID 55.0
#define ON 1
#define OFF 0

//Sensehat Constants
#define NUMBARS 8
#define NUMPTS 8.0
#define TBAR 7
#define HBAR 5
#define PBAR 3
#define SENSEHAT 1

//Alarm Constants
#define NALARMS 7
#define ALARMNMSZ 18
#define LOWERATEMP 10
#define UPPERATEMP 30
#define LOWERAHUMID 25
#define UPPERAHUMID 70
#define LOWERAPRESS 985
#define UPPERAPRESS 1016

// Enumerated Types
typedef enum {NOALARM,HTEMP,LTEMP,HHUMID,LHUMID,HPRESS,LPRESS} alarm_e;

//Structures
typedef struct alarmlimits
{
    double hight;
    double lowt;
    double highh;
    double lowh;
    double highp;
    double lowp;
}alarmlimit_s;

typedef struct alarms
{
    alarm_e code;
    time_t atime;
    double value;
}alarm_s;

typedef struct readings
{
	time_t rtime;
	double temperature;
	double humidity;
	double pressure;
}reading_s;

typedef struct setpoints
{
    double temperature;
    double humidity;
}setpoint_s;

typedef struct controls
{
    int heater;
    int humidifier;
}control_s;

//Function Prototypes
///@cond INTERNAL
double GhGetHumidity(void);
double GhGetPressure(void);
double GhGetTemperature(void);
void GhControllerInit(void);
void GhDisplayControls(control_s ctrl);
void GhDisplayReadings(reading_s rdata);
void GhDisplayTargets(setpoint_s spts);
reading_s GhGetReadings(void);
setpoint_s GhSetTargets(void);
control_s GhSetControls(setpoint_s target, reading_s rdata);
void GhDisplayHeader(const char * sname);
u_int64_t GhGetSerial(void);
int GhGetRandom(int range);
void GhDelay(int milliseconds);
int GhLogData(char * fname, reading_s ghdata);
int GhSaveSetpoints(char * fname, setpoint_s spts);
setpoint_s GhRetrieveSetpoints(char * fname);
void GhDisplayAll(reading_s rd,setpoint_s sd);
alarmlimit_s GhSetAlarmLimits(void);
void GhSetAlarms(alarm_s calarm[NALARMS], alarmlimit_s alarmpt, reading_s rdata);
void GhDisplayAlarms(alarm_s alarmdisplay[NALARMS]);
///@endcond
#endif
