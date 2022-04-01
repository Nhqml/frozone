sources = files([
  'carto.c',
])

carto_lib = both_libraries(
  'frozone_carto', sources,
  include_directories: includes,
  gnu_symbol_visibility: 'hidden',
  dependencies : meson.get_compiler('c').find_library('kvm')
)
