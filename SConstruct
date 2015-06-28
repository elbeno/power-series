import os

buildType = 'debug'

range_include = '#contrib/range-v3/include'
test_include = '#contrib/testinator/src/include'

include = '#build/$BUILDTYPE/include'
lib = '#build/$BUILDTYPE/lib'
bin = '#build/$BUILDTYPE/bin'

env = Environment(BUILDTYPE = buildType,
                  CPPPATH = [include, range_include, test_include],
                  LIBPATH = [lib])

env.Append(CCFLAGS = "-g -std=c++1y")
env.Append(CCFLAGS = ["-pedantic"
                      , "-ftemplate-backtrace-limit=0"
                      , "-Wall"
                      , "-Wextra"
                      , "-Wcast-align"
                      , "-Wcast-qual"
                      , "-Wctor-dtor-privacy"
                      , "-Wdisabled-optimization"
                      , "-Wformat=2"
                      , "-Winit-self"
                      , "-Wmissing-include-dirs"
                      , "-Woverloaded-virtual"
                      , "-Wredundant-decls"
                      , "-Wsign-conversion"
                      , "-Wsign-promo"
                      , "-Wstrict-overflow=5"
                      , "-Wswitch-default"
                      , "-Wundef"
                      , "-Werror"
                      , "-Wno-unused"
                      , "-Wno-unused-parameter"])

compiler = 'clang++'
#compiler = 'g++'
if 'CXX' in os.environ and os.environ['CXX']:
  compiler = os.environ['CXX']
env.Replace(CXX = compiler)

if compiler[:5] == 'clang':
    env.Append(CCFLAGS = "-stdlib=libc++")
    env.Append(LINKFLAGS = "-lc++")

env['PROJNAME'] = os.path.basename(Dir('.').srcnode().abspath)
print env['PROJNAME']

Export('env')
env.SConscript('src/SConscript', variant_dir='build/$BUILDTYPE')
