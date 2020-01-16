# python-tilemap3
python-tilemap3: python interface  to tilemap3 (above python3.0).

# Overview
Tilemap3 program maps between geographic coordinates and various grids used by the
MODIS Land Science Team.

# Prerequisites
HDF4, HDF-EOS2, [SDP ToolKit](https://observer.gsfc.nasa.gov/ftp/edhs/sdptk/previous_releases/) of NASA.

# Installation
'''
python3 setup.py install
'''

# Usage

## Functions

  To_TilePixelCoordinate(projection, pixel_size, latitude, longitude)
  
  To_LatLonCoordinate(projection, pixel_size, vertical_tile, horizon_tile, tile_line, tile_samp)
  
  To_GlobalMapCoordinate(projection, pixel_size, latitude, longitude)
  
  To_GlobalPixelCoordinate(projection, pixel_size, latitude, longitude)
  
 ## arguments
 
   <projection>: is_k, is_h, is_q, gh, np or sp
     is_k - Integerized Sinusoidal using a ~1km (actually 30 arcsec) 
            underlying grid row size
     is_h - Integerized Sinusoidal using a ~500m (actually 15 arcsec)
            underlying grid row size
     is_q - Integerized Sinusoidal using a ~250m (actually 7.5 arcsec)
            underlying grid row size
     gh   - Goodes Homolosine
     np   - Lamberts Azmuthal Equal Area (LAEA), North Polar Region, EASI Grid
     sp   - Lamberts Azmuthal Equal Area (LAEA), South Polar Region, EASI Grid
     sn   - Sinusoidal grid
     ha   - Hammer Atioff grid

  <pixel_size>: k, h or q
     k - ~1km
     h - ~500m
     q - ~250m
     s - ~240 km (small global grid)
     m - ~40 km (medium global grid)
     l - ~20 km (large global grid)
     x - ~5 km (extra large global grid)

  
 
  
