cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
ctest -T Test -T Coverage
find | grep gcno$
find | grep gcda$
find build -name '*.gcda' | xargs gcov
lcov --directory build --capture --output-file coverage.info
genhtml -o coverage coverage.info
google-chrome coverage/index.html
