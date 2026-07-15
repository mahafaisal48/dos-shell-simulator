#pragma once
#include<utility>
#include<iostream>
#include<fstream>
#include"line.h"
using namespace std;

class Para
{
	list<Line*> Ls;
	int get_size() const
	{
		int count = 0;
		for (auto i = Ls.begin();i != Ls.end();i++)
			count++;
		return count;
	}
public:
	Para() {}

	~Para()
	{
		for (auto i = Ls.begin();i != Ls.end();i++)
		{
			if (*i)
				delete* i;
		}
		Ls.clear();
	}

	void add_line_end(Line* other)
	{
		if (other)
			Ls.push_back(other);
	}

	void insert_line_at(int idx, Line* line)
	{
		if (!line || idx < 0)
			return;
		int size = get_size();
		if (idx > size)
			return;
		auto itr = Ls.begin();
		for (int i = 0;i < idx;i++)
			itr++;
		Ls.insert(itr, line);
	}

	void delete_line(int idx)
	{
		if (idx < 0)
			return;
		int size = get_size();
		if (idx >= size)
			return;
		auto itr = Ls.begin();
		for (int i = 0;i < idx;i++)
			itr++;
		if (*itr)
			delete* itr;
		Ls.erase(itr);
	}

	Line* get_line(int idx)
	{
		if (idx < 0)
			return nullptr;
		int size = get_size();
		if (idx >= size)
			return nullptr;
		auto itr = Ls.begin();
		for (int i = 0;i < idx;i++)
			itr++;
		return *itr;
	}

	vector<pair<int, vector<int>>> find_word(string word)
	{
		vector<pair<int, vector<int>>> v;
		if (word.empty())
			return v;
		int idx = 0;
		for (auto i = Ls.begin();i != Ls.end();i++, idx++)
		{
			vector<int> v1 = (*i)->find_all_word(word);
			pair<int, vector<int>> p(idx, v1);
			v.push_back(p);
		}
		return v;
	}

	void replace_all(string to_repl, string res)
	{
		if (to_repl.empty())
			return;
		for (auto i = Ls.begin();i != Ls.end();i++)
			(*i)->replace_all(to_repl, res);
	}

	int total_words()
	{
		int count = 0;
		for (auto i = Ls.begin();i != Ls.end();i++)
			count += (*i)->word_count();
		return count;
	}

	int largest_word()
	{
		int max_len = 0;
		for (auto i = Ls.begin();i != Ls.end();i++)
		{
			int temp = (*i)->largest_word();
			if (temp > max_len)
				max_len = temp;
		}
		return max_len;
	}

	int smallest_word()
	{
		int min_len = 0;
		bool is_first = true;
		for (auto i = Ls.begin();i != Ls.end();i++)
		{
			int temp = (*i)->smallest_word();
			if (temp > 0)
			{
				if (is_first)
				{
					min_len = temp;
					is_first = false;
				}
				else if (temp < min_len)
					min_len = temp;
			}
		}
		if (is_first)
			return 0;
		return min_len;
	}

	void write_to_file(ofstream& writer)
	{
		for (auto i = Ls.begin();i != Ls.end();i++)
		{
			(*i)->write_to_file(writer);
			writer << '\n';
		}
	}

	int get_line_count() const
	{
		return get_size();
	}
};