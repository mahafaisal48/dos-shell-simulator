#pragma once

struct letter
{
    char ch;
    letter* next;
    letter(char c) : ch(c), next(nullptr) {}
    char get_ch()
    {
        return ch;
    }
    letter* get_next()
    {
        return next;
    }
    void set_ch(char ch2)
    {
        ch = ch2;
    }
};