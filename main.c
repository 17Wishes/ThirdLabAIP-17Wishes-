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

void printer(unsigned short* num, int size) {
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
    while (j > 1 && num1[j] == 0) {j--;}

    if (i > j) {return 1;}
    if (i < j) {return -1;}

    while (i >= 1) {
        if (num1[i] > num2[i]) {return 1;}
        if (num1[i] < num2[i]) {return -1;}
        i--;
    }

    return 0;
}

int sum(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2) { //проблема переполнения
    unsigned short carry = 0;
    int i = 1;
    while (i < parts_num1 || i < parts_num2 || carry > 0) {
        unsigned short d1 = (i < parts_num1) ? num1[i] : 0;
        unsigned short d2 = (i < parts_num2) ? num2[i] : 0;
        unsigned int sum = d1 + d2 + carry;

        num1[i] = (unsigned short)(sum % BASE);

        carry = sum / BASE;
        i++;
    }
    return parts_num1;
}

int diff(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2) {
    unsigned short borrow = 0;
    for (int i = 1; i < parts_num1; i++) {
        unsigned short d2 = (i < parts_num2) ? num2[i] : 0;
        int diff = num1[i] - d2 - borrow;
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {borrow = 0;}
        num1[i] = (unsigned short)diff;
    }

    return parts_num1;
}

int sub(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2) {
     if (num1[0] != num2[0]) {
         return sum(num1, parts_num1, num2, parts_num2);
     }
    int compare_result = compare(num1, parts_num1, num2, parts_num2);
    if (compare_result > 0) {
        return diff(num1, parts_num1, num2, parts_num2);
    }
    else if (compare_result == 0) {
        num1[0] = 0;
        num1[1] = 0;
        return 2;
    }
    else {
        int new_len = diff(num2, parts_num2, num1, parts_num1);

        for (int i = 1 ;i < new_len; i++) {
            num1[i] = num2[i];
        }

        num1[0] = (num1[0] == 0) ? 1 : 0;
        return new_len;
    }
}

int add(unsigned short *num1, int parts_num1, unsigned short *num2, int parts_num2) {
    num2[0] = num2[0] == 0 ? 1 : 0;
    int len = sub(num1, parts_num1, num2, parts_num2);
    num2[0] = (num2[0] == 0) ? 1 : 0;
    return len;
}

int main(void) {
    char num[] = "-3";
    size_t len = strlen(num);
    int size = (len / 4) + 2;
    unsigned short *digits = malloc(size * sizeof(unsigned short));
    splitter(num, digits, size);
    test_print(digits, size);
    unsigned short test_num[2] = {0, 5  };
    size = add(digits, size, test_num, 2);
    test_print(digits, size);
}