#pragma once
#include <queue>
#include <string>
#include <ctime>
#include <iostream>
#include "node.h"
using namespace std;

class print
{
    Node* file_to_print;
    int time_remaining;
    int arrival_time_stamp;
    int priority;
public:
    print(Node* temp_file, int temp_time, int temp_arrival, int temp_priority) :file_to_print(temp_file), time_remaining(temp_time), arrival_time_stamp(temp_arrival), priority(temp_priority)
    {
    }
    Node* get_fileToPrint()const
    {
        return file_to_print;
    }
    int get_time()const
    {
        return time_remaining;
    }
    int get_arrival()const
    {
        return arrival_time_stamp;
    }
    int get_priority()const
    {
        return priority;
    }
    void set_time(int temp_time)
    {
        time_remaining = temp_time;
    }
};

class normalPrintQueue
{
    queue<print> q;
    bool currently_printing;
public:
    normalPrintQueue() : currently_printing(false) {}

    void add(Node* file, int arrival)
    {
        if (!file)
        {
            cout << "File not found\n";
            return;
        }
        int print_time = file->get_print_time_remaining();
        if (print_time == 0)
            print_time = 5;
        print temp_new(file, print_time, arrival, 1);
        q.push(temp_new);
        cout << "Added " << file->get_name() << " to normal queue\n";
        display();
    }

    void display()
    {
        if (q.empty())
            cout << "Normal queue is empty\n";
        else
        {
            cout << "Normal Print Queue:\n";
            queue<print> temp = q;
            int num = 1;
            while (!temp.empty())
            {
                print job = temp.front();
                cout << num << ". " << job.get_fileToPrint()->get_name() << " - Time: " << job.get_time();
                if (num == 1 && currently_printing)
                    cout << " Currently Printing";
                cout << "\n";
                temp.pop();
                num++;
            }
        }
    }

    void tick()
    {
        if (q.empty())
        {
            currently_printing = false;
            return;
        }
        currently_printing = true;
        print temp = q.front();
        q.pop();
        temp.set_time(temp.get_time() - 1);
        if (temp.get_time() <= 0)
        {
            cout << "Finished printing: " << temp.get_fileToPrint()->get_name() << "\n";
            currently_printing = false;
        }
        else
            q.push(temp);
    }
};

class priorityPrintQueue
{
    vector<print> pq;
    int current_idx;
    bool currently_printing;
public:
    priorityPrintQueue() : current_idx(-1), currently_printing(false) {}

    void add(Node* file, int arrival)
    {
        if (!file)
        {
            cout << "File not found\n";
            return;
        }
        int prior = file->get_print_priority();
        if (prior == 0)
        {
            cout << "Enter priority for " << file->get_name() << " ,higher number = higher priority: ";
            cin >> prior;
            cin.ignore();
            file->set_print_priority(prior);
        }
        int print_time = file->get_print_time_remaining();
        if (print_time == 0)
            print_time = 5;
        print temp2(file, print_time, arrival, prior);
        if (current_idx != -1 && currently_printing)
        {
            pq.push_back(temp2);
            for (int i = pq.size() - 1; i > current_idx + 1; i--)
            {
                if (pq[i].get_priority() > pq[i - 1].get_priority() || (pq[i].get_priority() == pq[i - 1].get_priority() && pq[i].get_arrival() < pq[i - 1].get_arrival()))
                    swap(pq[i], pq[i - 1]);
                else
                    break;
            }
        }
        else
        {
            pq.push_back(temp2);
            for (int i = pq.size() - 1; i > 0; i--)
            {
                if (pq[i].get_priority() > pq[i - 1].get_priority() || (pq[i].get_priority() == pq[i - 1].get_priority() && pq[i].get_arrival() < pq[i - 1].get_arrival()))
                    swap(pq[i], pq[i - 1]);
                else
                    break;
            }
            if (current_idx >= 0)
                current_idx++;
        }
        cout << "Added " << file->get_name() << " to priority queue\n";
        display();
    }

    void display()
    {
        if (pq.empty())
        {
            cout << "Priority queue is empty\n";
            return;
        }
        cout << "Priority Print Queue:\n";
        for (int i = 0; i < pq.size(); i++)
        {
            cout << i + 1 << ". " << pq[i].get_fileToPrint()->get_name() << " - Priority: " << pq[i].get_priority() << " - Time: " << pq[i].get_time();
            if (i == current_idx && currently_printing)
                cout << " Currently Printing";
            cout << "\n";
        }
    }

    void tick()
    {
        if (pq.empty())
        {
            currently_printing = false;
            current_idx = -1;
            return;
        }
        if (current_idx == -1 || current_idx >= pq.size())
        {
            current_idx = 0;
            currently_printing = true;
        }
        print temp = pq[current_idx];
        temp.set_time(temp.get_time() - 1);
        pq[current_idx].set_time(temp.get_time());
        if (temp.get_time() <= 0)
        {
            cout << "Finished printing: " << temp.get_fileToPrint()->get_name() << "\n";
            pq.erase(pq.begin() + current_idx);
            current_idx = -1;
            currently_printing = false;
        }
    }
};