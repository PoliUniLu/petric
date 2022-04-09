from distutils.core import setup, Extension

module = Extension('native_petric',
                   define_macros = [('MAJOR_VERSION', 0),
                                    ('MINOR_VERSION', 1)],
                   sources = ['src/native_petric.cpp',
                             'src/petric.cpp'],
                  # headers = ['src/petric.h'],
                   extra_compile_args=['-std=c++17'])

setup(name = 'NativePetric',
      version = 0.1,
      description = 'Native implementation test',
      ext_modules = [module],
      test_suite='tests')