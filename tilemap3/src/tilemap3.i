%module tilemap3

%{

typedef struct {
  int vert_tile;
  int horiz_tile;
  double line;
  double samp;
} TilePixelCoord;

typedef struct {
  double lat;
  double lon;
} LatLonCoord;

typedef struct {
  double x;
  double y;
} GlobalMapCoord;

typedef struct {
  double line_global;
  double samp_global;
} GlobalPixelCoord;

extern TilePixelCoord To_TilePixelCoordinate(char *, char *, double *, double *);

extern LatLonCoord To_LatLonCoordinate(char *, char*, int, int, double, double);

extern GlobalMapCoord To_GlobalMapCoordinate(char *, char *, double, double);

extern GlobalPixelCoord To_GlobalPixelCoordinate(char *, char *, double, double);

%}

typedef struct {
  int vert_tile;
  int horiz_tile;
  double line;
  double samp;
} TilePixelCoord;

typedef struct {
  double lat;
  double lon;
} LatLonCoord;

typedef struct {
  double x;
  double y;
} GlobalMapCoord;

typedef struct {
  double line_global;
  double samp_global;
} GlobalPixelCoord;

extern TilePixelCoord To_TilePixelCoordinate(char *, char *, double *, double *);

extern LatLonCoord To_LatLonCoordinate(char *, char*, int, int, double, double);

extern GlobalMapCoord To_GlobalMapCoordinate(char *, char *, double, double);

extern GlobalPixelCoord To_GlobalPixelCoordinate(char *, char *, double, double);