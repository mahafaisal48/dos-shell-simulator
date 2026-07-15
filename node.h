#pragma once
#include <string>
#include <vector>
#include<ctime>
#include"document.h"
using namespace std;
class Node
{
    string name;
    bool is_file_node;
    Node* parent_node;
    vector<Node*> children_nodes;
    string creation_time_stamp;
    string node_owner;
    bool read_only_flag;
    int character_count;
    int line_count;
    double average_chars_per_line;
    int print_priority;
    int print_time_remaining;
    string file_type_extension;
    Document* doc;
public:
    Node(string temp_name, bool temp_is_file)
    {
        doc = nullptr;
        name = temp_name;
        is_file_node = temp_is_file;
        parent_node = nullptr;
        time_t current_time;
        time(&current_time);
        char time_buffer[26];
        ctime_s(time_buffer, sizeof(time_buffer), &current_time);
        creation_time_stamp = time_buffer;
        if (!creation_time_stamp.empty() && creation_time_stamp.back() == '\n')
            creation_time_stamp.pop_back();
        node_owner = "System";
        read_only_flag = false;
        character_count = 0;
        line_count = 0;
        average_chars_per_line = 0.0;
        print_priority = 1;
        print_time_remaining = 0;
        if (is_file_node)
            file_type_extension = ".txt";
        else
            file_type_extension = "";
    }

    Document* get_document()
    {
        return doc;
    }

    void set_document(Document* temp_doc)
    {
        doc = temp_doc;
    }

    void add_child_node(Node* temp_child)
    {
        children_nodes.push_back(temp_child);
        temp_child->parent_node = this;
    }

    void remove_child_node(const string& temp_name)
    {
        for (int i = 0;i < children_nodes.size();i++)
        {
            if (children_nodes[i]->get_name() == temp_name)
            {
                for (int j = i + 1;j < children_nodes.size();j++)
                {
                    children_nodes[j - 1] = children_nodes[j];
                }
                int temp_s = children_nodes.size();
                children_nodes.resize(temp_s - 1);
                break;
            }
        }
    }

    Node* find_child_node(const string& temp_name, bool file)
    {
        for (int i = 0;i < children_nodes.size();i++)
        {
            if (children_nodes[i]->get_name() == temp_name && children_nodes[i]->get_is_file() == file)
            {
                return children_nodes[i];
            }
        }
        return nullptr;
    }
    vector<Node*> get_children()
    {
        return children_nodes;
    }
    string get_name() const
    {
        return name;
    }
    bool get_is_file() const
    {
        return is_file_node;
    }

    Node* get_parent() const
    {
        return parent_node;
    }

    string get_creation_time() const
    {
        return creation_time_stamp;
    }

    string get_owner() const
    {
        return node_owner;
    }

    bool get_read_only() const
    {
        return read_only_flag;
    }

    int get_character_count() const
    {
        return character_count;
    }

    int get_line_count() const
    {
        return line_count;
    }

    double get_avg_chars_per_line() const
    {
        return average_chars_per_line;
    }

    int get_print_priority() const
    {
        return print_priority;
    }

    int get_print_time_remaining() const
    {
        return print_time_remaining;
    }

    string get_file_type_extension()
    {
        return file_type_extension;
    }

    void set_name(const string& temp_name)
    {
        name = temp_name;
    }

    void set_parent(Node* temp_parent)
    {
        parent_node = temp_parent;
    }

    void set_creation_time(const string& temp_time)
    {
        creation_time_stamp = temp_time;
    }

    void set_owner(const string& temp_owner)
    {
        node_owner = temp_owner;
    }

    void set_read_only(bool temp_flag)
    {
        read_only_flag = temp_flag;
    }

    void set_character_count(int temp_count)
    {
        character_count = temp_count;
    }

    void set_line_count(int temp_count)
    {
        line_count = temp_count;
    }

    void set_avg_chars_per_line(double temp_avg)
    {
        average_chars_per_line = temp_avg;
    }

    void set_print_priority(int temp_priority)
    {
        print_priority = temp_priority;
    }

    void set_print_time_remaining(int temp_time)
    {
        print_time_remaining = temp_time;
    }

    void set_file_type_extension(const string& temp_extension)
    {
        file_type_extension = temp_extension;
    }
    void clear_children()
    {
        children_nodes.clear();
    }

    ~Node()
    {
        if (doc)
            delete doc;
    }
};