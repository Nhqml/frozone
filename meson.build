project('frozone', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

includes = include_directories('carto/include')

subdir('carto/src')
#subdir('freezer')

if get_option('buildtype') == 'debug'
  subdir('tests')
  subdir('carto/src/tests/')
endif
