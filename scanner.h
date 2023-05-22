#ifndef TICKET_SYSTEM_SCANNER_H
#define TICKET_SYSTEM_SCANNER_H

#include <iostream>
#include <cstring>
#include "log.h"
#include "train.h"
#include "tool.h"

class token_scanner {
private:
    char command_line[5000];//表示处理的字符串
    int now = -1;//表示现在处理的字符串的位置
    int len = -1;//表示处理的字符串的长度
    char load[5000];//用于承载切片

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

    char *get_now() { return load; }

    void read_by_command(char tmp[30][5000]) {
        char cnt;
        while (next_token()) {
            cnt = load[1] - 'a';
            next_token();
            strcpy(tmp[cnt], load);
        }
    };
};

class main_scanner {
private:
    token_scanner Token_scanner;
    log Log;
    train Train;
    char tmp[30][5000];//作为临时变量
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
        int ttt = char_to_int(aaa);
        ttt;
        printf("%s ", Token_scanner.get_now());//输出时间戳
        Token_scanner.next_token();
        if (strcmp(Token_scanner.get_now(), "add_user") == 0) {
            Token_scanner.read_by_command(tmp);
            Log.add_user(tmp['c' - 'a'], char_to_int(tmp['g' - 'a']), tmp['u' - 'a'],
                         tmp['p' - 'a'], tmp['n' - 'a'], tmp['m' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "login") == 0) {
            Token_scanner.read_by_command(tmp);
            Log.login(tmp['u' - 'a'], tmp['p' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "logout") == 0) {
            Token_scanner.read_by_command(tmp);
            Log.logout(tmp['u' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "query_profile") == 0) {
            Token_scanner.read_by_command(tmp);
            Log.query_profile(tmp['c' - 'a'], tmp['u' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "modify_profile") == 0) {
            char cnt;
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
            if (flag[3]) { privilege = char_to_int(tmp['g' - 'a']); }
            else { privilege = -1; }
            Log.modify_profile(tmp['c' - 'a'], tmp['u' - 'a'], flag, tmp['p' - 'a'],
                               tmp['n' - 'a'], tmp['m' - 'a'], privilege);
        } else if (strcmp(Token_scanner.get_now(), "add_train") == 0) {
            Token_scanner.read_by_command(tmp);
            Train.add_train(tmp['i' - 'a'], char_to_int(tmp['n' - 'a']), char_to_int(tmp['m' - 'a']),
                            tmp['s' - 'a'], tmp['p' - 'a'], tmp['x' - 'a'], tmp['t' - 'a'],
                            tmp['o' - 'a'], tmp['d' - 'a'], tmp['y' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "delete_train") == 0) {
            Token_scanner.read_by_command(tmp);
            Train.delete_train(tmp['i' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "release_train") == 0) {
            Token_scanner.read_by_command(tmp);
            Train.release_train(tmp['i' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "query_train") == 0) {
            Token_scanner.read_by_command(tmp);
            Train.query_train(tmp['i' - 'a'], tmp['d' - 'a']);
        } else if (strcmp(Token_scanner.get_now(), "exit") == 0) {
            printf("bye\n");
            end = true;
        }
    }

    inline bool is_end() { return end; }

};

#endif //TICKET_SYSTEM_SCANNER_H
