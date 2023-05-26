#include "scanner.h"

bool train::possible_train::flag = true;
bool train::best_transfer::flag = true;

int main() {
    main_scanner a;
    while (!a.is_end()) { a.deal_line(); }
    return 0;
}