
# Kunity test runner module
The Kunity test runner runs any kunity tests (see [KUnity-Core](https://github.com/stubbfel/KUnity-Core)) by a kernel module. 
This module search for test function in the symbol table of the kernel space (https://www.linux.com/learn/kernel-newbie-corner-kernel-symbols-whats-available-your-module-what-isnt) and made them runable for the userspace (by proc files).

## proc file struce

```plain
 + /proc/kunity_test
    + <modulename>
        - <modulename> -> read of this file, will be run all test case of this module
        + single_tests
            - <test_name> -> read of this file, will be run the specific test case
```

## building module

### cmake

``` bash
mkdir build
cd build
cmake ../
# cmake -DKERNEL_DIR=<path/to/kernel/module/build> ../
make
#load module by "insmod kunity_test_runner_module/kunity_test_runner_module.ko"
```
By default the `/lib/modules/${CMAKE_SYSTEM_VERSION}/build` path is use for the kernel build path. This path can be change by using the `KERNEL_DIR` option.

### KBuild

```bash
make -C /lib/modules/<KernelVersion>/build  M=${PWD} modules KBUILD_EXTRA_SYMBOLS=${PWD}/Module.symvers
#load module by "insmod kunity_test_runner_module.ko"
```

## execution

simple use `cat` on the proc file :) like : `cat /proc/kunity_test/my_module/single_tests/my_first_module_test`

## module parameter

  * proc_test_root_folder_name - The name of the proc folder, which will contains the test proc files. (Default is "kunity_test")
  * test_function_filter - a filter option for test function names, which the runner is looking for. (Default is "kunity_test_*")
  * module_filter - a filter option for modules, which the runner is looking for test functions.  (Default is * -> search in all modules)
