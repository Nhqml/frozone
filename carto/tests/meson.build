sources = files([
  'main.c',
  'test.c',
])

cunit_tests = executable(
  'cunit_tests', sources,
  include_directories: includes,
  link_with: carto_lib,
  dependencies: [
    dependency('cunit'),
  ],
)

test('CUnit', cunit_tests)
