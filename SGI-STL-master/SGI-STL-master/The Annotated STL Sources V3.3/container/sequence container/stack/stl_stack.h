/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef __SGI_STL_INTERNAL_STACK_H
#define __SGI_STL_INTERNAL_STACK_H

#include <sequence_concepts.h>

__STL_BEGIN_NAMESPACE

// Forward declarations of operators == and <, needed for friend declaration.
// 前置声明 __STL_DEPENDENT_DEFAULT_TMPL( deque< _Tp > )  相当于  __STL_DEPENDENT_DEFAULT_TMPL(Tp) = Tp
template < class _Tp , class _Sequence __STL_DEPENDENT_DEFAULT_TMPL( deque< _Tp > ) >
class stack;

template <class _Tp, class _Seq>
bool operator==( const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y );

template <class _Tp, class _Seq>
bool operator<( const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y );

// stack 类，_Sequence 容器 是_Sequence的一个配接器
template <class _Tp, class _Sequence>
class stack {

  // requirements:
  __STL_CLASS_REQUIRES(_Tp, _Assignable);
  
  __STL_CLASS_REQUIRES(_Sequence, _BackInsertionSequence);

  typedef typename _Sequence::value_type _Sequence_value_type ;
  
  __STL_CLASS_REQUIRES_SAME_TYPE(_Tp , _Sequence_value_type ) ;


#ifdef __STL_MEMBER_TEMPLATES

  template <class _Tp1, class _Seq1>
  friend bool operator== ( const stack<_Tp1, _Seq1>& , const stack<_Tp1, _Seq1>& );

  template <class _Tp1, class _Seq1>
  friend bool operator< (const stack<_Tp1, _Seq1>& , const stack<_Tp1, _Seq1>& );
  
#else /* __STL_MEMBER_TEMPLATES */

  friend bool __STD_QUALIFIER
  operator== __STL_NULL_TMPL_ARGS (const stack&, const stack&);

  friend bool __STD_QUALIFIER
  operator< __STL_NULL_TMPL_ARGS (const stack&, const stack&);

#endif /* __STL_MEMBER_TEMPLATES */

public:
  typedef typename _Sequence::value_type      value_type;
  typedef typename _Sequence::size_type       size_type;
  typedef          _Sequence                  container_type;

  typedef typename _Sequence::reference       reference;
  typedef typename _Sequence::const_reference const_reference;
protected:
  _Sequence c;  // stack 底部容器 （deque）
public:
	stack() : c() 
	{

	}

	explicit stack(const _Sequence& __s) : c(__s) 
	{

	}

	// 判断 stack 是否为空
	bool empty() const 
	{
		return c.empty(); 
	}

	// 判断 stack 的大小
	size_type size() const 
	{
		return c.size();
	}  

	// 尾部元素
	reference top() 
	{
		return c.back();
	} 
	
	const_reference top() const 
	{
		return c.back(); 
	}
	
	// 尾部插入元素
	void push(const value_type& __x) 
	{ 
		c.push_back(__x); 
	}  

	// 尾部弹出元素
	void pop() 
	{ 
		c.pop_back();
	} 
  
};

// 比较两个 stack 
template <class _Tp, class _Seq>
bool operator==(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return __x.c == __y.c;
}

template <class _Tp, class _Seq>
bool operator<(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return __x.c < __y.c;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class _Tp, class _Seq>
bool operator!=(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return !(__x == __y);
}

template <class _Tp, class _Seq>
bool operator>(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return __y < __x;
}

template <class _Tp, class _Seq>
bool operator<=(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return !(__y < __x);
}

template <class _Tp, class _Seq>
bool operator>=(const stack<_Tp,_Seq>& __x, const stack<_Tp,_Seq>& __y)
{
	return !(__x < __y);
}

#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_STACK_H */

// Local Variables:
// mode:C++
// End:
