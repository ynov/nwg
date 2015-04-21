Building (using GCC by default):

    make deps && make -j4

... or if you prefer clang:

    make deps TOOLSET=clang && make CXX=clang++ -j4
