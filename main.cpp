//#include "B_plus_tree.h"
//#include "exceptions.h"
//#include <iostream>
//#include <cstring>
//#include "log.h"
#include "scanner.h"
#include "train.h"

class v {
public:
    int vv;

    v(int vv_) { vv = vv_; }

    v() {}

    bool operator<(const v &obj) const { return vv < obj.vv; }
};


class k {
public:
    char kk[65];

    k(const char o[]) { strcpy(kk, o); }

    k() {}

    bool operator<(const k &obj) const { return strcmp(kk, obj.kk) < 0; }
};

class find_operator {
public:
    class v A;

    void find(v aa) {
        A = aa;
        std::cout << aa.vv << ' ';
    }

    void not_find() { std::cout << "null"; }

    void modify(v &aa) { aa = A; }

    void check(v &aa) {

    }
};


const int M = 4096;


bool train::possible_train::flag = true;

int main() {
//    freopen("datas/pressure_1_easy/1.in", "r", stdin);
//    freopen("aaa", "w", stdout);

    main_scanner a;
    while (!a.is_end()) {
        try { a.deal_line(); }
        catch (exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

//    log aaaa;


    //B_plus_snapshot_tree<k, v, M, find_operator> a("a1", "a2", "a3");
//    freopen("data", "r", stdin);
//    freopen("qqq", "w", stdout);
//    B_plus_tree<k, v, M, find_operator> a("a", true);
//    int n;
//    scanf("%d", &n);
//    char insert[65], command[64];
//    int value;
//    for (int ii = 1; ii <= n; ++ii) {
//        scanf("%s", command);
//        try {
//            if (strcmp("insert", command) == 0) {
//                scanf("%s%d", insert, &value);
//                a.insert(k(insert), v{value});
//            } else if (strcmp("delete", command) == 0) {
//                scanf("%s%d", insert, &value);
//                a.erase(k(insert), v{value});
//            } else if (strcmp("find", command) == 0) {
//                scanf("%s", insert);
//                a.find(k(insert));
//                std::cout << std::endl;
//            } else if (strcmp("modify", command) == 0) {
////                scanf("%s", insert);
////                scanf("%d", &value);
////                a.Info_operator.A = v(value);
////                a.modify(k(insert));
////                std::cout << std::endl;
//            }
//        } catch (exception &e) {
//            std::cout << e.what() << std::endl;
//        }
//    }
    return 0;
}
