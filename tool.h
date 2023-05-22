#ifndef TICKET_SYSTEM_TOOL_H
#define TICKET_SYSTEM_TOOL_H

#include <cstring>
#include "exceptions.h"

int char_to_int(char to_transfer[]) {
    int tmp = 0, l = strlen(to_transfer);
    for (int i = 0; i < l; ++i) {
        if (to_transfer[i] >= '0' && to_transfer[i] <= '9') {
            tmp = tmp * 10 + (to_transfer[i] - '0');
        } else { throw other_error(); }//输入不合法
    }
    return tmp;
}

template<int size>
void parse_by_bar(char to_transfer[], char target[][size]) {
    int j = 0, k = 0, l = strlen(to_transfer);
    for (int i = 0; i < l; ++i) {
        if (to_transfer[i] == '|') {
            target[j++][k] = '\0';
            k = 0;
        } else { target[j][k++] = to_transfer[i]; }
    }
    target[j][k] = '\0';
}

//将to_transfer按'|'切分，并将每段翻译成int，target维护前缀和
//target[0]已经存好初值
//例如: to_transfer[]= "10|2|6", target[0] = 0
//那么: target[] = { 0, 10, 12, 18 }
void parse_by_bar_to_sum(char to_transfer[], int target[]) {
    int tmp = 0, j = 1, l = strlen(to_transfer);
    for (int i = 0; i < l; ++i) {
        if (to_transfer[i] == '|') {
            target[j] = target[j - 1] + tmp;
            ++j;
            tmp = 0;
        } else { tmp = tmp * 10 + (to_transfer[i] - '0'); }
    }
    target[j] = target[j - 1] + tmp;
}

//将to_transfer按'|'切分，并将每段翻译成int，target维护前缀和
//交替维护target1,target2
//target1首项，target2的首项和末项已经存好初值
//例如: to_transfer1[]= "10|2|6", to_transfer2[] = "1|13", target1[0] = target2[3] = -1, target2[0] = 1
//那么: target1[] = { -1, 11, 14, 33 }
//     target2[] = { 1, 12, 27, -1 }
void parse_by_bar_to_sum_with_two(char to_transfer1[], char to_transfer2[],
                                  int target1[], int target2[]) {
    int tmp1 = 0, tmp2 = 0, i1 = 0, i2 = 0, j1 = 1, j2 = 1;
    while (to_transfer1[i1] != '\0') {
        while (to_transfer1[i1] != '\0' && to_transfer1[i1] != '|') {
            tmp1 = tmp1 * 10 + to_transfer1[i1++] - '0';
        }
        target1[j1] = target2[j1 - 1] + tmp1;
        tmp1 = 0;
        ++i1, ++j1;
        while (to_transfer2[i2] != '\0' && to_transfer2[i2] != '|') {
            tmp2 = tmp2 * 10 + to_transfer2[i2++] - '0';
        }
        target2[j2] = target1[j2] + tmp2;
        tmp2 = 0;
        ++i2, ++j2;
    }
}

void parse_by_bar_to_date(char to_transfer[], int &target1, int &target2) {
    target1 = (to_transfer[4] + 10 * to_transfer[3] + 100 * to_transfer[1]) - 111 * '0';
    target2 = (to_transfer[10] + 10 * to_transfer[9] + 100 * to_transfer[7]) - 111 * '0';
}

int time_to_int(char to_transfer[]) {
    return (to_transfer[4] + 10 * to_transfer[3] + 60 * to_transfer[1] + 600 * to_transfer[0]) - 671 * '0';
}

int date_to_int(char to_transfer[]) {
    return (to_transfer[4] + 10 * to_transfer[3] + 100 * to_transfer[1]) - 111 * '0';
}

template<class date>
void print_date_and_time(date date_, int time_) {
    if (time_ == -1) {
        printf("xx-xx xx:xx");
        return;
    }
    (date_ + time_ / 1440).print();
    time_ = time_ % 1440;
    printf(" %02d%c%02d", time_ / 60, ':', time_ % 60);
}


#endif //TICKET_SYSTEM_TOOL_H
