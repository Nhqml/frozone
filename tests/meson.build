sources = files([
  'main.c',
  'test_carto.c',
])

cunit_tests_public = executable(
  'cunit_tests_public', sources,
  include_directories: includes,
  link_with: carto_lib,
  dependencies: [
    dependency('cunit'),
  ],
)

test('CUnit - Public', cunit_tests_public)
