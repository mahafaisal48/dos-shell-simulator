#pragma once
#include"line.h"
#include"para.h"
#include<vector>
#include<string>
#include<fstream>
using namespace std;

class Document
{
	vector<Para*> paras;
	string file_name;
	int dri, dci, dli;
public:

	Document(string temp_file_name)
	{
		file_name = temp_file_name;
		dri = 0;
		dli = 0;
		dci = 0;
	}

	~Document()
	{
		for (int i = 0;i < paras.size();i++)
		{
			if (paras[i])
				delete paras[i];
		}
		paras.clear();
	}

	void load()
	{
		ifstream reader(file_name);
		if (!reader.is_open())
		{
			Para* p = new Para();
			Line* l = new Line();
			p->add_line_end(l);
			paras.push_back(p);
			dri = 0;
			dli = 0;
			dci = 0;
			return;
		}
		Para* current_para = new Para();
		string t_line;
		bool has_lines = false;
		while (getline(reader, t_line))
		{
			has_lines = true;
			Line* l = new Line();
			for (int i = 0; i < t_line.size(); i++)
				l->append_char(t_line[i]);
			current_para->add_line_end(l);
		}
		reader.close();
		if (has_lines)
			paras.push_back(current_para);
		else
		{
			delete current_para;
			Para* p = new Para();
			Line* l = new Line();
			p->add_line_end(l);
			paras.push_back(p);
		}
		dri = 0;
		dli = 0;
		dci = 0;
	}

	void save()
	{
		ofstream writer(file_name);
		if (!writer)
			return;
		for (int i = 0; i < paras.size(); i++)
		{
			Para* p = paras[i];
			int line_count = p->get_line_count();
			for (int j = 0; j < line_count; j++)
			{
				Line* l = p->get_line(j);
				if (l)
				{
					int size = l->get_size();
					for (int k = 0; k < size; k++)
					{
						char ch = l->get_char(k);
						writer << ch;
					}
					writer << '\n';
				}
			}
		}
		writer.close();
	}

	void move_left()
	{
		if (dci > 0)
			dci--;
		else if (dli > 0)
		{
			dli--;
			Line* l = paras[dri]->get_line(dli);
			if (l)
				dci = l->get_size();
		}
		else if (dri > 0)
		{
			dri--;
			int line_count = paras[dri]->get_line_count();
			if (line_count > 0)
			{
				dli = line_count - 1;
				Line* l = paras[dri]->get_line(dli);
				if (l)
					dci = l->get_size();
			}
		}
	}

	void move_right()
	{
		if (dri >= paras.size())
			return;
		Line* l = paras[dri]->get_line(dli);
		if (!l)
			return;
		if (dci < l->get_size())
			dci++;
		else if (dli + 1 < paras[dri]->get_line_count())
		{
			dli++;
			dci = 0;
		}
		else if (dri + 1 < paras.size())
		{
			dri++;
			dli = 0;
			dci = 0;
		}
	}

	void move_up()
	{
		if (dli > 0)
			dli--;
		else if (dri > 0)
		{
			dri--;
			int line_count = paras[dri]->get_line_count();
			if (line_count > 0)
				dli = line_count - 1;
			else
				dli = 0;
		}
	}

	void move_down()
	{
		if (dli + 1 < paras[dri]->get_line_count())
			dli++;
		else if (dri + 1 < paras.size())
		{
			dri++;
			dli = 0;
		}
	}

	void insert_char(char ch)
	{

		if (paras.empty())
		{
			Para* p = new Para();
			Line* l = new Line();
			p->add_line_end(l);
			paras.push_back(p);
			dri = 0;
			dli = 0;
			dci = 0;
		}
		Para* cur_para = paras[dri];
		Line* cur_line = cur_para->get_line(dli);
		if (!cur_line)
		{
			cur_line = new Line();
			cur_para->insert_line_at(dli, cur_line);
			dci = 0;
		}
		cur_line->insert_char(ch, dci);
		dci++;
	}

	void join_line()
	{
		if (dri == 0)
			return;
		Line* prev = paras[dri - 1]->get_line(0);
		Line* cur = paras[dri]->get_line(0);
		if (!prev || !cur)
			return;
		int temp_size = cur->get_size();
		for (int i = 0;i < temp_size;i++)
			prev->append_char(cur->get_char(i));
		delete paras[dri];
		paras.erase(paras.begin() + dri);
		dri--;
		dci = prev->get_size();
	}

	void backspace()
	{
		if (dci > 0)
		{
			Line* l = paras[dri]->get_line(dli);
			if (l)
			{
				l->backspace_char(dci);
				dci--;
			}
		}
		else if (dli > 0)
		{
			Line* prev_line = paras[dri]->get_line(dli - 1);
			Line* cur_line = paras[dri]->get_line(dli);
			if (!prev_line || !cur_line)
				return;
			int prev_size = prev_line->get_size();
			for (int i = 0;i < cur_line->get_size();i++)
				prev_line->append_char(cur_line->get_char(i));
			paras[dri]->delete_line(dli);
			dli--;
			dci = prev_size;
		}
		else if (dri > 0)
		{
			Para* prev_para = paras[dri - 1];
			Para* cur_para = paras[dri];
			int prev_line_count = prev_para->get_line_count();
			if (prev_line_count > 0 && cur_para->get_line_count() > 0)
			{
				Line* prev_last_line = prev_para->get_line(prev_line_count - 1);
				Line* cur_first_line = cur_para->get_line(0);
				int prev_size = prev_last_line->get_size();
				for (int i = 0;i < cur_first_line->get_size();i++)
					prev_last_line->append_char(cur_first_line->get_char(i));
				cur_para->delete_line(0);
				if (cur_para->get_line_count() == 0)
				{
					delete paras[dri];
					paras.erase(paras.begin() + dri);
				}
				dri--;
				dli = prev_line_count - 1;
				dci = prev_size;
			}
		}
	}

	void delete_char()
	{
		Line* temp_l = paras[dri]->get_line(dli);
		if (temp_l)
			temp_l->delete_char(dci);
	}

	void new_line()
	{
		Para* cur_para = paras[dri];
		Line* cur_line = cur_para->get_line(dli);
		if (!cur_line)
			return;
		Line* new_line = new Line();
		int cur_line_size = cur_line->get_size();
		while (dci < cur_line_size)
		{
			char ch = cur_line->get_char(dci);
			new_line->append_char(ch);
			cur_line->delete_char(dci);
			cur_line_size--;
		}
		cur_para->insert_line_at(dli + 1, new_line);
		dli++;
		dci = 0;
	}

	bool find_word(string temp_word)
	{
		if (temp_word.empty())
			return false;
		for (int i = 0;i < paras.size();i++)
		{
			auto res = paras[i]->find_word(temp_word);
			for (int j = 0;j < res.size();j++)
			{
				if (!res[j].second.empty())
					return true;
			}
		}
		return false;
	}

	vector<pair<int, int>> find_all(string to_find)
	{
		vector<pair<int, int>> v;
		if (to_find.empty())
			return v;
		int pidx = 0;
		for (int i = 0;i < paras.size();i++)
		{
			Para* p = paras[i];
			auto res = p->find_word(to_find);
			for (int j = 0;j < res.size();j++)
			{
				auto& pr = res[j];
				for (int k = 0;k < pr.second.size();k++)
				{
					int cidx = pr.second[k];
					v.push_back(make_pair(pidx, cidx));
				}
			}
			pidx++;
		}
		return v;
	}

	void replace_first(string to_repl, string res)
	{
		if (to_repl.empty())
			return;
		for (int i = 0;i < paras.size();i++)
		{
			Para* p = paras[i];
			if (p->find_word(to_repl).size() > 0)
			{
				Line* l = p->get_line(0);
				if (l)
				{
					l->replace_first(to_repl, res);
					return;
				}
			}
		}
	}

	void replace_all(string to_repl, string res)
	{
		if (to_repl.empty())
			return;
		for (int i = 0;i < paras.size();i++)
			paras[i]->replace_all(to_repl, res);
	}

	int smallest_word()
	{
		int min_len = 0;
		bool first = true;
		for (int i = 0;i < paras.size();i++)
		{
			int temp = paras[i]->smallest_word();
			if (temp > 0)
			{
				if (first)
				{
					min_len = temp;
					first = false;
				}
				else if (temp < min_len)
					min_len = temp;
			}
		}
		if (first)
			return 0;
		return min_len;
	}

	int largest_word()
	{
		int max_len = 0;
		for (int i = 0;i < paras.size();i++)
		{
			int temp = paras[i]->largest_word();
			if (temp > max_len)
				max_len = temp;
		}
		return max_len;
	}

	int total_words()
	{
		int sum = 0;
		for (int i = 0;i < paras.size();i++)
			sum += paras[i]->total_words();
		return sum;
	}

	int para_count()
	{
		return paras.size();
	}

	int sentence_count()
	{
		int count = 0;
		for (int i = 0;i < paras.size();i++)
		{
			int line_count = paras[i]->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = paras[i]->get_line(j);
				if (l)
				{
					int size = l->get_size();
					for (int k = 0;k < size;k++)
					{
						char ch = l->get_char(k);
						if (ch == '.' || ch == '?' || ch == '!')
							count++;
					}
				}
			}
		}
		return count;
	}

	int special_char_count()
	{
		int count = 0;
		for (int i = 0;i < paras.size();i++)
		{
			int line_count = paras[i]->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = paras[i]->get_line(j);
				if (l)
				{
					int size = l->get_size();
					for (int k = 0;k < size;k++)
					{
						char ch = l->get_char(k);
						if (!isalpha(ch) && !isdigit(ch) && ch != ' ')
							count++;
					}
				}
			}
		}
		return count;
	}

	void add_prefix(string to_find, string prefix)
	{
		if (to_find.empty() || prefix.empty())
			return;
		for (int i = 0;i < paras.size();i++)
		{
			int line_count = paras[i]->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = paras[i]->get_line(j);
				if (l)
				{
					vector<int> words_inst = l->find_all_word(to_find);
					int num = 0;
					for (int k = 0;k < words_inst.size();k++)
					{
						int idx = words_inst[k] + num;
						for (int m = prefix.size() - 1;m >= 0;m--)
						{
							l->insert_char(prefix[m], idx);
							num++;
						}
					}
				}
			}
		}
	}

	void add_postfix(string to_find, string postfix)
	{
		if (to_find.empty() || postfix.empty())
			return;
		for (int i = 0;i < paras.size();i++)
		{
			int line_count = paras[i]->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = paras[i]->get_line(j);
				if (l)
				{
					vector<int> word_inst = l->find_all_word(to_find);
					int num = 0;
					for (int k = 0;k < word_inst.size();k++)
					{
						int idx = word_inst[k] + to_find.size() + num;
						for (int m = 0;m < postfix.size();m++)
						{
							l->insert_char(postfix[m], idx + m);
							num++;
						}
					}
				}
			}
		}
	}

	void merge_doc(Document& d2)
	{
		for (int i = 0;i < d2.paras.size();i++)
			paras.push_back(d2.paras[i]);
		d2.paras.clear();
	}

	void print()
	{
		for (int i = 0;i < paras.size();i++)
		{
			Para* p = paras[i];
			int line_count = p->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = p->get_line(j);
				if (l)
				{
					l->print_line();
					cout << '\n';
				}
			}
		}
	}

	double avg_word_length()
	{
		int total_chars = 0;
		int total_word_count = 0;
		for (int i = 0;i < paras.size();i++)
		{
			int line_count = paras[i]->get_line_count();
			for (int j = 0;j < line_count;j++)
			{
				Line* l = paras[i]->get_line(j);
				if (l)
				{
					int size = l->get_size();
					int current_word_len = 0;
					for (int k = 0;k < size;k++)
					{
						char ch = l->get_char(k);
						if (isalpha(ch))
							current_word_len++;
						else
						{
							if (current_word_len > 0)
							{
								total_chars += current_word_len;
								total_word_count++;
								current_word_len = 0;
							}
						}
					}
					if (current_word_len > 0)
					{
						total_chars += current_word_len;
						total_word_count++;
					}
				}
			}
		}
		if (total_word_count == 0)
			return 0.0;
		return (double)total_chars / (double)total_word_count;
	}

	void set_cursor_from_screen(int screen_row, int screen_col)
	{
		int row = 0;
		for (int i = 0; i < paras.size(); i++)
		{
			int temp = paras[i]->get_line_count();
			for (int j = 0; j < temp; j++)
			{
				if (row == screen_row)
				{
					dri = i;
					dli = j;
					Line* temp_l = paras[i]->get_line(j);
					if (temp_l)
					{
						int size = temp_l->get_size();
						if (screen_col < 0) 
							dci = 0;
						else if (screen_col > size) 
							dci = size;
						else 
							dci = screen_col;
					}
					return;
				}
				row++;
			}
		}
		if (!paras.empty()) 
		{
			int last_p = paras.size() - 1;
			int last_line = paras[last_p]->get_line_count() - 1;
			if (last_line < 0) 
				last_line = 0;
			dri = last_p;
			dli = last_line;
			Line* temp_l = paras[dri]->get_line(dli);
			if (temp_l) 
				dci = min(screen_col, temp_l->get_size());
		}
	}

	void set_filename(string f_name)
	{
		file_name = f_name;
	}
};