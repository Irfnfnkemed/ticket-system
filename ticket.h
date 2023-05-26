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
        int beg;
        int end;
        int leave_time;
        int arrive_time;
        int price;
        int number;
        int status;//为1，表success；为2，表pending；为3，表refunded

        user_order(int time_stamp_, char train_id_[], char station_from_[], char station_to_[],
                   train::date train_date_, int beg_, int end_, int leave_time_,
                   int arrive_time_, int price_, int number_, int status_) {
            time_stamp = time_stamp_;
            strcpy(train_id, train_id_);
            strcpy(station_from, station_from_);
            strcpy(station_to, station_to_);
            train_date = train_date_;
            beg = beg_;
            end = end_;
            leave_time = leave_time_;
            arrive_time = arrive_time_;
            price = price_;
            number = number_;
            status = status_;
        }

        user_order() {}

        bool operator<(const user_order &obj) const {
            return time_stamp > obj.time_stamp;
        }
    };

    class pending {
    public:
        char username[21];
        int time_stamp;
        int beg;
        int end;
        int number;

        pending() {}

        pending(char username_[], int time_stamp_, int beg_, int end_, int number_) {
            strcpy(username, username_);
            time_stamp = time_stamp_;
            beg = beg_;
            end = end_;
            number = number_;
        }

        bool operator<(const pending &obj) const { return time_stamp < obj.time_stamp; }
    };

#pragma pack(pop)

    class user_order_operation {
    public:
        vector<user_order> orders;
        int number;
        int mode;
        //mode为1，表示将找到的所有目标都按顺序放入orders中
        //mode为2，将第number个放入orders中，并同时改变其状态为refunded
        //mode为3，将对应的一项的状态由pending改为success
        //mode为0，表示find不应继续下去，抛出错误
        bool fail_tag = false;

        void find(user_order &obj, bool &tmp) {
            if (mode == 1) { orders.push_back(obj); }
            else if (mode == 2) {
                --number;
                if (number == 0) {
                    orders.push_back(obj);
                    if (obj.status == 3) {//无法退订
                        fail_tag = true;
                        return;
                    }
                    obj.status = 3;
                    mode = 0;
                    tmp = true;//终止B+树
                }
            } else if (mode == 3) {
                if (obj.time_stamp == number && obj.status == 2) {
                    obj.status = 1;
                    mode = 0;
                    tmp = true;//终止B+树
                }
            }
        }

        void not_find() { fail_tag = true; }

        void modify(user_order &obj, bool &tmp) {}

        void set_find_all() {
            orders.clear();
            mode = 1;
            fail_tag = false;
        }

        void set_find_n(int n_) {
            orders.clear();
            mode = 2;
            number = n_;
            fail_tag = false;
        }

        void set_find_special(int time_stamp_) {
            orders.clear();
            mode = 3;
            number = time_stamp_;
            fail_tag = false;
        }

    };

    class pending_operation {
    public:
        vector<pending> all_pending;

        void find(const pending &obj, bool &tmp) { all_pending.push_back(obj); }

        void not_find() {}

        void modify(pending &obj, bool &tmp) {}
    };

    B_plus_tree<user, user_order, 4096, user_order_operation> Orders;
    B_plus_tree<train::ID_and_date, pending, 4096, pending_operation> Pending;

public:
    ticket(log *Log_, train *Train_) : Orders("order", true), Pending("pending", true) {
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
            if (beg == -1 && strcmp(station_from_, info.stations[i]) == 0) {
                beg = i;
            }
            if (end == -1 && strcmp(station_to_, info.stations[i]) == 0) {
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
        if (Train->change_seats(train_id_, this_day - info.leave_time[beg] / 1440,
                                beg, end, -number_)) {//购票成功
            Orders.insert(user(user_name_),
                          user_order(time_stamp_, train_id_, station_from_, station_to_,
                                     this_day - info.leave_time[beg] / 1440, beg, end,
                                     info.leave_time[beg], info.arrive_time[end],
                                     info.sum_price[end] - info.sum_price[beg], number_, 1));
            printf("%d\n", number_ * (info.sum_price[end] - info.sum_price[beg]));
        } else if (!ac_refund) {
            printf("-1\n");//购票失败
        } else {
            if (info.seat_num < number_) {
                printf("-1\n");//超过上限，失败
                return;
            }
            //加入候补队列
            Pending.insert(train::ID_and_date(train_id_, this_day - info.leave_time[beg] / 1440),
                           pending(user_name_, time_stamp_, beg, end, number_));
            Orders.insert(user(user_name_),
                          user_order(time_stamp_, train_id_, station_from_, station_to_,
                                     this_day - info.leave_time[beg] / 1440, beg, end,
                                     info.leave_time[beg], info.arrive_time[end],
                                     info.sum_price[end] - info.sum_price[beg], number_, 2));
            printf("queue\n");
        }
    }

    void query_order(char user_name_[]) {
        if (!Log->check_login(user_name_)) {
            printf("-1\n");//用户未登录
            return;
        }
        Orders.Info_operator.set_find_all();//重置
        Orders.find(user(user_name_));
        if (Orders.Info_operator.fail_tag) {
            printf("0\n");//无对应信息
            return;
        }
        printf("%d\n", Orders.Info_operator.orders.size());
        for (auto it = Orders.Info_operator.orders.begin();
             it != Orders.Info_operator.orders.end(); ++it) {
            switch (it->status) {
                case 1:
                    printf("[success] ");
                    break;
                case 2:
                    printf("[pending] ");
                    break;
                case 3:
                    printf("[refunded] ");
            }
            printf("%s %s ", it->train_id, it->station_from);
            print_date_and_time(it->train_date, it->leave_time);
            printf(" -> ");
            printf("%s ", it->station_to);
            print_date_and_time(it->train_date, it->arrive_time);
            printf(" %d %d\n", it->price, it->number);
        }
    }

    void refund_ticket(char user_name_[], int n_) {
        if (!Log->check_login(user_name_)) {
            printf("-1\n");//用户未登录
            return;
        }
        Orders.Info_operator.set_find_n(n_);//设置模式
        Orders.find(user(user_name_));
        if (Orders.Info_operator.fail_tag) {
            printf("-1\n");//无订单，或者并不是交易成功的记录
            return;
        }
        if (Orders.Info_operator.orders.size() == 0) {
            printf("-1\n");//无n_条交易记录
            return;
        }
        user_order this_order = Orders.Info_operator.orders[0];
        if (this_order.status == 1) {//需要修改车票，并满足候补队列的要求
            Train->change_seats(this_order.train_id, this_order.train_date,
                                this_order.beg, this_order.end, this_order.number);//改变座位
            Pending.Info_operator.all_pending.clear();
            Pending.find(train::ID_and_date(this_order.train_id, this_order.train_date));
            if (Pending.Info_operator.all_pending.size() != 0) {
                for (auto it = Pending.Info_operator.all_pending.begin();
                     it != Pending.Info_operator.all_pending.end(); ++it) {
                    if (Train->get_max_seats(it->beg, it->end) >= it->number) {
                        Train->change_this_seats(it->beg, it->end, -(it->number));
                        Pending.erase(train::ID_and_date(this_order.train_id, this_order.train_date), *it);
                        Orders.Info_operator.set_find_special(it->time_stamp);
                        Orders.find(user(it->username)); //此处，find充当修改的功能，将状态由pending改为success
                    }
                }
            }
        } else if (this_order.status == 2) {//需要从等待队列里删去
            Pending.erase(train::ID_and_date(this_order.train_id, this_order.train_date),
                          pending(user_name_, this_order.time_stamp,
                                  this_order.beg, this_order.end, this_order.number));
        }
        printf("0\n");
    }

    void clean() {
        Orders.clean();
        Pending.clean();
    }
};

#endif //TICKET_SYSTEM_TICKET_H
