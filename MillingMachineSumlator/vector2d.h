#ifndef _VECTOR2D_
#define _VECTOR2D_

#include <vector>
using namespace std;
#pragma once

template <typename T>
class vector2d
{
	typedef typename vector<T>::iterator        iterator;
	typedef typename vector<T>::const_iterator  const_iterator;
	typedef typename vector<T>::size_type       size_type;
private:

protected:
	vector<iterator>	rows;   
	vector<T>			elems;         
public:
	size_type r;            
    size_type c;            

	vector2d(void);
	vector2d(size_type _r, size_type _c, const T& val = T());
    
	void			assign(int x,int y,const T& val = T());
	void			resize(int x,int y);
	void			clear();

	T&				getAt(int indX,int indY);
	const T&		getAt(int indX,int indY) const;

	iterator		operator[] (size_type i) { return rows[i]; }
    const_iterator	operator[] (size_type i) const { return rows[i]; }
};

template<typename T>
vector2d<T>::vector2d() {
	r = c = 0;
}


template<typename T>
vector2d<T>::vector2d(size_type _r, size_type _c, const T& val) : r(_r), c(_c), elems (_r*_c, val), rows (_r) {   
	if (elems.empty())
		return;
	rows[0] = elems.begin();
	for (size_type i = 1; i < r; ++i)
		rows[i] = rows[i-1] + c; 
}


template<typename T>
inline void vector2d<T>::clear() {
	rows.clear();
	elems.clear();
}	


template<typename T>
void vector2d<T>::resize(int x,int y) { 
	r = y;
	c = x;
	elems.resize(r*c);
	rows.resize(r);
	if (elems.empty())
		return;
	rows[0] = elems.begin();
	for (size_type i = 1; i < r; ++i)
		rows[i] = rows[i-1] + c; 
};


template<typename T>
void vector2d<T>::assign(int x,int y,const T& val ) { 
	r = y;
	c = x;
	elems.assign(r*c,val);
	rows.resize(r);
	if (elems.empty())
		return;
	rows[0] = elems.begin();
	for (size_type i = 1; i < r; ++i)
		rows[i] = rows[i-1] + c; 
};


template<typename T>
inline T& vector2d<T>::getAt(int indX,int indY) {
	return rows[indY][indX];
}


template<typename T>
inline const T& vector2d<T>::getAt(int indX, int indY) const {
	return rows[indY][indX];
}



#endif