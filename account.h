#ifndef TICKET_SYSTEM_ACCOUNT_H
#define TICKET_SYSTEM_ACCOUNT_H

#include <cstring>
#include "B_plus_tree.h"

class account {
#pragma pack(push, 1)

    class User_name {
    public:
        char username[21];

        User_name(const char username_[]) { strcpy(username, username_); }

        User_name() {}

        bool operator<(const User_name &obj) const { return strcmp(username, obj.username) < 0; }
    };

    class User_info {
    public:
        char password[31];
        char name[16];
        char mail_addr[31];
        int privilege;

        User_info(const char password_[], const char name_[],
                  const char mail_addr_[], int privilege_) {
            strcpy(password, password_);
            strcpy(name, name_);
            strcpy(mail_addr, mail_addr_);
            privilege = privilege_;
        }

        User_info() {}

        bool operator<(const User_info &obj) const {
            int comp;
            comp = strcmp(password, obj.password);
            if (!comp) { return comp < 0; }
            comp = strcmp(name, obj.name);
            if (!comp) { return comp < 0; }
            comp = strcmp(mail_addr, obj.mail_addr);
            if (!comp) { return comp < 0; }
            return privilege < obj.privilege;
        }
    };

#pragma pack(pop)

    class info_operator {
    public:
        char user_name[21], cur_user_name[21];

        class User_info user_info;

        bool print = true;
        int cur_privilege = -1;
        bool flag[4];

        void find(const User_info &user_info_) {
            user_info = user_info_;
            if (print) {
                if (cur_privilege > user_info.privilege || strcmp(user_name, cur_user_name) == 0) {
                    printf("%s %s %s %d\n", user_name, user_info.name,
                           user_info.mail_addr, user_info.privilege);
                } else {
                    printf("-1\n");
                    throw operator_failed();
                }
            }
        }

        void not_find() {
            if (print) { printf("-1\n"); }
            throw operator_failed();
        }

        void modify(User_info &user_info_) {
            if (cur_privilege > user_info_.privilege || strcmp(cur_user_name, user_name) == 0) {
                if (flag[0]) { strcpy(user_info_.password, user_info.password); }
                if (flag[1]) { strcpy(user_info_.name, user_info.name); }
                if (flag[2]) { strcpy(user_info_.mail_addr, user_info.mail_addr); }
                if (flag[3]) { user_info_.privilege = user_info.privilege; }
                printf("%s %s %s %d\n", user_name, user_info_.name,
                       user_info_.mail_addr, user_info_.privilege);
            } else {
                printf("-1\n");
                throw operator_failed();
            }
        }
    };

private:
    static const int M = 4096;
    B_plus_tree<User_name, User_info, M, info_operator> user_account;
    bool is_empty;
public:
    account() : user_account("account", false) {
        is_empty = user_account.is_empty();
    }

    ~account() {}

    void add_user(const char user_name_[], const char password_[], const char name_[],
                  const char mail_addr_[], int privilege_) {
        try {
            user_account.insert(User_name(user_name_),
                                User_info(password_, name_, mail_addr_, privilege_));
        } catch (repeated_key) {
            printf("-1\n");
            return;
        }
        printf("0\n");
        return;
    }

    bool find_user(const char user_name_[], bool print_ = true, int privilege_ = -1) {
        user_account.Info_operator.print = print_;
        user_account.Info_operator.cur_privilege = privilege_;
        try { user_account.find(User_name(user_name_)); }
        catch (operator_failed) { return false; }
        return true;
    }

    bool modify_user(const char user_name_[], int privilege_) {
        user_account.Info_operator.cur_privilege = privilege_;
        try { user_account.modify(user_name_); }
        catch (operator_failed) { return false; }
        return true;
    }

    bool get_password_and_privilege(const char user_name_[], char password_[], int &privilege) {
        try { find_user(user_name_, false); }
        catch (other_error) { return false; }
        privilege = user_account.Info_operator.user_info.privilege;
        strcpy(password_, user_account.Info_operator.user_info.password);
        return true;
    }

    void set_user(const char cur_user_name_[], const char user_name_[]) {
        strcpy(user_account.Info_operator.cur_user_name, cur_user_name_);
        strcpy(user_account.Info_operator.user_name, user_name_);
    }

    void modify_user_password(const char password_[]) {
        user_account.Info_operator.flag[0] = true;
        strcpy(user_account.Info_operator.user_info.password, password_);
    }

    void modify_user_name(const char name_[]) {
        user_account.Info_operator.flag[1] = true;
        strcpy(user_account.Info_operator.user_info.name, name_);
    }

    void modify_user_mail(const char mail_addr_[]) {
        user_account.Info_operator.flag[2] = true;
        strcpy(user_account.Info_operator.user_info.mail_addr, mail_addr_);
    }

    void modify_user_privilege(int privilege_) {
        user_account.Info_operator.flag[3] = true;
        user_account.Info_operator.user_info.privilege = privilege_;
    }

    inline bool empty() { return is_empty; }

    void change_empty() { is_empty = false; }

    void reset_modify() {
        for (int i = 0; i <= 3; ++i) { user_account.Info_operator.flag[i] = false; }
    }
};


#endif //TICKET_SYSTEM_ACCOUNT_H
