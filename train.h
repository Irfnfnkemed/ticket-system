#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include "B_plus_tree.h"
#include "tool.h"
#include "vector.h"
#include <cstring>

class train {
    friend class ticket;

public:
#pragma pack(push, 1)

    class date {
    public:
        int date_int;

        date() {};

        date(int date_int_) { date_int = date_int_; }

        date &operator++() {
            if (date_int == 630) { date_int = 701; }
            else if (date_int == 731) { date_int = 801; }
            else if (date_int == 831) { date_int = 901; }
            else { ++date_int; }
            return *this;
        }

        date &operator--() {
            if (date_int == 701) { date_int = 630; }
            else if (date_int == 801) { date_int = 731; }
            else if (date_int == 901) { date_int = 831; }
            else { --date_int; }
            return *this;
        }

        date operator+(int add) {
            date tmp = *this;
            for (int i = 1; i <= add; ++i) { ++tmp; }
            return tmp;
        }

        date operator-(int minus) {
            date tmp = *this;
            for (int i = 1; i <= minus; ++i) { --tmp; }
            return tmp;
        }

        int operator-(const date &obj) {
            date tmp = *this;
            int out = 0;
            if (tmp < obj) {
                while (tmp.date_int != obj.date_int) {
                    ++tmp;
                    --out;
                }
            } else {
                while (tmp.date_int != obj.date_int) {
                    --tmp;
                    ++out;
                }
            }
            return out;
        }

        bool operator<(const date &obj) const { return date_int < obj.date_int; }

        void print() { printf("%c%d%c%02d", '0', date_int / 100, '-', date_int % 100); }
    };

    class train_ID {
    public:
        char train_id[21];

        train_ID() {}

        train_ID(char train_id_[]) { strcpy(train_id, train_id_); }

        bool operator<(const train_ID &obj) const { return strcmp(train_id, obj.train_id) < 0; }

        bool operator==(train_ID &obj) {
            return strcmp(train_id, obj.train_id) == 0;
        }

        inline char *get_content() { return train_id; }
    };

    class train_info {
    public:
        bool release = false;
        char type;
        int station_num;
        int seat_num;
        char stations[100][31];
        int sum_price[100];
        int arrive_time[100];
        int leave_time[100];
        date begin_date;//对于日期，采用一个三位数的int来记录。比如：07-21，用721记录
        date end_date;

        train_info() {}

        train_info(int station_num_, int seat_num_, char stations_[], char prices_[],
                   char start_time_[], char travel_times_[], char stopover_times_[],
                   char sale_date[], char type_[]) {
            release = false;
            type = type_[0];
            station_num = station_num_;
            seat_num = seat_num_;
            parse_by_bar<31>(stations_, stations);
            sum_price[0] = 0;
            parse_by_bar_to_sum(prices_, sum_price);
            leave_time[0] = time_to_int(start_time_);
            parse_by_bar_to_sum_with_two(travel_times_, stopover_times_, arrive_time, leave_time);
            arrive_time[0] = leave_time[station_num - 1] = -1;
            parse_by_bar_to_date(sale_date, begin_date.date_int, end_date.date_int);
        }

        bool operator<(const train_info &obj) const { return true; }//由于ID不会重复，此函数仅为形式上的函数
    };

    class ID_and_date {
    public:
        char train_id[21];
        date train_date;

        ID_and_date() {}

        ID_and_date(char train_id_[], date train_date_) {
            strcpy(train_id, train_id_);
            train_date = train_date_;
        }

        bool operator<(const ID_and_date &obj) const {
            int comp = strcmp(train_id, obj.train_id);
            if (comp != 0) { return comp < 0; }
            return train_date < obj.train_date;
        }
    };

    class train_seat {
    public:
        int station_num;
        int seat[99];//最多100站，99个区间

        train_seat() {}

        train_seat(int station_num_, int seat_) {
            station_num = station_num_;
            for (int i = 0; i < station_num - 1; ++i) { seat[i] = seat_; }
        }

        bool operator<(const train_seat &obj) const { return true; }//由于键值不会重复，因此仅是形式上的函数
    };

    class station {
    public:
        char station_name[31];

        station() {}

        station(char station_name_[]) { strcpy(station_name, station_name_); }

        bool operator<(const station &obj) const {
            return strcmp(station_name, obj.station_name) < 0;
        }

        bool operator==(const station &obj) const {
            return strcmp(station_name, obj.station_name) == 0;
        }

        inline char *get_content() { return station_name; }
    };

    class pass {
    public:
        char train_id[21];
        date train_date_begin;
        date train_date_end;
        int arrive_time;
        int leave_time;
        int price;
        int number;

        pass() {}

        pass(char train_id_[], date train_date_beg_, date train_date_end_,
             int arrive_time_, int leave_time_, int price_, int number_) {
            strcpy(train_id, train_id_);
            train_date_begin = train_date_beg_;
            train_date_end = train_date_end_;
            arrive_time = arrive_time_;
            leave_time = leave_time_;
            price = price_;
            number = number_;
        }

        bool operator<(const pass &obj) const {
            return strcmp(train_id, obj.train_id) < 0;
        }
    };

#pragma pack(pop)

    class train_info_operator {
    public:
        train_info Train_info;
        bool fail_tag = false;

        void find(const train_info &obj, bool &tmp) { Train_info = obj; }

        void not_find() { fail_tag = true; }

        void modify(train_info &obj, bool &tmp) {
            Train_info = obj;
            if (obj.release) { fail_tag = true; }//已经释放，不可再次释放
            obj.release = true;
        }

        void set_fail_tag() { fail_tag = false; }
    };

    class train_seat_operator {
    public:
        train_seat *Train_seat;
        int beg;
        int end;
        int change;//从beg到end需要改变的票数
        bool fail_tag = false;

        void find(train_seat &obj, bool &tmp) { Train_seat = &obj; }

        void not_find() { fail_tag = true; }

        void modify(train_seat &obj, bool &tmp) {
            Train_seat = &obj;
            if (change < 0 && get_seats(beg, end) < -change) { fail_tag = true; }
            else { change_seats(beg, end, change); }
        }


        int get_seats(int beg_, int end_) const {
            int max_seats = Train_seat->seat[beg_];
            for (int i = beg_ + 1; i < end_; ++i) {
                if (Train_seat->seat[i] < max_seats) { max_seats = Train_seat->seat[i]; }
            }
            return max_seats;
        }

        void change_seats(int beg_, int end_, int change_) {
            for (int i = beg_; i < end_; ++i) {
                Train_seat->seat[i] += change_;
            }
        }

        void set_change(int beg_, int end_, int change_) {
            beg = beg_;
            end = end_;
            change = change_;
            fail_tag = false;
        }
    };

    class pass_operator {
    public:
        vector<pass> pass_train;
        bool fail_tag = false;

        void find(const pass &obj, bool &tmp) { pass_train.push_back(obj); }

        void not_find() { fail_tag = true; }

        void modify(pass &obj, bool &tmp) {}

        void set_fail_tag() {
            fail_tag = false;
            pass_train.clear();
        }
    };

    class query_info {
    public:
        int leave_time;
        int price;
        int beg;

        query_info() {}

        query_info(int leave_time_, int price_, int beg_) {
            leave_time = leave_time_;
            price = price_;
            beg = beg_;
        }

        static query_info not_find() { return query_info(-1, -1, -1); }
    };

    template<class from, class to>
    class hash {
    private:
        int base = 10007;
        int max;
    public:
        hash(int max_) {
            max = max_;
        }

        int operator()(from from_) {
            unsigned int out = 0;
            char *tmp = from_.get_content();
            for (int i = 0; i < strlen(tmp); ++i) {
                out = out * base + tmp[i];
            }
            return out % max;
        }

        to not_find_tag() { return to::not_find(); }
    };

    class possible_train {
    public:
        char id[21];
        int leave_time;
        int arrive_time;
        int price;
        int beg;
        int end;
        static bool flag;//为true，以time排序；为false，以cost排序

        possible_train(char id_[], int leave_time_, int arrive_time_,
                       int price_, int beg_, int end_) {
            strcpy(id, id_);
            leave_time = leave_time_;
            arrive_time = arrive_time_;
            price = price_;
            beg = beg_;
            end = end_;
        }

        static void set_time() { flag = true; }

        static void set_cost() { flag = false; }

        bool operator<(possible_train &obj) {
            if (flag) {
                if (arrive_time - leave_time != obj.arrive_time - obj.leave_time) {
                    return arrive_time - leave_time < obj.arrive_time - obj.leave_time;
                } else { return strcmp(id, obj.id) < 0; }
            } else {
                if (price != obj.price) { return price < obj.price; }
                else { return strcmp(id, obj.id) < 0; }
            }
        }
    };

    class mid_element {
    public:
        char train_id[21];
        int leave_time = -1;//从起始站出发时间
        int arrive_time = -1;//到达中间站的时间
        int price;
        int beg;
        int end;

        mid_element() {}

        mid_element(char train_id_[], int leave_time_, int arrive_time_,
                    int price_, int beg_, int end_) {
            strcpy(train_id, train_id_);
            leave_time = leave_time_;
            arrive_time = arrive_time_;
            price = price_;
            beg = beg_;
            end = end_;
        }
    };

    class possible_mid {
    public:
        vector<mid_element> possible_mid_vec;

        possible_mid() {}

        possible_mid(const possible_mid &obj) { possible_mid_vec = obj.possible_mid_vec; }

        static possible_mid not_find() { return possible_mid(); }
    };

    class best_transfer {
    public:
        char train_id_1[21];
        char train_id_2[21];
        int leave_time_1;
        int arrive_time_1;
        int leave_time_2;
        int arrive_time_2;
        int price_1;
        int price_2;
        char mid_station[31];
        int beg_1;
        int end_1;
        int beg_2;
        int end_2;
        date train_1_date;//第一辆车从其始发站出发的日期
        date train_2_date;//第二辆车从其始发站出发的日期

        static bool flag;//为true，以time排序；为false，以cost排序

        best_transfer() {
            train_1_date.date_int = 701;
            train_2_date.date_int = 701;
            price_1 = price_2 = 1e8;
            leave_time_1 = 0;
            arrive_time_2 = 1e8;
        }

        static void set_time() { flag = true; }

        static void set_cost() { flag = false; }

        bool operator<(best_transfer &obj) {
            int cmp;
            if (flag) {
                cmp = (train_2_date - train_1_date) * 1440 + (arrive_time_2 - leave_time_1) -
                      (obj.train_2_date - obj.train_1_date) * 1440 - (obj.arrive_time_2 - obj.leave_time_1);
                if (cmp != 0) { return cmp < 0; }
                cmp = (price_1 + price_2) - (obj.price_1 + obj.price_2);
                if (cmp != 0) { return cmp < 0; }
            } else {
                cmp = (price_1 + price_2) - (obj.price_1 + obj.price_2);
                if (cmp != 0) { return cmp < 0; }
                cmp = (train_2_date - train_1_date) * 1440 + (arrive_time_2 - leave_time_1) -
                      (obj.train_2_date - obj.train_1_date) * 1440 - (obj.arrive_time_2 - obj.leave_time_1);
                if (cmp != 0) { return cmp < 0; }
            }
            cmp = strcmp(train_id_1, obj.train_id_1);
            if (cmp != 0) { return cmp < 0; }
            return strcmp(train_id_2, obj.train_id_2) < 0;
        }

        void set(char train_id_1_[], char train_id_2_[],
                 int leave_time_1_, int arrive_time_1_, int leave_time_2_, int arrive_time_2_,
                 int price_1_, int price_2_, char mid_station_[],
                 int beg_1_, int end_1_, int beg_2_, int end_2_,
                 date train_1_date_, date train_2_date_) {
            strcpy(train_id_1, train_id_1_);
            strcpy(train_id_2, train_id_2_);
            leave_time_1 = leave_time_1_;
            arrive_time_1 = arrive_time_1_;
            leave_time_2 = leave_time_2_;
            arrive_time_2 = arrive_time_2_;
            price_1 = price_1_;
            price_2 = price_2_;
            strcpy(mid_station, mid_station_);
            beg_1 = beg_1_;
            end_1 = end_1_;
            beg_2 = beg_2_;
            end_2 = end_2_;
            train_1_date = train_1_date_;
            train_2_date = train_2_date_;
        }
    };

    B_plus_tree<train_ID, train_info, 26100, train_info_operator> Trains;
    B_plus_tree<ID_and_date, train_seat, 3500, train_seat_operator> Train_seats;
    B_plus_tree<station, pass, 4096, pass_operator> Stations;
    hash_link<train_ID, query_info, 1007, hash<train_ID, query_info>> tmp_hash;
    vector<possible_train> tmp_vec;
    hash_link<station, possible_mid, 1007, hash<station, possible_mid>> tmp_mid_hash;
public:

    train() : Trains("trains", false), Train_seats("seats", false), Stations("stations", true) {}

    bool find_train(char train_id_[]) {
        Trains.Info_operator.set_fail_tag();
        Trains.find(train_id_);
        if (Trains.Info_operator.fail_tag) { return false; }
        return true;
    }

    train_info &get_train_info() { return Trains.Info_operator.Train_info; }

    bool change_seats(char train_id_[], date date_, int beg, int end, int change) {
        Train_seats.Info_operator.set_change(beg, end, change);
        Train_seats.modify(ID_and_date(train_id_, date_));
        if (Train_seats.Info_operator.fail_tag) { return false; }
        return true;
    }

    int get_max_seats(int beg_, int end_) {
        return Train_seats.Info_operator.get_seats(beg_, end_);
    }

    void change_this_seats(int beg_, int end_, int change_) {
        Train_seats.Info_operator.change_seats(beg_, end_, change_);
    }

    void add_train(char train_id_[], int station_num_, int seat_num_, char stations_[],
                   char prices_[], char start_time_[], char travel_times_[],
                   char stopover_times_[], char sale_date_[], char type_[]) {
        try {
            Trains.insert(train_ID(train_id_),
                          train_info(station_num_, seat_num_, stations_, prices_, start_time_,
                                     travel_times_, stopover_times_, sale_date_, type_));
        } catch (repeated_key) {
            printf("-1\n");
            return;//插入失败
        }
        printf("0\n");//插入成功
    }

    void delete_train(char train_id_[]) {
        Trains.Info_operator.set_fail_tag();
        Trains.find(train_id_);
        if (Trains.Info_operator.fail_tag) {
            printf("-1\n");//无列车
            return;
        }
        if (Trains.Info_operator.Train_info.release) {
            printf("-1\n");//已发布
            return;
        } else { Trains.erase(train_ID(train_id_), Trains.Info_operator.Train_info); }
        printf("0\n");
    }

    void release_train(char train_id_[]) {
        Trains.Info_operator.set_fail_tag();
        Trains.modify(train_id_);
        if (Trains.Info_operator.fail_tag) {
            printf("-1\n");//已经发布或者不存在
            return;
        }
        for (date day = Trains.Info_operator.Train_info.begin_date;
             !(Trains.Info_operator.Train_info.end_date < day); ++day) {
            Train_seats.insert(ID_and_date(train_id_, day),
                               train_seat(Trains.Info_operator.Train_info.station_num,
                                          Trains.Info_operator.Train_info.seat_num));//添加座位
        }
        for (int i = 0; i < Trains.Info_operator.Train_info.station_num; ++i) {
            Stations.insert(station(Trains.Info_operator.Train_info.stations[i]),
                            pass(train_id_, Trains.Info_operator.Train_info.begin_date,
                                 Trains.Info_operator.Train_info.end_date,
                                 Trains.Info_operator.Train_info.arrive_time[i],
                                 Trains.Info_operator.Train_info.leave_time[i],
                                 Trains.Info_operator.Train_info.sum_price[i], i));//添加经过各站的火车
        }
        printf("0\n");
        return;
    }

    void query_ticket(char station_start_[], char station_to_[],
                      char date_[], bool sort_) {//sort为true，按时间排序；反之，按价格排序
        Stations.Info_operator.set_fail_tag();//重置
        Stations.find(station(station_start_));
        if (Stations.Info_operator.fail_tag) {
            printf("0\n");//无过站车辆，结束
            return;
        }
        date this_day = date(date_to_int(date_));
        tmp_hash.clear();
        for (auto it = Stations.Info_operator.pass_train.begin();
             it != Stations.Info_operator.pass_train.end(); ++it) {
            if (this_day < it->train_date_begin + it->leave_time / 1440 ||
                it->train_date_end + it->leave_time / 1440 < this_day) {
                continue;//不在指定时间内
            }
            tmp_hash.insert(train_ID(it->train_id),
                            query_info(it->leave_time, it->price, it->number));
        }
        Stations.Info_operator.set_fail_tag();//重置
        Stations.find(station(station_to_));
        if (Stations.Info_operator.fail_tag) {
            printf("0\n");//无过站车辆，结束
            return;
        }
        tmp_vec.clear();
        query_info tmp;
        for (auto it = Stations.Info_operator.pass_train.begin();
             it != Stations.Info_operator.pass_train.end(); ++it) {
            tmp = tmp_hash.find(train_ID(it->train_id));
            if (tmp.leave_time != -1 && tmp.beg < it->number) {//将可行解放入
                tmp_vec.push_back(possible_train(it->train_id, tmp.leave_time,
                                                 it->arrive_time, it->price - tmp.price,
                                                 tmp.beg, it->number));
            }
        }
        if (tmp_vec.empty()) {
            printf("0\n");//无满足的车辆，结束
            return;
        }
        if (sort_) { possible_train::set_time(); }
        else { possible_train::set_cost(); }
        tmp_vec.sort();
        printf("%d\n", tmp_vec.size());
        for (auto it = tmp_vec.begin(); it != tmp_vec.end(); ++it) {
            printf("%s %s ", it->id, station_start_);
            print_date_and_time(this_day - it->leave_time / 1440, it->leave_time);
            printf(" -> ");
            printf("%s ", station_to_);
            print_date_and_time(this_day - it->leave_time / 1440, it->arrive_time);
            printf(" %d ", it->price);
            Train_seats.find(ID_and_date(it->id, this_day - it->leave_time / 1440));
            printf("%d\n", Train_seats.Info_operator.get_seats(it->beg, it->end));
        }
    }

    void query_train(char train_id_[], char date_[]) {
        Trains.Info_operator.set_fail_tag();
        Trains.find(train_ID(train_id_));
        if (Trains.Info_operator.fail_tag) {
            printf("-1\n");//未找到
            return;
        }
        date this_day = date(date_to_int(date_));
        if (this_day < Trains.Info_operator.Train_info.begin_date ||
            Trains.Info_operator.Train_info.end_date < this_day) {
            printf("-1\n");//不在发车区间内
            return;
        }
        if (Trains.Info_operator.Train_info.release) {
            Train_seats.find(ID_and_date(train_id_, this_day));
        }
        printf("%s %c\n", train_id_, Trains.Info_operator.Train_info.type);
        for (int i = 0; i < Trains.Info_operator.Train_info.station_num; ++i) {
            printf("%s ", Trains.Info_operator.Train_info.stations[i]);
            print_date_and_time(this_day, Trains.Info_operator.Train_info.arrive_time[i]);
            printf(" -> ");
            print_date_and_time(this_day, Trains.Info_operator.Train_info.leave_time[i]);
            printf(" %d", Trains.Info_operator.Train_info.sum_price[i]);
            if (i != Trains.Info_operator.Train_info.station_num - 1) {
                if (Trains.Info_operator.Train_info.release) {
                    printf(" %d\n", Train_seats.Info_operator.Train_seat->seat[i]);
                } else { printf(" %d\n", Trains.Info_operator.Train_info.seat_num); }
            } else { printf(" x\n"); }
        }
    }

    void query_transfer(char station_start_[], char station_to_[],
                        char date_[], bool sort_) {//sort为true，按时间排序；反之，按价格排序
        Stations.Info_operator.set_fail_tag();//重置
        Stations.find(station(station_start_));
        if (Stations.Info_operator.fail_tag) {
            printf("0\n");//无过站车辆，结束
            return;
        }
        date this_day = date(date_to_int(date_));
        tmp_mid_hash.clear();
        for (auto it = Stations.Info_operator.pass_train.begin();
             it != Stations.Info_operator.pass_train.end(); ++it) {
            if (this_day < it->train_date_begin + it->leave_time / 1440 ||
                it->train_date_end + it->leave_time / 1440 < this_day) {
                continue;//不在售票区间
            }
            Trains.find(it->train_id);
            for (int i = it->number + 1; i < Trains.Info_operator.Train_info.station_num; ++i) {
                if (tmp_mid_hash.find(station(Trains.Info_operator.Train_info.stations[i])).possible_mid_vec.empty()) {
                    tmp_mid_hash.insert(station(Trains.Info_operator.Train_info.stations[i]), possible_mid());
                }
                vector<mid_element> &tmp_mid_vec =
                        tmp_mid_hash.find(station(Trains.Info_operator.Train_info.stations[i])).possible_mid_vec;
                tmp_mid_vec.push_back(mid_element(it->train_id, it->leave_time,
                                                  Trains.Info_operator.Train_info.arrive_time[i],
                                                  Trains.Info_operator.Train_info.sum_price[i] - (it->price),
                                                  it->number, i));//加入对应的中间站信息
            }
        }
        Stations.Info_operator.set_fail_tag();//重置
        Stations.find(station(station_to_));
        if (Stations.Info_operator.fail_tag) {
            printf("0\n");//无过站车辆，结束
            return;
        }
        if (sort_) { best_transfer::set_time(); }
        else { best_transfer::set_cost(); }
        best_transfer best, best_tmp;
        bool next_day;
        date first_day, mid_day;//第一辆、第二辆车从其始发站出发的日期
        for (auto it = Stations.Info_operator.pass_train.begin();
             it != Stations.Info_operator.pass_train.end(); ++it) {
            Trains.find(it->train_id);
            for (int i = 0; i < it->number; ++i) {
                vector<mid_element> &tmp_mid_vec =
                        tmp_mid_hash.find(station(Trains.Info_operator.Train_info.stations[i])).possible_mid_vec;
                if (tmp_mid_vec.empty()) { continue; }//无对应的中间站
                for (auto it_bef = tmp_mid_vec.begin(); it_bef != tmp_mid_vec.end(); ++it_bef) {
                    if (strcmp(it_bef->train_id, it->train_id) == 0) { continue; }//不可换乘同一辆车
                    first_day = this_day - it_bef->leave_time / 1440;
                    if (it_bef->arrive_time % 1440 > Trains.Info_operator.Train_info.leave_time[i] % 1440) {
                        next_day = true;
                    } else { next_day = false; }
                    if (it->train_date_end + Trains.Info_operator.Train_info.leave_time[i] / 1440 <
                        first_day + it_bef->arrive_time / 1440 + next_day) {
                        continue;//不在售票区间内
                    } else if (first_day + it_bef->arrive_time / 1440 + next_day <
                               it->train_date_begin + Trains.Info_operator.Train_info.leave_time[i] / 1440) {
                        mid_day = it->train_date_begin;
                    } else {
                        mid_day = first_day + (it_bef->arrive_time) / 1440 + next_day -
                                  Trains.Info_operator.Train_info.leave_time[i] / 1440;
                    }
                    best_tmp.set(it_bef->train_id, it->train_id,
                                 it_bef->leave_time, it_bef->arrive_time,
                                 Trains.Info_operator.Train_info.leave_time[i], it->arrive_time,
                                 it_bef->price, it->price - Trains.Info_operator.Train_info.sum_price[i],
                                 Trains.Info_operator.Train_info.stations[i],
                                 it_bef->beg, it_bef->end, i, it->number,
                                 first_day, mid_day);
                    if (best_tmp < best) { best = best_tmp; }
                }
            }
        }
        if (best.price_1 == 1e8) {
            printf("0\n");
            return;
        }
        Train_seats.find(ID_and_date(best.train_id_1, best.train_1_date));
        int seat_1 = get_max_seats(best.beg_1, best.end_1);
        Train_seats.find(ID_and_date(best.train_id_2, best.train_2_date));
        int seat_2 = get_max_seats(best.beg_2, best.end_2);
        printf("%s %s ", best.train_id_1, station_start_);
        print_date_and_time(best.train_1_date, best.leave_time_1);
        printf(" -> ");
        printf("%s ", best.mid_station);
        print_date_and_time(best.train_1_date, best.arrive_time_1);
        printf(" %d %d\n", best.price_1, seat_1);
        printf("%s %s ", best.train_id_2, best.mid_station);
        print_date_and_time(best.train_2_date, best.leave_time_2);
        printf(" -> ");
        printf("%s ", station_to_);
        print_date_and_time(best.train_2_date, best.arrive_time_2);
        printf(" %d %d\n", best.price_2, seat_2);
    }

    void clean() {
        Trains.clean();
        Train_seats.clean();
        Stations.clean();
    }

};

#endif //TICKET_SYSTEM_TRAIN_H
