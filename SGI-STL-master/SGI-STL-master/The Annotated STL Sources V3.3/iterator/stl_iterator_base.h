/*
 * 迭代器基类
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
 * Copyright (c) 1996-1998
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

// 内部头文件，被 STL 标准头文件 <iterator> 包含
#ifndef __SGI_STL_INTERNAL_ITERATOR_BASE_H
#define __SGI_STL_INTERNAL_ITERATOR_BASE_H

// This file contains all of the general iterator-related utilities.
// The internal file stl_iterator.h contains predefined iterators, 
// such as front_insert_iterator and istream_iterator.

#include <concept_checks.h>

__STL_BEGIN_NAMESPACE

/**
* ptrdiff_t 数据类型
* 这是有符号整数类型，它是两个指针相减的结果
* ptrdiff_t通常被定义为long int类型
*/

// iterator_category 五种迭代器类型
// 标记
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


/**
* @berif input_iterator 设计适当的input_iterator是容器的责任
* @author wq 
* @date 2023/07/19
* @return
*/
// The base classes input_iterator, output_iterator, forward_iterator,
// bidirectional_iterator, and random_access_iterator are not part of
// the C++ standard.  (They have been replaced by struct iterator.)
// They are included for backward compatibility with the HP STL.
template <class _Tp, class _Distance> 
struct input_iterator {
  typedef input_iterator_tag iterator_category;
  typedef _Tp                value_type;
  typedef _Distance          difference_type;
  typedef _Tp*               pointer;
  typedef _Tp&               reference;
};

/**
* @berif output_iterator 设计适当的output_iterator是容器的责任
* @author wq 
* @date 2023/07/19
* @return
*/
struct output_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;
};


/**
* @berif forward_iterator 设计适当的 forward_iterator 是容器的责任
* @author wq 
* @date 2023/07/19
* @return
*/
template <class _Tp, class _Distance> 
struct forward_iterator {
  typedef forward_iterator_tag iterator_category;
  typedef _Tp                  value_type;
  typedef _Distance            difference_type;
  typedef _Tp*                 pointer;
  typedef _Tp&                 reference;
};


/**
* @berif bidirectional_iterator 设计适当的 bidirectional_iterator 是容器的责任
* @author wq 
* @date 2023/07/19
* @return
*/
template <class _Tp, class _Distance> 
struct bidirectional_iterator {
  typedef bidirectional_iterator_tag iterator_category;
  typedef _Tp                        value_type;
  typedef _Distance                  difference_type;
  typedef _Tp*                       pointer;
  typedef _Tp&                       reference;
};

/**
* @berif random_access_iterator 设计适当的 random_access_iterator 是容器的责任
* @author wq 
* @date 2023/07/19
* @return
*/
template <class _Tp, class _Distance> 
struct random_access_iterator {
  typedef random_access_iterator_tag iterator_category;
  typedef _Tp                        value_type;
  typedef _Distance                  difference_type;
  typedef _Tp*                       pointer;
  typedef _Tp&                       reference;
};





// 用户自己的写的迭代器最好继承此 std::iterator
#ifdef __STL_USE_NAMESPACES

	template <class _Category , class _Tp , class _Distance = ptrdiff_t ,
	          class _Pointer = _Tp* , class _Reference = _Tp&>
	struct iterator {
	  typedef _Category  iterator_category;
	  typedef _Tp        value_type;
	  typedef _Distance  difference_type;
	  typedef _Pointer   pointer;
	  typedef _Reference reference;
	};

#endif /* __STL_USE_NAMESPACES */


#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION   /* 定义 __STL_CLASS_PARTIAL_SPECIALIZATION 使用traits编程技法 */

	// traits 获取各个迭代器的特性(相应类型)-----类型特性类
	template <class _Iterator>
	struct iterator_traits {
	  typedef typename _Iterator::iterator_category iterator_category; // 迭代器类别 （input_...._tag , output_..._tag , forward_..._tag , bidirectional..._tag , random_access_..._tag)
	  typedef typename _Iterator::value_type        value_type;   //迭代器解除引用后所得到的值的类型
	  typedef typename _Iterator::difference_type   difference_type; // 两个迭代器之间的距离
	  typedef typename _Iterator::pointer           pointer;      //指向被迭代类型的指针
	  typedef typename _Iterator::reference         reference;   // 被迭代类型的引用类型
	};

	// 针对原生指针(native pointer)而设计的 traits 偏特化版
	template <class _Tp>
	struct iterator_traits<_Tp*> {
	  typedef random_access_iterator_tag  iterator_category;
	  typedef _Tp                         value_type;
	  typedef ptrdiff_t                   difference_type;  // C++ 内建的 ptrdiff_t 类型
	  typedef _Tp*                        pointer;
	  typedef _Tp&                        reference;
	};

	// 针对原生之 pointer-to-const 而设计的 traits 偏特化版
	template <class _Tp>
	struct iterator_traits<const _Tp*> {
	  typedef random_access_iterator_tag  iterator_category;
	  typedef _Tp                         value_type;
	  typedef ptrdiff_t                   difference_type;
	  typedef const _Tp*                  pointer;
	  typedef const _Tp&                  reference;
	};

	// The overloaded functions iterator_category, distance_type, and
	// value_type are not part of the C++ standard.  (They have been
	// replaced by struct iterator_traits.)  They are included for
	// backward compatibility with the HP STL.

	// We introduce internal names for these functions.

	// 决定某个迭代器的类型-category 类别
	template <class _Iter>
	inline typename iterator_traits<_Iter>::iterator_category __iterator_category(const _Iter&)
	{
		typedef  typename iterator_traits<_Iter>::iterator_category  _Category; //重定义

		return _Category();
	}

	// 决定某个迭代器的类型-difference type
	template <class _Iter>
	inline typename iterator_traits<_Iter>::difference_type* __distance_type(const _Iter&)
	{
		return static_cast<typename iterator_traits<_Iter>::difference_type*>(0);
	}

	// 决定某个迭代器的类型-value_type
	template <class _Iter>
	inline typename iterator_traits<_Iter>::value_type* __value_type(const _Iter&)
	{
		return static_cast<typename iterator_traits<_Iter>::value_type*>(0);
	}

	// 封装 __iterator_category 函数
	template <class _Iter>
	inline typename iterator_traits<_Iter>::iterator_category iterator_category(const _Iter& __i) 
	{ 
		return __iterator_category(__i); 
	}

	// 封装 __distance_type 函数
	template <class _Iter>
	inline typename iterator_traits<_Iter>::difference_type* distance_type(const _Iter& __i) 
	{ 
		return __distance_type(__i); 
	}

	// 封装 value_type 函数
	template <class _Iter>
	inline typename iterator_traits<_Iter>::value_type* value_type(const _Iter& __i) 
	{ 
		return __value_type(__i); 
	}

	//宏定义 函数调用
	#define __ITERATOR_CATEGORY(__i) __iterator_category(__i)

	#define __DISTANCE_TYPE(__i)     __distance_type(__i)

	#define __VALUE_TYPE(__i)        __value_type(__i)
	

#else /* 未定义__STL_CLASS_PARTIAL_SPECIALIZATION ，未使用 traits 编程技法 */

	/****************************重载 iterator_category()五种迭代器+一个原生指针************************************/
	template <class _Tp, class _Distance> 
	inline input_iterator_tag  iterator_category(const input_iterator<_Tp, _Distance>&)
	{ 
	 	return input_iterator_tag(); 
	}

	inline output_iterator_tag iterator_category(const output_iterator&)
	{ 
	 	return output_iterator_tag(); 
	}

	template <class _Tp, class _Distance> 
	inline forward_iterator_tag iterator_category(const forward_iterator<_Tp, _Distance>&)
	{ 
		return forward_iterator_tag(); 
	}

	template <class _Tp, class _Distance> 
	inline bidirectional_iterator_tag iterator_category(const bidirectional_iterator<_Tp, _Distance>&)
	{ 
		return bidirectional_iterator_tag(); 
	}

	template <class _Tp, class _Distance> 
	inline random_access_iterator_tag iterator_category(const random_access_iterator<_Tp, _Distance>&)
	{ 
		return random_access_iterator_tag(); 
	}

	template <class _Tp>
	inline random_access_iterator_tag iterator_category(const _Tp*)
	 { 
	 	return random_access_iterator_tag(); 
	}

	 
	/***************************重载 value_type() 4种迭代器+一个原生指针****************************************/
	template <class _Tp, class _Distance> 
	inline _Tp* value_type(const input_iterator<_Tp, _Distance>&)
	{ 
	  return (_Tp*)(0); 
	}

	template <class _Tp, class _Distance> 
	inline _Tp* value_type(const forward_iterator<_Tp, _Distance>&)
	{ 
	 	return (_Tp*)(0); 
	}

	template <class _Tp, class _Distance> 
	inline _Tp* value_type(const bidirectional_iterator<_Tp, _Distance>&)
	{ 
		return (_Tp*)(0); 
	}

	template <class _Tp, class _Distance> 
	inline _Tp* value_type(const random_access_iterator<_Tp, _Distance>&)
	{ 
		return (_Tp*)(0); 
	}

	template <class _Tp>
	inline _Tp* value_type(const _Tp*) 
	{ 
		return (_Tp*)(0); 
	}


	/*************************** 重载 distance_type() 4种迭代器 + 一个原生指针 ****************************************/
	template <class _Tp, class _Distance> 
	inline _Distance* distance_type(const input_iterator<_Tp, _Distance>&)
	{
	  return (_Distance*)(0);
	}

	template <class _Tp, class _Distance> 
	inline _Distance* distance_type(const forward_iterator<_Tp, _Distance>&)
	{
	  return (_Distance*)(0);
	}

	template <class _Tp, class _Distance> 
	inline _Distance* distance_type(const bidirectional_iterator<_Tp, _Distance>&)
	{
	  return (_Distance*)(0);
	}

	template <class _Tp, class _Distance> 
	inline _Distance* distance_type(const random_access_iterator<_Tp, _Distance>&)
	{
	  return (_Distance*)(0);
	}

	template <class _Tp>
	inline ptrdiff_t* distance_type(const _Tp*) 
	{ 
		return (ptrdiff_t*)(0); 
	}

	// Without partial specialization we can't use iterator_traits, so
	// we must keep the old iterator query functions around.  

	#define __ITERATOR_CATEGORY(__i) iterator_category(__i)

	#define __DISTANCE_TYPE(__i)     distance_type(__i)

	#define __VALUE_TYPE(__i)        value_type(__i)

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

/**
 * distance 函数 ： 计算两个迭代器之间的距离  
 * @param __first 迭代器1
 * @param __last 迭代器2
 * @param __n 距离
 */
 /* 未定义__STL_CLASS_PARTIAL_SPECIALIZATION ，未使用 traits 编程技法 */
// 逐一累计距离
template <class _InputIterator, class _Distance>
inline void __distance(_InputIterator __first , _InputIterator __last , _Distance& __n , input_iterator_tag)
{
	while (__first != __last) 
	{ 
		++__first; 
		++__n; 
	}
}

// 直接计算距离
template <class _RandomAccessIterator, class _Distance>
inline void __distance(_RandomAccessIterator __first , _RandomAccessIterator __last , _Distance& __n, random_access_iterator_tag)
{
	__STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
	__n += __last - __first;
}

//封装的接口
template <class _InputIterator, class _Distance>
inline void distance(_InputIterator __first , _InputIterator __last , _Distance& __n)
{
	__STL_REQUIRES(_InputIterator, _InputIterator); //do {} while(0)
	__distance(__first, __last, __n, iterator_category(__first));
}


// traits_iterator
#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION  /* 定义 __STL_CLASS_PARTIAL_SPECIALIZATION 使用traits编程技法 */

	// 逐一累计距离
	template <class _InputIterator>
	inline typename iterator_traits<_InputIterator>::difference_type __distance(_InputIterator __first, _InputIterator __last, input_iterator_tag)
	{
	  typename iterator_traits<_InputIterator>::difference_type __n = 0;
	  while (__first != __last) 
	  {
	    ++__first; 
		++__n;
	  }
	  return __n;
	}

	// 直接计算距离
	template <class _RandomAccessIterator>
	inline typename iterator_traits<_RandomAccessIterator>::difference_type __distance(_RandomAccessIterator __first , _RandomAccessIterator __last , random_access_iterator_tag) 
	{
	  __STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
	  return __last - __first;
	}

	template <class _InputIterator>
	inline typename iterator_traits<_InputIterator>::difference_type distance(_InputIterator __first, _InputIterator __last)
	{
		typedef typename iterator_traits<_InputIterator>::iterator_category 
		_Category;
		__STL_REQUIRES(_InputIterator, _InputIterator);
		return __distance(__first, __last, _Category());
	}
	

#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

/ **
* advance 函数 ： 迭代器前进n个距离
* @param __i : 迭代器
* @param __n : 前进的步数s
*/
template <class _InputIter, class _Distance>
inline void __advance(_InputIter& __i, _Distance __n, input_iterator_tag)
{
	while (__n--)
		++__i;  // 单向，加上标记 input_iterator_tag 
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
	#pragma set woff 1183	
#endif

template <class _BidirectionalIterator, class _Distance>
inline void __advance(_BidirectionalIterator& __i , _Distance __n , bidirectional_iterator_tag) 
{
	//双向迭代器 ，可能 ++ 或则 --
	__STL_REQUIRES(_BidirectionalIterator, _BidirectionalIterator);
	if (__n >= 0)    // 双向，加上标记 bidirectional_iterator_tag 
	{
		while (__n--) 
			++__i;
	}
	else
	{
		while (__n++) 
			--__i;
	}
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
	#pragma reset woff 1183
#endif

template <class _RandomAccessIterator, class _Distance>
inline void __advance( _RandomAccessIterator& __i , _Distance __n , random_access_iterator_tag) {
  __STL_REQUIRES(_RandomAccessIterator, _RandomAccessIterator);
  __i += __n;  // 双向，跳跃前进，加上标记 random_access_iterator_tag
}

template <class _InputIterator, class _Distance>
inline void advance(_InputIterator& __i, _Distance __n) 
{
  __STL_REQUIRES(_InputIterator, _InputIterator);
  __advance(__i, __n, iterator_category(__i));
}

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_ITERATOR_BASE_H */



// Local Variables:
// mode:C++
// End:
