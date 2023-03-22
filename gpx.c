#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
  char buf[BUFFER_SIZE];
  int pcount = 0;
  char *needle;
  double coords[COORDS];

  // file process start
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
  // file process end

  // waypoint start
  double total = 0;
  int wcount = 0;
  double waypoints[COORDS];
  for(int i=0; i < pcount; i+=2) {
    if (coords[i + 2] == 0.0) { break; }

    total += distance(coords[i], coords[i + 1], coords[i + 2], coords[i + 3]);
    if (total >= 80) {
      waypoints[wcount++] = coords[i + 2];
      waypoints[wcount++] = coords[i + 3];
      total = 0;
    }
  }
  // waypoint end

  for(int i=0; i<wcount; i+=2){
    printf("<wpt lat='%f' lon='%f'>\n", waypoints[i], waypoints[i + 1]);
    printf("  <name>%d km</name>\n", (i / 2 + 1) * 80);
    printf("  <desc>%0.1f mi</desc>\n", (0.621371 * (i / 2 + 1) * 80));
    printf("</wpt>\n");
  }

  return 0;
}
