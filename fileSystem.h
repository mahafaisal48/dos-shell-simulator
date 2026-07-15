#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include<fstream>
#include "node.h"
using namespace std;

vector<string> split_slash(const string& str)
{
    vector<string> result;
    string temp;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '\\')
        {
            if (!temp.empty())
            {
                result.push_back(temp);
                temp.clear();
            }
        }
        else
        {
            temp += str[i];
        }
    }
    if (!temp.empty())
        result.push_back(temp);
    return result;
}

class FileSystem
{
    Node* root_directory;
    Node* current_directory;
    string command_prompt;

    void delete_directory_recursive(Node* dir_node)
    {
        vector<Node*> children = dir_node->get_children();
        for (int i = 0; i < children.size(); i++)
        {
            Node* temp = children[i];
            if (temp->get_is_file())
                delete temp;
            else
            {
                delete_directory_recursive(temp);
                delete temp;
            }
        }
    }

    vector<string> split_comma(const string& str)
    {
        vector<string> result;
        string temp;
        for (int i = 0; i < str.size(); i++)
        {
            if (str[i] == ',')
            {
                result.push_back(temp);
                temp.clear();
            }
            else
                temp += str[i];
        }
        if (!temp.empty())
            result.push_back(temp);
        return result;
    }

    void update_command_prompt(string path)
    {
        int slash_count = 0;
        for (int i = 0;i < path.size();i++)
        {
            if (path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            command_prompt.pop_back();
            command_prompt.pop_back();
            command_prompt.pop_back();
            command_prompt = command_prompt + "\\" + path + "> ";
        }
        else
        {
            if (path.size() > 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
            {
                command_prompt = "V:";
                string temp_str;
                for (int i = 3;i < path.size();i++)
                    temp_str.push_back(path[i]);
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0;i < path_parts.size();i++)
                    command_prompt += "\\" + path_parts[i];
                command_prompt += "> ";
            }
            else
            {
                command_prompt.pop_back();
                command_prompt.pop_back();
                command_prompt.pop_back();
                vector<string> path_parts = split_slash(path);
                for (int i = 0;i < path_parts.size();i++)
                    command_prompt += "\\" + path_parts[i];
                command_prompt += "> ";
            }
        }
    }

    void empty_directory_recursive(Node* dir_node)
    {
        vector<Node*> children = dir_node->get_children();
        vector<Node*> files_to_delete;
        for (int i = 0; i < children.size(); i++)
        {
            Node* temp = children[i];
            if (temp->get_is_file())
                files_to_delete.push_back(temp);
            else
                empty_directory_recursive(temp);
        }
        for (int i = 0; i < files_to_delete.size(); i++)
        {
            dir_node->remove_child_node(files_to_delete[i]->get_name());
            delete files_to_delete[i];
        }
    }

    void save_tree_recursive(Node* node, ofstream& out, string current_path)
    {
        if (!node)
            return;
        if (node != root_directory)
        {
            string full_path = current_path;
            if (node != root_directory)
                full_path = current_path + "\\" + node->get_name();
            if (node->get_is_file())
                out << "F:" << full_path << "," << node->get_owner() << "," << node->get_read_only() << "," << node->get_character_count() << "," << node->get_line_count() << "," << node->get_avg_chars_per_line() << "," << node->get_print_priority() << "," << node->get_print_time_remaining() << "," << node->get_file_type_extension() << "\n";
            else
                out << "D:" << full_path << "," << node->get_owner() << "," << node->get_read_only() << "\n";
        }
        if (!node->get_is_file())
        {
            string new_path = current_path;
            if (node != root_directory)
                new_path = current_path + "\\" + node->get_name();
            vector<Node*> children = node->get_children();
            for (int i = 0; i < children.size(); i++)
                save_tree_recursive(children[i], out, new_path);
        }
    }

public:
    FileSystem()
    {
        root_directory = new Node("V:", false);
        current_directory = root_directory;
        command_prompt = "V:\\> ";
    }

    void save_tree(const string filename)
    {
        ofstream out(filename);
        if (!out.is_open())
        {
            cout << "Cannot create tree file: " << filename << "\n";
            return;
        }
        save_tree_recursive(root_directory, out, "V:");
        out.close();
        cout << "Tree saved to: " << filename << "\n";
    }

    void load_tree(const string filename)
    {
        ifstream reader(filename);
        if (!reader)
        {
            cout << "Cannot open tree file: " << filename << "\n";
            return;
        }
        cout << "Loading tree from: " << filename << "\n";
        string temp;
        while (getline(reader, temp))
        {
            if (temp.size() > 0)
            {
                char node_type = temp[0];
                string data;
                for (int i = 2; i < temp.size(); i++)
                    data += temp[i];
                vector<string> parts = split_comma(data);
                if (parts.size() > 0)
                {
                    string path = parts[0];
                    if (path.size() >= 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
                    {
                        string temp_str;
                        for (int i = 3; i < path.size(); i++)
                            temp_str += path[i];
                        if (temp_str.size() > 0)
                        {
                            vector<string> path_parts = split_slash(temp_str);
                            if (path_parts.size() > 0)
                            {
                                if (node_type == 'D')
                                {
                                    Node* t_d = root_directory;
                                    for (int i = 0; i < path_parts.size(); i++)
                                    {
                                        Node* nxt = t_d->find_child_node(path_parts[i], false);
                                        if (!nxt)
                                        {
                                            nxt = new Node(path_parts[i], false);
                                            t_d->add_child_node(nxt);
                                        }
                                        t_d = nxt;
                                    }
                                    if (parts.size() > 1)
                                        t_d->set_owner(parts[1]);
                                    if (parts.size() > 2)
                                    {
                                        if (parts[2].size() > 0 && parts[2][0] == '1')
                                            t_d->set_read_only(true);
                                        else
                                            t_d->set_read_only(false);
                                    }
                                }
                                else if (node_type == 'F')
                                {
                                    Node* temp_root = root_directory;
                                    for (int i = 0; i < path_parts.size() - 1; i++)
                                    {
                                        Node* next = temp_root->find_child_node(path_parts[i], false);
                                        if (!next)
                                        {
                                            next = new Node(path_parts[i], false);
                                            temp_root->add_child_node(next);
                                        }
                                        temp_root = next;
                                    }
                                    Node* existing_file = temp_root->find_child_node(path_parts[path_parts.size() - 1], true);
                                    if (!existing_file)
                                    {
                                        Node* new_file = new Node(path_parts[path_parts.size() - 1], true);
                                        temp_root->add_child_node(new_file);
                                        if (parts.size() > 1)
                                            new_file->set_owner(parts[1]);
                                        if (parts.size() > 2)
                                        {
                                            if (parts[2].size() > 0 && parts[2][0] == '1')
                                                new_file->set_read_only(true);
                                            else
                                                new_file->set_read_only(false);
                                        }
                                        if (parts.size() > 3)
                                        {
                                            int char_count = 0;
                                            for (int i = 0; i < parts[3].size(); i++)
                                                char_count = char_count * 10 + (parts[3][i] - '0');
                                            new_file->set_character_count(char_count);
                                        }
                                        if (parts.size() > 4)
                                        {
                                            int line_count = 0;
                                            for (int i = 0; i < parts[4].size(); i++)
                                                line_count = line_count * 10 + (parts[4][i] - '0');
                                            new_file->set_line_count(line_count);
                                        }
                                        if (parts.size() > 5)
                                        {
                                            double avg = 0.0;
                                            int dec = -1;
                                            for (int i = 0; i < parts[5].size(); i++)
                                            {
                                                if (parts[5][i] == '.')
                                                    dec = i;
                                            }
                                            if (dec != -1)
                                            {
                                                int whole_part = 0;
                                                for (int i = 0; i < dec; i++)
                                                    whole_part = whole_part * 10 + (parts[5][i] - '0');
                                                double frac_part = 0.0;
                                                double divisor = 10.0;
                                                for (int i = dec + 1; i < parts[5].size(); i++)
                                                {
                                                    frac_part += (parts[5][i] - '0') / divisor;
                                                    divisor *= 10.0;
                                                }
                                                avg = whole_part + frac_part;
                                            }
                                            else
                                            {
                                                for (int i = 0; i < parts[5].size(); i++)
                                                    avg = avg * 10 + (parts[5][i] - '0');
                                            }
                                            new_file->set_avg_chars_per_line(avg);
                                        }
                                        if (parts.size() > 6)
                                        {
                                            int priority = 0;
                                            for (int i = 0; i < parts[6].size(); i++)
                                                priority = priority * 10 + (parts[6][i] - '0');
                                            new_file->set_print_priority(priority);
                                        }
                                        if (parts.size() > 7)
                                        {
                                            int print_time = 0;
                                            for (int i = 0; i < parts[7].size(); i++)
                                                print_time = print_time * 10 + (parts[7][i] - '0');
                                            new_file->set_print_time_remaining(print_time);
                                        }
                                        if (parts.size() > 8)
                                            new_file->set_file_type_extension(parts[8]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        reader.close();
        cout << "Tree loaded successfully\n";
    }

    void change_directory(const string temp_dir_name)
    {
        if (temp_dir_name == "..")
        {
            if (current_directory->get_parent() != nullptr)
            {
                current_directory = current_directory->get_parent();
                string temp_str = command_prompt;
                temp_str.pop_back();
                temp_str.pop_back();
                temp_str.pop_back();
                int idx = -1;
                for (int i = temp_str.size() - 1; i >= 0; i--)
                {
                    if (temp_str[i] == '\\')
                    {
                        idx = i;
                        break;
                    }
                }
                if (idx != -1)
                {
                    if (current_directory && current_directory == root_directory)
                        command_prompt = "V:\\> ";
                    else if (current_directory)
                    {
                        command_prompt = "";
                        for (int i = 0; i < idx; i++)
                            command_prompt += temp_str[i];
                        command_prompt += "> ";
                    }
                }
                else
                    command_prompt = "V:\\> ";
            }
            else
                cout << "Already at root directory\n";
        }
        else if (temp_dir_name == "\\")
        {
            current_directory = root_directory;
            command_prompt = "V:\\> ";
        }
        else if (temp_dir_name == ".")
        {
            cout << current_directory->get_name() << "\n";
        }
        else
        {
            int slash_count = 0;
            for (int i = 0; i < temp_dir_name.size(); i++)
            {
                if (temp_dir_name[i] == '\\')
                    slash_count++;
            }
            if (slash_count == 0)
            {
                Node* temp = current_directory->find_child_node(temp_dir_name, false);
                if (temp && temp->get_is_file())
                    cout << temp_dir_name << " is not a directory" << "\n";
                else if (temp)
                {
                    current_directory = temp;
                    update_command_prompt(temp_dir_name);
                }
                else
                    cout << "Directory not found: " << temp_dir_name << "\n";
            }
            else
            {
                if (temp_dir_name.size() >= 3 && temp_dir_name[0] == 'V' && temp_dir_name[1] == ':' && temp_dir_name[2] == '\\')
                {
                    string temp_str;
                    for (int i = 3; i < temp_dir_name.size(); i++)
                        temp_str.push_back(temp_dir_name[i]);
                    Node* temp = root_directory;
                    vector<string> path_parts = split_slash(temp_str);
                    for (int i = 0; i < path_parts.size(); i++)
                    {
                        temp = temp->find_child_node(path_parts[i], false);
                        if (!temp)
                            break;
                    }
                    if (temp && !temp->get_is_file())
                    {
                        current_directory = temp;
                        update_command_prompt(temp_dir_name);
                    }
                    else if (temp)
                        cout << path_parts[path_parts.size() - 1] << " is not a directory\n";
                    else
                        cout << "Directory not found\n";
                }
                else
                {
                    vector<string> path_parts = split_slash(temp_dir_name);
                    Node* temp = current_directory;
                    for (int i = 0; i < path_parts.size(); i++)
                    {
                        temp = temp->find_child_node(path_parts[i], false);
                        if (!temp)
                        {
                            cout << "Directory not found\n";
                            break;
                        }
                    }
                    if (temp && !temp->get_is_file())
                    {
                        current_directory = temp;
                        update_command_prompt(temp_dir_name);
                    }
                    else if (temp)
                        cout << path_parts[path_parts.size() - 1] << " is not a directory\n";
                }
            }
        }
    }

    void print_working_directory()
    {
        vector<string> path;
        Node* temp = current_directory;
        while (temp)
        {
            path.push_back(temp->get_name());
            temp = temp->get_parent();
        }
        for (int i = path.size() - 1; i >= 0; i--)
        {
            cout << path[i];
            if (i != 0)
                cout << "\\";
        }
        cout << "\n";
    }

    void list_directory_contents()
    {
        vector<Node*> temp_chil = current_directory->get_children();
        if (temp_chil.empty())
        {
            cout << "Directory is empty\n";
        }
        else
        {
            for (int i = 0; i < temp_chil.size(); i++)
            {
                cout << temp_chil[i]->get_name();
                if (temp_chil[i]->get_is_file())
                    cout << " (File)";
                else
                    cout << " (Directory)";
                cout << "\n";
            }
        }
    }

    void display_tree_structure(Node* temp_node, int temp_depth)
    {
        for (int i = 0; i < temp_depth; i++)
            cout << "  ";
        cout << temp_node->get_name();
        if (temp_node->get_is_file())
            cout << " (File)";
        cout << "\n";
        vector<Node*> temp_chil = temp_node->get_children();
        for (int i = 0; i < temp_chil.size(); i++)
            display_tree_structure(temp_chil[i], temp_depth + 1);
    }

    void create_directory(const string& path)
    {
        int slash_count = 0;
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            if (current_directory->find_child_node(path, false) != nullptr)
            {
                cout << "Directory already exists: " << path << "\n";
                return;
            }
            Node* temp = new Node(path, false);
            current_directory->add_child_node(temp);
            cout << "Directory created: " << path << "\n";
        }
        else
        {
            if (path.size() >= 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < path.size(); i++)
                    temp_str.push_back(path[i]);
                Node* temp = root_directory;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                if (temp && !temp->get_is_file())
                {
                    if (temp->find_child_node(path_parts[path_parts.size() - 1], false))
                        cout << "Directory " << path_parts[path_parts.size() - 1] << " already exists\n";
                    else
                    {
                        Node* temp2 = new Node(path_parts[path_parts.size() - 1], false);
                        temp->add_child_node(temp2);
                        cout << "Directory created: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                }
                else if (temp && temp->get_is_file())
                    cout << temp->get_name() << " is not a directory\n";
                else
                    cout << "Parent directory not found\n";
            }
            else
            {
                vector<string> path_parts = split_slash(path);
                Node* temp = current_directory;
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                    {
                        cout << "Parent directory not found\n";
                        return;
                    }
                }
                if (temp && !temp->get_is_file())
                {
                    if (temp->find_child_node(path_parts[path_parts.size() - 1], false))
                        cout << "Directory " << path_parts[path_parts.size() - 1] << " already exists\n";
                    else
                    {
                        Node* temp2 = new Node(path_parts[path_parts.size() - 1], false);
                        temp->add_child_node(temp2);
                        cout << "Directory created: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                }
                else if (temp && temp->get_is_file())
                    cout << temp->get_name() << " is not a directory\n";
            }
        }
    }

    void remove_directory(const string& path)
    {
        int slash_count = 0;
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            Node* dir_to_remove = current_directory->find_child_node(path, false);
            if (!dir_to_remove)
            {
                cout << "Directory not found: " << path << "\n";
                return;
            }
            if (dir_to_remove->get_is_file())
            {
                cout << path << " is not a directory\n";
                return;
            }
            delete_directory_recursive(dir_to_remove);
            current_directory->remove_child_node(path);
            delete dir_to_remove;
            cout << "Directory removed: " << path << "\n";
        }
        else
        {
            if (path.size() >= 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < path.size(); i++)
                    temp_str.push_back(path[i]);
                Node* temp = root_directory;
                Node* temp_parent = nullptr;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp_parent = temp;
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                if (temp && !temp->get_is_file())
                {
                    delete_directory_recursive(temp);
                    temp_parent->remove_child_node(path_parts[path_parts.size() - 1]);
                    delete temp;
                    cout << "Directory removed: " << path_parts[path_parts.size() - 1] << "\n";
                }
                else if (temp && temp->get_is_file())
                    cout << path_parts[path_parts.size() - 1] << " is not a directory\n";
                else
                    cout << "Directory not found\n";
            }
            else
            {
                Node* temp = current_directory;
                Node* temp_par = nullptr;
                vector<string> path_parts = split_slash(path);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp_par = temp;
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                    {
                        cout << "Directory not found\n";
                        return;
                    }
                }
                if (temp && !temp->get_is_file())
                {
                    delete_directory_recursive(temp);
                    temp_par->remove_child_node(path_parts[path_parts.size() - 1]);
                    delete temp;
                    cout << "Directory removed: " << path_parts[path_parts.size() - 1] << "\n";
                }
                else if (temp && temp->get_is_file())
                    cout << path_parts[path_parts.size() - 1] << " is not a directory\n";
            }
        }
    }

    void create_file(const string& path)
    {
        int slash_count = 0;
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            if (current_directory->find_child_node(path, true) != nullptr)
            {
                cout << "File already exists: " << path << "\n";
                return;
            }
            Node* temp = new Node(path, true);
            current_directory->add_child_node(temp);
            cout << "File created: " << path << "\n";
        }
        else
        {
            if (path.size() >= 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < path.size(); i++)
                    temp_str.push_back(path[i]);
                Node* temp = root_directory;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                if (temp && !temp->get_is_file())
                {
                    if (temp->find_child_node(path_parts[path_parts.size() - 1], true))
                        cout << "File " << path_parts[path_parts.size() - 1] << " already exists\n";
                    else
                    {
                        Node* temp2 = new Node(path_parts[path_parts.size() - 1], true);
                        temp->add_child_node(temp2);
                        cout << "File created: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                }
                else if (temp && temp->get_is_file())
                    cout << temp->get_name() << " is not a directory\n";
                else
                    cout << "Parent directory not found\n";
            }
            else
            {
                vector<string> path_parts = split_slash(path);
                Node* temp = current_directory;
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                    {
                        cout << "Parent directory not found\n";
                        return;
                    }
                }
                if (temp && !temp->get_is_file())
                {
                    if (temp->find_child_node(path_parts[path_parts.size() - 1], true))
                        cout << "File " << path_parts[path_parts.size() - 1] << " already exists\n";
                    else
                    {
                        Node* temp2 = new Node(path_parts[path_parts.size() - 1], true);
                        temp->add_child_node(temp2);
                        cout << "File created: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                }
                else if (temp && temp->get_is_file())
                    cout << temp->get_name() << " is not a directory\n";
            }
        }
    }

    void delete_file(const string& path)
    {
        int slash_count = 0;
        for (int i = 0; i < path.size(); i++)
        {
            if (path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            Node* temp = current_directory->find_child_node(path, true);
            if (temp && temp->get_is_file())
            {
                current_directory->remove_child_node(path);
                delete temp;
                cout << "File deleted: " << path << "\n";
            }
            else if (temp)
                cout << path << " is not a file\n";
            else
                cout << "File not found: " << path << "\n";
        }
        else
        {
            if (path.size() >= 3 && path[0] == 'V' && path[1] == ':' && path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < path.size(); i++)
                    temp_str.push_back(path[i]);
                Node* temp = root_directory;
                Node* temp_parent = nullptr;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp_parent = temp;
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                if (temp && !temp->get_is_file())
                {
                    Node* del_file = temp->find_child_node(path_parts[path_parts.size() - 1], true);
                    if (del_file && del_file->get_is_file())
                    {
                        temp->remove_child_node(path_parts[path_parts.size() - 1]);
                        delete del_file;
                        cout << "File deleted: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                    else if (del_file)
                        cout << path_parts[path_parts.size() - 1] << " is not a file\n";
                    else
                        cout << "File not found: " << path_parts[path_parts.size() - 1] << "\n";
                }
                else if (temp)
                    cout << path_parts[path_parts.size() - 1] << " is not a file\n";
                else
                    cout << "File not found\n";
            }
            else
            {
                Node* temp = current_directory;
                Node* temp_parent = nullptr;
                vector<string> path_parts = split_slash(path);
                for (int i = 0; i < path_parts.size() - 1; i++)
                {
                    temp_parent = temp;
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                if (temp && !temp->get_is_file())
                {
                    Node* del_file = temp->find_child_node(path_parts[path_parts.size() - 1], true);
                    if (del_file && del_file->get_is_file())
                    {
                        temp->remove_child_node(path_parts[path_parts.size() - 1]);
                        delete del_file;
                        cout << "File deleted: " << path_parts[path_parts.size() - 1] << "\n";
                    }
                    else if (del_file)
                        cout << path_parts[path_parts.size() - 1] << " is not a file\n";
                    else
                        cout << "File not found: " << path_parts[path_parts.size() - 1] << "\n";
                }
                else if (temp)
                    cout << path_parts[path_parts.size() - 1] << " is not a file\n";
                else
                    cout << "File not found\n";
            }
        }
    }

    void convert_fileType(string type1, string type2)
    {
        vector<Node*> temp = current_directory->get_children();
        int count = 0;
        for (int i = 0; i < temp.size(); i++)
        {
            if (temp[i]->get_is_file() && temp[i]->get_file_type_extension() == type1)
            {
                temp[i]->set_file_type_extension(type2);
                count++;
            }
        }
        cout << "Converted " << count << " files from " << type1 << " to " << type2 << "\n";
    }

    void copy_file(string file_path, string dir_path)
    {
        int file_slash_count = 0, dir_slash_count = 0;
        for (int i = 0;i < file_path.size();i++)
        {
            if (file_path[i] == '\\')
                file_slash_count++;
        }
        for (int i = 0;i < dir_path.size();i++)
        {
            if (dir_path[i] == '\\')
                dir_slash_count++;
        }

        Node* file_loc = nullptr;
        Node* dir_loc = nullptr;
        string dir_name, file_name;
        if (file_slash_count == 0)
        {
            file_loc = current_directory->find_child_node(file_path, true);
            file_name = file_path;
        }
        else
        {
            if (file_path.size() >= 3 && file_path[0] == 'V' && file_path[1] == ':' && file_path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < file_path.size(); i++)
                    temp_str.push_back(file_path[i]);
                file_loc = root_directory;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    file_loc = file_loc->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!file_loc)
                        break;
                }
                file_name = path_parts[path_parts.size() - 1];
            }
            else
            {
                vector<string> path_parts = split_slash(file_path);
                file_loc = current_directory;
                for (int i = 0; i < path_parts.size(); i++)
                {
                    file_loc = file_loc->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!file_loc)
                    {
                        cout << "File not found\n";
                        return;
                    }
                }
                file_name = path_parts[path_parts.size() - 1];
            }
        }
        if (file_loc)
        {
            if (dir_slash_count == 0)
            {
                dir_loc = current_directory->find_child_node(dir_path, false);
                dir_name = dir_path;
            }
            else
            {
                if (dir_path.size() >= 3 && dir_path[0] == 'V' && dir_path[1] == ':' && dir_path[2] == '\\')
                {
                    string temp_str;
                    for (int i = 3; i < dir_path.size(); i++)
                        temp_str.push_back(dir_path[i]);
                    dir_loc = root_directory;
                    vector<string> path_parts = split_slash(temp_str);
                    for (int i = 0; i < path_parts.size(); i++)
                    {
                        dir_loc = dir_loc->find_child_node(path_parts[i], false);
                        if (dir_loc)
                            break;
                    }
                    dir_name = path_parts[path_parts.size() - 1];
                }
                else
                {
                    vector<string> path_parts = split_slash(dir_path);
                    for (int i = 0; i < path_parts.size(); i++)
                    {
                        dir_loc = dir_loc->find_child_node(path_parts[i], false);
                        if (!dir_loc)
                        {
                            cout << "File not found\n";
                            return;
                        }
                    }
                    dir_name = path_parts[path_parts.size() - 1];
                }
            }
        }

        if (file_loc && dir_loc)
        {
            Node* new_file = new Node(file_loc->get_name(), true);
            new_file->set_creation_time(file_loc->get_creation_time());
            new_file->set_owner(file_loc->get_owner());
            new_file->set_read_only(file_loc->get_read_only());
            new_file->set_character_count(file_loc->get_character_count());
            new_file->set_line_count(file_loc->get_line_count());
            new_file->set_avg_chars_per_line(file_loc->get_avg_chars_per_line());
            new_file->set_print_priority(file_loc->get_print_priority());
            new_file->set_print_time_remaining(file_loc->get_print_time_remaining());
            new_file->set_file_type_extension(file_loc->get_file_type_extension());
            dir_loc->add_child_node(new_file);
        }
        else if (file_loc)
            cout << "Directory " << dir_name << " doesn't exist\n";
        else if (dir_loc)
            cout << "File " << file_name << " doesn't exist\n";
        else
        {
            cout << "Directory " << dir_name << " doesn't exist\n";
            cout << "File " << file_name << " doesn't exist\n";
        }
    }

    void find_file(string file_name)
    {
        vector<Node*> children = current_directory->get_children();
        bool found = false;
        for (int i = 0; i < children.size(); i++)
        {
            if (children[i]->get_is_file() && children[i]->get_name() == file_name)
            {
                vector<string> path_parts;
                Node* path = current_directory;
                while (path)
                {
                    path_parts.insert(path_parts.begin(), path->get_name());
                    path = path->get_parent();
                }
                cout << "File found: ";
                for (int j = 0; j < path_parts.size(); j++)
                {
                    cout << path_parts[j];
                    if (j != path_parts.size() - 1)
                        cout << "\\";
                }
                cout << "\\" << file_name << "\n";
                found = true;
                break;
            }
        }
        if (!found)
            cout << "File " << file_name << " not found in current directory\n";
    }

    void set_command_prompt(const string& new_prompt)
    {
        command_prompt = new_prompt;
        cout << "Command prompt changed to: " << new_prompt << "\n";
    }

    void rename_file(const string& file_path, const string& new_name)
    {
        Node* file = nullptr;
        Node* parent = nullptr;
        int slash_count = 0;
        for (int i = 0; i < file_path.size(); i++)
        {
            if (file_path[i] == '\\')
                slash_count++;
        }
        if (slash_count == 0)
        {
            file = current_directory->find_child_node(file_path, true);
            parent = current_directory;
        }
        else
        {
            if (file_path.size() >= 3 && file_path[0] == 'V' && file_path[1] == ':' && file_path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < file_path.size(); i++)
                    temp_str.push_back(file_path[i]);
                Node* temp = root_directory;
                Node* temp_parent = nullptr;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp_parent = temp;
                    temp = temp->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!temp)
                        break;
                }
                if (temp && temp->get_is_file())
                {
                    file = temp;
                    parent = temp_parent;
                }
            }
            else
            {
                Node* temp = current_directory;
                Node* temp_par = nullptr;
                vector<string> path_parts = split_slash(file_path);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp_par = temp;
                    temp = temp->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!temp)
                        break;
                }
                if (temp && temp->get_is_file())
                {
                    file = temp;
                    parent = temp_par;
                }
            }
        }
        if (!file)
        {
            cout << "File not found: " << file_path << "\n";
            return;
        }
        if (parent->find_child_node(new_name, true))
        {
            cout << "File with new name already exists\n";
            return;
        }
        file->set_name(new_name);
        cout << "File renamed successfully\n";
    }

    void move_file(string file_path, string dir_path)
    {
        int file_slash_count = 0, dir_slash_count = 0;
        for (int i = 0; i < file_path.size(); i++)
        {
            if (file_path[i] == '\\')
                file_slash_count++;
        }
        for (int i = 0; i < dir_path.size(); i++)
        {
            if (dir_path[i] == '\\')
                dir_slash_count++;
        }

        Node* file_loc = nullptr;
        Node* file_parent = nullptr;
        Node* dir_loc = nullptr;

        if (file_slash_count == 0)
        {
            file_loc = current_directory->find_child_node(file_path, true);
            file_parent = current_directory;
        }
        else
        {
            if (file_path.size() >= 3 && file_path[0] == 'V' && file_path[1] == ':' && file_path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < file_path.size(); i++)
                    temp_str.push_back(file_path[i]);
                Node* temp = root_directory;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    file_parent = temp;
                    temp = temp->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!temp)
                        break;
                }
                file_loc = temp;
            }
            else
            {
                vector<string> path_parts = split_slash(file_path);
                Node* temp = current_directory;
                for (int i = 0; i < path_parts.size(); i++)
                {
                    file_parent = temp;
                    temp = temp->find_child_node(path_parts[i], i == path_parts.size() - 1);
                    if (!temp)
                    {
                        cout << "File not found\n";
                        return;
                    }
                }
                file_loc = temp;
            }
        }

        if (dir_slash_count == 0)
        {
            dir_loc = current_directory->find_child_node(dir_path, false);
        }
        else
        {
            if (dir_path.size() >= 3 && dir_path[0] == 'V' && dir_path[1] == ':' && dir_path[2] == '\\')
            {
                string temp_str;
                for (int i = 3; i < dir_path.size(); i++)
                    temp_str.push_back(dir_path[i]);
                Node* temp = root_directory;
                vector<string> path_parts = split_slash(temp_str);
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                        break;
                }
                dir_loc = temp;
            }
            else
            {
                vector<string> path_parts = split_slash(dir_path);
                Node* temp = current_directory;
                for (int i = 0; i < path_parts.size(); i++)
                {
                    temp = temp->find_child_node(path_parts[i], false);
                    if (!temp)
                    {
                        cout << "Directory not found\n";
                        return;
                    }
                }
                dir_loc = temp;
            }
        }

        if (file_loc && dir_loc && file_loc->get_is_file() && !dir_loc->get_is_file())
        {
            if (dir_loc->find_child_node(file_loc->get_name(), true))
            {
                cout << "File with same name already exists in target directory\n";
                return;
            }
            file_parent->remove_child_node(file_loc->get_name());
            dir_loc->add_child_node(file_loc);
            file_loc->set_parent(dir_loc);
            cout << "File moved successfully\n";
        }
        else if (!file_loc)
            cout << "File not found: " << file_path << "\n";
        else if (!dir_loc)
            cout << "Directory not found: " << dir_path << "\n";
        else if (!file_loc->get_is_file())
            cout << file_path << " is not a file\n";
        else if (dir_loc->get_is_file())
            cout << dir_path << " is not a directory\n";
    }

    void display_file_attributes(const string& temp_filename)
    {
        Node* file = current_directory->find_child_node(temp_filename, true);
        if (file && file->get_is_file())
        {
            cout << "Name: " << file->get_name() << "\n";
            cout << "Type: File\n";
            cout << "Created: " << file->get_creation_time() << "\n";
            cout << "Owner: " << file->get_owner() << "\n";
            cout << "Read Only: ";
            if (file->get_read_only())
                cout << "Yes\n";
            else
                cout << "No\n";
            cout << "Characters: " << file->get_character_count() << "\n";
            cout << "Lines: " << file->get_line_count() << "\n";
            cout << "Avg Chars/Line: " << file->get_avg_chars_per_line() << "\n";
            cout << "File Type: " << file->get_file_type_extension() << "\n";
        }
        else if (file)
            cout << temp_filename << " is not a file\n";
        else
            cout << "File not found: " << temp_filename << "\n";
    }

    void format_current_directory()
    {
        empty_directory_recursive(current_directory);
        cout << "Directory formatted\n";
    }

    void display_version_info()
    {
        cout << "DOS Shell v1.0\n";
        cout << "Author: Maha Faisal\n";
        cout << "Course: DSA-CS-24\n";
        cout << "Roll-No: Bscs24048\n";
    }

    void display_help_information()
    {
        cout << "Available commands:\n";
        cout << "ATTRIB - Display file attributes\n";
        cout << "CD - Change directory\n";
        cout << "CD. - Print working directory\n";
        cout << "CD.. - Change to parent directory\n";
        cout << "CD\\ - Change to root directory\n";
        cout << "CONVERT - Convert file types\n";
        cout << "COPY - Copy file to directory\n";
        cout << "CREATE - Create a file\n";
        cout << "DEL - Delete a file\n";
        cout << "DIR - List directory contents\n";
        cout << "EXIT - Quit program\n";
        cout << "FIND - Find file in current directory\n";
        cout << "FORMAT - Format current directory\n";
        cout << "HELP - Display this help\n";
        cout << "MKDIR - Create directory\n";
        cout << "MOVE - Move file to directory\n";
        cout << "PPRINT - Add file to priority print queue\n";
        cout << "PRINT - Add file to print queue\n";
        cout << "PQUEUE - Show priority print queue\n";
        cout << "PROMPT - Change command prompt\n";
        cout << "PWD - Print working directory\n";
        cout << "QUEUE - Show print queue\n";
        cout << "RENAME - Rename file\n";
        cout << "RMDIR - Remove directory\n";
        cout << "TREE - Display directory tree\n";
        cout << "VER - Display version\n";
    }

    Node* get_root() const
    {
        return root_directory;
    }

    Node* get_current_dir() const
    {
        return current_directory;
    }

    string get_command_prompt() const
    {
        return command_prompt;
    }
};