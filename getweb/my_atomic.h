#ifndef MY_ATOMIC_H
#define MY_ATOMIC_H

class my_atmic
{
public:
    volatile int m_coun;
	my_atmic():m_coun(0){}
    my_atmic(int cunn)
        :m_coun(cunn)
    {}
    ~my_atmic()
    {}
    my_atmic& operator++()
    {
        __sync_fetch_and_add(&m_coun,1);
        return *this;
    }
    my_atmic& operator--()
    {
        __sync_fetch_and_sub(&m_coun,1);
        return *this;
    }
    my_atmic operator++(int)
    {
        my_atmic ret(this->m_coun);
        ++*this;
        return ret;
    }
    my_atmic operator--(int)
    {
        my_atmic ret(this->m_coun);
        --*this;
        return ret;
    }
    my_atmic& operator+=(const my_atmic& m)
    {
        __sync_fetch_and_add(&m_coun,m.m_coun);
        return *this;
    }
    my_atmic& operator+=(int m)
    {
        __sync_fetch_and_add(&m_coun,m);
        return *this;
    }
    my_atmic& operator-=(const my_atmic& m)
    {
        __sync_fetch_and_sub(&m_coun,m.m_coun);
        return *this;
    }
    my_atmic& operator-=(int m)
    {
        __sync_fetch_and_sub(&m_coun,m);
        return *this;
    }
    my_atmic& operator=(int m)
	{
		m_coun = m;
		return *this;
	}
private:
    my_atmic(const my_atmic& );
    //
};
#endif // MY_ATOMIC_H
