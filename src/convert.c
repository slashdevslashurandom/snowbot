#include "convert.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>

enum convtype {
	CT_MASS,
	CT_LENGTH,
	CT_AREA,
	CT_VOLUME,
	CT_SPEED,
	CT_TIME,
	CT_TEMP
};

struct convert_rate {
	enum convtype type;
	const char* symbol;
	float mult; //
	float zero; //zero point in comparison to {1.0, 0.0}
};

struct convert_rate rates[] = {
	{CT_MASS, "g", 1.0, 0.0},
	{CT_MASS, "kg", 1000.0, 0.0},
	{CT_MASS, "mg", 0.001, 0.0},
	{CT_MASS, "t", 1e6, 0.0}, //metric ton
	{CT_MASS, "oz", 28.349523, 0.0}, //ounce
	{CT_MASS, "lb", 453.59237, 0.0}, //pound
	{CT_MASS, "ton", 907184.74, 0.0}, // short ton
	{CT_MASS, "lt", 1016046.909, 0.0}, //long ton
	{CT_LENGTH, "m", 1.0, 0.0},
	{CT_LENGTH, "km", 1e3, 0.0},
	{CT_LENGTH, "cm", 0.01, 0.0},
	{CT_LENGTH, "mm", 1e-3, 0.0},
	{CT_LENGTH, "um", 1e-6, 0.0}, //micrometer
	{CT_LENGTH, "in", 0.0254, 0.0},
	{CT_LENGTH, "ft", 0.3048, 0.0},
	{CT_LENGTH, "yd", 0.9144, 0.0},
	{CT_LENGTH, "mi", 1609.344, 0.0},
	{CT_AREA, "m2", 1.0, 0.0},
	{CT_AREA, "cm2", 1e-4, 0.0},
	{CT_AREA, "km2", 1e6, 0.0},
	{CT_AREA, "ft2", 0.09290341, 0.0},
	{CT_VOLUME, "L", 1.0, 0.0},
	{CT_VOLUME, "mL", 1e-3, 0.0},
	{CT_VOLUME, "kL", 1e3, 0.0},
	{CT_VOLUME, "ML", 1e6, 0.0},
	{CT_VOLUME, "m3", 1e3, 0.0},
	{CT_VOLUME, "cm3", 1e-3, 0.0},
	{CT_VOLUME, "mm3", 1e-6, 0.0},
	{CT_VOLUME, "km3", 1e12, 0.0},
	{CT_VOLUME, "in3", 0.016387064, 0.0},
	{CT_VOLUME, "ft3", 28.31685, 0.0},
	{CT_VOLUME, "yd3", 764.554857984, 0.0},
	{CT_VOLUME, "ac.ft", 1.233482e6, 0.0},
	{CT_VOLUME, "oz", 0.0295735295625, 0.0},
	{CT_VOLUME, "fl.oz", 0.0295735295625, 0.0},
	{CT_VOLUME, "tsp", 4.92892159375e-3, 0.0},
	{CT_VOLUME, "Tbsp", 14.78676478125e-3, 0.0},
	{CT_VOLUME, "gal", 3.785411784, 0.0},
	{CT_VOLUME, "l.bbl", 119.240471196, 0.0},
	{CT_VOLUME, "bbl", 158.987294928, 0.0},
	{CT_SPEED, "m/s", 1.0, 0.0},
	{CT_SPEED, "km/h", 1/3.6, 0.0},
	{CT_SPEED, "mph",  0.44704, 0.0},
	{CT_SPEED, "knot", 0.514444, 0.0},
	{CT_SPEED, "ft/s", 0.3048, 0.0},
	{CT_SPEED, "c", 299792458.0, 0.0}, //exactly!
	{CT_SPEED, "mach", 343.2, 0.0}, //speed of sound
	{CT_TIME, "s", 1.0, 0.0},
	{CT_TIME, "ms", 0.001, 0.0},
	{CT_TIME, "us", 1e-6, 0.0},
	{CT_TIME, "m", 60, 0.0},
	{CT_TIME, "min", 60, 0.0},
	{CT_TIME, "h", 3600, 0.0},
	{CT_TIME, "hr", 3600, 0.0},
	{CT_TIME, "d", 24*3600, 0.0},
	{CT_TIME, "day", 24*3600, 0.0},
	{CT_TIME, "yr", 31557600, 0.0}, //julian year, 365.25 days
	{CT_TEMP, "C", 1.0, 0.0}, //celsius
	{CT_TEMP, "F", 5/9.0, 32.0}, //fahrenheit
	{CT_TEMP, "K", 1.0, 273.15}, //kelvin	
};

const int rate_c = sizeof(rates) / sizeof(*rates);

unsigned int measure_type_count (const char* measure, uint64_t* mask) {

	unsigned int measure_c = 0;
	uint64_t measure_m = 0;

	for (int i = 0; i < rate_c; i++)
		if (strcasecmp(measure,rates[i].symbol) == 0) {
			measure_c++;
			measure_m |= (1 << rates[i].type);
		}

	if (mask) *mask = measure_m;
	return measure_c;
}

struct convert_rate* find_rate (const char* measure, enum convtype type) {

	for (int i = 0; i < rate_c; i++)
		if ( (strcasecmp(measure,rates[i].symbol) == 0) &&
				(rates[i].type == type) ) {
			return &rates[i];
		}

	return NULL;
}

int popcount_4(uint64_t x) {
	int count;
	for (count=0; x; count++)
		x &= x-1;
	return count;
}
//stolen from wikipedia.

int type_from_mask(uint64_t x) {

	int t = -1;
	while (x) {x >>= 1; t++;};
	return t;
}

float convert_value (float in_value, const char* src, const char* dest) {


	uint64_t mask1 = 0, mask2 = 0;
	unsigned int cnt1 = 0, cnt2 = 0;

	cnt1 = measure_type_count (src, &mask1);
	cnt2 = measure_type_count (dest, &mask2);

	if (!cnt1) return NAN; //couldn't find source measure
	if (!cnt2) return NAN; //couldn't find dest measure

	if ((mask1 & mask2) == 0) return NAN; //measures are incompatible

	if (popcount_4(mask1 & mask2) > 1) return NAN; //more than one compatible measure type

	enum convtype restype = type_from_mask(mask1 & mask2);

	struct convert_rate* crate1 = find_rate(src,restype);
	struct convert_rate* crate2 = find_rate(dest,restype);

	if (!crate1) return NAN; //couldn't find source measure
	if (!crate2) return NAN; //couldn't find dest measure

	return (in_value - crate1->zero) * crate1->mult / crate2->mult + crate2->zero;
};
