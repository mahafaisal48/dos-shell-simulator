#pragma once
#include<iostream>
#include<list>
#include<fstream>
#include<vector>
#include"letter.h"
using namespace std;
class Line
{
	int size;
	list<letter> Cs;
public:
	Line():size(0){}
	void insert_char(char ch,int col)
	{
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		Cs.insert(itr,letter(ch));
		size++;
	}

	void delete_char(int col)
	{
		if (col < 0 || col >= size)
			return;
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		Cs.erase(itr);
		size--;
	}

	char get_char(int col)
	{
		if (col >= size)
			return '\0';
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		return (*itr).get_ch();
	}

	int get_size() const
	{
		return size;
	}

	void print_line()
	{
		for (auto i = Cs.begin();i != Cs.end();i++)
			cout << (*i).get_ch();
	}

	void backspace_char(int col)
	{
		if (col <= 0 || col > size)
			return;
		auto itr = Cs.begin();
		for (int i = 0;i < col - 1;i++)
			itr++;
		Cs.erase(itr);
		size--;
	}

	void append_char(char ch)
	{
		Cs.push_back(letter(ch));
		size++;
	}

	int find_word(string word, int col)
	{
		if (word.empty() || col < 0 || col >= size)
			return -1;
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		int count = col;
		int w_idx = 0;
		for (auto i = itr;i != Cs.end();i++, count++)
		{
			if ((*i).get_ch() == word[w_idx])
			{
				w_idx++;
				if (w_idx == word.size())
					return count - w_idx + 1;
			}
			else
			{
				if (w_idx > 0)
				{
					count = count - w_idx;
					auto temp = i;
					for (int j = 0;j < w_idx;j++)
						temp--;
					i = temp;
				}
				w_idx = 0;
			}
		}
		return -1;
	}

	vector<int> find_all_word(string word)
	{
		int i = 0;
		vector<int> word_inst;
		while (i < size)
		{
			if (i >= size)
				break;
			int idx = find_word(word, i);
			if (idx == -1)
				return word_inst;
			word_inst.push_back(idx);
			i = idx + 1;
		}
		return word_inst;
	}

	bool replace_first(string to_replace, string res)
	{
		if (to_replace.empty() || res.empty())
			return false;
		int idx = find_word(to_replace,0);
		if (idx == -1)
			return false;
		int to_repl_idx = 0;
		int res_idx = 0;
		auto itr = Cs.begin();
		for (int i = 0;i < idx;i++)
			itr++;
		bool greater_res = false, smaller_res = false;
		if (to_replace.size() > res.size())
			smaller_res = true;
		else if (to_replace.size() < res.size())
			greater_res = true;
		for (int i = 0;i < to_replace.size();i++)
		{
			if (smaller_res)
				if (i >= res.size())
					break;
			(*itr).set_ch(res[res_idx]);
			res_idx++;
			to_repl_idx++;
			itr++;
		}
		if (smaller_res)
		{
			for (int i = res_idx;i < to_replace.size();i++)
			{
				itr=Cs.erase(itr);
			}
			size-= (to_replace.size() - res.size());
		}
		else if (greater_res)
		{
			for (int i = to_replace.size();i < res.size();i++)
			{
				Cs.insert(itr, letter(res[i]));
				itr++;
			}
			size += res.size() - to_replace.size();
		}
		return true;
	}

	void replace_all(string to_replace, string res)
	{
		if (to_replace.empty() || res.empty())
			return;
		vector<int> word_inst = find_all_word(to_replace);
		if (word_inst.size() == 0)
			return;
		int size_diff = res.size() - to_replace.size();
		for (int i = 0;i < word_inst.size();i++)
		{
			int idx = word_inst[i]+(i * size_diff);
			int to_repl_idx = 0;
			int res_idx = 0;
			auto itr = Cs.begin();
			for (int i = 0;i < idx;i++)
				itr++;
			bool greater_res = false, smaller_res = false;
			if (to_replace.size() > res.size())
				smaller_res = true;
			else if (to_replace.size() < res.size())
				greater_res = true;
			for (int j = 0;j < to_replace.size();j++)
			{
				if (smaller_res)
					if (j >= res.size())
						break;
				(*itr).set_ch(res[res_idx]);
				res_idx++;
				to_repl_idx++;
				itr++;
			}
			if (smaller_res)
			{
				for (int j = res_idx;j < to_replace.size();j++)
				{
					itr=Cs.erase(itr);
				}
				size -= (to_replace.size() - res.size());
			}
			else if (greater_res)
			{
				for (int j = to_replace.size();j < res.size();j++)
				{
					Cs.insert(itr, letter(res[j]));
					itr++;
				}
				size += res.size() - to_replace.size();
			}
		}
	}

	void to_upper(int col)
	{
		if (col < 0 || col >= size)
			return;
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		if ((*itr).get_ch() >= 97 && (*itr).get_ch() <= 122)
			(*itr).set_ch((*itr).get_ch() - 32);
	}

	void to_lower(int col)
	{
		if (col < 0 || col >= size)
			return;
		auto itr = Cs.begin();
		for (int i = 0;i < col;i++)
			itr++;
		if ((*itr).get_ch() >= 65 && (*itr).get_ch() <= 90)
			(*itr).set_ch((*itr).get_ch() + 32);
	}

	int word_count()
	{
		int count = 0;
		bool in_word = false;
		for (auto i = Cs.begin(); i != Cs.end(); i++)
		{
			char ch = (*i).get_ch();
			if (isalpha(ch))
			{
				if (!in_word)
				{
					count++;
					in_word = true;
				}
			}
			else
				in_word = false;
		}
		return count;
	}

	int largest_word()
	{
		int max_len = 0, cur_len = 0;
		for (auto i = Cs.begin(); i != Cs.end(); i++)
		{
			char ch = (*i).get_ch();
			if (isalpha(ch))
				cur_len++;
			else
			{
				if (cur_len > max_len)
					max_len = cur_len;
				cur_len = 0;
			}
		}
		if (cur_len > max_len)
			max_len = cur_len;
		return max_len;
	}

	int smallest_word()
	{
		int min_len = 0, cur_len = 0;
		bool first_word = true;
		for (auto i = Cs.begin(); i != Cs.end(); i++)
		{
			char ch = (*i).get_ch();
			if (isalpha(ch))
				cur_len++;
			else
			{
				if (cur_len > 0)
				{
					if (first_word)
					{
						min_len = cur_len;
						first_word = false;
					}
					else if (cur_len < min_len)
						min_len = cur_len;
					cur_len = 0;
				}
			}
		}
		if (cur_len > 0)
		{
			if (first_word || cur_len < min_len)
				min_len = cur_len;
		}
		if (first_word)
			return 0;
		return min_len;
	}


	void write_to_file(ofstream& writer)
	{
		for (auto i = Cs.begin(); i != Cs.end(); i++)
			writer << (*i).get_ch();
	}
};