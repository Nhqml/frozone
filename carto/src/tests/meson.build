sources = files([
  'main.c',
  'test_utils.c',
])

cunit_tests_private = executable(
  'cunit_tests_private', sources,
  include_directories: includes,
  link_with: carto_lib.get_static_lib(),
  dependencies: [
    dependency('cunit'),
  ],
)

test('CUnit - Carto - Private', cunit_tests_private)
