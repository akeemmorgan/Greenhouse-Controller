/** @brief Gh Control functions.
 *  @file ghcontrol.c
 */
#include "ghcontrol.h"
#include "pisensehat.h"

// Alarm Message Array
const char alarmnames[NALARMS][ALARMNMSZ] =
        {"No Alarms","High Temperature","Low Temperature",
        "High Humidity", "Low Humidity","High Pressure","Low Pressure"};

/** @brief Simulate or Read (via sensor) humidity value.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return Value for pressure.
*/
double GhGetHumidity(void)
{
#if SIMHUMIDITY
	return GhGetRandom(USHUMID-LSHUMID);
#else
    ht221sData_s ct = {0};

    ct = ShGetHT221SData();
	return ct.humidity;
#endif
}

/** @brief Simulate or Read (via sensor) pressure value.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return Value for pressure.
*/
double GhGetPressure(void)
{
#if SIMPRESSURE
	return GhGetRandom(USPRESS-LSPRESS)+LSPRESS;
#else
    lps25hData_s ct = {0};

    ct = ShGetLPS25HData();
	return ct.pressure;
#endif
}

/** @brief Simulate or Read (via sensor) temperature value.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return Value for temperature.
*/
double GhGetTemperature(void)
{
#if SIMTEMPERATURE
	return GhGetRandom(USTEMP-LSTEMP)+LSTEMP;
#else
	ht221sData_s ct = {0};

	ct = ShGetHT221SData();
	return ct.temperature;
#endif
}

/** @brief Provide dynamic sensor values for temperature, humidity and pressure.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return now
*/
reading_s GhGetReadings(void)
{
    reading_s now = {0};
    now.rtime = time(NULL);
    now.temperature = GhGetTemperature();
    now.humidity = GhGetHumidity();
    now.pressure = GhGetPressure();
    return now;
}

/** @brief Controller initialization.
*   @version 27MAR2021
*   @author Akeem Morgan
*   @param void
*   @return void
*/
void GhControllerInit(void)
{
#if SIMULATE
	srand((unsigned) time(NULL));
#endif // SIMULATE
#if SENSEHAT
	ShInit();
#endif // SENSEHAT
	GhDisplayHeader("Akeem Morgan");
}

/** @brief Determines the state of the controls based on the read values.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param setpoint_s datatype named "target" to pass the target values, and another structure, reading_s named "rdata" to pass sensor data.
*   @return cset
*/
control_s GhSetControls(setpoint_s target, reading_s rdata)
{
    control_s cset = {0};
    if (rdata.temperature < target.temperature) {cset.heater = ON;}
    else {cset.heater = OFF;}

    if (rdata.humidity < target.humidity) {cset.humidifier = ON;}
    else {cset.humidifier = OFF;}

    return cset;
}

/** @brief Print the state of the heater and humidifier to the console.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param control_s datatype, named "ctrl" to pass the heater and humidifier status.
*   @return void
*/
void GhDisplayControls(control_s ctrl)
{
	fprintf(stdout,"\nControls\tHeater: %d\tHumidifier: %d\n", ctrl.heater, ctrl.humidifier);
}

/** @brief Prints the time of the readings and the sensor values to the console.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param reading_s datatype, named "rdata" to pass sensor data and the time.
*   @return void
*/
void GhDisplayReadings(reading_s rdata)
{
    printf("\n%sReadings\tT: %4.1lfC\tH: %4.1lf%%\tP: %6.1lfmB",
    ctime(&rdata.rtime),rdata.temperature,rdata.humidity,
    rdata.pressure);
}

/** @brief Prints the target values for temperature and humidity to the console.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return void
*/
void GhDisplayTargets(setpoint_s spts)
{
	fprintf(stdout,"\nTargets\t\tT:%5.0lfC\tH:%5.0lf%%", spts.temperature, spts.humidity);
}

/** @brief Stores/Retrieves the defined target values for the sensors.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param void
*   @return cpoints
*/
setpoint_s GhSetTargets(void)
{
    setpoint_s cpoints = {0};
    cpoints = GhRetrieveSetpoints("setpoints.dat");
    if (cpoints.temperature == 0)
    {
        cpoints.temperature = STEMP;
        cpoints.humidity = SHUMID;
    }
    return cpoints;
}

/** @brief Prints the controller title.
*   @version 20FEB2021
*   @author Akeem Morgan
*   @param sname string with the operator's name.
*   @return void
*/
void GhDisplayHeader(const char * sname)
{
	fprintf(stdout, "%s's CENG153 Greenhouse Controller\n", sname);
}

/** @brief Read device serial number.
*   @version 20FEB2021
*   @author Akeem Morgan
*   @param void
*   @return unsigned integer 'serial', with the hw serial no.
*/
u_int64_t GhGetSerial(void)
{
	static u_int64_t serial = 0;
	FILE * fp;
	char buf[SYSINFOBUFSZ];
	char searchstring[] = SEARCHSTR;
	fp = fopen ("/proc/cpuinfo", "r");
	if (fp != NULL)
	{
		while (fgets(buf, sizeof(buf), fp) != NULL)
		{
			if(!strncasecmp(searchstring, buf, strlen(searchstring)))
			{
				sscanf(buf+strlen(searchstring), "%Lx", &serial);
			}
		}
		fclose(fp);
	}
	if(serial==0)
	{
		system("uname -a");
		system("ls --fu /usr/lib/codeblocks | grep -Po '\\.\\K[^ ]+'>stamp.txt");
		fp = fopen ("stamp.txt", "r");
		if (fp != NULL)
		{
			while (fgets(buf, sizeof(buf), fp) != NULL)
			{
				sscanf(buf, "%Lx", &serial);
			}
			fclose(fp);
		}
	}
	return serial;
}

/** @brief Randomizes a value within defined limits.
*   @version 20FEB2021
*   @author Akeem Morgan
*   @param integer 'range' to define the scope
*   @return randomized value within the limits of 'range'
*/
int GhGetRandom(int range)
{
	return rand() % range;
}

/** @brief Pauses the controller between readings.
*   @version 20FEB2021
*   @author Akeem Morgan
*   @param integer milliseconds, with the number of milliseconds
*   @return void
*/
void GhDelay(int milliseconds)
{
	long wait;
	clock_t now, start;

	wait = milliseconds*(CLOCKS_PER_SEC/1000);
	start = clock();
	now = start;
	while( (now-start) < wait )
	{
		now = clock();
	}
}

/** @brief Save data to a file.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param char pointer "fname" to a file name and reading_s datatype named "ghdata" to process values,
*   @return 0 = Operation failed. 1 = Operation successful.
*/
int GhLogData(char * fname, reading_s ghdata)
{
    FILE * fp;
    char ltime[CTIMESTRSZ];

    fp = fopen (fname,"a");
    if (fp == NULL)
    {
        return 0;
    }
    else
    {
        strcpy (ltime,ctime(&ghdata.rtime));
        ltime[3] = ',';
        ltime[7] = ',';
        ltime[10] = ',';
        ltime[19] = ',';

        fprintf(fp,"\n%.24s,%5.1lf,%5.1lf,%6.1lf", ltime, ghdata.temperature, ghdata.humidity, ghdata.pressure);
        fclose(fp);
        return 1;
    }
}

/** @brief Adds "save" ability
*   @version 02APR2021
*   @author Akeem Morgan
*   @param char pointer "fname" to a file name and setpoint_s datatype named "spts" to process values.
*   @return 0 = Operation failed. 1 = Operation successful.
*/
int GhSaveSetpoints(char * fname, setpoint_s spts)
{
    FILE * fp;

    fp = fopen(fname,"w");
    if (fp == NULL)
    {
        return 0;
    }
    else
    {
        fwrite(&spts,sizeof(spts),1,fp);
        fclose(fp);
        return 1;
    }
}

/** @brief Adds "open" ability
*   @version 02APR2021
*   @author Akeem Morgan
*   @param char pointer "fname" to a file name.
*   @return spts.
*/
setpoint_s GhRetrieveSetpoints(char * fname)
{
    setpoint_s spts = {0};
    FILE * fp;

    fp = fopen(fname,"r");
    if (fp == NULL)
    {
        return spts;
    }
    else
    {
        fread(&spts,sizeof(spts),1,fp);
        fclose(fp);
        return spts;
    }
}

/** @brief Display values on led array.
*   @version 02APR2021
*   @author Akeem Morgan
*   @param reading_s datatype named "rd" to process values.
*   @param setpoint_s datatype named "sd" to process values.
*   @return void
*/
void GhDisplayAll(reading_s rd,setpoint_s sd)
{
    int rv,sv,avh,avl;
    fbpixel_s pxc = {0};

    ShClearMatrix();

    // Set TEMPERATURE data bar and setpoints pixel...
    rv = (8.0*(((rd.temperature-LSTEMP)/(USTEMP-LSTEMP))+0.05))-1.0;
    sv = (8.0*(((sd.temperature-LSTEMP)/(USTEMP-LSTEMP))+0.05))-1.0;
    pxc.red = 0x00;
    pxc.green = 0xFF;
    pxc.blue = 0x00;
    ShSetVerticalBar(TBAR,pxc,rv);
    pxc.red = 0xF0;
    pxc.green = 0x0F;
    pxc.blue = 0xF0;
    ShSetPixel(TBAR,sv,pxc);

    // Set HUMIDITY data bar and setpoints pixel...
    rv = (8.0*(((rd.humidity-LSHUMID)/(USHUMID-LSHUMID))+0.05))-1.0;
    sv = (8.0*(((sd.humidity-LSHUMID)/(USHUMID-LSHUMID))+0.05))-1.0;
    pxc.red = 0x00;
    pxc.green = 0xFF;
    pxc.blue = 0x00;
    ShSetVerticalBar(HBAR,pxc,rv);
    pxc.red = 0xF0;
    pxc.green = 0x0F;
    pxc.blue = 0xF0;
    ShSetPixel(HBAR,sv,pxc);

    // Set PRESSURE data bar...
    rv = (8.0*(((rd.pressure-LSPRESS)/(USPRESS-LSPRESS))+0.05))-1.0;
    pxc.red = 0x00;
    pxc.green = 0xFF;
    pxc.blue = 0x00;
    ShSetVerticalBar(PBAR,pxc,rv);
}

/** @brief Set alarm limits.
*   @version 10APR2021
*   @author Akeem Morgan
*   @param void
*   @return alarmlimit_s object, calarm
*/
alarmlimit_s GhSetAlarmLimits(void)
{
    alarmlimit_s calarm;

    calarm.hight = UPPERATEMP;
    calarm.lowt = LOWERATEMP;
    calarm.highh = UPPERAHUMID;
    calarm.lowh = LOWERAHUMID;
    calarm.highp = UPPERAPRESS;
    calarm.lowp = LOWERAPRESS;

    return calarm;
}

/** @brief Set alarms.
*   @version 10APR2021
*   @author Akeem Morgan
*   @param array alarm_s calarm[NALARMS]
*   @param alarmlimit_s object alarmpt
*   @param reading_s object rdata
*   @return alarmlimit_s object calarm
*/
void GhSetAlarms(alarm_s calarm[NALARMS], alarmlimit_s alarmpt, reading_s rdata)
{
    for (int i=0; i<NALARMS; i++)
    {
        calarm[i].code = NOALARM;
    }

    if (rdata.temperature >= alarmpt.hight)
    {
        calarm[HTEMP].code = HTEMP;
        calarm[HTEMP].atime = rdata.rtime;
        calarm[HTEMP].value = rdata.temperature;
    }

    if (rdata.temperature <= alarmpt.lowt)
    {
        calarm[LTEMP].code = LTEMP;
        calarm[LTEMP].atime = rdata.rtime;
        calarm[LTEMP].value = rdata.temperature;
    }

    if (rdata.humidity >= alarmpt.highh)
    {
        calarm[HHUMID].code = HHUMID;
        calarm[HHUMID].atime = rdata.rtime;
        calarm[HHUMID].value = rdata.humidity;
    }

    if (rdata.humidity <= alarmpt.lowh)
    {
        calarm[LHUMID].code = LHUMID;
        calarm[LHUMID].atime = rdata.rtime;
        calarm[LHUMID].value = rdata.humidity;
    }

    if (rdata.pressure >= alarmpt.highp)
    {
        calarm[HPRESS].code = HPRESS;
        calarm[HPRESS].atime = rdata.rtime;
        calarm[HPRESS].value = rdata.pressure;
    }

    if (rdata.pressure <= alarmpt.lowp)
    {
        calarm[LPRESS].code = LPRESS;
        calarm[LPRESS].atime = rdata.rtime;
        calarm[LPRESS].value = rdata.pressure;
    }
}

/** @brief Prints the current state of the alarms to the display.
*   @version 10APR2021
*   @author Akeem Morgan
*   @param array alarm_s alarmdisplay[NALARMS]
*   @return void
*/
void GhDisplayAlarms(alarm_s alarmdisplay[NALARMS])
{
    fprintf(stdout,"\t\t\tALARMS\n");
    for (int i=1; i<NALARMS; i++)
    {
        if (alarmdisplay[i].code != NOALARM)
        fprintf(stdout,"%s alarm on %s",alarmnames[i],ctime(&alarmdisplay[i].atime));
    }
}
