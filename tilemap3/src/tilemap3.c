
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include "hdf.h"                /* HDF header files */
#include "mfhdf.h"
#include "HdfEosDef.h"
#include "PGS_GCT.h"
#include "error.h"
#include "proj.h"
#include "isinu.h"
#include "tile.h"
#include "pi.h"

/* #define DEBUG */

/* Constants */

#define HDF_ERROR (-1)

/* Type definitions */

#define NPIXEL_SIZE (7)
#define NTYPE_TILE (3)

enum {PIXEL_SIZE_RATIO_K = 1, PIXEL_SIZE_RATIO_H = 2, PIXEL_SIZE_RATIO_Q = 4, 
      PIXEL_SIZE_RATIO_S = -1, PIXEL_SIZE_RATIO_M = -2,  
      PIXEL_SIZE_RATIO_L = -3, PIXEL_SIZE_RATIO_X = -4};
enum {DIRECTION_FWD, DIRECTION_INV};
enum {TYPE_TILE_PIX, TYPE_GLOBAL_MAP, TYPE_GLOBAL_PIX};

typedef struct {
  int proj;
  int pixel_size_ratio;
  int direction;
  int type;
  char *map_name;
  double lon, lat;
  double line, samp;
  double line_global, samp_global;
  double x, y;
  int itile_line, itile_samp;
} Param_t;

typedef struct {
  double lon, lat;
  double line, samp;
  double line_global, samp_global;
  double x, y;
  int itile_line, itile_samp;
} Tile_loc;

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

/* Constants */

const Proj_t PROJ[NPROJ] = {
  {ISIN_K, PGSd_ISINUS, 1, 
   -20015109.354, 10007554.677, 926.62543305,
   -1, 6371007.181, 
   1200, 1200, 18, 36, 
   (18 * 1200), (36 * 1200),
   { 90,  540, 1080, 4320},  /* factors == 240, 40, 20, 5 */
   {180, 1080, 2160, 8640},
   {  0,    0,    0,    0},
   {  0,    0,    0,    0}},
  {ISIN_H, PGSd_ISINUS, 1, 
   -20015109.354, 10007554.677, 926.62543305,
   -1, 6371007.181, 
   1200, 1200, 18, 36, 
   (18 * 1200), (36 * 1200),
   { 90,  540, 1080, 4320},
   {180, 1080, 2160, 8640},
   {  0,    0,    0,    0},
   {  0,    0,    0,    0}},
  {ISIN_Q, PGSd_ISINUS, 1, 
   -20015109.354, 10007554.677, 926.62543305,
   -1, 6371007.181, 
   1200, 1200, 18, 36, 
   (18 * 1200), (36 * 1200),
   { 90,  540, 1080, 4320},
   {180, 1080, 2160, 8640},
   {  0,   0,    0,    0},
   {  0,   0,    0,    0}},
  {GOODE, PGSd_GOOD, 2,  
   -20015500.0, 8673500.0, 1000.0,
   19, 6370997.0,    
   964, 1112, 18, 36,
   17347, 40031,
   { 72,  433,  867, 3470},   /* factors == 240.93, 40.06, 20.01, 5.00 */
   {166, 1001, 2002, 8009},   /* factors == 241.15, 39.99, 20.00, 5.00 */
   {  0,   0,     0,    3},
   {  0,   0,     0,    1}}, 
  {LAEA_NP, PGSd_LAMAZ, 3, 
   -9058902.1845, 9058902.1845, 1002.701,
   -1, 6371228.0,
   951, 951, 19, 19,
   18069, 18069,
   { 70, 452, 903, 3614},   /* factors == 258.13, 39.98, 20.01, 5.00 */
   { 70, 452, 903, 3614},
   {  0,   1,   1,    7},
   {  0,   1,   1,    7}},
  {LAEA_SP, PGSd_LAMAZ, 4,  
   -9058902.1845, 9058902.1845, 1002.701,
   -1, 6371228.0,   
   951, 951, 19, 19,
   18069, 18069,
   { 70, 452, 903, 3614},
   { 70, 452, 903, 3614},
   {  0,   1,   1,    7},
   {  0,   1,   1,    7}},
  {SIN, PGSd_SNSOID, 5,  
   -20015109.354, 10007554.677, 926.62543305,
   -1, 6371007.181, 
   1200, 1200, 18, 36, 
   (18 * 1200), (36 * 1200),
   { 90,  540, 1080, 4320},
   {180, 1080, 2160, 8640},
   {  0,    0,    0,    0},
   {  0,    0,    0,    0}},
  {HAM, PGSd_HAMMER, 6,  
   -18020554.088,  9010277.044, 997.15328068, 
   -1, 6371228.0, 
   1004, 1004, 18, 36, 
   (18 * 1004), (36 * 1004),
   { 70, 452,  903, 3614},  /* factors == 258.17, 39.98, 20.01, 5.00 */
   {140, 904, 1806, 7228},
   {  0,   1,    1,    7},
   {  0,   2,    2,   14}}
};

/* Prototypes */

Tile_loc tilemap3 (int, ...);

/* Functions */

TilePixelCoord To_TilePixelCoordinate(char *proj, char *pixel_size, double lat, double lon) {

  Tile_loc tile_loc;
  TilePixelCoord coord;

  tile_loc = tilemap3(6, proj, pixel_size, "fwd", "tp", lat, lon);

  coord.vert_tile  = tile_loc.itile_line;
  coord.horiz_tile = tile_loc.itile_samp;
  coord.line       = tile_loc.line;
  coord.samp       = tile_loc.samp;

  return coord;
}

LatLonCoord To_LatLonCoordinate(char *proj, char *pixel_size, int vert_tile, int horiz_tile, double line, double samp) {

  Tile_loc tile_loc;
  LatLonCoord coord;

  tile_loc = tilemap3(6, proj, pixel_size, "inv", "tp", vert_tile, horiz_tile, line, samp);

  coord.lat = tile_loc.lat;
  coord.lon = tile_loc.lon;

  return coord;
}

GlobalMapCoord To_GlobalMapCoordinate(char *proj, char *pixel_szie, double lat, double lon) {
  
  Tile_loc tile_loc;
  GlobalMapCoord coord;
  
  tile_loc = tilemap3(6, proj, pixel_szie, "fwd", "gm", lat, lon);

  coord.x = tile_loc.x;
  coord.y = tile_loc.y;

  return coord;
}

GlobalPixelCoord To_GlobalPixelCoordinate(char *proj, char *pixel_size, double lat, double lon) {

  Tile_loc tile_loc;
  GlobalPixelCoord coord;

  tile_loc = tilemap3(6, proj, pixel_size, "fwd", "gp", lat, lon);

  coord.line_global = tile_loc.line_global;
  coord.samp_global = tile_loc.samp_global;

  return coord;
}

Tile_loc tilemap3 (int narg, ...) {
  
  va_list argptr;

  char *cproj;
  char *cpixel_size;
  char *cdirection;
  char *ctype;

  char *tproj[NPROJ] = {"is_k", "is_h", "is_q", "gh", "np", "sp", "sn", "ha"};
  char *tpixel_size[NPIXEL_SIZE] = {"k", "h", "q", "s", "m", "l", "x"};
  char *tdirection[2] = {"fwd", "inv"};
  char *ttype[NTYPE_TILE] = {"tp", "gm", "gp"};
  int pixel_size_ratio_lookup[NPIXEL_SIZE] = {
    PIXEL_SIZE_RATIO_K, PIXEL_SIZE_RATIO_H, PIXEL_SIZE_RATIO_Q,
    PIXEL_SIZE_RATIO_S, PIXEL_SIZE_RATIO_M, PIXEL_SIZE_RATIO_L,
    PIXEL_SIZE_RATIO_X
  };
  int direction_lookup[2] = {DIRECTION_FWD, DIRECTION_INV};
  int type_lookup[NTYPE_TILE] = {
    TYPE_TILE_PIX, TYPE_GLOBAL_MAP, TYPE_GLOBAL_PIX
  };

  Param_t param;
  Tile_t *tile;
  Tile_loc tile_loc;
  long nl, ns;
  int i = 0;

  if (narg != 6 && narg !=8) ERROR("invalid argument number of tilemap3 (6 or 8)", "GetParam");

  va_start(argptr, narg);

  cproj       = va_arg(argptr, char*);
  cpixel_size = va_arg(argptr, char*);
  cdirection  = va_arg(argptr, char*);
  ctype       = va_arg(argptr, char*);

  // get output projection
  for (i = 0; i < NPROJ; i++) 
    if (strcmp(cproj, tproj[i]) == 0) break;
  if (i >= NPROJ) ERROR("unknown value (prjo) of tilemap3", "GetParam");
  param.proj = i;

  // get pixel size ratio
  for (i = 0; i < NPIXEL_SIZE; i++) {
    if (strcmp(cpixel_size, tpixel_size[i]) ==0) break;
  }
  if (i >= NPIXEL_SIZE) ERROR("unkonwn value (pixel_size) of tilemap3", "GetParam");
  param.pixel_size_ratio = pixel_size_ratio_lookup[i];

  // direction
  for (i = 0; i < 3; i++) {
    if (strcmp(cdirection, tdirection[i]) == 0) break;
  }
  if (i >= 2) ERROR("unknown value (direction) of tilemap3", "GetParam");
  param.direction = direction_lookup[i];
  // type
  for (i = 0; i < NTYPE_TILE; i++) {
    if (strcmp(ctype, ttype[i]) == 0) break;
  }
  if (i >= NTYPE_TILE) ERROR("unknown value (type) of tilemap3", "GetParam");
  param.type = type_lookup[i];

  if (param.direction == DIRECTION_FWD) {
    if ((narg - 4) != 2) ERROR("argument number not match direction tilemap3", "GetParam");
    param.lat = va_arg(argptr, double) * RAD;
    param.lon = va_arg(argptr, double) * RAD;
  }
  else if (param.type == TYPE_TILE_PIX) {
    if ((narg - 4) != 4) ERROR("argument number not match type of tilemap3", "GetParam");
    param.itile_line = va_arg(argptr, int);
    param.itile_samp = va_arg(argptr, int);
    param.line       = va_arg(argptr, double);
    param.samp       = va_arg(argptr, double);
  }
  else if (param.type == TYPE_GLOBAL_MAP) {
    if ((narg - 4) != 2) ERROR("argument number not match type of tilemap3", "GetParam");
    param.x = va_arg(argptr, double);
    param.y = va_arg(argptr, double);
  }
  else if (param.type == TYPE_GLOBAL_PIX) {
    if ((narg - 4) != 2) ERROR("argument number not match type of tilemap3", "GetParam");
    param.line_global = va_arg(argptr, double);
    param.samp_global = va_arg(argptr, double);
  }va_end(argptr);

  tile = TileInit(param.proj, param.pixel_size_ratio, &nl, &ns);
  
  if (param.direction == DIRECTION_FWD) {
  
    //printf("lat %.6f  long %.6f  =>", (param.lat * DEG), (param.lon * DEG));
    if (TileFwd(tile, param.lon, param.lat, &param.itile_line, &param.itile_samp, &param.line, &param.samp)) {
      ERROR("invalid forward mapping (TileFwd)", "Process");
    }
  
    if (param.type == TYPE_TILE_PIX) {
      tile_loc.itile_line = param.itile_line;
      tile_loc.itile_samp = param.itile_samp;
      tile_loc.line       = param.line;
      tile_loc.samp       = param.samp;
      //printf("  vert tile %d  horiz tile %d  line %.2f  samp %.2f\n", param.itile_line, param.itile_samp, param.line, param.samp);
    } 
    else if (param.type == TYPE_GLOBAL_MAP) {
      if (TileFwdMap(tile, param.itile_line, param.itile_samp, param.line, param.samp, &param.x, &param.y)) {
        ERROR("invalid forward mapping (TileFwdMap)", "Process");
      }
      tile_loc.x = param.x;
      tile_loc.y = param.y;
      //printf("  x %.2f  y %.2f\n", param.x, param.y);
    } 
    else if (param.type == TYPE_GLOBAL_PIX) {
      if (TileFwdPix(tile, param.itile_line, param.itile_samp, param.line, param.samp, &param.line_global, &param.samp_global)) {
        ERROR("invalid forward mapping (TileFwdMap)", "Process");
      }
      tile_loc.line_global = param.line_global;
      tile_loc.samp_global = param.samp_global;
      //printf("  line %.2f  samp %.2f\n", param.line_global, param.samp_global);
    }
  } 
  else {
  
    if (param.type == TYPE_TILE_PIX) {
      //printf("vert tile %d  horiz tile %d  line %.2f  samp %.2f  =>", param.itile_line, param.itile_samp, param.line, param.samp);
      ;
    }
    else if (param.type == TYPE_GLOBAL_MAP) {
      //printf("x %.2f  y %.2f  =>", param.x, param.y);
      if (TileInvMap(tile, param.x, param.y, &param.itile_line, &param.itile_samp, &param.line, &param.samp)) {
        ERROR("invalid inverse mapping (TileInvMap)", "Process");
      }
    } 
    else if (param.type == TYPE_GLOBAL_PIX) {
      //printf("  line %.2f  samp %.2f  =>", param.line_global, param.samp_global);
      if (TileInvPix(tile, param.line_global, param.samp_global, &param.itile_line, &param.itile_samp, &param.line, &param.samp)) {
        ERROR("invalid inverse mapping (TileInvMap)", "Process");
      }
    }
  
    if (TileInv(tile, param.itile_line, param.itile_samp, param.line, param.samp, &param.lon, &param.lat)) {
      ERROR("invalid inverse mapping", "Process");
    }
    tile_loc.lat = param.lat * DEG;
    tile_loc.lon = param.lon * DEG;
    //printf("  lat %.6f  long %.6f\n", (param.lat * DEG), (param.lon * DEG));
  }
  
  TileFree(tile);

  return tile_loc;
}

