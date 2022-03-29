project('frozone', 'c',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'buildtype=debugoptimized'
  ],
  license: 'MIT',
  meson_version: '>=0.61.2'
)

includes = include_directories('carto/include')

subdir('carto/src')
subdir('freezer')
subdir('examples')

if get_option('buildtype').startswith('debug')
  subdir('tests')
  subdir('carto/src/tests/')
endif
