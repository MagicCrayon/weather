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
 *
 * TODO
 * - Make sure input city is only 4 chars
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

/* Function Prototypes */
void usage();
int showWeather(char *);

int main(int argc, char* argv[])
{
    char url[] = "http://weather.noaa.gov/pub/data/observations/metar/decoded/XXXX.TXT";
    char *city = "CYYZ";                       /* Show This Citys Weather  */
    char cityArray[3] = {0};                          /* Used When Appending To url[] */
    int sflag, hflag = 0;                       /* Set Flags                */
    char *svalue = NULL;                      /* Hold Prefered City       */
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
                city = optarg;
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
    
/*  * Replace XXXX in the url *
    const char *url = "http://weather.noaa.gov/pub/data/observations/metar/decoded/XXXX.TXT";
    char urlArray[strlen(url)];
    char *city = "CYYZ";                       
    char cityArray[4];                          

    
    strcpy(urlArray, url); 
    strcpy(cityArray, city);
    
                 
    urlArray[60] = cityArray[0];
    urlArray[61] = cityArray[1];
    urlArray[62] = cityArray[2];
    urlArray[63] = cityArray[3];
     
    printf("\ncityArray: %s\n", cityArray);
    printf("\nURL:%s\n", urlArray);     
*/

    return 0;
}

void usage()
{
    printf("Usage: ./main <argument> <paraneter>\n");
    printf("-h  -   Show Help\n");
    printf("-s  -   Set City \n");
}

/*
 * Good Enough For Now ;)
 */
int showWeather(char *cityname)
{
    printf("%s\n", cityname);
    printf("Oct 18, 2015 - 08:00 PM\n");
    printf("Temperature: -2 C\n");
    printf("Dew point: -4 C\n");
    printf("Sky conditions: mostly clear\n");
    return 1;
}
