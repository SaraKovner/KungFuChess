# CMake generated Testfile for 
# Source directory: C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess
# Build directory: C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(kungfu_chess_tests "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/build/Debug/kungfu_chess_tests.exe")
  set_tests_properties(kungfu_chess_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;102;add_test;C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(kungfu_chess_tests "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/build/Release/kungfu_chess_tests.exe")
  set_tests_properties(kungfu_chess_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;102;add_test;C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(kungfu_chess_tests "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/build/MinSizeRel/kungfu_chess_tests.exe")
  set_tests_properties(kungfu_chess_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;102;add_test;C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(kungfu_chess_tests "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/build/RelWithDebInfo/kungfu_chess_tests.exe")
  set_tests_properties(kungfu_chess_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;102;add_test;C:/Users/sksk1/Documents/לימודים/BootCamp/CTD/CTD25_Solutions/KCF/KungFuChess/CMakeLists.txt;0;")
else()
  add_test(kungfu_chess_tests NOT_AVAILABLE)
endif()
