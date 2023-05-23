#ifndef TICKET_SYSTEM_LOG_H
#define TICKET_SYSTEM_LOG_H

#include "account.h"

class log {
    class User_name {
    public:
        char username[21];

        User_name(const char username_[]) { strcpy(username, username_); }

        User_name() {}

        bool operator==(User_name &obj) {
            return strcmp(username, obj.username) == 0;
        }
    };

    class hash {
    private:
        int base = 10007;
        int max;
    public:
        hash(int max_) {
            max = max_;
        }

        int operator()(User_name user_name) {
            unsigned int out = 0;
            for (int i = 0; i < strlen(user_name.username); ++i) {
                out = out * base + user_name.username[i];
            }
            return out % max;
        }

        int not_find_tag() { return -1; }
    };

private:
    account Account;
    hash_link<User_name, int, 100003, hash> log_account;
public:
    log() : Account(), log_account() {}

    void add_user(char cur_user_name_[], int privilege_, char user_name_[],
                  char password_[], char name_[], char mail_addr_[]) {
        if (Account.empty()) {
            Account.add_user(user_name_, password_, name_, mail_addr_, 10);
            Account.change_empty();
        } else {
            int cur_privilege = log_account.find(User_name(cur_user_name_));
            if (cur_privilege == -1 || cur_privilege <= privilege_) { printf("-1\n"); }//当前账户未登录或者权限不足
            else { Account.add_user(user_name_, password_, name_, mail_addr_, privilege_); }
        }
    }

    void login(char user_name_[], char password_[]) {
        char correct_password[31];
        int privilege;
        if (!Account.get_password_and_privilege(user_name_, correct_password, privilege)) {
            printf("-1\n");//账户不存在
        } else {
            if (strcmp(correct_password, password_) != 0) { printf("-1\n"); }//密码错误
            else if (log_account.find(User_name(user_name_)) != -1) { printf("-1\n"); }//已经登录
            else {
                log_account.insert(User_name(user_name_), privilege);
                printf("0\n");//正常登录
            }
        }
    }

    void logout(char user_name_[]) {
        if (log_account.find(User_name(user_name_)) != -1) {
            log_account.erase(User_name(user_name_));
            printf("0\n");//正常登出
        } else { printf("-1\n"); }//未登录
    }

    void query_profile(char cur_user_name_[], char user_name_[]) {
        int cur_privilege = log_account.find(User_name(cur_user_name_));
        if (cur_privilege == -1) { printf("-1\n"); }//未登录
        else {
            Account.set_user(cur_user_name_, user_name_);//设置要查询的账户
            Account.find_user(user_name_, true, cur_privilege);
        }
    }

    void modify_profile(char cur_user_name_[], char user_name_[], bool flag[],
                        char password_[], char name_[], char mail_addr_[], int privilege_) {
        int cur_privilege = log_account.find(User_name(cur_user_name_));
        if (cur_privilege == -1) { printf("-1\n"); }//未登录
        else if (flag[3] && privilege_ >= cur_privilege) { printf("-1\n"); }//权限错误
        else {
            Account.set_user(cur_user_name_, user_name_);
            Account.reset_modify();
            if (flag[0]) { Account.modify_user_password(password_); }
            if (flag[1]) { Account.modify_user_name(name_); }
            if (flag[2]) { Account.modify_user_mail(mail_addr_); }
            if (flag[3]) { Account.modify_user_privilege(privilege_); }
            if (Account.modify_user(user_name_, cur_privilege) && flag[3]) {//检查已登录账户的权限
                log_account.modify(user_name_, privilege_);
            }
        }
    }

    bool check_login(char user_name_[]) {
        int cur_privilege = log_account.find(User_name(user_name_));
        if (cur_privilege == -1) { return false; }//未登录
        else { return true; }
    }
};

#endif //TICKET_SYSTEM_LOG_H
