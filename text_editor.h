#pragma once
#include"document.h"
#include<iostream>
#include<conio.h>
#include<windows.h>
#include<string>
using namespace std;

enum Mode {
	NORMAL_MODE,
	COMMAND_MODE,
	INSERT_MODE
};

class Text_Editor
{
	Document* doc;
	int cur_row;
	int cur_col;
	bool is_modified;
	bool is_running;
	Mode current_mode;
	bool is_read_only;
	string comm;

	void set_cursor_visibility(bool visible)
	{
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO info;
		info.dwSize = 100;
		info.bVisible = visible;
		SetConsoleCursorInfo(console, &info);
	}

	void update_cursor_position()
	{
		goto_row_col(cur_row, cur_col);
	}

	void refresh_display()
	{
		clear_screen();
		COORD coord;
		coord.X = 0;
		coord.Y = 0;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		doc->print();
		update_cursor_position();
	}

	int find_char_in_string(string str, char ch, int idx)
	{
		for (int i = idx;i < str.size();i++)
		{
			if (str[i] == ch)
				return i;
		}
		return -1;
	}

	void execute_command(string temp_comm)
	{
		if (temp_comm.size() == 2 && temp_comm[0] == 'w' && temp_comm[1] == 'q')
		{
			doc->save();
			is_running = false;
		}

		else if (temp_comm.size() == 1 && temp_comm[0] == 'q')
		{
			if (is_modified)
			{
				clear_screen();
				char ch='A';
				while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N')
				{
					cout << "File has unsaved changes! Do you want to save your file?Enter y for yes and n for no: \n";
					cin >> ch;
				}
				if (ch == 'y' || ch == 'Y')
					doc->save();
			}
			is_running = false;
		}

		else if (temp_comm.size() == 2 && temp_comm[0] == 'w' && temp_comm[1] == 'q')
		{
			doc->save();
			is_running = false;
		}

		else if (temp_comm.size() == 2 && temp_comm[0] == 'q' && temp_comm[1] == '!')
			is_running = false;
		else if (temp_comm.size() > 1 && temp_comm[0] == '/')
		{
			string search_text;
			for (int i = 1;i < temp_comm.size();i++)
				search_text += temp_comm[i];
			if (doc->find_word(search_text))
			{
				clear_screen();
				cout << "Found: " << search_text << "\n";
				_getch();
			}
			else
			{
				clear_screen();
				cout << "Not found: " << search_text << "\n";
				_getch();
			}
		}

		else if (temp_comm.size() > 1 && temp_comm[0] == '?')
		{
			string search_text;
			for (int i = 1;i < temp_comm.size();i++)
				search_text += temp_comm[i];
			if (doc->find_word(search_text))
			{
				clear_screen();
				cout << "Found: " << search_text << "\n";
				_getch();
			}
			else
			{
				clear_screen();
				cout << "Not found: " << search_text << "\n";
				_getch();
			}
		}

		else if (temp_comm.size() > 4)
		{
			bool is_replace = false;
			if (temp_comm[0] == '%' && temp_comm[1] == 's' && temp_comm[2] == '/')
				is_replace = true;
			if (is_replace)
			{
				int first_slash = find_char_in_string(temp_comm, '/', 0);
				int second_slash = find_char_in_string(temp_comm, '/', first_slash + 1);
				int third_slash = find_char_in_string(temp_comm, '/', second_slash + 1);
				if (first_slash != -1 && second_slash != -1)
				{
					string old_word;
					for (int i = first_slash + 1;i < second_slash;i++)
						old_word += temp_comm[i];
					string new_word;
					if (third_slash != -1)
					{
						for (int i = second_slash + 1;i < third_slash;i++)
							new_word += temp_comm[i];
					}
					else
					{
						for (int i = second_slash + 1;i < temp_comm.size();i++)
							new_word += temp_comm[i];
					}

					doc->set_cursor_from_screen(cur_row, cur_col);
					doc->replace_all(old_word, new_word);
					is_modified = true;
				}
			}
		}
		comm.clear();
		current_mode = NORMAL_MODE;
		refresh_display();
	}

	void handle_normal_mode(char ch)
	{
		if (ch == 16)
		{
			show_processing_menu();
			return;
		}
		if (ch == 'i')
		{
			if (is_read_only)
			{
				clear_screen();
				cout << "You cannot edit this file due to read only mode.\n";
				_getch();
				refresh_display();
			}
			else
			{
				current_mode = INSERT_MODE;
				update_cursor_position();
			}
		}
		else if (ch == 'h')
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->move_left();
			if (cur_col > 0)
				cur_col--;
			update_cursor_position();
		}
		else if (ch == 'l')
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->move_right();
			cur_col++;
			update_cursor_position();
		}
		else if (ch == 'j')
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->move_down();
			cur_row++;
			update_cursor_position();
		}
		else if (ch == 'k')
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->move_up();
			if (cur_row > 0)
				cur_row--;
			update_cursor_position();
		}
		else if (ch == 'x')
		{
			if (!is_read_only)
			{
				doc->set_cursor_from_screen(cur_row, cur_col);
				doc->delete_char();
				is_modified = true;
				refresh_display();
			}
		}
		else if (ch == ':')
		{
			current_mode = COMMAND_MODE;
			comm.clear();
		}
		else if (ch == '0')
		{
			cur_col = 0;
			update_cursor_position();
		}
		else if (ch == '$')
			update_cursor_position();
	}

	void handle_insert_mode(char ch)
	{
		if (is_read_only)
			return;
		if (ch == 27)
		{
			current_mode = NORMAL_MODE;
			refresh_display();
			update_cursor_position();
			return;
		}
		if (ch == 13)
		{
			doc->new_line();
			cur_row++;
			cur_col = 0;
			is_modified = true;
			refresh_display();
			doc->set_cursor_from_screen(cur_row, cur_col);
			return;
		}
		if (ch == 8)
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->backspace();
			if (cur_col > 0)
				cur_col--;
			else if (cur_row > 0)
				cur_row--;
			is_modified = true;
			refresh_display();
			doc->set_cursor_from_screen(cur_row, cur_col);
			return;
		}
		if (ch >= 32 && ch <= 126)
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->insert_char(ch);
			cout << ch;
			cur_col++;
			is_modified = true;
			update_cursor_position();
			return;
		}
	}

	int get_last_row()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		if (GetConsoleScreenBufferInfo(hConsole, &csbi))
			return csbi.srWindow.Bottom;
		return 24;
	}
	
	void refresh_command_line()
	{
		int row = get_last_row(); 
		goto_row_col(row, 0);
		cout << ":" << comm << " "; 
		goto_row_col(row, comm.size() + 1);
	}

	void handle_command_mode(char ch)
	{
		if (ch == 27)
		{
			current_mode = NORMAL_MODE;
			comm.clear();
			update_cursor_position();
			return;
		}
		if (ch == 13)
		{
			doc->set_cursor_from_screen(cur_row, cur_col);
			execute_command(comm);
			return;
		}
		if (ch == 8)
		{
			if (comm.size() > 0)
				comm.pop_back();
			return;
		}
		if (ch >= 32 && ch <= 126)
		{
			comm += ch;
			refresh_command_line();
			return;
		}
	}

	void show_processing_menu()
	{
		clear_screen();
		cout << "PROCESSING MODE:\n";
		cout << "1. Find word\n";
		cout << "2. Replace first\n";
		cout << "3. Replace all\n";
		cout << "4. Add prefix\n";
		cout << "5. Add postfix\n";
		cout << "6. Total words\n";
		cout << "7. Largest word length\n";
		cout << "8. Smallest word length\n";
		cout << "9. Average word length\n";
		cout << "10. Paragraph count\n";
		cout << "11. Sentence count\n";
		cout << "12. Special character count\n";
		cout << "0. Back to editing\n";
		cout << "Choice: ";
		int choice = 0;
		cin >> choice;
		cin.ignore();
		if (choice == 1)
		{
			cout << "Enter word to find: ";
			string word;
			getline(cin, word);
			if (doc->find_word(word))
				cout << "Word '" << word << "' found in document!\n";
			else
				cout << "Word '" << word << "' not found in document!\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 2)
		{
			cout << "Enter word to replace: ";
			string old_word;
			getline(cin, old_word);
			cout << "Enter new word: ";
			string new_word;
			getline(cin, new_word);
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->replace_first(old_word, new_word);
			is_modified = true;
			cout << "Replaced first occurrence!\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 3)
		{
			cout << "Enter word to replace: ";
			string old_word;
			getline(cin, old_word);
			cout << "Enter new word: ";
			string new_word;
			getline(cin, new_word);
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->replace_all(old_word, new_word);
			is_modified = true;
			cout << "Replaced all occurrences!\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 4)
		{
			cout << "Enter word to find: ";
			string word;
			getline(cin, word);
			cout << "Enter prefix: ";
			string prefix;
			getline(cin, prefix);
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->add_prefix(word, prefix);
			is_modified = true;
			cout << "Prefix added!\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 5)
		{
			cout << "Enter word to find: ";
			string word;
			getline(cin, word);
			cout << "Enter postfix: ";
			string postfix;
			getline(cin, postfix);
			doc->set_cursor_from_screen(cur_row, cur_col);
			doc->add_postfix(word, postfix);
			is_modified = true;
			cout << "Postfix added!\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 6)
		{
			cout << "Total words: " << doc->total_words() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 7)
		{
			cout << "Largest word length: " << doc->largest_word() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 8)
		{
			cout << "Smallest word length: " << doc->smallest_word() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 9)
		{
			cout << "Average word length: " << doc->avg_word_length() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 10)
		{
			cout << "Paragraph count: " << doc->para_count() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 11)
		{
			cout << "Sentence count: " << doc->sentence_count() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		else if (choice == 12)
		{
			cout << "Special character count: " << doc->special_char_count() << "\n";
			cout << "Press any key to continue...";
			_getch();
		}
		clear_screen();
		refresh_display();
	}

public:
	Text_Editor(Document* existing_doc, bool read_only_file)
	{
		doc = existing_doc;
		cur_row = 0;
		cur_col = 0;
		is_modified = false;
		is_running = true;
		current_mode = NORMAL_MODE;
		is_read_only = read_only_file;
	}

	void goto_row_col(int row, int col)
	{
		COORD coord;
		coord.X = col;
		coord.Y = row;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void clear_screen()
	{
		system("cls");
	}

	void handle_input()
	{
		char ch = _getch();
		if (ch == -32)
		{
			ch = _getch();
			if (ch == 72)
			{
				doc->move_up();
				if (cur_row > 0)
					cur_row--;
			}
			else if (ch == 80)
			{
				doc->move_down();
				cur_row++;
			}
			else if (ch == 75)
			{
				doc->move_left();
				if (cur_col > 0)
					cur_col--;
			}
			else if (ch == 77)
			{
				doc->move_right();
				cur_col++;
			}
			update_cursor_position();
			return;
		}
		if (current_mode == NORMAL_MODE)
			handle_normal_mode(ch);
		else if (current_mode == INSERT_MODE)
			handle_insert_mode(ch);
		else if (current_mode == COMMAND_MODE)
			handle_command_mode(ch);
	}

	void run()
	{
		doc->load();
		clear_screen();
		set_cursor_visibility(true);
		refresh_display();
		while (is_running)
			handle_input();
		set_cursor_visibility(true);
		clear_screen();
	}
};