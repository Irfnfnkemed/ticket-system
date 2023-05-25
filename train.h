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
    };

    template<class to>
    class hash {
    private:
        int base = 10007;
        int max;
    public:
        hash(int max_) {
            max = max_;
        }

        int operator()(train_ID id) {
            unsigned int out = 0;
            for (int i = 0; i < strlen(id.train_id); ++i) {
                out = out * base + id.train_id[i];
            }
            return out % max;
        }

        to not_find_tag() { return to(-1, -1, -1); }
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

    B_plus_tree<train_ID, train_info, 4096 * 20, train_info_operator> Trains;
    B_plus_tree<ID_and_date, train_seat, 4096 * 5, train_seat_operator> Train_seats;
    B_plus_tree<station, pass, 4096 * 5, pass_operator> Stations;
    hash_link<train_ID, query_info, 107, hash<query_info>> tmp_hash;
    vector<possible_train> tmp_vec;

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
        try { Trains.find(train_ID(train_id_)); }
        catch (operator_failed) {
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
};

#endif //TICKET_SYSTEM_TRAIN_H
