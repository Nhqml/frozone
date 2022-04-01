demo_carto = executable(
  'demo_carto', 'demo_carto.c',
  include_directories: includes,
  link_with: carto_lib,
)

if host_machine.system() == 'linux'
  demo_freezer = executable(
    'demo_freezer', 'demo_freezer.c',
    include_directories: freezer_includes,
    link_with: freezer_lib,
  )
endif
