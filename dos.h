#pragma once
#include<iostream>
#include<vector>
#include<string>
#include "fileSystem.h"
#include "print.h"
#include"text_editor.h"
using namespace std;

vector<string> split(const string& str)
{
    vector<string> result;
    string temp;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == ' ')
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

class dosShell
{
    FileSystem file_system;
    normalPrintQueue normal_print_queue;
    priorityPrintQueue priority_print_queue;
    int current_system_time;
public:
    dosShell() :current_system_time(0) {}
    void start_shell()
    {
        cout << "Author: Maha Faisal\n";
        cout << "Roll-Number: Bscs24048\n";
        cout << "Course: DSA-ITU-CS-24\n";
        ifstream check_tree("tree.txt");
        if (check_tree.is_open())
        {
            check_tree.close();
            file_system.load_tree("tree.txt");
        }
        run_command_loop();
    }
    void run_command_loop()
    {
        string temp_input;
        while (true)
        {
            cout << file_system.get_command_prompt();
            getline(cin, temp_input);
            vector<string> input = split(temp_input);
            if (!input.empty())
            {
                if (input[0] == "CD" || input[0] == "cd")
                {
                    if (input.size() > 1)
                        file_system.change_directory(input[1]);
                    else
                        cout << "CD requires a directory name\n";
                }
                else if (input[0] == "CONVERT" || input[0] == "convert")
                {
                    if (input.size() == 3)
                    {
                        file_system.convert_fileType(input[1], input[2]);
                    }
                    else
                        cout << "Invalid Command. Must add 2 types: old-type new-type\n";
                }
                else if (input[0] == "COPY" || input[0] == "copy")
                {
                    if (input.size() == 3)
                        file_system.copy_file(input[1], input[2]);
                    else
                        cout << "Invalid Command. Must add <file sourcePath> <destination directory's source path>\n";
                }
                else if (input[0] == "FIND" || input[0] == "find")
                {
                    if (input.size() > 1)
                        file_system.find_file(input[1]);
                    else
                        cout << "FIND requires a file name\n";
                }
                else if (input[0] == "FINDF" || input[0] == "findf")
                {
                    if (input.size() > 2)
                    {
                        string file_name = input[1];
                        string search_text;
                        for (int i = 2;i < input.size();i++)
                        {
                            search_text += input[i];
                            if (i < input.size() - 1)
                                search_text += " ";
                        }
                        Node* file = file_system.get_current_dir()->find_child_node(file_name, true);
                        if (file && file->get_is_file())
                        {
                            string physical_path = virtual_to_physical_path(file);
                            Document doc(physical_path);
                            doc.load();
                            if (doc.find_word(search_text))
                                cout << "Text '" << search_text << "' found in file: " << file_name << "\n";
                            else
                                cout << "Text '" << search_text << "' not found in file: " << file_name << "\n";
                        }
                        else if (file)
                            cout << file_name << " is not a file\n";
                        else
                            cout << "File not found: " << file_name << "\n";
                    }
                    else
                        cout << "FINDF requires filename and search text\n";
                }
                else if (input[0] == "FINDSTR" || input[0] == "findstr")
                {
                    if (input.size() > 1)
                    {
                        string search_text;
                        for (int i = 1;i < input.size();i++)
                        {
                            search_text += input[i];
                            if (i < input.size() - 1)
                                search_text += " ";
                        }
                        vector<Node*> files = file_system.get_current_dir()->get_children();
                        bool found = false;
                        for (int i = 0;i < files.size();i++)
                        {
                            if (files[i]->get_is_file())
                            {
                                string physical_path = virtual_to_physical_path(files[i]);
                                Document doc(physical_path);
                                doc.load();
                                if (doc.find_word(search_text))
                                {
                                    cout << "Found in: " << files[i]->get_name() << "\n";
                                    found = true;
                                }
                            }
                        }
                        if (!found)
                            cout << "String '" << search_text << "' not found in any file\n";
                    }
                    else
                        cout << "FINDSTR requires search text\n";
                }
                else if (input[0] == "DIR" || input[0] == "dir")
                    file_system.list_directory_contents();
                else if (input[0] == "PWD" || input[0] == "pwd")
                    file_system.print_working_directory();
                else if (input[0] == "MKDIR" || input[0] == "mkdir")
                {
                    if (input.size() > 1)
                        file_system.create_directory(input[1]);
                    else
                        cout << "MKDIR requires a directory name\n";
                }
                else if (input[0] == "RMDIR" || input[0] == "rmdir")
                {
                    if (input.size() > 1)
                        file_system.remove_directory(input[1]);
                    else
                        cout << "RMDIR requires a directory name\n";
                }
                else if (input[0] == "CREATE" || input[0] == "create")
                {
                    if (input.size() > 1)
                        file_system.create_file(input[1]);
                    else
                        cout << "CREATE requires a filename\n";
                }
                else if (input[0] == "MOVE" || input[0] == "move")
                {
                    if (input.size() == 3)
                        file_system.move_file(input[1], input[2]);
                    else
                        cout << "Invalid Command. Must add <file sourcePath> <destnation directory's source path>\n";
                }
                else if (input[0] == "PROMPT" || input[0] == "prompt")
                {
                    if (input.size() > 1)
                        file_system.set_command_prompt(input[1]);
                    else
                        cout << "PROMPT requires a new prompt statement\n";
                }
                else if (input[0] == "DEL" || input[0] == "del")
                {
                    if (input.size() > 1)
                        file_system.delete_file(input[1]);
                    else
                        cout << "DEL requires a filename\n";
                }
                else if (input[0] == "ATTRIB" || input[0] == "attrib")
                {
                    if (input.size() > 1)
                        file_system.display_file_attributes(input[1]);
                    else
                        cout << "ATTRIB requires a filename\n";
                }
                else if (input[0] == "EDIT" || input[0] == "edit")
                {
                    if (input.size() > 1)
                    {
                        Node* file = file_system.get_current_dir()->find_child_node(input[1], true);
                        if (file && file->get_is_file())
                        {
                            string physical_path = virtual_to_physical_path(file);
                            Document* doc = file->get_document();
                            if (doc == nullptr)
                            {
                                doc = new Document(physical_path);
                                doc->load();
                                file->set_document(doc);
                            }
                            bool is_readonly = file->get_read_only();
                            Text_Editor editor(doc, is_readonly);
                            editor.run();
                            int char_count = 0;
                            int line_count = 0;
                            ifstream counter(physical_path);
                            if (counter.is_open())
                            {
                                string temp_line;
                                while (getline(counter, temp_line))
                                {
                                    line_count++;
                                    char_count += temp_line.size();
                                }
                                counter.close();
                            }
                            file->set_character_count(char_count);
                            file->set_line_count(line_count);
                            if (line_count > 0)
                                file->set_avg_chars_per_line((double)char_count / (double)line_count);
                        }
                        else if (file)
                            cout << input[1] << " is not a file\n";
                        else
                            cout << "File not found: " << input[1] << "\n";
                    }
                    else
                        cout << "EDIT requires a filename\n";
                }
                else if (input[0] == "TREE" || input[0] == "tree")
                    file_system.display_tree_structure(file_system.get_root(), 0);
                else if (input[0] == "FORMAT" || input[0] == "format")
                    file_system.format_current_directory();
                else if (input[0] == "RENAME" || input[0] == "rename")
                {
                    if (input.size() == 3)
                        file_system.rename_file(input[1], input[2]);
                    else
                        cout << "Invalid Command. Must add <old file sourcePath> <new name>\n";
                }
                else if (input[0] == "HELP" || input[0] == "help")
                    file_system.display_help_information();
                else if (input[0] == "VER" || input[0] == "ver")
                    file_system.display_version_info();
                else if (input[0] == "PRINT" || input[0] == "print")
                {
                    if (input.size() > 1)
                        normal_print_queue.add(file_system.get_current_dir()->find_child_node(input[1], true), current_system_time);
                    else
                        cout << "PRINT requires a filename\n";
                }
                else if (input[0] == "PPRINT" || input[0] == "pprint")
                {
                    if (input.size() > 1)
                        priority_print_queue.add(file_system.get_current_dir()->find_child_node(input[1], true), current_system_time);
                    else
                        cout << "PPRINT requires a filename\n";
                }
                else if (input[0] == "QUEUE" || input[0] == "queue")
                    normal_print_queue.display();
                else if (input[0] == "PQUEUE" || input[0] == "pqueue")
                    priority_print_queue.display();
                else if (input[0] == "EXIT" || input[0] == "exit")
                {
                    cout << "Goodbye!\n";
                    file_system.save_tree("tree.txt");
                    break;
                }
                else if (input[0] == "SAVE" || input[0] == "save")
                {
                    if (input.size() > 1)
                    {
                        Node* file = file_system.get_current_dir()->find_child_node(input[1], true);
                        if (file)
                        {
                            cout << "Enter " << file->get_name() << " file's attributes : \n";
                            int priority = 0;
                            cout << "Enter file's priority for printing:\n";
                            cin >> priority;
                            file->set_print_priority(priority);
                            bool t_read_only = false;
                            char ch = 'A';
                            while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N')
                            {
                                cout << "Your file to be saved in read only mode? Enter y for yes and n for no:\n";
                                cin >> ch;
                            }
                            if (ch == 'y' || ch == 'Y')
                                t_read_only = true;
                            file->set_read_only(t_read_only);
                            file_system.save_tree("tree.txt");
                        }
                        else
                            cout << "File doesn't exist\n";
                    }
                }
                else if (input[0] == "LOADTREE" || input[0] == "loadtree")
                {
                    file_system.load_tree("tree.txt");
                }
                else
                    cout << "Unknown command: " << input[0] << "\n";

                current_system_time++;
                normal_print_queue.tick();
                priority_print_queue.tick();
            }
        }
    }

    string virtual_to_physical_path(Node* temp_file)
    {
        string physical_path;
        vector<string> parts;
        Node* current = temp_file;
        while (current != nullptr)
        {
            parts.push_back(current->get_name());
            current = current->get_parent();
        }
        for (int i = parts.size() - 1; i >= 0; i--)
        {
            physical_path += parts[i];
            if (i != 0)
                physical_path += "_";
        }
        return physical_path;
    }

    string physical_to_virtual_path(const string& temp_physical_path)
    {
        string virtual_path;
        vector<string> parts;
        string temp_part;
        for (int i = 0; i < temp_physical_path.size(); i++)
        {
            if (temp_physical_path[i] == '_')
            {
                parts.push_back(temp_part);
                temp_part.clear();
            }
            else
                temp_part += temp_physical_path[i];
        }

        parts.push_back(temp_part);
        for (int i = 0; i < parts.size(); i++)
        {
            virtual_path += parts[i];
            if (i != parts.size() - 1)
                virtual_path += "\\";
        }
        return virtual_path;
    }
};