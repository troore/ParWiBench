/*
   K20Power v1.0: This code profiles the power sensor of K20 and K40 GPUs, corrects
   the power profile, and outputs the original and corrected profiles as well as
   the energy used.  See http://cs.txstate.edu/~mb92/papers/gpgpu14.pdf for more
   detail.

   Copyright (c) 2014, Texas State University. All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted for academic, research, experimental, or personal use provided
   that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Texas State University-San Marcos nor the names of its
 contributors may be used to endorse or promote products derived from this
 software without specific prior written permission.

 For all other uses, please contact the Office for Commercialization and Industry
 Relations at Texas State University-San Marcos <http://www.txstate.edu/ocir/>.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher <burtscher@txstate.edu>
 */


#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include "nvml.h"

#define DEVICE 0  /* may need to be changed */

#define NEAR_IDLE_DELTA 500  /* mW */
#define IDLE_DELTA 250  /* mW */
#define SAMPLE_DELAY 14000  /* usec */
#define RAMP_DELAY 4000000  /* usec */
#define TIME_OUT 30000000  /* usec */
#define STABLE_COUNT 5  /* sec */

#define power2watts 0.001  /* mW -> W */
#define time2seconds 0.000001  /* usec -> sec */
#define capacitance 840000.0  /* usec */
#define ACTIVE_IDLE 55  /* W */
#define SAMPLES (1024*1024)  /* 4.3 hours */

static int samples = 0;
static int p_sample[SAMPLES];  /* power */
static long long t_sample[SAMPLES];  /* time */
static double truepower[SAMPLES];  /* true power */
static double max_power;  /* power cap in W */


static nvmlDevice_t initAndTest()
{
	nvmlReturn_t result;
	nvmlDevice_t device;
	int power;

	result = nvmlInit();
	if (NVML_SUCCESS != result) {
		printf("failed to initialize NVML: %s\n", nvmlErrorString(result));
		exit(-1);
	}

	result = nvmlDeviceGetHandleByIndex(DEVICE, &device);
	if (NVML_SUCCESS != result) {
		printf("failed to get handle for device: %s\n", nvmlErrorString(result));
		exit(-1);
	}

	result = nvmlDeviceGetPowerUsage(device, (unsigned int *)&power);
	if (NVML_SUCCESS != result) {
		printf("failed to read power: %s\n", nvmlErrorString(result));
		exit(-1);
	}

	result = nvmlDeviceGetPowerManagementLimit(device, (unsigned int *)&power);
	if (NVML_SUCCESS != result) {
		printf("failed to read power limit: %s\n", nvmlErrorString(result));
		exit(-1);
	}
	max_power = power * power2watts;

	return device;
}


static inline long long getTime()  /* usec */
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000000 + time.tv_usec;
}


static void getSample(nvmlDevice_t device, int *power, long long *time)  /* mW usec */
{
	nvmlReturn_t result;
	int samplepower;
	static long long sampletime = LONG_LONG_MIN;

	sampletime += SAMPLE_DELAY;
	do {} while (getTime() < sampletime);
	result = nvmlDeviceGetPowerUsage(device, (unsigned int *)&samplepower);
	sampletime = getTime();

	if (NVML_SUCCESS != result) {
		printf("failed to read power: %s\n", nvmlErrorString(result));
		exit(-1);
	}

	p_sample[samples] = samplepower;
	t_sample[samples] = sampletime;
	samples++;
	if (samples >= SAMPLES) {
		printf("out of memory for storing samples\n");
		exit(-1);
	}

	if (samples >= 3) {
		int s = samples - 2;
		double tp = (p_sample[s] + capacitance * (p_sample[s + 1] - p_sample[s - 1]) / (t_sample[s + 1] - t_sample[s - 1])) * power2watts;
		if (tp < 0.0) tp = 0.0;
		if (tp > max_power) tp = max_power;
		truepower[s] = tp;
	}

	*power = samplepower;
	*time = sampletime;
}


int main(int argc, char *argv[])
{
	int i, count, active_samples;
	nvmlDevice_t device;
	int power, prevpower, nearidlepower, diff;
	long long time, timeout, endtime;
	double activetime, activeenergy, mindt;
	FILE *f;
	char hostname[1024];
	char filename[1100];

	printf("K20Power 1.0\n");

	if (argc < 2) {
		printf("usage: %s command_line\n", argv[0]);
		exit(-1);
	}

	hostname[0] = 0;
	gethostname(hostname, 1023);
	hostname[1023] = 0;

	sprintf(filename, "K20Power_%s_%ld.trace", hostname, getTime());
	f = fopen(filename, "wt");
	fprintf(f, "K20Power 1.0\t#version\n");
	fprintf(f, "%s\t#hostname\n", hostname);
	for (i = 1; i < argc; i++) {
		fprintf(f, "%s ", argv[i]);
	}
	fprintf(f, "\t#command line\n\n");

	device = initAndTest();

	getSample(device, &power, &time);
	timeout = time + TIME_OUT;
	count = 0;
	do {
		prevpower = power;
		sleep(1);
		getSample(device, &power, &time);
		count++;
		diff = power - prevpower;
		if (diff < 0) diff = -diff;
		if (diff >= IDLE_DELTA) count = 0;
	} while ((count < STABLE_COUNT) && (time < timeout));

	if (time >= timeout) {
		printf("timed out waiting for idle power to stabilize\n");
		exit(-1);
	}

	samples = 0;
	getSample(device, &power, &time);
	endtime = time + RAMP_DELAY;
	do {
		getSample(device, &power, &time);
	} while (time < endtime);
	nearidlepower = power + NEAR_IDLE_DELTA;

	pid_t res = fork();
	if (res < 0) {
		printf("could not fork child\n");
		exit(-1);
	}
	if (res == 0) {
		execvp(argv[1], &argv[1]);
		printf("execvp returned unexpectedly");
		exit(-1);
	}

	getSample(device, &power, &time);
	timeout = time + TIME_OUT;
	do {
		getSample(device, &power, &time);
		if (power > nearidlepower) {
			timeout = time + TIME_OUT;
		}
	} while (time < timeout);

	getSample(device, &power, &time);
	getSample(device, &power, &time);

	samples--;
	active_samples = 0;
	activetime = 0.0;
	activeenergy = 0.0;
	mindt = TIME_OUT;
	for (i = 1; i < samples; i++) {
		if (truepower[i] > ACTIVE_IDLE) {
			active_samples++;
			double dt = (t_sample[i] - t_sample[i - 1]) * time2seconds;
			if (mindt > dt) mindt = dt;
			activetime += dt;
			activeenergy += dt * truepower[i];
		}
	}

	fprintf(f, "%.4f\t#active time [s]\n", activetime);
	fprintf(f, "%.4f\t#active energy [J]\n", activeenergy);
	if ((activetime - 5e-7) > 0.0) 
	{
		fprintf(f, "%.4f\t#power [W]\n", activeenergy / activetime);
	}
	else
	{
		fprintf(f, "No Energy, No Power.\n");
	}

	fprintf(f, "\ntime [s]\tpower [W]\ttrue power [W]\n");
	for (i = 1; i < samples; i++) {
		fprintf(f, "%.6f\t%.3f\t%.3f\n", (t_sample[i] - t_sample[1]) * time2seconds, p_sample[i] * power2watts, truepower[i]);
	}
	fclose(f);

	nvmlShutdown();
	return 0;
}
