#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    int **array;
    int row = 11451;
    int col = 11451;

    array = (int **)malloc(row * sizeof(int *));

//赋值
    for(int i = 0; i < row; ++i){
        array[i] = (int *)malloc(col * sizeof(int));
        for(int j = 0; j < col; ++j){
            array[i][j] = 1;
        }
    }

//先行后列
    clock_t start,end;
    double t1;

    start = clock();
    int sum = 0;
    for(int i = 0; i < row; ++i){
        for(int j = 0; j < col; ++j){
            sum += array[i][j];
        }
    }
    end = clock();

    t1 = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("%f秒\n", t1);

//先列后行
    clock_t start1,end1;
    double t2;

    start1 = clock();
    int sum1 = 0;
    for(int i = 0; i < row; ++i){
        for(int j = 0; j < col; ++j){
            sum1 += array[j][i];
        }
    }
    end1 = clock();

    t2 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
    printf("%f秒\n", t2);

/*
通过调换sum += array[i][j]中i与j的位置分别实现先行后列和先列后行的计算。
时间如下：0.275000秒
         1.108000秒
可以看到先列后行的运行时间远大于先行后列。

思考原因：
运行时间差异主要源于计算机内存访问的局部性原理（locality of reference）和缓存（cache）机制的工作方式。
在现代计算机中，CPU 有多个层次的缓存（L1、L2、L3），这些缓存用于提高数据访问速度。缓存的工作原理是基于局部性原理，包括时间局部性和空间局部性：

时间局部性：如果某个数据被访问过，未来很有可能再次被访问。
空间局部性：如果某个数据被访问，附近的数据很可能也会被访问。

代码中：
先行后列（sum += array[i][j]）：我们按行访问数组。
由于数组在内存中是连续存储的，访问同一行的元素时，CPU 可以利用缓存预取机制，因为这些元素在内存中是相邻的。这导致更高的缓存命中率，从而提高访问速度。
先列后行（sum1 += array[j][i]）：我们按列访问数组。
虽然在逻辑上你仍在访问整个数组，但由于数组在内存中是按行存储的，访问不同列的元素时，CPU 需要频繁地跳转到不同的内存地址。这通常会导致缓存未命中，从而增加访问延迟。

由于上述原因，先行后列的访问模式通常比先列后行的访问模式快得多。
*/

// 释放内存
    for (int i = 0; i < row; ++i) {
        free(array[i]); // 释放每一行
    }
    free(array); // 释放行指针数组

    return 0;

}