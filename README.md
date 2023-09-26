# Efficient Dynamic Weighted Set Sampling and Its Extension

This repository implements dynamic weighted set sampling (WSS) and weighted independent range sampling (WIRS) of *Efficient Dynamic Weighted Set Sampling and Its Extension*. For a set of elements with three attributes <key, value, weight>, it can support the following operations
1. Insert an element.
2. Delete an element with a given key.
3. Performs a weighted sampling from all elements in the set.
4. Given a range $[l,r]$, perform $t$ weighted samplings from all elements whose keys belong to this range.

## About the running environment
All experiments were conducted on a Linux machine with an Intel Xeon(R) CPU and 256GB of memory, running Ubuntu 18.04.1 LTS. We have implemented all methods using the C++17 standard and turned on the O3 optimization flag.
## About the datasets
All the real world datasets used can be downloaded by clicking on the link below and then cleaned in a similar way. Datasets:
[USA Road Networks](http://users.diag.uniroma1.it/challenge9/download.shtml), [Delicious](http://delicious.com/), [Twitter](https://anlab-kaist.github.io/traces/).

In addition to the real-world dataset, synthetic datasets are generated to test all the methods. We use uniform_distribution and exponential_distribution to generate two datasets.

## Input format
An input dataset should contain multiple lines, with each line consisting of two integers and one real number separated by spaces, representing the three attributes <key, value, weight> of an element, respectively.

、、、
key_1 value_1 weight_1
key_2 value_2 weight_2
...
key_i value_i weight_i
...
、、、

In which the i-th line represents the i-th element with key key_i, value value_i, and will be sampled with a weight of weight_i. A simple input dataset can be found at "WSSWIRS/dataset/input.data" as an example.
## Compile the code
Please note that the path of the input dataset needs to be configured by modifying the code. Specifically, the variable *dataHome* represents the path to the dataset, *tmpData* indicates the path for storing intermediate results during program execution, and *resultHome* signifies the path for storing the results of the test program. You can easily locate these in the *main.cpp* file.

After you have configured the file path, you can execute the following commands to generate the executable file *DynamicWeighteSetSampling*.

```sh
$ cd WSSWIRS
$ cmake ..
$ make
```
Note that the CMake version should be higher than 3.22, and the g++ version should be 11.1.0. After compiling the code, an executable file called *DynamicWeighteSetSampling* is generated.

## Run the code
After placing the dataset, one can run the following command directly. For the {testName} problem, execute tests of {op} operation on {fileName} dataset，and output the runtime and required memory space size.
```sh
$ ./DynamicWeightedSetSampling WSS {fileName} {dataNum} {op} [parameters]
$ ./DynamicWeightedSetSampling WIRS {fileName} {dataNum} {op} [parameters]
```
The {dataNum} represents the number of elements in the dataset. The parameter {op} represents the name of the operation to be tested, including *build*, *query*, *ins*, *del*, *mix*, etc. Different [parameters] are required for each operation. Here are some examples:
* If you want to test the construction speed of {method} in the *WSS* problem, you need to enter the command:
  ```sh
  $ ./DynamicWeightedSetSampling WSS {fileName} {dataNum} build {method} {dataNum} {id}
  ```
  Where {id} represents the test identifier used to distinguish the output results of multiple tests.
* If you need to test the insertion speed of all methods for the *WSS* problem, you need to enter the command:
  ```sh
  $ ./DynamicWeightedSetSampling WSS {fileName} {dataNum} ins {method} {optNum} 
  ```
  Where {optNum} represents the number of insertion operation.
* If you need to test the mix operation speed of all methods for the *WSS* problem, you need to enter the command:
  ```sh
  $ ./DynamicWeightedSetSampling WSS {fileName} {dataNum} mix {method} {optNum} {updatePercent} {sampleTimes}
  ```
  Where {optNum} represents the number of insertion operation, {updatePercent} is an integer less than 100, indicating the percentage of update operations relative to the total operations, while {sampleTimes}represents the number of samples for each query operation.

Details about the parameters {op} for other different operations and other corresponding [parameters] that need to be set can be found in the code.
## Example
## About the code

* *main.cpp*  has some examples of how to construct data structures, and how to modify them.
* *basic_bst.hpp* includes the implementation of BST sampling methods for WSS problem.
* *basic_alias.hpp* includes the implementation of alias sampling method.
* *QuickBucket.hpp* includes the implementation of BUS sampling method.
* *BST.hpp*includes the implementation of BST sampling methods for WIRS problem.
* *scapegoatTree.hpp* implements a $O(n\log{n})$ structure for the WIRS problem.
* *QuickChunkScapegoatTree.hpp* implements a linear space structure for the WIRS problem, and it supports modifiable chunk size.
* Read the code for more details.
