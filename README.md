# Efficient Dynamic Weighted Set Sampling and Its Extension: Bridging Theory and Practice

This repository implements dynamic weighted set sampling (WSS) and weighted independent range sampling (WIRS) of *Optimal dynamic weighted set sampling and its extension*. For a set of elements with three attributes <key, value, weight>, it can support the following operations
1. Insert an element.
2. Delete an element with a given key.
3. Performs a weighted sampling from all elements in the set.
4. Given a range $[l,r]$, perform $t$ weighted samplings from all elements whose keys belong to this range.

## Some notes

* main.cpp has some examples of how to construct data structures, and how to modify them.
* BUS in paper is implemented in *QuickBucket.hpp*.
* *scapegoatTree.hpp* implements a $O(n\log{n})$ structure for the WIRS problem.
* *QuickChunkScapegoatTree.hpp* implements a linear space structure WIRS-BUS for the WIRS problem, and it supports modifiable chunk size.
* *QuickChunkScapegoatTree.hpp* implements a linear space structure WIRS-BUS for the WIRS problem, and it supports modifiable chunk size. *ChunkScapegoatTreeWithAlias* implements a linear structure similar to WIRS-BUS, but implements an alias structure in each chunk to test performance.
* *tableBucket.hpp* implements the lookup table method [23] for WSS problem. 
* We also include methods from previous work as baselines. Read the code for more details.
