from distutils.core import setup, Extension
import os

script_root = f"{os.path.dirname(os.path.realpath(__file__))}"
src_root    = script_root + "/tilemap3/src/"

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
              [src_root+"tilemap3_wrap.c", src_root+"tilemap3.c", src_root+"tile.c", src_root+"error.c", src_root+"isinu.c"],
              include_dirs=[script_root+'/tilemap3/include', hdf4_env+"/include", hdf_eos2_env+"/include", sdptk_env+"/include"],
              library_dirs=[hdf4_env+"/lib", hdf_eos2_env+"/lib", sdptk_env+"/lib"],
              libraries=["df", "hdfeos", "PGSTK"]
              )
             ]

setup(name = "tilemap3", 
      version = "1.0",
      ext_modules = extensions,
      py_modules=["tilemap3"])