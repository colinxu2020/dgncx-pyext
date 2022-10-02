from distutils.core import setup, Extension

module = Extension('dgncx',
                    sources = ['dgncxmodule.cpp'])

setup (name = 'dgncx',
       version = '1.0.9',
       description = 'dgncx',
       ext_modules = [module])
