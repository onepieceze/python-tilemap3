from distutils.core import setup, Extension
import os



def get_env(string):
    env = os.getenv(string)
    if env == None:
        print(f"[Error]: {string} library environment not define.")
        exit()
    return env

hdf4_env     = get_env("HDF4")
hdf_eos2_env = get_env("HDF_EOS2")
sdptk_env    = get_env("SDPTK")


extensions = [
    Extension("_tilemap3", 
              ["tilemap3_wrap.c", "tilemap3.c", "tile.c", "error.c", "isinu.c"],
              include_dirs=[hdf4_env+"/include", hdf_eos2_env+"/include", sdptk_env+"/include"],
              library_dirs=[hdf4_env+"/lib", hdf_eos2_env+"/lib", sdptk_env+"/lib"],
              libraries=["df", "hdfeos", "PGSTK"]
              )
             ]

setup(name = "tilemap3", 
      version = "1.0",
      ext_modules = extensions,
      py_modules=["tilemap3"])