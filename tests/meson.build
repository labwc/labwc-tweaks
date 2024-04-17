test_lib = static_library(
  'tests',
  sources: files(
    '../src/xml.c',
  ),
  dependencies: [dependency('libxml-2.0'), dependency('glib-2.0')],
)

  t = 't1000-add-xpath-node.c'
  testname = t.split('.')[0].underscorify()
  exe = executable(testname, sources: [t, 'tap.c'], dependencies: [dependency('glib-2.0')], link_with: [test_lib])
  test(testname, exe)

  t = 't1001-nodenames.c'
  testname = t.split('.')[0].underscorify()
  exe = executable(testname, sources: [t, 'tap.c'], dependencies: [dependency('libxml-2.0'), dependency('glib-2.0')])
  test(testname, exe)

