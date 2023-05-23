#ifndef TICKET_SYSTEM_TICKET_H
#define TICKET_SYSTEM_TICKET_H

#include <cstring>
#include "log.h"
#include "train.h"

class ticket {
private:


    log *Log;
    train *Train;

#pragma pack(push, 1)

    class user {
    public:
        char username[21];

        user(const char username_[]) { strcpy(username, username_); }

        user() {}

        bool operator<(const user &obj) const { return strcmp(username, obj.username) < 0; }
    };

    class user_order {
    public:
        int time_stamp;
        char train_id[21];
        char station_from[31];
        char station_to[31];
        train::date train_date;
        int leave_time;
        int arrive_time;
        int price;
        int number;
        int status;//为1，表success；为2，表pending；为3，表refunded

        user_order(int time_stamp_, char train_id_[], char station_from_[], char station_to_[],
                   train::date train_date_, int leave_time_, int arrive_time_,
                   int price_, int number_, int status_) {
            time_stamp = time_stamp_;
            strcpy(train_id, train_id_);
            strcpy(station_from, station_from_);
            strcpy(station_to, station_to_);
            train_date = train_date_;
            leave_time = leave_time_;
            arrive_time = arrive_time_;
            price = price_;
            number = number_;
            status = status_;
        }

        user_order() {}

        bool operator<(const user_order &obj) const {
            return time_stamp < obj.time_stamp;
        }
    };

#pragma pack(pop)

    class user_order_operation {
        void find(const user_order &obj) {}

        void not_find() { throw operator_failed(); }

        void modify(user_order &obj) {
        }
    };

    B_plus_tree<user, user_order, 4096 * 5, user_order_operation> Orders;
public:
    ticket(log *Log_, train *Train_) : Orders("order", true) {
        Log = Log_;
        Train = Train_;
    }

    void buy_ticket(char user_name_[], char train_id_[], char date_[], char station_from_[],
                    char station_to_[], int number_, int time_stamp_, bool ac_refund = false) {
        if (!Log->check_login(user_name_)) {
            printf("-1\n");//用户未登录
            return;
        }
        if (!Train->find_train(train_id_)) {
            printf("-1\n");//火车不存在
            return;
        }
        train::train_info info = Train->get_train_info();
        if (!info.release) {
            printf("-1\n");//火车未释放
            return;
        }
        int beg = -1, end = -1;
        for (int i = 0; i < info.station_num; ++i) {
            if (beg == -1 || strcmp(station_from_, info.stations[i]) == 0) {
                beg = i;
            }
            if (end == -1 || strcmp(station_to_, info.stations[i]) == 0) {
                end = i;
            }
        }
        if (beg == -1 || end == -1 || beg >= end) {
            printf("-1\n");//站名错误
            return;
        }
        train::date this_day = train::date(date_to_int(date_));
        if (this_day < info.begin_date + info.leave_time[beg] / 1440 ||
            info.end_date + info.leave_time[beg] / 1440 < this_day) {
            printf("-1\n");//不在售票区间
            return;
        }
        int max_seats = 0;
        if (Train->change_seats(train_id_, this_day - info.leave_time[beg] / 1440,
                                beg, end, -number_, max_seats)) {
            //购票成功
            Orders.insert(user(user_name_),
                          user_order(time_stamp_, train_id_, station_from_, station_to_,
                                     this_day - info.leave_time[beg] / 1440,
                                     info.leave_time[beg], info.arrive_time[end],
                                     info.sum_price[end] - info.sum_price[beg], number_, 1));
            printf("%d\n", number_ * (info.sum_price[end] - info.sum_price[beg]));
        } else if (!ac_refund) {
            printf("-1\n");//购票失败
        } else {
            //加入候补队列
            Orders.insert(user(user_name_),
                          user_order(time_stamp_, train_id_, station_from_, station_to_,
                                     this_day - info.leave_time[beg] / 1440,
                                     info.leave_time[beg], info.arrive_time[end],
                                     info.sum_price[end] - info.sum_price[beg], number_, 2));
            printf("queue\n");
        }
    }

};

#endif //TICKET_SYSTEM_TICKET_H
