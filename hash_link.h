#ifndef TICKET_SYSTEM_HASH_LINK_H
#define TICKET_SYSTEM_HASH_LINK_H

#include <cstdio>

//实现从Key类到Value类的映射
//需要有最大的容量max，以及用于hash散列的仿函数类hash
//另外，hash类需要有Value not_find_tag()函数，用于标记未找到的情形
template<class Key, class Value, int max, class hash>
class hash_link {//实现从size_t到cache_node的散列表
private:
    struct node {
        Key key;
        Value value;
        node *next;

        node(Key key_, Value value_, node *next_) {
            key = key_;
            value = value_;
            next = next_;
        }
    };

    node *link[max];//用取模来实现散列
    hash Hash;
    Value not_find_tag;

public:
    hash_link() : Hash(max) {
        for (int i = 0; i < max; ++i) { link[i] = nullptr; }
        not_find_tag = Hash.not_find_tag();
    }

    ~hash_link() { clear(); }

    void insert(Key key_, Value value_) {
        int pos = Hash(key_);
        link[pos] = new node(key_, value_, link[pos]);
    }

    void erase(Key key_) {
        int pos = Hash(key_);
        node *p_pre = nullptr, *p = link[pos];
        while (p != nullptr) {
            if (p->key == key_) {
                if (p_pre != nullptr) { p_pre->next = p->next; }
                else { link[pos] = p->next; }
                delete p;
                return;
            } else {
                p_pre = p;
                p = p->next;
            }
        }
    }

    Value &find(Key key_) {
        node *p = link[Hash(key_)];
        while (p != nullptr) {
            if (p->key == key_) {
                return p->value;
            } else { p = p->next; }
        }
        return not_find_tag;
    }

    bool modify(Key key_, Value value_) {
        node *p = link[Hash(key_)];
        while (p != nullptr) {
            if (p->key == key_) {
                p->value = value_;
                return true;
            } else { p = p->next; }
        }
        return false;
    }

    void clear() {
        node *p;
        for (int i = 0; i < max; ++i) {
            if (link[i] == nullptr) { continue; }
            else {
                while (true) {
                    p = link[i];
                    if (p == nullptr) { break; }
                    link[i] = link[i]->next;
                    delete p;
                }
            }
        }
    }
};

#endif //TICKET_SYSTEM_HASH_LINK_H
