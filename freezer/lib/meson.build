sources = files([
  'freezer.c',
  'user_netlink.c',
])

freezer_lib = both_libraries(
  'frozone_freeze', sources,
  include_directories: freezer_includes,
  gnu_symbol_visibility: 'hidden'
)
