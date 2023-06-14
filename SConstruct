import os

env = Environment(ENV = os.environ)
env.Append( CPPFLAGS = [ "-g", "-std=c++20", "-fcoroutines", "-Wextra", "-Wall", "-Werror" ] )
env.SConscript("src/SConscript", variant_dir="build", exports="env", duplicate=0)
