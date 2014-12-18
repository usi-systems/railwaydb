# Railway Adaptive Storage for Interaction Graphs
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


1. The partitioning algorithm depends on the Gurobi optimizer library. This is available
   for free under an academic license. Follow the instructions at: http://www.gurobi.com to install Gurobi.

2. First fetch and build boost using libc++
3. We expect this libc++ boost to be installed at '/opt/local/libcpp-software'
<br/><br/><pre><code>mkdir tmp
cd tmp
http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz
tar -zxvf boost_1_55_0.tar.gz
cd boost_1_55_0
./bootstrap.sh --prefix=/opt/local/libcpp-software
sudo ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" --layout=tagged  install
cd ../../
rm -fr tmp
</code></pre>

4. Second, build the dependent libraries included in the 'ext' directory. These are the 'leveldb' and the 'libspatialindex' libraries

5. Enter into the external software dir
<br/><br/><pre><code>
cd ext/src
</code></pre>

6. Build leveldb
<br/><br/><pre><code>cd leveldb
CC=clang CXX=clang++ make -j 4
cd ..
</code></pre>

7. Build libspatialindex
<br/><br/><pre><code>cd libspatialindex
CC=clang CXX=clang++ ./configure --prefix $(cd ../..; pwd)
make -j 4
make install
cd ..
</code></pre>

8. Build gtest
<br/><br/><p.re><code>cd gtest
CC=clang CXX=clang++ ./configure --prefix $(cd ../..; pwd)
make
cp -fr include/gtest ../../include
cp lib/.libs/libgtest* ../../lib/
</code></pre>

9. Go back to the top level
<br/><br/><pre><code>cd ../..
</code></pre>

10. Third, build libintergdb
<br/><br/><pre><code>cd libintergdb
make -j 4
cd ..
</code></pre>

11. Last, run a test application
<br/><br/><pre><code>cd test/SimpleTest
make
./bin/SimpleTest 
</code></pre>






