// vim:cin:sts=4:sw=4
#include "irc_commands_weather.h"
#include "irc_commands.h"

#include <string.h>
#include <math.h>
#include <time.h>

#include "irc_user.h"
#include "weather.h"
const char* wmode_desc( int wmode) {

    switch(wmode) {
	case WM_CELSIUS: return "°C";
	case WM_FAHRENHEIT: return "°F";
	case WM_KELVIN: return "°K";
	default: return "°K";
    }
}
float convert_temp(float temp_k, int wmode) {

    switch(wmode) {
	case WM_CELSIUS: return (temp_k - 273.15f); 
	case WM_FAHRENHEIT: return (((temp_k - 273.15f)*1.8f)+32.0f);
	case WM_KELVIN: return temp_k; 
	default: return temp_k;
    }
}

void fill_format_temp(float temp_k, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (temp_k >= (273.15f + 30.0f)) { *fmtst = "\00300,04"; return;}
    if (temp_k >= (273.15f + 25.0f)) { *fmtst = "\00304"; return;}
    if (temp_k >= (273.15f + 20.0f)) { *fmtst = "\00307"; return;}
    if (temp_k >= (273.15f + 15.0f)) { *fmtst = "\00308"; return;}
    if (temp_k >= (273.15f + 10.0f)) { *fmtst = "\00309"; return;}
    if (temp_k >= (273.15f + 5.0f))  { *fmtst = "\00311"; return;}
    if (temp_k >= (273.15f - 5.0f))  { *fmtst = "\00312"; return;}
    if (temp_k >= (273.15f - 15.0f)) { *fmtst = "\00302"; return;}
    if (temp_k < (273.15f - 15.0f))  { *fmtst = "\00300,02"; return;}
    return;
};

void fill_format_temp_dark (float temp_k, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (temp_k >= (273.15f + 30.0f)) { *fmtst = "\00300,05"; return;}
    if (temp_k >= (273.15f + 25.0f)) { *fmtst = "\00305"; return;}
    if (temp_k >= (273.15f + 20.0f)) { *fmtst = "\00304"; return;}
    if (temp_k >= (273.15f + 15.0f)) { *fmtst = "\00307"; return;}
    if (temp_k >= (273.15f + 10.0f)) { *fmtst = "\00303"; return;}
    if (temp_k >= (273.15f + 5.0f))  { *fmtst = "\00310"; return;}
    if (temp_k >= (273.15f - 5.0f))  { *fmtst = "\00302"; return;}
    if (temp_k >= (273.15f - 15.0f)) { *fmtst = "\00306"; return;}
    if (temp_k < (273.15f - 15.0f))  { *fmtst = "\00300,02"; return;}
    return;
};

void fill_format_temp_mono (float temp_k, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (temp_k >= (273.15f + 30.0f)) { *fmtst = "\026"; return;}
    if (temp_k >= (273.15f + 25.0f)) { *fmtst = "\002"; return;}
    if (temp_k >= (273.15f + 20.0f)) { *fmtst = "\002"; return;}
    if (temp_k >= (273.15f + 15.0f)) { *fmtst = ""; return;}
    if (temp_k >= (273.15f + 10.0f)) { *fmtst = ""; return;}
    if (temp_k >= (273.15f + 5.0f))  { *fmtst = ""; return;}
    if (temp_k >= (273.15f - 5.0f))  { *fmtst = "\002"; return;}
    if (temp_k >= (273.15f - 15.0f)) { *fmtst = "\002"; return;}
    if (temp_k < (273.15f - 15.0f))  { *fmtst = "\026"; return;}
    return;
};

void fill_format_wind(float wspd_ms, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (wspd_ms >= 30.0f) { *fmtst = "\00301,04"; return; }
    if (wspd_ms >= 25.0f) { *fmtst = "\00304"; return; }
    if (wspd_ms >= 20.0f) { *fmtst = "\00307"; return; }
    if (wspd_ms >= 15.0f) { *fmtst = "\00308"; return; }
    if (wspd_ms >= 10.0f) { *fmtst = "\00309"; return; }
    if (wspd_ms >= 5.0f)  { *fmtst = "\00311"; return; }

    return;
};

void fill_format_wind_dark(float wspd_ms, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (wspd_ms >= 30.0f) { *fmtst = "\00301,05"; return; }
    if (wspd_ms >= 25.0f) { *fmtst = "\00305"; return; }
    if (wspd_ms >= 20.0f) { *fmtst = "\00304"; return; }
    if (wspd_ms >= 15.0f) { *fmtst = "\00303"; return; }
    if (wspd_ms >= 10.0f) { *fmtst = "\00302"; return; }
    if (wspd_ms >= 5.0f)  { *fmtst = "\00312"; return; }

    return;
};

void fill_format_wind_mono(float wspd_ms, char** fmtst, char** fmted) {

    *fmted = "\017";
    if (wspd_ms >= 30.0f) { *fmtst = "\026"; return; }
    if (wspd_ms >= 25.0f) { *fmtst = "\026"; return; }
    if (wspd_ms >= 20.0f) { *fmtst = "\002"; return; }
    if (wspd_ms >= 15.0f) { *fmtst = "\002"; return; }
    if (wspd_ms >= 10.0f) { *fmtst = ""; return; }
    if (wspd_ms >= 5.0f)  { *fmtst = ""; return; }

    return;
};

const char* format_on(const struct weather_id* wid, struct irc_user_params* up) {

    switch(up->color_scheme) {
	case 0: return wid->format_on;
	case 1: return wid->format_on_d;
	case 2: return wid->format_on_m;
	default: return wid->format_on;
    }
    return NULL;
}

void format_temp(float temp_k, struct irc_user_params* up, char** fmtst, char** fmted) {

    switch(up->color_scheme) {
	case 0: return fill_format_temp(temp_k,fmtst,fmted);
	case 1: return fill_format_temp_dark(temp_k,fmtst,fmted);
	case 2: return fill_format_temp_mono(temp_k,fmtst,fmted);
	default: return fill_format_temp(temp_k,fmtst,fmted);
    }
}

void format_wind(float wspd_ms, struct irc_user_params* up, char** fmtst, char** fmted) {

    switch(up->color_scheme) {
	case 0: return fill_format_wind(wspd_ms,fmtst,fmted);
	case 1: return fill_format_wind_dark(wspd_ms,fmtst,fmted);
	case 2: return fill_format_wind_mono(wspd_ms,fmtst,fmted);
	default: return fill_format_wind(wspd_ms,fmtst,fmted);
    }
}

int handle_weather_current(irc_session_t* session, const char* restrict nick, const char* restrict channel, struct weather_loc* wloc, struct weather_data* wdata) {
    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    if (wdata->cod != 200) {
	char errormsg[256];
	snprintf(errormsg,256,"Sorry, but OWM returned no results for your location. (Error %d)", wdata->cod);
	respond(session,nick,channel,errormsg);
	return 0;
    }

    char* weathermsg = malloc(128);

    char weathertmp[256];

    if (wloc->city_id) {
	snprintf (weathermsg,1024,"weather in %s (#%d)",wloc->city_name,wloc->city_id);
	if (strlen(wloc->sys_country) == 2) {
	    snprintf (weathertmp,256,",%2s",wloc->sys_country);
	    weathermsg = strrecat(weathermsg,weathertmp);
	}
    } else {
	snprintf (weathermsg,1024,"weather in your location");
    }


    weathermsg = strrecat(weathermsg,": ");

    for (int i=0; i < (wdata->weather_c); i++) {
	const struct weather_id* wid = getwid(wdata->weather_id[i]);
	if (wid->format_on)
	    snprintf(weathertmp,256,"%s%s%s, ",format_on(wid,up),wid->description,wid->format_off); else
		snprintf(weathertmp,256,"%s, ",wid->description);
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    char* fmtst = ""; char* fmted = "";

    format_temp(wdata->main_temp, up, &fmtst, &fmted);

    enum weather_modes second_mode;
    if (up->wmode == WM_CELSIUS) second_mode = WM_FAHRENHEIT;
    if (up->wmode == WM_FAHRENHEIT) second_mode = WM_CELSIUS;
    if (up->wmode == WM_KELVIN) second_mode = WM_KELVIN;

    snprintf (weathertmp,255,"%s%+.1f%s%s",fmtst,convert_temp(wdata->main_temp,up->wmode),wmode_desc(up->wmode),fmted);

    if (second_mode != up->wmode) {
    
	weathermsg = strrecat(weathermsg,weathertmp);
	
	snprintf (weathertmp,255,"/%s%+.1f%s%s",fmtst,convert_temp(wdata->main_temp,second_mode),wmode_desc(second_mode),fmted);
    }

    weathermsg = strrecat(weathermsg,weathertmp);

    if (fabsf(wdata->main_temp_max - wdata->main_temp_min > 0.5f)) {

	snprintf(weathertmp,255," (%+.1f° .. %+.1f°)",convert_temp(wdata->main_temp_min,up->wmode),convert_temp(wdata->main_temp_max,up->wmode));
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    if (wdata->main_pressure >= 0.0) {
	snprintf(weathertmp,255,", pressure: %.1f hPa", wdata->main_pressure);
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    if (wdata->main_humidity >= 0) {
	snprintf(weathertmp,255,", %d%% humidity", wdata->main_humidity);
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    if (wdata->wind_speed >= 0.0) {

	int wspd = (int) round(wdata->wind_speed);

	char* fmtst = ""; char* fmted = "";

	format_wind(wspd, up, &fmtst, &fmted);

	snprintf(weathertmp,255,", wind: %s%.1f%s m/s (%s%.1f%s mph)", fmtst,wdata->wind_speed,fmted, fmtst,wdata->wind_speed * 2.23694f,fmted);
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    if (wdata->wind_deg >= 0.0) {
	snprintf(weathertmp,255," %s", describe_wind_direction(wdata->wind_deg));
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    respond(session,nick,channel,weathermsg);
    free(weathermsg);
    return 0;
}
int handle_long_forecast(irc_session_t* session, const char* restrict nick, const char* restrict channel, struct weather_loc* wloc, struct forecast_data* wdata, int cnt) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    if (wdata[0].cod != 200) {
	char errormsg[256];
	snprintf(errormsg,256,"Sorry, but OWM returned no results for your location. (Error %d)", wdata->cod);
	respond(session,nick,channel,errormsg);
	return 0;
    }

    //if (!(wloc[0].city_id)) {
    //	respond(session,nick,channel,"Sorry, but OWM returned no results for your location.");
    //	return 0;
    //    }

    while (wdata[cnt-1].temp_day < 1.0) cnt--; //avoid -273

    char* weathermsg = malloc(128);

    char weathertmp[512];

    snprintf (weathermsg,128,"\021 day #:");

    char weathertmp2[32];
    memset(weathertmp,0,sizeof weathertmp);

    struct tm weathertime;
    memset(&weathertime,0,sizeof weathertime);

    for (int i=0; i<cnt; i++) {

	gmtime_r(&((wdata+i)->dt), &weathertime);

	char* daycolor = "";
	if (weathertime.tm_wday == 6) daycolor = "\00312";
	else if (weathertime.tm_wday == 0) daycolor = "\00304";
	snprintf(weathertmp2,16,"%s%3d \017",daycolor,weathertime.tm_mday);
	strcat(weathertmp,weathertmp2);
    }

    weathermsg = strrecat(weathermsg,weathertmp);

    respond(session,nick,channel,weathermsg);
    
    char* fmtst = ""; char* fmted = "";

    snprintf (weathermsg,128,"\021day %s:",wmode_desc(up->wmode));

    memset(weathertmp,0,sizeof weathertmp);
    for (int i=0; i<cnt; i++) {
	
	format_temp(wdata[i].temp_day, up, &fmtst, &fmted);
	snprintf(weathertmp2,16,"%s%3d %s",fmtst,(int)round(convert_temp((wdata+i)->temp_day,up->wmode)),fmted);
	strcat(weathertmp,weathertmp2);
    }
    weathermsg = strrecat(weathermsg,weathertmp);
    respond(session,nick,channel,weathermsg);

    snprintf (weathermsg,128,"\021nite%s:",wmode_desc(up->wmode));

    memset(weathertmp,0,sizeof weathertmp);
    for (int i=0; i<cnt; i++) {
	format_temp(wdata[i].temp_night, up, &fmtst, &fmted);
	snprintf(weathertmp2,16,"%s%3d %s",fmtst,(int)round(convert_temp((wdata+i)->temp_night,up->wmode)),fmted);
	strcat(weathertmp,weathertmp2);
    }
    weathermsg = strrecat(weathermsg,weathertmp);
    respond(session,nick,channel,weathermsg);

    snprintf (weathermsg,128,"\021  wind:");

    for (int i=0; i<cnt; i++) {

	int wspd = (int) round((wdata+i)->wind_speed);

	char* fmtst = ""; char* fmted = "";
	format_wind(wspd, up, &fmtst, &fmted);

	snprintf(weathertmp,255,"%s%3.0f %s", fmtst,(wdata+i)->wind_speed,fmted);
	weathermsg = strrecat(weathermsg,weathertmp);
    }

    respond(session,nick,channel,weathermsg);

    int wcnt = 0;
    for (int i=0; i<cnt; i++)
	if ((wdata+i)->weather_c > wcnt) wcnt = (wdata+i)->weather_c;

    for (int c=0; c < wcnt; c++) {

	snprintf (weathermsg,128,"\021status:");

	memset(weathertmp,0,sizeof weathertmp);
	//char weathertmp3[16];
	for (int i=0; i<cnt; i++) {

	    const struct weather_id* wid = getwid((wdata+i)->weather_id[c]);

	    if (wid->format_on)
		snprintf(weathertmp2,16," %s%s%s ",format_on(wid,up),wid->symbol,wid->format_off); else
		    snprintf(weathertmp2,16," %s ",wid->symbol);
	    strcat(weathertmp,weathertmp2);
	}
	weathermsg = strrecat(weathermsg,weathertmp);
	respond(session,nick,channel,weathermsg);
    }
    free(weathermsg);
    return 0;
}
int handle_weather_forecast(irc_session_t* session, const char* restrict nick, const char* restrict channel, struct weather_loc* wloc, struct weather_data* wdata, int cnt) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    if (wdata[0].cod != 200) {
	char errormsg[256];
	snprintf(errormsg,256,"Sorry, but OWM returned no results for your location. (Error %d)", wdata->cod);
	respond(session,nick,channel,errormsg);
	return 0;
    }

    while (wdata[cnt-1].main_temp < 1.0) cnt--; //avoid -273

    char* weathermsg = malloc(128);

    char weathertmp[512];

    snprintf (weathermsg,128,"\021hr:");

    char weathertmp2[32];
    memset(weathertmp,0,sizeof weathertmp);

    struct tm weathertime;
    memset(&weathertime,0,sizeof weathertime);

    for (int i=0; i<cnt; i++) {

	time_t x = wdata[i].dt + wloc->timezone;

	gmtime_r(&x, &weathertime);
	snprintf(weathertmp2,16,"%3d",weathertime.tm_hour);
	strcat(weathertmp,weathertmp2);
    }

    weathermsg = strrecat(weathermsg,weathertmp);

    respond(session,nick,channel,weathermsg);

    snprintf (weathermsg,128,"\021%s:",wmode_desc(up->wmode));

    memset(weathertmp,0,sizeof weathertmp);
    char* fmtst = ""; char* fmted = "";

    for (int i=0; i<cnt; i++) {

	format_temp(wdata[i].main_temp, up, &fmtst, &fmted);
	snprintf(weathertmp2,16,"%s%3d%s",fmtst,(int)round(convert_temp((wdata+i)->main_temp,up->wmode)),fmted);
	strcat(weathertmp,weathertmp2);
    }
    weathermsg = strrecat(weathermsg,weathertmp);
    respond(session,nick,channel,weathermsg);


    snprintf(weathermsg,128,"\021wd:");

    memset(weathertmp,0,sizeof weathertmp);
    for (int i=0; i<cnt; i++) {

	int wspd = (int) round((wdata+i)->wind_speed);

	char* fmtst = ""; char* fmted = "";

	format_wind(wspd, up, &fmtst, &fmted);

	snprintf(weathertmp2,16,"%s%3d%s",fmtst,wspd,fmted);
	strcat(weathertmp,weathertmp2);
    }

    weathermsg = strrecat(weathermsg,weathertmp);
    respond(session,nick,channel,weathermsg);

    int wcnt = 0;
    for (int i=0; i<cnt; i++)
	if ((wdata+i)->weather_c > wcnt) wcnt = (wdata+i)->weather_c;

    for (int c=0; c < wcnt; c++) {

	snprintf (weathermsg,128,"\021sp:");

	memset(weathertmp,0,sizeof weathertmp);
	//char weathertmp3[16];
	for (int i=0; i<cnt; i++) {

	    const struct weather_id* wid = getwid((wdata+i)->weather_id[c]);

	    if (wid->format_on)
		snprintf(weathertmp2,16," %s%s%s",format_on(wid,up),wid->symbol,wid->format_off); else
		    snprintf(weathertmp2,16," %s",wid->symbol);

	    strcat(weathertmp,weathertmp2);
	}
	weathermsg = strrecat(weathermsg,weathertmp);
	respond(session,nick,channel,weathermsg);
    }
    return 0;
}
int handle_weather_search(irc_session_t* session, const char* restrict nick, const char* restrict channel, struct weather_loc* wloc, struct weather_data* wdata, int cnt) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    if (cnt == 0) {
	respond(session,nick,channel,"Sorry, but OWM returned no results for your location.");
	return 0;
    }

    while (wdata[cnt-1].main_temp < 1.0) cnt--; //avoid -273

    for (int i=0; i < cnt; i++) {

	char weathertmp[512];

	char* t_fmtst = ""; char* t_fmted = "";
	format_temp(wdata[i].main_temp, up, &t_fmtst, &t_fmted);

	const struct weather_id* wid = getwid(wdata[i].weather_id[0]);

	snprintf(weathertmp,512,"%d. #%-7d: %s,%s [%.2f°%c, %.2f°%c] / %s%+.1f%s%s, %s%s%s", 
		i+1, wloc[i].city_id, wloc[i].city_name, wloc[i].sys_country, 
		fabs(wloc[i].coord_lat), (wloc[i].coord_lat >= 0 ? 'N' : 'S'), //latitude
		fabs(wloc[i].coord_lon), (wloc[i].coord_lon >= 0 ? 'E' : 'W'), //longitude
		t_fmtst, convert_temp(wdata[i].main_temp,up->wmode),wmode_desc(up->wmode), t_fmted,
		wid->format_on ? ( format_on(wid,up) ) : "",
		wid->description,
		wid->format_off ? wid->format_off : "");


	respond(session,nick,NULL,weathertmp);
    }

    return 0;
}

int escape_location (char* city_name) {

    for (unsigned int i=0; i < strlen(city_name); i++)
	if (city_name[i] == ' ') city_name[i] = '+';

    return 0;
}

int load_location (int argc, const char** argv, struct irc_user_params* up, struct weather_loc* wloc) {

    //argv[0] refers to the actual first parameter, not name of command.

    if (argc == 0) return 1;

    if (argv[0][0] == '#') { //city id
	char* endid = NULL;
	wloc->city_id = strtol(argv[0]+1,&endid,10);
	if (endid != (argv[0]+1)) return 0; else return 1;
    }

    if (argv[0][0] == '@') { //city id
	strncpy(wloc->postcode,argv[0]+1,16);
	if (argc == 2) strncpy(wloc->sys_country,argv[1],2);
	return 0;
    }

    if (argc == 2) {

	float lat = -91.0f, lon = -181.0f;
	char* endlat = NULL, *endlon = NULL;

	lat = strtof(argv[0],&endlat);
	lon = strtof(argv[1],&endlon);

	if ((endlat != argv[0]) && (endlon != argv[1])) {
	    wloc->coord_lat = lat; wloc->coord_lon = lon;
	    return 0;
	}
    }


    if ( (argc == 2) && (strlen(argv[1]) == 2) ) {
	// two words, the second one is two letters long
	strncpy(wloc->city_name,argv[0],64);
   	escape_location(wloc->city_name);
	strncpy(wloc->sys_country,argv[1],2);
	return 0; 
    } else if (argc >= 2) { 
    	//two or more words, all to be interpreted as one long name
	char fullcityname[64];
	fullcityname[0]=0;
	for (int i=0; i < argc; i++) {
	    if (i>0) strncat(fullcityname, " ", 64 - strlen(fullcityname));
	    strncat(fullcityname, argv[i], 64 - strlen(fullcityname));
	}
        escape_location(fullcityname);
        strncpy(wloc->city_name,fullcityname,64);
        return 0;
    } else {
	strncpy(wloc->city_name,argv[0],64);
   	escape_location(wloc->city_name);
   	return 0;
    }
}

int weather_current_cb(irc_session_t* session, const char* restrict nick, const char* restrict channel, size_t argc, const char** argv) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    struct weather_loc wloc;
    memset(&wloc,0,sizeof wloc);
    struct weather_data wdata;
    memset(&wdata,0,sizeof wdata);

    if ((argc == 1) && (!up->cityid)) {
	respond(session,nick,channel,"Usage: .owm <location>");
	respond(session,nick,channel,"Location is one of:  #<OWM city ID>, @<zip code>[ <2char country code>], \"<city name>\"[ <2char country code>], <longitude> <latitude>"); return 0; }

    else {
	int r = load_location(argc-1, argv+1,up,&wloc);

	if (r) r = ((wloc.city_id = up->cityid) == 0);
	if (r) {respond(session,nick,channel,"Can't understand the parameters. Sorry."); return 0;}
    }

    get_current_weather( &wloc, &wdata);

    handle_weather_current(session, nick, channel, &wloc, &wdata);
    return 0;
}	
int weather_forecast_cb(irc_session_t* session, const char* restrict nick, const char* restrict channel, size_t argc, const char** argv) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    struct weather_loc wloc;
    memset(&wloc,0,sizeof wloc);

    if ((argc == 1) && (!up->cityid)) {
	respond(session,nick,channel,"Usage: .owf <# of 3-hour intervals> <location>");
	respond(session,nick,channel,"Location is one of:  #<OWM city ID>, @<zip code>[ <2char country code>], \"<city name>\"[ <2char country code>], <longitude> <latitude>"); return 0; }

    int cnt = 0;

    char* endcnt = NULL;

    int r = 1;

    if (argc > 1) 
    { cnt = strtol(argv[1],&endcnt,10);

	if (cnt < 0) cnt = 1;
	if (cnt == 0) cnt = 16;
	if (cnt>40) cnt=40;
	if (endcnt == argv[1]) cnt = 16; }

    else cnt = 16;

    struct weather_data wdata[cnt];
    memset(&wdata,0,sizeof (struct weather_data) *cnt);

    if (argc > 1) r = load_location((endcnt != argv[1]) ? argc-2 : argc-1,(endcnt != argv[1]) ? argv+2: argv+1,up,&wloc);
    if (r) r = ((wloc.city_id = up->cityid) == 0);

    if (r) respond(session,nick,channel,"Can't understand the parameters. Sorry."); else {

	cnt = get_weather_forecast( &wloc, wdata, cnt);
	handle_weather_forecast( session, nick, channel, &wloc, wdata, cnt);

    }
    return 0;
}	
int weather_longforecast_cb(irc_session_t* session, const char* restrict nick, const char* restrict channel, size_t argc, const char** argv) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    struct weather_loc wloc;
    memset(&wloc,0,sizeof wloc);

    if ((argc == 1) && (!up->cityid)) {
	respond(session,nick,channel,"Usage: .owl <# of 1-day intervals> <location>");
	respond(session,nick,channel,"Location is one of:  #<OWM city ID>, @<zip code>[ <2char country code>], \"<city name>\"[ <2char country code>], <longitude> <latitude>"); return 0; }

    int cnt = 0;

    char* endcnt = NULL;

    int r = 1;

    if (argc > 1) 
    { cnt = strtol(argv[1],&endcnt,10);

	if (cnt < 0) cnt = 1; 
	if (endcnt == argv[1]) cnt = 7; }

    if (cnt == 0) cnt = 7;
    if (cnt>15) cnt=15;

    struct forecast_data wdata[cnt];
    memset(&wdata,0,sizeof (struct forecast_data) *cnt);

    if (argc > 1) r = load_location((endcnt != argv[1]) ? argc-2 : argc-1,(endcnt != argv[1]) ? argv+2: argv+1,up,&wloc);
    if (r) r = ((wloc.city_id = up->cityid) == 0);

    if (r) respond(session,nick,channel,"Can't understand the parameters. Sorry."); else {

	cnt = get_long_forecast( &wloc, wdata, cnt);
	handle_long_forecast( session, nick, channel, &wloc, wdata, cnt);

    }
    return 0;
}	

#define MAXSEARCH 10

int weather_search_cb(irc_session_t* session, const char* restrict nick, const char* restrict channel, size_t argc, const char** argv) {

    struct irc_user_params* up = get_user_params(nick, EB_LOAD);

    struct weather_loc wloc;
    memset(&wloc,0,sizeof wloc); //input location
    struct weather_loc o_wloc[MAXSEARCH];
    memset(o_wloc,0,sizeof o_wloc); //output location
    struct weather_data o_wdata[MAXSEARCH];
    memset(o_wdata,0,sizeof o_wdata); //output data

    if ((argc == 1) && (!up->cityid)) {
	respond(session,nick,channel,"Usage: .owm_s <location>");
	respond(session,nick,channel,"Location is one of:  #<OWM city ID>, @<zip code>[ <2char country code>], \"<city name>\"[ <2char country code>], <longitude> <latitude>"); return 0; }

    else {
	int r = load_location(argc-1, argv+1,up,&wloc);

	if (r) r = ((wloc.city_id = up->cityid) == 0);
	if (r) {respond(session,nick,channel,"Can't understand the parameters. Sorry."); return 0;}
    }

    int c = search_weather( &wloc, MAXSEARCH, o_wloc, o_wdata);

    handle_weather_search(session, nick, channel, o_wloc, o_wdata, c);
    return 0;
}	
