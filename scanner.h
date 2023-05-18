#ifndef TICKET_SYSTEM_SCANNER_H
#define TICKET_SYSTEM_SCANNER_H

#include <iostream>
#include <cstring>
#include "log.h"

class token_scanner {
private:
    char command_line[500];//表示处理的字符串
    int now = -1;//表示现在处理的字符串的位置
    int len = -1;//表示处理的字符串的长度
    char load[500];//用于承载切片

    void read_line() {
        int cur_char, i = 0;
        while (true) {
            cur_char = getchar();
            if (cur_char == EOF || cur_char == '\n') {//读到行末或文件结尾EOF
                command_line[i] = '\0';
                break;
            }
            command_line[i] = char(cur_char);
            ++i;
        }
    }

public:

    //读入一行命令语句
    void scan() {
        read_line();
        now = 0;
        len = strlen(command_line);
    }

    //以空格为分隔符，获得下一个片段
    bool next_token() {
        if (now == -1) {//空
            load[0] = '\0';
            return false;
        }
        if (now == len) {//读到末尾
            command_line[0] = '\0';
            now = len = -1;
            load[0] = '\0';
            return false;
        }
        int i = 0;
        while (command_line[now] == ' ') { ++now; }
        while (command_line[now] != ' ' && now < len) { load[i++] = command_line[now++]; }
        load[i] = '\0';
        return true;
    }

    int to_int(char target[]) {
        int tmp = 0;
        for (int i = 0; i < strlen(target); ++i) {
            if (target[i] >= '0' && target[i] <= '9') {
                tmp = tmp * 10 + (target[i] - '0');
            } else { throw other_error(); }//输入不合法
        }
        return tmp;
    }

    char *get_now() { return load; }
};

class main_scanner {
private:
    token_scanner Token_scanner;
    log Log;
    char tmp[30][500];//作为临时变量
    bool end = false;
public:
    void deal_line() {
        Token_scanner.scan();
        Token_scanner.next_token();
        char aaa[100];
        for (int i = 1; i < strlen(Token_scanner.get_now()); ++i) {
            aaa[i - 1] = Token_scanner.get_now()[i];
            if (i == strlen(Token_scanner.get_now()) - 1) {
                aaa[i - 1] = '\0';
            }
        }
        int ttt = Token_scanner.to_int(aaa);
        ttt;
        printf("%s ", Token_scanner.get_now());//输出时间戳
        Token_scanner.next_token();
        int cnt;
        if (strcmp(Token_scanner.get_now(), "add_user") == 0) {
            while (Token_scanner.next_token()) {
                cnt = (Token_scanner.get_now())[1] - 'a';
                Token_scanner.next_token();
                strcpy(tmp[cnt], Token_scanner.get_now());
            }
            Log.add_user(tmp['c' - 'a'], Token_scanner.to_int(tmp['g' - 'a']), tmp['u' - 'a'],
                         tmp['p' - 'a'], tmp['n' - 'a'], tmp['m' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "login") == 0) {
            while (Token_scanner.next_token()) {
                cnt = (Token_scanner.get_now())[1] - 'a';
                Token_scanner.next_token();
                strcpy(tmp[cnt], Token_scanner.get_now());
            }
            Log.login(tmp['u' - 'a'], tmp['p' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "logout") == 0) {
            while (Token_scanner.next_token()) {
                cnt = (Token_scanner.get_now())[1] - 'a';
                Token_scanner.next_token();
                strcpy(tmp[cnt], Token_scanner.get_now());
            }
            Log.logout(tmp['u' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "query_profile") == 0) {
            while (Token_scanner.next_token()) {
                cnt = (Token_scanner.get_now())[1] - 'a';
                Token_scanner.next_token();
                strcpy(tmp[cnt], Token_scanner.get_now());
            }
            Log.query_profile(tmp['c' - 'a'], tmp['u' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "modify_profile") == 0) {
            bool flag[4] = {false, false, false, false};
            while (Token_scanner.next_token()) {
                cnt = (Token_scanner.get_now())[1] - 'a';
                switch (cnt) {
                    case 'p' - 'a':
                        flag[0] = true;
                        break;
                    case 'n' - 'a':
                        flag[1] = true;
                        break;
                    case 'm' - 'a':
                        flag[2] = true;
                        break;
                    case 'g' - 'a':
                        flag[3] = true;
                }
                Token_scanner.next_token();
                strcpy(tmp[cnt], Token_scanner.get_now());
            }
            int privilege;
            if (flag[3]) { privilege = Token_scanner.to_int(tmp['g' - 'a']); }
            else { privilege = -1; }
            Log.modify_profile(tmp['c' - 'a'], tmp['u' - 'a'], flag, tmp['p' - 'a'],
                               tmp['n' - 'a'], tmp['m' - 'a'], privilege);
        } else if (strcmp(Token_scanner.get_now(), "exit") == 0) {
            printf("bye\n");
            end = true;
        }
    }

    inline bool is_end() { return end; }
};

#endif //TICKET_SYSTEM_SCANNER_H