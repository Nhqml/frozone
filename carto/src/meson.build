sources = files([
  'carto.c',
  'utils.c',
  'users.c',
  'processes.c',
])

carto_lib = both_libraries(
  'frozone_carto', sources,
  include_directories: includes,
  gnu_symbol_visibility: 'hidden',
  link_with: 'kvm',
)
