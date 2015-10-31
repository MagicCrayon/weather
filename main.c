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
 * FIXME
 *
 * New Plan:::
 *
 * Use cURL fopen thing
 * Send the html output into the /tmp file
 * Set up a two dimensinal array
 * Read each line of the temp into the array
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
int getLine(FILE* );

//void curlWrite(void, size_t, size_t, void);

typedef struct weatherPoints
{
    char *ICAO;
    char *time;
    char *temperature;
    char *dewPoint;
    char *skyCond;
} weatherPoints;

struct MemoryStruct
{
    char *memory;
    size_t size;
};

/* Globals */
char weatherInfo[20][256] = {0};

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

    showWeather(city);

    return 0;
}

void usage() // TODO Fix the msgs in the help
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

int getLine(FILE* dataFile)
{
   
    char line[256];
    int currentline = 0;

    while (fgets(line, sizeof(line), dataFile))
    {
        strcpy(weatherInfo[currentline], line);
        currentline++;
    }
    return 1;
}

int showWeather(char *cityname)
{

    const char *url = "http://weather.noaa.gov/pub/data/observations/metar/decoded/XXXX.TXT";
    char urlArray[strlen(url)];
    char *city = cityname;                       
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
    strcpy(cityArray, city);
    
                 
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

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK)
    {
           fprintf(stderr, "curl_easy_perform() failed: %s\n", 
                curl_easy_strerror(res));
    }
    else 
    {
        //printf("%lu bytes retrieved\n", (long)chunk.size);
        //printf("%s\n", chunk.memory);

       /*
        * Write data to temp
        */

        FILE* weatherTemp = fopen("/tmp/weathertemp.tmp", "w");
        if (weatherTemp == NULL)
        {
            fprintf(stderr, "Error: Opening Temp File Failed");
            exit(-1);
        }

        fwrite(chunk.memory, chunk.size, sizeof(chunk.memory), weatherTemp);
        fclose(weatherTemp);
        
        /* 
         * Get whole data, send to temp file and then struct
         */
        
        weatherTemp = fopen("/tmp/weathertemp.tmp", "rb");
        if (weatherTemp == NULL)
        {
            fprintf(stderr, "Failed to open temp file");
            exit(-1);
        }


        struct weatherPoints data;
        getLine(weatherTemp);
    
        /*
         * Get ICAO from first line
         */
        
        size_t length = strlen(weatherInfo); 

        int count = 0;          
        while (count <= length)
        {
            /* Compare array with ascii */ 
            if (weatherInfo[0][count] == 40) 
            {
                char ICAO[5] = {0};
                ICAO[0] = weatherInfo[0][count+1]; // Maybe change to something like strcpy(str2, str1+4);
                ICAO[1] = weatherInfo[0][count+2];
                ICAO[2] = weatherInfo[0][count+3];
                ICAO[3] = weatherInfo[0][count+4];
                ICAO[4] = '\0';
                data.ICAO = ICAO;
            }
            count++;
        }

        printf("%s\n", data.ICAO);
        /*
         * Get Time from second line
         */
                
        char time[28] = {0};
        for (int i = 0; i <= 28; i++)
        {
            time[i] = weatherInfo[1][i];
        }
        time[28] = '\0';
        data.time = time; 

        /*
         * Get temp from sixth line
         */
        
        length = strlen(weatherInfo[5]);
        
        count = 0;
        while (count <= length)
        {
            if(weatherInfo[5][count] == 40)
            {
                char temperature[2] = {0};
                temperature[0] = weatherInfo[5][count+1];
                temperature[1] = weatherInfo[5][count+3];
                temperature[2] = '\0';
                data.temperature = temperature;
            }
            count++;
        }


        printf("%s\n", data.ICAO);




    } // end else

    curl_easy_cleanup(curl_handle);

    free(chunk.memory);

    curl_global_cleanup();

    return 1;
}



