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

freezer_includes = include_directories('freezer')

if host_machine.system() == 'linux'
    subdir('carto/src')

    subdir('freezer/lib')
    subdir('freezer/module')

elif host_machine.system() == 'openbsd'
    subdir('openbsd')
endif

subdir('examples')

if get_option('buildtype').startswith('debug')
    subdir('tests')
    subdir('carto/src/tests/')
    subdir('freezer/tests')
endif
