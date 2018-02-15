
# Kunity test runner module

Is a runner module for runnig kunity test cases (see xxx). The module search for test function in the symbol table and made the them executionable byy uspase (via proc file)


## proc file struce

* /proc/kunity_test
    * <modulename>-folder
        *  <modulename>-file -> read of this file, will be run all test case of this module
        * single_tests
            * test_name-file -> read of this file, will be run the specific test case

## building module

### cmake

``` bash
mkdir build
cd build
cmake ../
# cmake -DKERNEL_DIR=<path/to/kernel/module/build/#!/usr/bin/env  ../
make

#load module by "insmod kunity_test_runner_module/kunity_test_runner_module.ko"
```
By default "the /lib/modules/${CMAKE_SYSTEM_VERSION}/build" path is use for the kernel build path. This path can be change  by using the KERNEL_DIR option

### KBuild

```bash
make -C /lib/modules/<KVerision>/build  M=${PWD} modules KBUILD_EXTRA_SYMBOLS=${PWD}/Module.symvers
#load module by "insmod kunity_test_runner_module.ko"
```

## execution

simple use cat on the proc file :)


## module parameter

  * proc_test_root_folder_name = The name of the proc folder, which will contains the test proc files. (Default is "kunity_test")
  * test_function_filter =a filter option for test function names, which the runner is looking for. (Default is "kunity_test_*")
  * module_filter = a filter option for modules, which the runner is looking for test functions.  (Default is * -> search in all modules)
