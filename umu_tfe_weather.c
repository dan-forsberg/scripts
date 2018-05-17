#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};

#define URL                                                                    \
	"http://www8.tfe.umu.se/weatherold/"                                   \
	"weatherdata_to_csv.asp?tid=senast&antaldagar=1&medtemp=ON&vindh=ON&"  \
	"regn=ON&sol=ON"

static size_t write_memory(void *contents, size_t size, size_t nmemb,
			   void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

double real_feel(double temp, double wind)
{
	/* do as TFE, if temp is <= 10°c and wind >= 1.4 calculate RF */
	if (temp > 10 || wind < 1.4)
		return temp;

	double rf = 0;

	double w16 = pow(wind, 0.16);

	rf = 13.12667 + (0.6215 * temp) - 13.924748 * (w16) +
	     0.4875195 * temp * w16;

	return rf;
}

int data_age(char *date)
{
	strtok(date, " ");
	if (date == NULL)
		return -1;

	/* get the hour number, and check if it's AM/PM... in case of PM, add
	 * 12hrs */
	char *time_str = strtok(NULL, "M");
	char AM_PM = time_str[strlen(time_str) - 1];
	int hr = atoi(strtok(time_str, ":"));

	if (AM_PM == 'P' && hr != 12) /* the data is weird.. 12:00 = 12:00 PM */
		hr += 12;

	time_t now = time(NULL);
	struct tm *now_tm = localtime(&now);

	int diff = (now_tm->tm_hour - hr) * 60 + (now_tm->tm_min);

	return diff;
}

void parse_and_print(struct MemoryStruct data)
{
	char *html = data.memory;

	int index;
	for (int i = data.size; i > 0; i--) {
		if (html[i] == '>' && html[i - 1] == 'r' &&
		    html[i - 2] == 'b' && html[i - 3] == '<') {
			index = i + 1;
			break;
		}
	}

	char string[65];
	for (int i = 0; i < 64; i++) {
		if (html[index + i] == '\r') {
			break;
		}
		string[i] = html[index + i];
	}

	char *date = strtok(string, ";"); /* date and time */
	double temp = atof(strtok(NULL, "; "));
	double wind = atof(strtok(NULL, "; "));
	double rain = atof(strtok(NULL, "; "));
	double suns = atof(strtok(NULL, "; "));
	double rf = real_feel(temp, wind);

	int age = data_age(date);

	printf("%.1f°c ", temp);
	if (rf != temp)
		printf("(%.1f°c) ", rf);

	if (wind > 0.85)
		printf("%.2f m/s ", wind);

	if (rain > 0)
		printf("%.2f mm ", rain);

	if (suns <= 20)
		printf("dark");
	else if (suns < 50)
		printf("eve");
	else if (suns < 100)
		printf("cloudy");
	else if (suns < 200)
		printf("bright");
	else if (suns < 300)
		printf("sunny");
	else
		printf("shit...");

	printf(" (%d min)", age);
}

int main(void)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	curl_easy_setopt(curl_handle, CURLOPT_URL, URL);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	res = curl_easy_perform(curl_handle);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	} else {
		parse_and_print(chunk);
	}

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	free(chunk.memory);

	return 0;
}
