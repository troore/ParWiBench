// $ cat /sys/class/micras/power
// 104000000  //Total power, win 0 uW
// 104000000  //Total power, win 1 uW
// 106000000  //PCI-E connector power
// 217000000  //Instantaneous power uW
// 32000000   //Max Instantaneous power
// 26000000   //2x3 connector power
// 48000000   //2x4 connector power
// 1) Core rail; Power reading(uW)
// 2) Core rail; Current(uA)
// 3) Core rail; Voltage(uV)
// 24000000 0 995000 
// 33000000 0 1000000 //Uncore rail; Power reading, Current, Voltage
// 32000000 0 1501000 //Memory subsystem rail; Power reading, Current, Voltage

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer/meas.h"
//volatile bool flag = false;

/*
double A[100][100];
double B[100][100];
double C[100][100];
*/

#define MICPOWER_MAX_COUNTERS 16

typedef struct MICPOWER_control_state
{
	long long counts[MICPOWER_MAX_COUNTERS];    // used for caching
	long long lastupdate;
} MICPOWER_control_state_t;

// code snippet stolen from PAPI 
static int read_sysfs_file(long long* counts)
{
	FILE *fp = NULL;
	int i;
	int retval = 1;
	
	fp = fopen("/sys/class/micras/power", "r");
	if (!fp)
		return 0;
	for (i = 0; i < MICPOWER_MAX_COUNTERS - 9; i++)
	{
		retval &= fscanf(fp, "%lld", &counts[i]);
	}
	for (i = MICPOWER_MAX_COUNTERS - 9; i < MICPOWER_MAX_COUNTERS; i += 3)
	{
		retval &= fscanf(fp, "%lld %lld %lld", &counts[i], &counts[i + 1], &counts[i + 2]);
	}
	fclose(fp);
	
	return retval;
}

volatile bool keepAlive = true;
double passEnergy = 0.0;
int _50ms_counts=0;
double Energy_thread_time;
void* recordEnergy(void *arg)
{
	int retval = 0;
	long long counts[MICPOWER_MAX_COUNTERS];    // used for caching
	struct timespec ts;

//	flag = true;

	ts.tv_sec = 0;
	ts.tv_nsec = 50000000L; // 50 milliseconds
//	ts.tv_nsec = 50L; // 0.05 usec
	double energy = 0.0;
	double tbegin,ttime;
	tbegin=dtime();
	while (keepAlive)
	{
		//	printf("%d\n", keepAlive);
		retval = read_sysfs_file(counts);
		energy += counts[0];
		_50ms_counts++;
		nanosleep(&ts, NULL);
	}
	ttime=dtime();
	Energy_thread_time=ttime-tbegin;
//	keepAlive = true;
	passEnergy = energy;
//	energy = 0.0;

	return NULL;
}

pthread_t micPthread;
void micpower_start()
{
	int iret1 = pthread_create(&micPthread, NULL, recordEnergy, (void *)NULL);
}

double micpower_finalize()
{
	keepAlive = false;
	pthread_join(micPthread, NULL);
	printf("counts = %d\n",_50ms_counts);
	printf("Energy_thread_time = %lf\n", Energy_thread_time);
//	printf("%lf\n", Energy_thread_time / _50ms_counts);
	// (uw * ms) / 10e-9 = J
//	return (passEnergy * 70.0) / (1000 * 1000 * 1000);
	return (passEnergy * (Energy_thread_time / _50ms_counts)) / (1000 * 1000 * 1000);
}

/*
int main() {
  micpower_start();

 // while (!flag) {}

  for (int i = 0; i < 100; i++)
	  for (int j = 0; j < 100; j++)
	  {
		  A[i][j] = 1.0;
		  B[i][j] = 2.0;
		  C[i][j] = 0.0;
	  }
  for (int i = 0; i < 100; i++)
	  for (int j = 0; j < 100; j++)
		  for (int k = 0; k < 100; k++)
		  {
			  C[i][j] += A[i][k] * B[k][j];
		  }

//  printf("sum=%f\n", sum);
  printf("C[5][5]=%f\n", C[5][5]);

  double energy = micpower_finalize();
  printf("Energy used in %lf\n", energy);

  return 0;
}
*/

