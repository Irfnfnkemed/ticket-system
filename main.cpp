#include "scanner.h"

bool train::possible_train::flag = true;
bool train::best_transfer::flag = true;

int main() {
    freopen("datas/pressure_3_hard/1.in", "r", stdin);
    freopen("ass", "w", stdout);

    main_scanner a;
    while (!a.is_end()) { a.deal_line(); }
    return 0;
}