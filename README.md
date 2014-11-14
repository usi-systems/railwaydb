graphdb
=======

This repository contains an adaptive disk layout called the railway layout for
optimizing disk block storage for interaction graphs. The key idea is to divide
blocks into one or more sub-blocks, where each sub-block contains a subset of
the attributes, but the entire graph structure is replicated within each
sub-block. This improves query I/O, at the cost of increased storage overhead.
We introduce optimal ILP formulations for partitioning disk blocks into
sub-blocks with overlapping and non-overlapping attributes. Additionally, we
present greedy heuristic approaches that can scale better compared to the ILP
alternatives, yet achieve close to optimal query I/O. To demonstrate the
benefits of the railway layout, we provide an extensive experimental study
comparing our approach to a few baseline alternatives.

# First fetch and build boost using libc++
# We expect this libc++ boost to be installed at '/opt/local/libcpp-software'

mkdir tmp
cd tmp
wget http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz
tar -zxvf boost_1_55_0.tar.gz
cd boost_1_55_0
./bootstrap.sh --prefix=/opt/local/libcpp-software
sudo ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --layout=tagged  install
cd ../../
rm -fr tmp

# Second, build the dependent libraries included in the 'ext' directory
# These are the 'leveldb' and the 'libspatialindex' libraries

## Enter into the external software dir
cd ext/src

## Build leveldb
cd leveldb
CC=clang CXX=clang++ make -j 4
cd ..

## Build libspatialindex
cd libspatialindex
CC=clang CXX=clang++ ./configure --prefix $(cd ../..; pwd)
make -j 4
make install
cd ..

## Go back to the top level
cd ../..

# Third, build libintergdb
cd libintergdb
make -j 4
cd ..

# Last, run a test application
cd test/SimpleTest
make
./bin/SimpleTest 







