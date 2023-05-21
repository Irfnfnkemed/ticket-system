#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include "B_plus_tree.h"
#include "tool.h"
#include "cstring"

class train {
private:
#pragma pack(push, 1)

    class train_ID {
    public:
        char train_id[21];

        train_ID() {}

        train_ID(char train_id_[]) { strcpy(train_id, train_id_); }

        bool operator<(const train_ID &obj) const { return strcmp(train_id, obj.train_id) < 0; }
    };

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

        date &operator+=(int add) {
            for (int i = 1; i <= add; ++i) { ++(*this); }
            return *this;
        }

        bool operator<(const date &obj) const { return date_int < obj.date_int; }
    };

    class train_info {
    public:
        bool release = false;
        char type;
        int station_num;
        int seat_num;
        char stations[101][31];
        int sum_price[101];
        int arrive_time[101];
        int leave_time[101];
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
        int seat[100];//最多100站，99个区间

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

    class ID_and_sale_date {
    public:
        char train_id[21];
        date train_date_begin;
        date train_date_end;

        ID_and_sale_date() {}

        ID_and_sale_date(char train_id_[], date train_date_beg_, date train_date_end_) {
            strcpy(train_id, train_id_);
            train_date_begin = train_date_beg_;
            train_date_end = train_date_end_;
        }

        bool operator<(const ID_and_sale_date &obj) const {
            return strcmp(train_id, obj.train_id) < 0;
        }
    };

#pragma pack(pop)

    class train_info_operator {
    public:
        char Train_id[21];
        train_info Train_info;

        void find(const train_info &obj) { Train_info = obj; }

        void not_find() { throw operator_failed(); }

        void modify(train_info &obj) {
            Train_info = obj;
            if (obj.release) { throw operator_failed(); }//已经释放，不可再次释放
            obj.release = true;
        }
    };

    class train_seat_operator {
    public:
        char Train_id[21];
        train_info Train_info;

        void find(const train_info &obj) { Train_info = obj; }

        void not_find() { throw operator_failed(); }

        void modify(train_info &obj) {
            Train_info = obj;
            if (obj.release) { throw operator_failed(); }//已经释放，不可再次释放
            obj.release = true;
        }
    };

    class ID_and_sale_date_operator {
    public:
        char Train_id[21];
        train_info Train_info;

        void find(const train_info &obj) { Train_info = obj; }

        void not_find() { throw operator_failed(); }

        void modify(train_info &obj) {
            Train_info = obj;
            if (obj.release) { throw operator_failed(); }//已经释放，不可再次释放
            obj.release = true;
        }
    };

    B_plus_tree<train_ID, train_info, 4096 * 20, train_info_operator> Trains;
    B_plus_tree<ID_and_date, train_seat, 4096 * 5, train_seat_operator> Train_seats;
    B_plus_tree<station, ID_and_sale_date, 4096 * 5, ID_and_sale_date_operator> Stations;

public:

    train() : Trains("trains", false), Train_seats("seats", false), Stations("stations", true) {}

    void add_train(char train_id_[], int station_num_, int seat_num_, char stations_[],
                   char prices_[], char start_time_[], char travel_times_[],
                   char stopover_times_[], char sale_date[], char type_[]) {
        try {
            Trains.insert(train_ID(train_id_),
                          train_info(station_num_, seat_num_, stations_, prices_, start_time_,
                                     travel_times_, stopover_times_, sale_date, type_));
        } catch (repeated_key) {
            printf("-1\n");
            return;//插入失败
        }
        printf("0\n");//插入成功
    }

    void delete_train(char train_id_[]) {
        try { Trains.find(train_id_); }
        catch (operator_failed) {
            printf("-1\n");//无列车
            return;
        }
        if (Trains.Info_operator.Train_info.release) { printf("-1\n"); }//已发布
        else { Trains.erase(train_ID(train_id_), Trains.Info_operator.Train_info); }
    }

    void release_train(char train_id_[]) {
        try { Trains.modify(train_id_); }
        catch (operator_failed) {
            printf("-1\n");//已经发布或者不存在
            return;
        }
        for (date day = Trains.Info_operator.Train_info.begin_date;
             !(Trains.Info_operator.Train_info.end_date < day); ++day) {
            Train_seats.insert(ID_and_date(train_id_, day),
                               train_seat(Trains.Info_operator.Train_info.station_num,
                                          Trains.Info_operator.Train_info.seat_num));//添加座位
        }
        date beg, end;
        for (int i = 0; i < Trains.Info_operator.Train_info.station_num - 1; ++i) {
            beg = Trains.Info_operator.Train_info.begin_date;
            end = Trains.Info_operator.Train_info.end_date;
            beg += (Trains.Info_operator.Train_info.leave_time[i]) / 86400;
            end += (Trains.Info_operator.Train_info.leave_time[i]) / 86400;
            Stations.insert(station(Trains.Info_operator.Train_info.stations[i]),
                            ID_and_sale_date(train_id_, beg, end));//添加经过各站的火车
        }
        printf("0\n");
        return;
    }

};

#endif //TICKET_SYSTEM_TRAIN_H
