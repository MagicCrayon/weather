/*
 * main.c
 *
 * weather info from. http://weather.noaa.gov/pub/data/observations/metar/decoded/CYYZ.TXT
 *
 * ./weather -s cyyz
 *
 * ./weather -h
 * SHOW USAGE
 *
 * ./weather
 * CYYZ
 * Oct 18, 2015 - 08:00 PM 
 * Temperature: -2 C
 * Dew Point: -4 C
 * Sky conditions: mostly clear
 *2
 * TODO
 * - Make sure ICAO is only 4 chars
 * - Write the rest of the program
 *
 * - Get Input From Parameters on which city need to show, set one by defualt
 * - On no params just print out weather of specfied city
 *      - 
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <curl/curl.h>

/* Function Prototypes */
void usage();
int showWeather(char *);
//void curlWrite(void, size_t, size_t, void);

/* 
 * Information parsed from the weather site
 * */
typedef struct info 
{
    char *ICAO;
    char *date;
    char *temp;
    char *dewPoint;
    char *skyCond;
} info;

int main(int argc, char* argv[])
{
    char *city = "CYYZ";                       /* Show This Citys Weather  */
    int hflag = 0;                            /* Set Flags                */
    int c;

    opterr = 0;

    /*
     * Handle Arguments
     */
    while ((c = getopt (argc, argv, "hs:")) != -1)
    {
        switch(c)
        {
            case 'h':
                hflag = 1;
                break;
            case 's':
                if ( strlen(optarg) == 4)
                    city = optarg;
                else
                {
                    printf("Code Invalid\n");
                    exit(-1);
                }
                break;
            case '?':
                if (optopt == 's')
                    fprintf(stderr, "Option -%c requires an argument. \n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                return 1;
            default:
                exit(-1);
        }
    }


    /* Usage */
    if ( hflag == 1)
    {
        usage();
        exit(1);
    }

    showWeather(city);

    return 0;
}

void usage()
{
    printf("Usage: ./main <argument> <paraneter>\n");
    printf("-h  -   Show Help\n");
    printf("-s  -   Set City \n");
}

void curlWrite(void *contents, size_t size, size_t nmemb, void *userp)
{
    printf("FEFEEFE=============================================\n");
    printf("%s", contents);
}

int showWeather(char *cityname)
{

    CURL *curl_handel;
    CURLcode res;

    /*
     * Replace XXXX with ICAO code
     */
    const char *url = "http://weather.noaa.gov/pub/data/observations/metar/decoded/XXXX.TXT";
    char urlArray[strlen(url)];
    char *city = cityname;                       
    char cityArray[4];                          

    
    strcpy(urlArray, url); 
    strcpy(cityArray, city);
    
                 
    urlArray[60] = cityArray[0];
    urlArray[61] = cityArray[1];
    urlArray[62] = cityArray[2];
    urlArray[63] = cityArray[3];
     
    //printf("\ncityArray: %s\n", cityArray);
    //printf("\nURL:%s\n", urlArray);

    /*
     * Using cURL get information
     */
    curl_handel = curl_easy_init();

    if(curl_handel)
    {
        curl_easy_setopt(curl_handel, CURLOPT_URL, urlArray);
        curl_easy_setopt(curl_handel, CURLOPT_WRITEFUNCTION, curlWrite);
        res = curl_easy_perform(curl_handel); /* Output It */
        curl_easy_cleanup(curl_handel);
    }


    return 1;
}
