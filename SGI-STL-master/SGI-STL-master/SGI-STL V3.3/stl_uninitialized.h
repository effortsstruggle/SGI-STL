/*
 * 内存处理基本工具
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

#ifndef __SGI_STL_INTERNAL_UNINITIALIZED_H
#define __SGI_STL_INTERNAL_UNINITIALIZED_H

__STL_BEGIN_NAMESPACE

// uninitialized_copy 

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.


/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_copy_aux 将 [__first , __last)的值,复制给 __result 形成的区间
 * @param __first 开始迭代器
 * @param __last 结束迭代器
 * @param __result 结果
 * @param __true_type / __false_type 用来判断可否采用最快速的方式进行拷贝
 * @return __result 
 *
 * copy 最低层源码：
 *		for ( ; __first != __last; ++__result, ++__first)
 *			__result = *__first;
 *		return __result;
 *  此时__result 标志在末尾；
 *
 * _ForwardIter : 允许 “写入型”算法(例如 replace())，在此种选代器所形成的区间上进行读写操作
 */
template <class _InputIter, class _ForwardIter>
inline _ForwardIter  __uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                         _ForwardIter __result,
                         __true_type)
{
  return ::copy(__first, __last, __result); 
}


template <class _InputIter, class _ForwardIter>
_ForwardIter  __uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                         _ForwardIter __result,
                         __false_type)
{
  _ForwardIter __cur = __result;
  
  __STL_TRY  //捕获异常 try 
  { 
    for ( ; __first != __last ; ++__first, ++__cur )
    {
    	_Construct(&(*__cur), *__first); //定位new表达式（使用的是 *cur 的地址）： new ( (void*) &(*__cur) ) _T1( *__first );   // placement new，调用 _T1::_T1( *__first );
    }
	
	return __cur;
  }
  __STL_UNWIND( _Destroy(__result, __cur ) ); // __result ： first  __cur ：last
  //  __STL_UNWIND (action) catch(...) { action; throw; }
}



template <class _InputIter, class _ForwardIter, class _Tp>
inline _ForwardIter __uninitialized_copy(_InputIter __first, _InputIter __last,
                     _ForwardIter __result, _Tp*)
{
  typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;  //_Tp 是类，还是基本类型 , 类：__false_type ，基本类型：__true_type , 原生指针：__true_type
  return __uninitialized_copy_aux(__first, __last, __result, _Is_POD());
}

template <class _InputIter, class _ForwardIter>
inline _ForwardIter uninitialized_copy(_InputIter __first, _InputIter __last,
                     _ForwardIter __result)
{
  return __uninitialized_copy(__first , __last , __result , __VALUE_TYPE(__result) );
}

inline char* uninitialized_copy(const char* __first, const char* __last,
                                char* __result) {
  memmove(__result, __first, __last - __first);
  return __result + (__last - __first);
}

/**
* wchar_t ：宽字符，一般为16位或32位 （32位系统与64位系统）
**/
inline wchar_t* uninitialized_copy(const wchar_t* __first, const wchar_t* __last,
                   wchar_t* __result)
{
  memmove(__result, __first, sizeof(wchar_t) * (__last - __first));
  return __result + (__last - __first);
}

// uninitialized_copy_n (not part of the C++ standard)
/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_copy_n 将  [__first , __last) 的值，复制给__result 形成的区间
 * @param __first 开始迭代器
 * @param __n 个数
 * @param __x 值
 * @param input_iterator_tag 仅仅用作重载函数的标记
 * @return 
 *
 * fill_n 最低层源码：
	 for ( ; __first != __last; ++__first)
		 *__first = __x;
 */
template <class _InputIter, class _Size, class _ForwardIter>
pair<_InputIter, _ForwardIter> __uninitialized_copy_n(_InputIter __first, _Size __count,
                       _ForwardIter __result,
                       input_iterator_tag)
{
  _ForwardIter __cur = __result;
  __STL_TRY {
    for ( ; __count > 0 ; --__count, ++__first, ++__cur) 
      _Construct(&*__cur, *__first);
    return pair<_InputIter, _ForwardIter>(__first, __cur);
  }
  __STL_UNWIND(_Destroy(__result, __cur));
}

template <class _RandomAccessIter, class _Size, class _ForwardIter>
inline pair<_RandomAccessIter, _ForwardIter> __uninitialized_copy_n(_RandomAccessIter __first, _Size __count,
                       _ForwardIter __result,
                       random_access_iterator_tag) {
  _RandomAccessIter __last = __first + __count;
  return pair<_RandomAccessIter, _ForwardIter>(
                 __last,
                 uninitialized_copy(__first, __last, __result));
}

template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter> __uninitialized_copy_n(_InputIter __first, _Size __count,
                     _ForwardIter __result) {
  return __uninitialized_copy_n(__first, __count, __result,
                                __ITERATOR_CATEGORY(__first));
}

template <class _InputIter, class _Size, class _ForwardIter>
inline pair<_InputIter, _ForwardIter> uninitialized_copy_n(_InputIter __first, _Size __count,
                     _ForwardIter __result) {
  return __uninitialized_copy_n(__first, __count, __result,
                                __ITERATOR_CATEGORY(__first));
}


// Valid if copy construction is equivalent to assignment, and if the
// destructor is trivial.

/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_fill_aux 将  [__first , __last) 的空间，全部填充成__x
 * @param __first 开始迭代器
 * @param __last 结束迭代器
 * @param __x 值
 * @param __true_type 仅仅用作重载函数的标记
 * @return 
 *
 * fill 最低层源码：
	 for ( ; __first != __last; ++__first)
		 *__first = __x;
 */

template <class _ForwardIter, class _Tp>
inline void __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, 
                         const _Tp& __x, __true_type)
{
  fill(__first, __last, __x);
}

template <class _ForwardIter, class _Tp>
void __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last, 
                         const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __cur != __last; ++__cur)
      _Construct(&*__cur, __x);
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Tp, class _Tp1>
inline void __uninitialized_fill(_ForwardIter __first, 
                                 _ForwardIter __last, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  __uninitialized_fill_aux(__first, __last, __x, _Is_POD());
                   
}

template <class _ForwardIter, class _Tp>
inline void uninitialized_fill(_ForwardIter __first,
                               _ForwardIter __last, 
                               const _Tp& __x)
{
  __uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
}

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_fill_n_aux 将 [__first , __first+n) 的空间，全部填充成__x
 * @param __first 开始迭代器
 * @param __n 个数
 * @param __x 值
 * @param __true_type 仅仅用作重载函数的标记
 * @return 
 *
 * fill_n 最低层源码：
	  for ( ; __n > 0; --__n, ++__first)
    	*__first = __value;
  	  return __first;
 */

template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                           const _Tp& __x, __true_type)
{
  return fill_n(__first, __n, __x);
}

template <class _ForwardIter, class _Size, class _Tp>
_ForwardIter __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                           const _Tp& __x, __false_type)
{
  _ForwardIter __cur = __first;
  __STL_TRY {
    for ( ; __n > 0; --__n, ++__cur)
      _Construct(&*__cur, __x);
    return __cur;
  }
  __STL_UNWIND(_Destroy(__first, __cur));
}

template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
inline _ForwardIter  __uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x, _Tp1*)
{
  typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
  return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
}

template <class _ForwardIter, class _Size, class _Tp>
inline _ForwardIter  uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x)
{
  return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
}


// Extensions: __uninitialized_copy_copy, __uninitialized_copy_fill, 
// __uninitialized_fill_copy.

// __uninitialized_copy_copy
// Copies [first1, last1) into [result, result + (last1 - first1)), and
//  copies [first2, last2) into
//  [result, result + (last1 - first1) + (last2 - first2)).

/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_copy_copy 复制 [first1, last1) 到 [result, result + (last1 - first1)),
 									And  复制 [first2, last2) 到 [result, result + (last1 - first1) + (last2 - first2)).
 * @param __first 开始迭代器
 * @param __n 个数
 * @param __x 值
 * @param __true_type 仅仅用作重载函数的标记
 * @return 
 */
template <class _InputIter1, class _InputIter2, class _ForwardIter>
inline _ForwardIter __uninitialized_copy_copy(_InputIter1 __first1, _InputIter1 __last1,
                          _InputIter2 __first2, _InputIter2 __last2,
                          _ForwardIter __result)
{
  _ForwardIter __mid = uninitialized_copy(__first1, __last1, __result);
  __STL_TRY {
    return uninitialized_copy(__first2, __last2, __mid);
  }
  __STL_UNWIND(_Destroy(__result, __mid));
}

// __uninitialized_fill_copy
// Fills [result, mid) with x, and copies [first, last) into
//  [mid, mid + (last - first)).

/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_fill_copy [result, mid)区间内填充成__x
 									[ mid , mid+(__last - __first) ]区间内，赋值为[__first , __last )区间内的值
 * @param __result 开始迭代器
 * @param __mid 结束迭代器
 * @param __x 值
 * @param __first 
 * @param __last
 * @param __true_type 仅仅用作重载函数的标记
 * @return 
 */
template <class _ForwardIter, class _Tp, class _InputIter>
inline _ForwardIter __uninitialized_fill_copy(_ForwardIter __result, _ForwardIter __mid,
                          const _Tp& __x,
                          _InputIter __first, _InputIter __last)
{
  uninitialized_fill(__result, __mid, __x);
  __STL_TRY {
    return uninitialized_copy(__first, __last, __mid);
  }
  __STL_UNWIND(_Destroy(__result, __mid));
}

// __uninitialized_copy_fill
// Copies [first1, last1) into [first2, first2 + (last1 - first1)), and
//  fills [first2 + (last1 - first1), last2) with x.

/**
 * @author wangqin
 * @time 2023 / 06 / 11
 * @title  
 * @brief __uninitialized_copy_fill [first2, first2 + (last1 - first1))区间 ，赋值为[first1, last1)的值  
 									[first2 + (last1 - first1), last2)区间， 填充为 x
 * @param __result 开始迭代器
 * @param __mid 结束迭代器
 * @param __x 值
 * @param __first 
 * @param __last
 * @param __true_type 仅仅用作重载函数的标记
 * @return 
 */
template <class _InputIter, class _ForwardIter, class _Tp>
inline void __uninitialized_copy_fill(_InputIter __first1, _InputIter __last1,
                          _ForwardIter __first2, _ForwardIter __last2,
                          const _Tp& __x)
{
  _ForwardIter __mid2 = uninitialized_copy(__first1, __last1, __first2);
  __STL_TRY {
    uninitialized_fill(__mid2, __last2, __x);
  }
  __STL_UNWIND(_Destroy(__first2, __mid2));
}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_UNINITIALIZED_H */

// Local Variables:
// mode:C++
// End:
