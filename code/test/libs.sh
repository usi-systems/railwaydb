#!/bin/bash

LIBS="libboost_coroutine-mt.dylib 
libboost_context-mt.dylib 
libboost_filesystem-mt.dylib 
libboost_iostreams-mt.dylib 
libboost_chrono-mt.dylib 
libboost_date_time-mt.dylib 
libboost_system-mt.dylib 
libboost_thread-mt.dylib"

for l in $LIBS
do
  echo "$l"
#  install_name_tool -change $l /opt/local/libcpp-software/lib/$l SimpleTest 
done

# install_name_tool -change libboost_coroutine-mt.dylib  /opt/local/libcpp-software/lib/libboost_coroutine-mt.dylib SimpleTest 
