sources = files([
  'carto.c',
  'utils.c',
  'users.c',
  'processes.c',
])

if host_machine.system() == 'openbsd'
    cc = meson.get_compiler('c')
    kvm_dep = cc.find_library('kvm')
endif

if host_machine.system() == 'openbsd'
    carto_lib = both_libraries(
    'frozone_carto', sources,
    include_directories: includes,
    gnu_symbol_visibility: 'hidden',
    dependencies : kvm_dep
    )
else
    carto_lib = both_libraries(
    'frozone_carto', sources,
    include_directories: includes,
    gnu_symbol_visibility: 'hidden',
    )
endif
