#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// �ֽ⸡����Ϊ���š������β��
void depart(float num, uint32_t* s, uint32_t* e, uint32_t* m) {
    uint32_t num_32 = *((uint32_t*)&num);
    *s = (num_32 >> 31);//����λ
    *e = (num_32 >> 23) & 0xFF;//����
    *m = num_32 & 0x7FFFFF;//β��
    if (*e) {
        *m |= 0x800000; 
    }
    else {
        *m <<= 1;
    }
}

// ���Ϊ������
float compose(uint32_t s, uint32_t e, uint32_t m) {
    if (!e) {
        m >>= 1;
    }
    uint32_t num_32 = (s << 31) | (e << 23) | (m & 0x7FFFFF);
    return *((float*)&num_32);
}

// ���
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

// �������ӷ�
float add(float a, float b) {
    uint32_t a_s, a_e, a_m;
    uint32_t b_s, b_e, b_m;
    depart(a, &a_s, &a_e, &a_m);
    depart(b, &b_s, &b_e, &b_m);

    // �Ƚ�ָ����С����β�����ж���
    if (a_e > b_e) {
        b_m >>= (a_e - b_e);
        b_e = a_e;
    } else {
        a_m >>= (b_e - a_e);
        a_e = b_e;
    }

    // ���ݷ���λ�����ӷ����Ǽ���
    uint32_t res_s, res_e, res_m;
    if (a_s == b_s) {
        res_m = a_m + b_m;
        res_s = a_s;
    } else {
        res_s = (a_m >= b_m) ? a_s : b_s;
        res_m = (a_m >= b_m) ? a_m - b_m : b_m - a_m;
    }

    // ���ָ��ȡ�ϴ�ֵ
    res_e = (a_e > b_e) ? a_e : b_e;

    normalize(&res_e, &res_m);

    return compose(res_s, res_e, res_m);
}

int main() {
    float a, b;

    printf("�����������Ϸ��ĸ�������\n");
    scanf("%f %f", &a, &b);
    
    float result = add(a, b);
    
    printf("%f", result);
    return 0;
}

