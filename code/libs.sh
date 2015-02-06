#!/bin/bash

export DYLD_LIBRARY_PATH=/opt/local/libcpp-software/lib/:$DYLD_LIBRARY_PATH

#LIBS="libboost_coroutine-mt.dylib 
#libboost_context-mt.dylib 
#libboost_filesystem-mt.dylib 
#libboost_iostreams-mt.dylib 
#libboost_chrono-mt.dylib 
#libboost_date_time-mt.dylib 
#libboost_system-mt.dylib 
#libboost_thread-mt.dylib"
#
#for l in $LIBS
#do
##  echo "$l"
## install_name_tool -change $l /opt/local/libcpp-software/lib/$l 
#done


