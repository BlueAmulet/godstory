#pragma once


template<class _Ty>
class Table
{
public:
	struct _Entry 
	{
		_Ty		value;
		_Entry*	next;

		_Entry(const _Ty& v, _Entry* prt)
		{
			value = v;
			next  = prt;
		}
	};

	typedef _Ty			value_type;
	typedef _Ty&		reference;

	typedef _Entry*			iterator;
	typedef const _Entry*	const_iterator;

	Table(int initSize)
	{
		m_size = 0;
		m_cap = initSize;
		m_table = new _Entry*[initSize];
		memset(m_table, 0, m_cap * sizeof(_Entry*));
	}

	~Table()
	{
		clear();
		delete[] m_table;
	}
	//iterator begin()
	//{
	//	return m_table[0];
	//}
	iterator end()
	{
		return NULL;
	}
	iterator at(int index)
	{
		return m_table[index];
	}
	iterator find(int key)
	{
		if (key< 0|| key >= m_cap)
			return end();
		return m_table[key];
	}
	iterator get(int id)
	{
		int index = id >> 8;
		int sub = id & 0xff;
		if (index< 0|| index >= m_cap)
			return end();
		_Entry* p = m_table[index];
		while (sub && p)
		{
			p = p->next;
			sub--;
		}
		return p;
	}
	void init(int initSize)
	{
		clear();
		delete[] m_table;

		m_size = 0;
		m_cap = initSize;
		m_table = new _Entry*[initSize];
		memset(m_table, 0, m_cap * sizeof(_Entry*));
	}
	int size() const
	{
		return m_size;
	}
	int capacity() const
	{
		return m_cap;
	}
	bool empty() const
	{
		return m_size == 0;
	}
	int insert(int key, const _Ty& v)
	{
		m_size++;
		int n = 0;
		_Entry** p = &m_table[key];
		while (*p)//move to end
		{
			p = &(*p)->next;
			n++;
		}
		*p = new _Entry(v, NULL);
		return (key << 8) | n;
	}
	void erase(int id)
	{
		int index = id >> 8;
		int sub = id & 0xff;
		if (index< 0|| index >= m_cap)
			return;
		_Entry* p = m_table[index];
		_Entry** q = &m_table[index];
		while (p)
		{
			if (sub == 0)
			{
				m_size--;
				*q = p->next;
				delete p;
				break;
			}
			q = &p->next;
			p = p->next;
			sub--;
		}
	}
	void erase(int key, iterator it)
	{
		_Entry* p = m_table[key];
		_Entry** q = &m_table[key];
		while (p)
		{
			if (p == it)
			{
				m_size--;
				*q = p->next;
				delete p;
				break;
			}
			q = &p->next;
			p = p->next;
		}
	}
	void clear()
	{
		m_size = 0;
		for (int i = 0; i < m_cap; i++)
		{
			_Entry* p = m_table[i];
			while (p)
			{
				_Entry* o = p;
				p = p->next;
				delete o;
			}
		}
		memset(m_table, 0, m_cap * sizeof(_Entry*));
	}

protected:
	_Entry** m_table;
	int		 m_size;
	int		 m_cap;
};

