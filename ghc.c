/** @brief Defines the entry point for the console application.
 *  @file ghc.c
 */
#include "ghcontrol.h"

int main(void)
{
	time_t now;
	int logged;

	control_s ctrl = {0};
	reading_s creadings = {0};
	setpoint_s sets = {0};
	sets = GhSetTargets();
	alarmlimit_s alimits = {0};
    alarm_s warn[NALARMS] = {0};
    alimits = GhSetAlarmLimits();

    GhControllerInit();

	while (1)
	{
		creadings = GhGetReadings();
		logged = GhLogData("ghdata.txt", creadings);
		ctrl = GhSetControls(sets, creadings);
		GhSetAlarms(warn, alimits, creadings);
		GhDisplayAll(creadings,sets);
		GhDisplayReadings(creadings);
		GhDisplayTargets(sets);
		GhDisplayControls(ctrl);
		GhDisplayAlarms(warn);
		GhDelay(GHUPDATE);
	}
	return EXIT_FAILURE;
}
