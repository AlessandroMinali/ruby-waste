#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 4096
#define COORDS 32 * 1024
#define DEG2RAD 0.017453292519943295

double distance(double lat, double lng, double lat0, double lng0) {
  lat  *= DEG2RAD;
  lng  *= DEG2RAD;
  lat0 *= DEG2RAD;
  lng0 *= DEG2RAD;

  return 12742 * asin(sqrt((
    (1 - cos(lat0 - lat)) +
    (1 - cos(lng0 - lng)) * cos(lat) * cos(lat0)
  ) / 2.0));
}

void program(double *r) {
  char buf[BUFFER_SIZE];
  int pcount = 0;
  char *needle;
  double coords[COORDS];

  // file process start
  struct timespec tstart={0,0}, tend={0,0};
  clock_gettime(CLOCK_MONOTONIC, &tstart);
  FILE *f = fopen("/Users/alessandro/Desktop/gpx/CompleteBajaDivide-Reduced_V1.3(preferred).gpx", "rb");
  while(fgets(buf, BUFFER_SIZE, f)) {
    needle = strstr(buf, " lat=");
    if (needle) {
      needle[16] = '\0';
      needle[34] = '\0';
      coords[pcount++] = strtod(&needle[6], NULL);
      coords[pcount++] = strtod(&needle[22], NULL);
    }
  };
  fclose(f);
  clock_gettime(CLOCK_MONOTONIC, &tend);
  r[1] = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
  // file process end

  // waypoint start
  double total = 0;
  int wcount = 0;
  double waypoints[COORDS];
  struct timespec t2start={0,0}, t2end={0,0};
  clock_gettime(CLOCK_MONOTONIC, &t2start);
  for(int i=0; i < pcount; i+=2) {
    if (coords[i + 2] == 0.0) { break; }

    total += distance(coords[i], coords[i + 1], coords[i + 2], coords[i + 3]);
    if (total >= 80) {
      waypoints[wcount++] = coords[i + 2];
      waypoints[wcount++] = coords[i + 3];
      total = 0;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &t2end);
  r[2] = ((double)t2end.tv_sec + 1.0e-9*t2end.tv_nsec) - 
           ((double)t2start.tv_sec + 1.0e-9*t2start.tv_nsec);
  r[0] = wcount / 2;
  // waypoint end

  // for(int i=0; i<wcount; i+=2){
  //   printf("<wpt lon='%f' lat='%f'>\n", waypoints[i], waypoints[i + 1]);
  //   printf("  <name>%d km</name>\n", (i + 1) * 80);
  //   printf("  <desc>%0.1f mi</desc>\n", (0.621371 * (i + 1) * 80));
  //   printf("</wpt>\n");
  // }
}

int main() {
  unsigned int waypoints = 0;
  double pt = 999;
  double dt = 999;
  double result[3] = {0, 0, 0};

  for(int i=0; i < 100; ++i) {
    program(result);
    waypoints += result[0];
    pt = result[1] < pt ? result[1] : pt;
    dt = result[2] < dt ? result[2] : dt;
  }

  printf("Waypoints: %d\n", waypoints);
  printf("Parse time:\t%f\t| Parse point/second:\t%f\n", pt, 10000 / pt);
  printf("Waypoints time:\t%f\t| Waypoint/second:\t%f\n", dt, 33 / dt);

  return 0;
}

// Waypoints: 3300
// Parse time: 0.002281  | Parse point/second: 4384042.266776
// Waypoints time: 0.000343  | Waypoint/second:  96210.028950