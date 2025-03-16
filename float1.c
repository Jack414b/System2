#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// 分解浮点数为符号、阶码和尾数
void depart(float num, uint32_t* s, uint32_t* e, uint32_t* m) {
    uint32_t num_32 = *((uint32_t*)&num);
    *s = (num_32 >> 31);//符号位
    *e = (num_32 >> 23) & 0xFF;//阶码
    *m = num_32 & 0x7FFFFF;//尾数
    if (*e) {
        *m |= 0x800000; 
    }
    else {
        *m <<= 1;
    }
}

// 组合为浮点数
float compose(uint32_t s, uint32_t e, uint32_t m) {
    if (!e) {
        m >>= 1;
    }
    uint32_t num_32 = (s << 31) | (e << 23) | (m & 0x7FFFFF);
    return *((float*)&num_32);
}

// 规格化
void normalize(uint32_t* e, uint32_t* m) {
    while (*m >= (1 << 24)) {
        *m >>= 1;
        (*e)++;
    }
    while (*m < (1 << 23) && *e > 0) {
        *m <<= 1;
        (*e)--;
    }
}

// 浮点数加法
float add(float a, float b) {
    uint32_t a_s, a_e, a_m;
    uint32_t b_s, b_e, b_m;
    depart(a, &a_s, &a_e, &a_m);
    depart(b, &b_s, &b_e, &b_m);

    // 比较指数大小并对尾数进行对齐
    if (a_e > b_e) {
        b_m >>= (a_e - b_e);
        b_e = a_e;
    } else {
        a_m >>= (b_e - a_e);
        a_e = b_e;
    }

    // 根据符号位决定加法还是减法
    uint32_t res_s, res_e, res_m;
    if (a_s == b_s) {
        res_m = a_m + b_m;
        res_s = a_s;
    } else {
        res_s = (a_m >= b_m) ? a_s : b_s;
        res_m = (a_m >= b_m) ? a_m - b_m : b_m - a_m;
    }

    // 结果指数取较大值
    res_e = (a_e > b_e) ? a_e : b_e;

    normalize(&res_e, &res_m);

    return compose(res_s, res_e, res_m);
}

int main() {
    float a, b;

    printf("请输入两个合法的浮点数：\n");
    scanf("%f %f", &a, &b);
    
    float result = add(a, b);
    
    printf("%f", result);
    return 0;
}

