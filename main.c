#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BASE 65536


void test_print(uint16_t *massive, int num) {
    printf("-----\n");
    for (int i = 0; i < num; i++) {
        printf("%u -- 65536^%d\n", massive[i], i - 1);
    }
    printf("-----\n");
}

void printer(unsigned short* num, int size) { //дописать функцию
    if (num[0] == 1) {
        printf("-");
    }

    char digits[500] = {};

    while (size - 1 > 0) {
        int rem = 0;
        for (int i = size - 1; i > 0; i--) {
            unsigned int cur = rem * BASE + num[i];
            num[i] = cur / 10;
            rem = cur % 10;
        }
    }
}

int splitter(char *num, unsigned short *digits, int blocks) { //blocks прикрутить
    int start = 0;
    digits[0] = 0;
    if (num[0] == '-') {
        digits[start] = 1;
        start = 1;
    } else if (num[0] == '+') {
        start = 1;
    }

    digits[1] = 0;
    int size = 2;

    for (int i = start; num[i] != '\0'; i++) {
        unsigned short carry = num[i] - '0';

        for (int j = 1; j < size + 1; j++) {
            unsigned int cur = (unsigned int)digits[j] * 10 + carry;
            digits[j] = cur % BASE;
            carry = cur / BASE;
        }

        if (carry > 0) {
            digits[size] = carry;
            size++;
        }
    }

    return size;
}

int compare(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2) {
    int i = parts_num1 - 1;
    int j = parts_num2 - 1;

    while (i > 1 && num1[i] == 0) {i--;}
    while (j > 1 && num2[j] == 0) {j--;}

    if (i > j) {return 1;}
    if (i < j) {return -1;}

    while (i >= 1) {
        if (num1[i] > num2[i]) {return 1;}
        if (num1[i] < num2[i]) {return -1;}
        i--;
    }

    return 0;
}

unsigned short * sum(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int * out_size) { //проблема переполнения
    int res_size = (parts_num1 > parts_num2) ? parts_num1 : parts_num2;
    res_size+= 1;
    unsigned short * res = calloc(res_size, sizeof (unsigned short));
    res[0] = num1[0];
    unsigned short carry = 0;

    for (int i = 1; i < res_size; i++) {
        unsigned short d1 = (i < parts_num1) ? num1[i] : 0;
        unsigned short d2 = (i < parts_num2) ? num2[i] : 0;
        unsigned int sum = d1 + d2 + carry;

        res[i] = (unsigned short)(sum % BASE);
        carry = sum / BASE;
    }
    if (res_size > 2 && res[res_size - 1] == 0) {
        res_size--;
    }
    *out_size = res_size;
    return res;
}

unsigned short * diff(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int *out_size) {
    int res_size = (parts_num1 > parts_num2)? parts_num1 : parts_num2;
    unsigned short *res = calloc(res_size, sizeof (unsigned short));
    unsigned short borrow = 0;
    for (int i = 1; i < res_size; i++) {
        unsigned short d1 = (i < parts_num1) ? num1[i] : 0;
        unsigned short d2 = (i < parts_num2) ? num2[i] : 0;
        int diff = d1 - d2 - borrow;
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {borrow = 0;}
        res[i] = (unsigned short)diff;
    }
    while (res_size > 2 && res[res_size - 1] == 0) {
        res_size--;
    }
    *out_size = res_size;
    return res;
}

unsigned short* sub(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int *out_size) {
    int sign_is_equal = (num1[0] == num2[0]) ? 1 : 0;
    if (!sign_is_equal) {
        return sum(num1, parts_num1, num2, parts_num2, out_size);
    }
    if (compare(num1, parts_num1, num2, parts_num2) == 1) {
        return diff(num1, parts_num1, num2, parts_num2, out_size);
    }
    if(compare(num1, parts_num1, num2, parts_num2) == -1) {
        return diff(num2, parts_num2, num1, parts_num1, out_size);
    }
    unsigned short * res = calloc(2, sizeof (unsigned short));
    *out_size = 0;
    return res;
}

unsigned short * add(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int *out_size) {
    int sign_is_equal = (num1[0] == num2[0]) ? 1 : 0;
    if (sign_is_equal) {
        return sum(num1, parts_num1, num2, parts_num2, out_size);
    }
    if (compare(num1, parts_num1, num2, parts_num2) == 1) {
        return diff(num1, parts_num1, num2, parts_num2, out_size);
    }
    if(compare(num1, parts_num1, num2, parts_num2) == -1) {
        return diff(num2, parts_num2, num1, parts_num1, out_size);
    }
    unsigned short * res = calloc(2, sizeof (unsigned short));
    *out_size = 0;
    return res;
}

unsigned short * multiply(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int *out_size) {
    int size = parts_num1  + parts_num2;

    unsigned short *result = calloc(size, sizeof (unsigned short));

    result[0] = num1[0] == num2[0] ? 0 : 1;

    for (int i = 1; i < parts_num1; i++) {
        unsigned int carry = 0;
        for (int j = 1; j < parts_num2; j++) {
            unsigned long long cur = (unsigned long long)num1[i] * num2[j] + result[i + j - 1] + carry;
            result[i + j - 1] = (unsigned short)(cur % BASE);
            carry = cur / BASE;
        }
        result[i + parts_num2 - 1] += carry;
    }

    int true_size = size;
    while (true_size > 2 && result[true_size - 1] == 0) {
        true_size--;
    }

    if (true_size == 2 && result[1] == 0) {
        result[0] = 0;
    }

    *out_size = true_size;
    return result;
}

unsigned short * power(unsigned short *num1, int parts_num1, unsigned int exp, int *out_size) {
    char flag = 0;
    if (exp % 2 == 1 && num1[0] == 1) {
        flag = 1;
    }
    int res_size;
    unsigned short *res  = malloc(2 * sizeof (unsigned short));
    res[0] = 0;
    res[1] = 1;

    if (exp == 0) {
        *out_size = 2;
        return res;
    }

    int size = parts_num1;
    unsigned short *num = malloc(size * sizeof (unsigned short));
    for (int i = 0; i < parts_num1; i++) {
        num[i] = num1[i];
    }
    num[0] = 0;

    while (exp > 0) {
        if (exp % 2 == 1) {
            int new_size = 0;
            unsigned short *new_num = multiply(res, res_size, num, size, &new_size);
            free(res);
            res = new_num;
        }

        exp /= 2;
        if (exp > 0) {
            int next_size = 0;
            unsigned short *next_num = multiply(num, size, num, size, &next_size);
            free(num);
            num = next_num;
        }
    }

    if (flag == 1) {
        res[0] = 1;
    }

    *out_size = size;
    return res;
}

unsigned short * factorial(unsigned short *num1, int parts_num1, int *out_size) {
    int res_size = 2;
    unsigned short *res = malloc(3 * sizeof (unsigned short));
    unsigned int n = num1[1];
    res[0] = 0;
    res[1] = 1;
    if (num1[0] == 1) {
        printf("Факториал отрицательного числа неопределён!\n");
        *out_size = res_size;
        return res;
    }
    int too_big = 0;
    for (int i = 2; i < parts_num1; i++) {
        if (num1[i] != 0) {
            too_big = 1;
            break;
        }
    }
    if (n > 810 || too_big) {
        printf("Число для вычисления слишком большое!\n");
        *out_size = res_size;
        return res;
    }
    printf("%d\n", n);
    for (int i = 2; i <= n; i++) {
        unsigned short mult[3];
        int size = 0;

        mult[0] = 0;
        if (i < BASE) {
            mult[1] = (unsigned short)i;
            size = 2;
        } else {
            mult[1] = (unsigned short)(i % BASE);
            mult[2] = (unsigned short)(i / BASE);
            size = 3;
        }

        int new_size = 0;
        unsigned short *new_num = multiply(res, res_size, mult, size, &new_size);

        free(res);
        res = new_num;
        res_size = new_size;
    }

    *out_size = res_size;
    return res;
}

void devide_by_two(unsigned short *num, int parts_num1) {
    unsigned int carry = 0;
    for (int i = parts_num1 - 1; i >= 1; i++) {
        int cur = (carry * BASE) + num[i];
        num[i] = (unsigned short)(cur / 2);
        carry = cur % 2;
    }
}
/**
unsigned short * row(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2, int *out_size) {
    unsigned short *res = malloc(2 * sizeof (unsigned short));
    int res_size = 2;
    if (parts_num1 > 1000 || parts_num2 > 1000) {
        printf("Слишком большие числа!\n");
        *out_size = 2;
        return res;
    }

    //Реализовать функцию сравнения, т.к. если число A > B, то начало отрезка больше его конца и их надо поменять местами

    unsigned short *num1copy = malloc(2 * sizeof (unsigned short));
    num1copy = num1;
    int size_num_copy = add(num1copy, 2, num2, parts_num2);



}
**/
int main(void) {
    char num[] = "-2";
    size_t len = strlen(num);
    int size = (len / 4) + 2;
    unsigned short *digits = malloc(size * sizeof(unsigned short));
    splitter(num, digits, size);
    test_print(digits, size);
    unsigned short test_num[3] = {1,100, 0};
    digits = add(digits, size, test_num, 3, &size);
    //digits = power(digits, size, 3, &size);
    test_print(digits, size);
}