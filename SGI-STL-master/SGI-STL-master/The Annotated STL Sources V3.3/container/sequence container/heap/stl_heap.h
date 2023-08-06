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
 * Copyright (c) 1997
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
// heap 并没提供外部调用，在 SGI STL 里用于 priority queue 实现
#ifndef __SGI_STL_INTERNAL_HEAP_H
#define __SGI_STL_INTERNAL_HEAP_H

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma set woff 1209
#endif

// Heap-manipulation functions: push_heap, pop_heap, make_heap, sort_heap.
// push_heap 做 shift up 操作，调整满足 max-heap 特性
/**
*@breif __push_heap		往尾部插入数据
*@data 2023/08/02
*@author wq
*@param1: __first
*@param2: __holeIndex 尾部的位置
*@param3: __topIndex 顶部索引 (默认 为 0)
*@param4: __value
*@步骤:
* 	见注释
*/
template <class _RandomAccessIterator , class _Distance , class _Tp>
void __push_heap( _RandomAccessIterator __first , _Distance __holeIndex , _Distance __topIndex , _Tp __value )
{
	_Distance __parent = ( __holeIndex - 1 ) / 2; //找到插入节点的父节点位置 （ 数组第0位保留，插入数据从第一位开始 ）

	//尚未达到顶端 ， 当插入值大于其父节点的值
	while ( __holeIndex > __topIndex  &&  *( __first + __parent ) < __value) 
	{ 
		*( __first + __holeIndex ) = *( __first + __parent );   // 将其插入节点的父节点的值赋给插入节点位置

		__holeIndex = __parent;  // 索引位置改变，插入值的索引为父节点索引

		__parent = ( __holeIndex - 1 ) / 2; // 插入值的索引的父节点
	} 

	*(__first + __holeIndex) = __value; // 交换完后，找到插入值的真正位置，赋值
	
}

// push_heap 操作的调整函数(函数模板)
template <class _RandomAccessIterator, class _Distance, class _Tp>
inline void __push_heap_aux(_RandomAccessIterator __first ,         _RandomAccessIterator __last, _Distance*, _Tp*)
{
  	// 第四个参数就是要插入的值，位于 vector 尾部
	__push_heap( __first ,  _Distance( ( __last - __first ) - 1 ) ,  _Distance(0) , _Tp( *(__last - 1) ) );  
}

// push_heap 实现，接受两个迭代器
template <class _RandomAccessIterator>
inline void push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{

  __STL_REQUIRES( _RandomAccessIterator , _Mutable_RandomAccessIterator );
  
  __STL_REQUIRES( typename iterator_traits<_RandomAccessIterator>::value_type , _LessThanComparable );
  
  // 此函数被调用时，新元素应置于底部容器的最尾端
  __push_heap_aux( __first , __last , __DISTANCE_TYPE(__first) , __VALUE_TYPE(__first) );
  
}

template <class _RandomAccessIterator , class _Distance , class _Tp , class _Compare>
void __push_heap( _RandomAccessIterator __first , _Distance __holeIndex ,
						_Distance __topIndex , _Tp __value , _Compare __comp )
{
  _Distance __parent = (__holeIndex - 1) / 2;
  
  while (__holeIndex > __topIndex && __comp(*(__first + __parent), __value))
  {
    *(__first + __holeIndex) = *(__first + __parent);
	
    __holeIndex = __parent;
	
    __parent = (__holeIndex - 1) / 2;
	
  }
  
  *(__first + __holeIndex) = __value;
}

template <class _RandomAccessIterator , class _Compare , class _Distance , class _Tp>
inline void __push_heap_aux(_RandomAccessIterator __first ,        _RandomAccessIterator __last , _Compare __comp ,
					                _Distance*, _Tp* ) 
{ 
	__push_heap(__first , _Distance( (__last - __first) - 1 ) , _Distance(0) , _Tp( *(__last - 1) ) , __comp);
}

// 这个 push_heap 允许指定 “大小比较标准”
template <class _RandomAccessIterator, class _Compare>
inline void push_heap( _RandomAccessIterator __first , _RandomAccessIterator __last , _Compare __comp )
{

  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator );
  
  __push_heap_aux( __first, __last , __comp , __DISTANCE_TYPE( __first ) , __VALUE_TYPE( __first ) );

}

// vector 头部索引为 0 开始 , __value 为尾部元素
// pop_heap 调整
template <class _RandomAccessIterator, class _Distance, class _Tp>
void __adjust_heap( _RandomAccessIterator __first , _Distance __holeIndex , _Distance __len , _Tp __value )
{
	//以下例子：以__make_heap 中 ， 以 50 , 65 , 68 , 22 , 31 , 21 为例
	
	_Distance __topIndex = __holeIndex;  //__holeIndex = 0，为 heap 的根节点

	_Distance __secondChild = 2 * __holeIndex + 2 ; //根节点的右节点的索引 （holeIndex 从0开始算起）

													
	while ( __secondChild < __len )
	{
		if ( *( __first + __secondChild ) < *( __first + (__secondChild - 1) ) ) // 比较根节点的左右节点值 ,右节点 < 左节点
			__secondChild-- ;

		*( __first + __holeIndex ) = *( __first + __secondChild );  // while 里做 shift down 操作 （左右节点谁大，谁赋值给根节点）

		__holeIndex = __secondChild; //( 更新根节点位置 )
		
		__secondChild = 2 * ( __secondChild + 1 ); //新根节点的右节点索引

	}	//第一步结束（50 , 31 , 68 , 22 ,  , 21）
		//第二步结束（68 , 65 ,        , 22 , 31 , 21 ）

	// 没有右子节点，只有左子节点
	if (__secondChild == __len) 
	{ 
		*(__first + __holeIndex) = *(__first + (__secondChild - 1));
		
		__holeIndex = __secondChild - 1;

	}

	// 第一步：__holdIndex = 4 , __topIndex = 1 , __value = 65 ， 经过push_heap() 之后 顺序：50 , 65 , 68 , 22 , 31 , 21 
	// 第二步：__holdIndex = 2 , __topIndex = 0 , __value = 50 ， 经过push_heap之后，顺序：68 ， 65 ， 50 ， 22 ， 31 ， 21 
  	// 找到真正的位置，插入     			
	__push_heap( __first , __holeIndex , __topIndex , __value );   
  
}

template <class _RandomAccessIterator, class _Tp, class _Distance>
inline void __pop_heap(_RandomAccessIterator __first  , _RandomAccessIterator __last,
           					_RandomAccessIterator __result , _Tp __value, _Distance*)
{
  *__result = *__first;  // 先保存 vector 头部元素，放到 vector 的尾端
  
  __adjust_heap( __first , _Distance(0) , _Distance( __last - __first ) , __value ); // 再做 shift down 操作调整
    
}

template <class _RandomAccessIterator, class _Tp>
inline void __pop_heap_aux(_RandomAccessIterator __first, 
									_RandomAccessIterator __last,
               						_Tp*)
{
  // 调整 heap 后，变为[first, last-1)
  __pop_heap( __first , __last - 1 , __last - 1 , _Tp( *(__last - 1) ) , __DISTANCE_TYPE( __first ) ); 
  
}

// pop_heap
template <class _RandomAccessIterator>
inline void pop_heap( _RandomAccessIterator __first , _RandomAccessIterator __last )
{
  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
  __STL_REQUIRES( typename iterator_traits<_RandomAccessIterator>::value_type ,  _LessThanComparable );

 // pop_heap 调整函数，将 vector 头部元素放到 vector 的尾部
  __pop_heap_aux( __first , __last , __VALUE_TYPE(__first) ); 
 
}

template <class _RandomAccessIterator, class _Distance , class _Tp, class _Compare>
void __adjust_heap(_RandomAccessIterator __first , 		 _Distance __holeIndex , 
						 _Distance __len , _Tp __value, _Compare __comp)
{
  _Distance __topIndex = __holeIndex;
  
  _Distance __secondChild = 2 * __holeIndex + 2;
  
  while (__secondChild < __len) 
  {
    if ( __comp( *(__first + __secondChild) ,  *(__first + (__secondChild - 1) ) ) )
      __secondChild--;
	
    *(__first + __holeIndex) = *(__first + __secondChild);
	
    __holeIndex = __secondChild;
	
    __secondChild = 2 * (__secondChild + 1);
	
  }
  
  if (__secondChild == __len) 
  {
    *( __first + __holeIndex ) = *( __first + (__secondChild - 1) ) ;
	
    __holeIndex = __secondChild - 1 ;
	
  }
  
  __push_heap(__first, __holeIndex, __topIndex, __value, __comp);
  
}

template <class _RandomAccessIterator, class _Tp, class _Compare , class _Distance>
inline void __pop_heap(_RandomAccessIterator __first, 
							_RandomAccessIterator __last,
           					_RandomAccessIterator __result, 
           					_Tp __value, _Compare __comp,
           					_Distance* )
{
  *__result = *__first;
  
  __adjust_heap(__first, _Distance(0) , _Distance(__last - __first) , __value , __comp);
  
}

template <class _RandomAccessIterator, class _Tp, class _Compare>
inline void __pop_heap_aux(_RandomAccessIterator __first,
								  _RandomAccessIterator __last, 
								  _Tp*, _Compare __comp)
{
  __pop_heap(__first , __last - 1 , __last - 1 , _Tp(*(__last - 1)) , __comp , __DISTANCE_TYPE(__first) );
}

// pop_heap 允许指定大小比较标准
template <class _RandomAccessIterator, class _Compare>
inline void pop_heap(_RandomAccessIterator __first,
        				 _RandomAccessIterator __last, 
        				 _Compare __comp )
{
  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
  __pop_heap_aux(__first, __last, __VALUE_TYPE(__first), __comp);
}

// [first, last) 调整为 heap
template <class _RandomAccessIterator, class _Tp, class _Distance>
void __make_heap( _RandomAccessIterator __first,
            		   _RandomAccessIterator __last, 
            		   _Tp*, _Distance* )
{
	//50 , 65 , 68 , 22 , 31 , 21 -- 从0开始数( make_heap 以此为例 )

	// 如果长度为 0 或 1，不排列
	if (__last - __first < 2) 
		return; 

	_Distance __len = __last - __first; // 待排列的长度

	_Distance __parent = ( __len - 2 ) / 2; // 父节点i 和 右子节点 2i+2 （从0算起）


	// 这里 __parent 就是待插入节点索引
	while (true) 
	{ 
		__adjust_heap( __first , __parent , __len , _Tp( *( __first + __parent ) ) );

		if ( __parent == 0 )
			return;

		__parent-- ;
		
	}
  
}

// make_heap 不指定大小比较标准，将现有的数据转化一个 heap
template <class _RandomAccessIterator>
inline void make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type,
                 _LessThanComparable);
  __make_heap(__first , __last , __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}

template <class _RandomAccessIterator, class _Compare , class _Tp , class _Distance>
void __make_heap(_RandomAccessIterator __first, 
						_RandomAccessIterator __last,
            		   _Compare __comp, _Tp*, _Distance*)
{
	if (__last - __first < 2)
		return;

	_Distance __len = __last - __first;

	_Distance __parent = ( __len - 2 ) / 2;

	while (true) 
	{ 

		__adjust_heap( __first , __parent , __len , _Tp(*(__first + __parent)) , __comp);

		if (__parent == 0) 
			return;

		__parent--;

	}
}

// make_heap 允许指定大小比较标准
template <class _RandomAccessIterator, class _Compare>
inline void make_heap(_RandomAccessIterator __first, 
          					_RandomAccessIterator __last, 
          					_Compare __comp)
{
	__STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
	__make_heap(__first , __last , __comp , __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
}

// sort_heap 不指定大小比较标准
template <class _RandomAccessIterator>
void sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
  
  __STL_REQUIRES(typename iterator_traits<_RandomAccessIterator>::value_type ,_LessThanComparable);
  while (__last - __first > 1)
    pop_heap(__first, __last--); // 调用 pop_heap
}

// sort_heap 允许指定大小比较标准
template <class _RandomAccessIterator, class _Compare>
void sort_heap(_RandomAccessIterator __first , _RandomAccessIterator __last, _Compare __comp)
{
  __STL_REQUIRES(_RandomAccessIterator, _Mutable_RandomAccessIterator);
  while (__last - __first > 1)
    pop_heap(__first, __last--, __comp);
}

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1209
#endif

__STL_END_NAMESPACE

#endif /* __SGI_STL_INTERNAL_HEAP_H */

// Local Variables:
// mode:C++
// End:
