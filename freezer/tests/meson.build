sources = files([
  'main.c',
  'test_freezer.c',
])

cunit_tests_freezer = executable(
  'cunit_tests_freezer', sources,
  include_directories: freezer_includes,
  link_with: freezer_lib.get_static_lib(),
  dependencies: [
    dependency('cunit'),
  ],
)

test('CUnit - Freezer', cunit_tests_freezer)
