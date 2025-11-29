# CMake generated Testfile for 
# Source directory: C:/___myProject/practiceCopilot/task03/tests
# Build directory: C:/___myProject/practiceCopilot/task03/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("C:/___myProject/practiceCopilot/task03/build/tests/test_book_gtest[1]_include.cmake")
add_test(BookModuleTests "C:/___myProject/practiceCopilot/task03/build/bin/tests/test_book.exe")
set_tests_properties(BookModuleTests PROPERTIES  LABELS "unit;book" TIMEOUT "30" WORKING_DIRECTORY "C:/___myProject/practiceCopilot/task03/build" _BACKTRACE_TRIPLES "C:/___myProject/practiceCopilot/task03/tests/CMakeLists.txt;13;add_test;C:/___myProject/practiceCopilot/task03/tests/CMakeLists.txt;0;")
add_test(BookModuleGoogleTests "C:/___myProject/practiceCopilot/task03/build/bin/tests/test_book_gtest.exe")
set_tests_properties(BookModuleGoogleTests PROPERTIES  LABELS "unit;book;gtest" TIMEOUT "30" WORKING_DIRECTORY "C:/___myProject/practiceCopilot/task03/build" _BACKTRACE_TRIPLES "C:/___myProject/practiceCopilot/task03/tests/CMakeLists.txt;44;add_test;C:/___myProject/practiceCopilot/task03/tests/CMakeLists.txt;0;")
