/*
 * main.c
 *
 * TODO
 * - [FIXME]Segmentation fault (core dumped) when running program and
 *   the tmp file does not exist
 *   ex: rm /tmp/weather.tmp
 *       ./main
 *       core dump
 * - [FIXME] Output garbage using printInfo()
 *   - Output changes when array amount changes.
 * 
 * - Change paramters for getFunctions to accept the struct
 *
 * - With -s argument change default ICAO, maybe in some note it down
 *   in a  file.
 *
 * - Fix dangrous code
 *
 * - Add header file
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <curl/curl.h>

/* Function Prototypes */
void usage();
int getWeather(char*);
int getLine(FILE*);
void getICAO();
void getTime();
void getTemp();
void getSky();
void printInfo();

/*
 * Structures
 */
struct weatherData
{
    char ICAO[5];
    char time[100];
    char temperature[100];
    char dewPoint[100];
    char sky[100];
};

struct MemoryStruct
{
    char *memory;
    size_t size;
};


/* Globals */
char weatherGlobalInfo[20][256] = {0};

int main(int argc, char* argv[])
{
    char *city = "CYYZ";                       
    int hflag = 0;                            
    int c;

    opterr = 0;

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

    if ( hflag == 1)
    {
        usage();
        exit(1);
    }


    FILE* tempfile = fopen("/tmp/weather.tmp", "r");
    
    getWeather(city);
    
    getLine(tempfile);

    getICAO();

    getTime();

    getTemp();

    getSky();

    //printInfo();   
    
    fclose(tempfile);

    return 0;
}

void usage() 
{
    printf("Usage: ./main <argument> <paraneter>\n");
    printf("-h  -   Show Help\n");
    printf("-s  -   Set City \n");
}

/*
 * The following function came from a cURL example.
 * http://curl.haxx.se/libcurl/c/getinmemory.html
 */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{   
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL)
    {
        /* out of memory */
        printf("not enough memory (realloc returned NULL\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;

}

/*
 * Weather data information in the temp dataFile
 * is copyed from it into globalArray (2D array). 
 */
int getLine(FILE* dataFile)
{
    char line[256];
    int currentline = 0;

    while ( fgets(line, sizeof(line), dataFile) )
    {
        strcpy(weatherGlobalInfo[currentline], line);
        currentline++;
    }

    return 0;

}

/*
 * Get weather information from the website, 
 * send that information into a block of memory.
 * After write that data to a temp file.
 */
int getWeather(char* cityCode)
{
     const char *url = "http://weather.noaa.gov/pub/data/observations/metar/decoded/XXXX.TXT";
     char urlArray[strlen(url)];
     char cityArray[4];

     CURL *curl_handle;
     CURLcode res;

     struct MemoryStruct chunk;
     chunk.memory = malloc(1);
     chunk.size = 0;

     /*
      * Replace XXXX with ICAO
      */

    strcpy(urlArray, url);
    strcpy(cityArray, cityCode);

    urlArray[60] = cityArray[0];
    urlArray[61] = cityArray[1];
    urlArray[62] = cityArray[2];
    urlArray[63] = cityArray[3];

    /*
     * Retrieve data
     */

    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, urlArray);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else
    {
        /*
         * Write data to temp file
         */

        FILE* dataFile = fopen("/tmp/weather.tmp", "w");
        if (dataFile == NULL)
        {
            fprintf(stderr, "Error: Opening Temp File Failed");
            exit(-1);
        }

        fwrite(chunk.memory, chunk.size, sizeof(chunk.memory), dataFile);
        fclose(dataFile);
    }

    curl_easy_cleanup(curl_handle);

    free(chunk.memory);

    curl_global_cleanup();
    
    return 0;

}

void getICAO()
{
    struct weatherData data;
    size_t length = strlen( (char *)weatherGlobalInfo);

    for (int count = 0; count <= length; count++)
    {
        if (weatherGlobalInfo[0][count] == 40)
        {
            char ICAO[5] = {0};
            ICAO[0] = weatherGlobalInfo[0][count+1];
            ICAO[1] = weatherGlobalInfo[0][count+2];
            ICAO[2] = weatherGlobalInfo[0][count+3];
            ICAO[3] = weatherGlobalInfo[0][count+4];
            ICAO[4] = '\0';
            strcpy(data.ICAO, ICAO);
        }
    }
    printf("%s\n", data.ICAO);
}

void getTime()
{
    struct weatherData data;
    char time[28] = {0};
    strncpy(time, weatherGlobalInfo[1], 27);
    strcpy(data.time, time);

    printf("%s\n", data.time);
}

void getTemp()
{
    struct weatherData data;
    size_t length = strlen(weatherGlobalInfo[6]);

    for (int count = 0; count <= length; count++)
    {
        if (weatherGlobalInfo[6][count] == 40)
        {
            char temperature[2] = {0};
            temperature[0] = weatherGlobalInfo[6][count+1];
            temperature[1] = weatherGlobalInfo[6][count+3]; // count+2 = " "
            temperature[2] = '\0';
            strcpy(data.temperature, temperature);
        }
    }

    printf("Temperature: %s\n", data.temperature);
}

void getSky()
{
    struct weatherData data;
    strcpy(data.sky, weatherGlobalInfo[4]);

    printf("%s", data.sky);
}

void printInfo()
{
    struct weatherData data;

    printf("%s\n", data.ICAO);
    printf("%s\n", data.time);
    printf("Temperature: %s\n", data.temperature);
    printf("%s\n", data.sky);
}



