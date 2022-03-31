sources = files([
  'carto.c',
  'utils.c',
  'users.c',
  'processes.c',
])

cc = meson.get_compiler('c')
kvm_dep = cc.find_library('kvm')

carto_lib = both_libraries(
  'frozone_carto', sources,
  include_directories: includes,
  gnu_symbol_visibility: 'hidden',
  dependencies : kvm_dep
)
