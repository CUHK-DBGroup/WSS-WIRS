# Efficient Dynamic Weighted Set Sampling and Its Extension

This repository implements dynamic weighted set sampling (WSS) and weighted independent range sampling (WIRS) of *Efficient Dynamic Weighted Set Sampling and Its Extension*. For a set of elements with three attributes <key, value, weight>, it can support the following operations
1. Insert an element.
2. Delete an element with a given key.
3. Performs a weighted sampling from all elements in the set.
4. Given a range $[l,r]$, perform $t$ weighted samplings from all elements whose keys belong to this range.

## About the running environment
All experiments are conducted on a Linux machine with an Intel Xeon(R) CPU with 256GB memory. All methods are implemented in C++ and compiled with full optimization.

## About the datasets
All the real world datasets used can be downloaded by clicking on the link below and then cleaned in a similar way. Datasets:
[USA Road Networks](http://users.diag.uniroma1.it/challenge9/download.shtml), [Delicious](http://delicious.com/), [Twitter](https://anlab-kaist.github.io/traces/).

In addition to the real-world dataset, synthetic datasets are generated to test all the methods. We use uniform_distribution and exponential_distribution to generate two datasets.

## Compile the code
Please note that the path of the input dataset needs to be configured by modifying the code. After you have configured the file path, you can execute the following commands to generate the executable file *---*.

```sh
$ cd src
$ make clean
$ make
```
After compiling the code, an executable file called *---* is generated.


## Run the code
After placing the dataset, one can run the following command directly. For the {testName} problem, execute tests of {op} operation on {fileName} dataset.
<!-- cleaning the dataset, you can run the following command directly. Build the BOTBIN of dataset {file_id} with the paramet $\rho$ and $\delta$. -->
```sh
$ ./DynamicWeightedSetSampling {testName} {fileName} {op} [parameters]
```
The {testName} can be set as *WSS* or *WIRS*. Details about the parameters {op} for the different operations and other corresponding [parameters] that need to be set can be found in the code.
## About the code

* main.cpp has some examples of how to construct data structures, and how to modify them.
* *BST.hpp* and *basic_bst.hpp* includes the implementation of BST sampling methods.
* *basic_alias.hpp* includes the implementation of alias sampling method.
* *QuickBucket.hpp* includes the implementation of BUS sampling method.
* *scapegoatTree.hpp* implements a $O(n\log{n})$ structure for the WIRS problem.
* *QuickChunkScapegoatTree.hpp* implements a linear space structure for the WIRS problem, and it supports modifiable chunk size.
* Read the code for more details.
