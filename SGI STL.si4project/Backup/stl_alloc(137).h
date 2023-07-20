/*
 * 空间配置器
 * Copyright (c) 1996-1997
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

// <stl_alloc.h> 被包含到其它的 STL 头文件中，并不是直接使用
// SGI STL 两级配置

#ifndef __SGI_STL_INTERNAL_ALLOC_H
	#define __SGI_STL_INTERNAL_ALLOC_H
#ifdef __SUNPRO_CC
	#define __PRIVATE public
   // Extra access restrictions prevent us from really making some things
   // private.
#else
	#define __PRIVATE private
#endif

#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
	#define __USE_MALLOC
#endif


// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#ifndef __THROW_BAD_ALLOC
	#if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
		#include <stdio.h>
		#include <stdlib.h>
		#define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
	#else /* Standard conforming out-of-memory handling */
		#include <new>
		#define __THROW_BAD_ALLOC throw std::bad_alloc()
	#endif
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef __RESTRICT
	#define __RESTRICT
#endif

#ifdef __STL_THREADS
	#include <stl_threads.h>
	#define __NODE_ALLOCATOR_THREADS true
	#ifdef __STL_SGI_THREADS
		// We test whether threads are in use before locking.
		// Perhaps this should be moved into stl_threads.h, but that
		// probably makes it harder to avoid the procedure call when
		// it isn't needed.
		extern "C" {
		  extern int __us_rsthread_malloc;
		}
		// The above is copied from malloc.h.  Including <malloc.h>
		// would be cleaner but fails with certain levels of standard
		// conformance.
		#define __NODE_ALLOCATOR_LOCK if (threads && __us_rsthread_malloc) \
		            { _S_node_allocator_lock._M_acquire_lock(); }
		#define __NODE_ALLOCATOR_UNLOCK if (threads && __us_rsthread_malloc) \
		            { _S_node_allocator_lock._M_release_lock(); }
	#else /* !__STL_SGI_THREADS */
		#define __NODE_ALLOCATOR_LOCK \
			{ if (threads) _S_node_allocator_lock._M_acquire_lock(); }
		#define __NODE_ALLOCATOR_UNLOCK \
			{ if (threads) _S_node_allocator_lock._M_release_lock(); }
	#endif
#else
	//  Thread-unsafe
	#define __NODE_ALLOCATOR_LOCK
	#define __NODE_ALLOCATOR_UNLOCK
	#define __NODE_ALLOCATOR_THREADS false
#endif

__STL_BEGIN_NAMESPACE

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
	#pragma set woff 1174
#endif

// Malloc-based allocator.  Typically slower than default alloc below.
// Typically thread-safe and more storage efficient.
#ifdef __STL_STATIC_TEMPLATE_MEMBER_BUG
	#ifdef __DECLARE_GLOBALS_HERE
    	void (* __malloc_alloc_oom_handler)() = 0;
    // g++ 2.7.2 does not handle static template data members.
	#else
    	extern void (* __malloc_alloc_oom_handler)();
	#endif
#endif

// SGI STL 第一级配置器
// 无 “template 类型参数”，“非类型参数 __inst”，完全没有用
template <int __inst>
class __malloc_alloc_template {

private:
  
  // 以下函数将用来处理内存不足的情况
  static void* _S_oom_malloc(size_t);
  static void* _S_oom_realloc(void*, size_t);

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
  static void (* __malloc_alloc_oom_handler)(); //变量定义： 类型（ void(*)() ） 变量名 （ __malloc_alloc_oom_handler ） ;  
#endif

public:

  // 第一级配置器直接调用 malloc()
  static void* allocate(size_t __n)
  {
    void* __result = malloc(__n); 
    // 以下无法满足需求时（malloc申请内存失败），改用 _S_oom_malloc()
    if (0 == __result) 
		__result = _S_oom_malloc(__n);
    return __result;
  }

  // 第一级配置器直接调用 free()
  static void deallocate(void* __p, size_t /* __n */)
  {
    	free(__p);
  }
  
  // 第一级配置器直接调用 realloc() --重新分配内存块大小
  static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz)
  {
    void* __result = ::realloc(__p, __new_sz);
    // 以下无法满足需求时，改用 _S_oom_realloc()
    if (0 == __result)
		__result = _S_oom_realloc(__p, __new_sz);
    return __result;
  }

	/**
	* @berif 当配置内存失败时，使用out-of-memory handler释放内存
	* @author wq 
	* @date 2023/07/17
	*/
  // 以下仿真 C++ 的 set_new_handler()，可以通过它指定自己的 out-of-memory handler
  // 为什么不使用 C++ new-handler 机制，因为第一级配置器并没有 ::operator new 来配置内存
  static void (* __set_malloc_handler(void (*__f)()))()
  {
    void (* __old)() =  __malloc_alloc_template<int>::__malloc_alloc_oom_handler; 
	
    __malloc_alloc_template<int>::__malloc_alloc_oom_handler = __f;

	return (__old); //返回上一次的 out-of-memory handler 处理函数
  }

  /**
  * 变量定义： 类型 变量名；
  * 函数定义：返回值 函数名(形参列表) ）；
  *
  * 通俗易懂写法
  * typedef void (* AA )(); 
  * typedef void (* BB )();
  * static AA __set_malloc_handler ( BB f )
  *	{
  *  	void (* __old)() = __malloc_alloc_oom_handler;
  *  	__malloc_alloc_oom_handler = __f;
  *  	return(__old);
  *	}
  */

};

// malloc_alloc out-of-memory handling

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
	// 初值为0，由客户自行设定
	template <int __inst>
	void (* __malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif


template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_malloc(size_t __n)
{
    void (* __my_malloc_handler)();
    void* __result;

    // 不断尝试释放、配置
    for (;;) {
        __my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == __my_malloc_handler) 
		{ 
			__THROW_BAD_ALLOC; 
		}
		
        (*__my_malloc_handler)();  // 调用处理例程，企图释放内存
        
        __result = malloc(__n);   // 再次尝试配置内存
        
        if (__result) 
			return(__result);
    }
}

template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_realloc(void* __p, size_t __n)
{
    void (* __my_malloc_handler)();
    void* __result;

    //  给一个已经分配了地址的指针重新分配空间，参数 __p 为原有的空间地址，__n 是重新申请的地址长度
    for (;;) {
	// 当 "内存不足处理例程" 并未被客户设定，便调用 __THROW_BAD_ALLOC，丢出 bad_alloc 异常信息
        __my_malloc_handler = __malloc_alloc_oom_handler;

		if (0 == __my_malloc_handler) 
			{ __THROW_BAD_ALLOC; }
		
        (*__my_malloc_handler)();   // 调用处理例程，企图释放内存

		__result = realloc( __p , __n );  // 再次尝试配置内存，扩大内存大小

		if (__result) 
			return(__result);
    }
}

/**
* @berif 重定义 “第一级配置器”
* @author wq 
* @date 2023/07/17
*/
// 直接将参数 __inst 指定为0 ()
typedef __malloc_alloc_template<0> malloc_alloc; 



// 单纯地转调用，调用传递给配置器(第一级或第二级)；多一层包装，使 _Alloc 具备标准接口
template<class _Tp, class _Alloc>
class simple_alloc {

public:
    // 配置 n 个元素
    static _Tp* allocate(size_t __n)
      { return 0 == __n ? 0 : (_Tp*) _Alloc::allocate(__n * sizeof (_Tp)); }
    static _Tp* allocate(void)
      { return (_Tp*) _Alloc::allocate(sizeof (_Tp)); }
    static void deallocate(_Tp* __p, size_t __n)
      { if (0 != __n) _Alloc::deallocate(__p, __n * sizeof (_Tp)); }
    static void deallocate(_Tp* __p)
      { _Alloc::deallocate(__p, sizeof (_Tp)); }
};


/**
* @berif 调试alloc , 报错使用asserts
* @author wq 
* @date 2023/07/17
*/
// Allocator adaptor to check size arguments for debugging.
// Reports errors using assert.  Checking can be disabled with
// NDEBUG, but it's far better to just use the underlying allocator
// instead when no checking is desired.
// There is some evidence that this can confuse Purify.
template <class _Alloc>
class debug_alloc {

private:

  enum {_S_extra = 8};  // Size of space used to store size.  Note
                        // that this must be large enough to preserve
                        // alignment.

public:

  static void* allocate(size_t __n)
  {
    char* __result = (char*)_Alloc::allocate(__n + (int) _S_extra);
    *(size_t*)__result = __n;
    return __result + (int) _S_extra;
  }

  static void deallocate(void* __p, size_t __n)
  {
    char* __real_p = (char*)__p - (int) _S_extra;
    assert(*(size_t*)__real_p == __n);
    _Alloc::deallocate(__real_p, __n + (int) _S_extra);
  }

  static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz)
  {
    char* __real_p = (char*)__p - (int) _S_extra;
    assert(*(size_t*)__real_p == __old_sz);
    char* __result = (char*)
      _Alloc::reallocate(__real_p, __old_sz + (int) _S_extra,
                                   __new_sz + (int) _S_extra);
    *(size_t*)__result = __new_sz;
    return __result + (int) _S_extra;
  }

};


#ifdef __USE_MALLOC

	typedef malloc_alloc alloc;  // 令 alloc 为第一级配置器
	typedef malloc_alloc single_client_alloc;

#else


// Default node allocator.
// With a reasonable compiler, this should be roughly as fast as the
// original STL class-specific allocators, but with less fragmentation.
// Default_alloc_template parameters are experimental and MAY
// DISAPPEAR in the future.  Clients should just use alloc for now.
//
// Important implementation properties:
// 1. If the client request an object of size > _MAX_BYTES, the resulting
//    object will be obtained directly from malloc.
// 2. In all other cases, we allocate an object of size exactly
//    _S_round_up(requested_size).  Thus the client has enough size
//    information that we can return the object to the proper free list
//    without permanently losing part of the object.
//

// The first template parameter specifies whether more than one thread
// may use this allocator.  It is safe to allocate an object from
// one instance of a default_alloc and deallocate it with another
// one.  This effectively transfers its ownership to the second one.
// This may have undesirable effects on reference locality.
// The second parameter is unreferenced and serves only to allow the
// creation of multiple default_alloc instances.
// Node that containers built on different allocator instances have
// different types, limiting the utility of this approach.

#if defined(__SUNPRO_CC) || defined(__GNUC__)
	// breaks if we make these template class members:
	enum {_ALIGN = 8};
	enum {_MAX_BYTES = 128};
	enum {_NFREELISTS = 16}; // _MAX_BYTES/_ALIGN
#endif


// SGI STL 第二级配置器，GCC 默认使用第二级配置器，其作用是避免太多小额区块造成内存的碎片
// 无 “template 类型参数”，且第二参数也没有用，其中第一参数用于多线程环境下
template <bool threads, int inst>
class __default_alloc_template {

private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
	#if ! ( defined(__SUNPRO_CC) || defined(__GNUC__) )
	    enum {_ALIGN = 8};  // 小型区块的上调边界
	    enum {_MAX_BYTES = 128}; // 小区区块的上限
	    enum {_NFREELISTS = 16}; // _MAX_BYTES/_ALIGN  free-list 的个数
	#endif 

	// It would be nice to use _STL_auto_lock here.  But we
	// don't need the NULL check.  And we do need a test whether
	// threads have actually been started.
    class _Lock;
    friend class _Lock;
    class _Lock 
	{
	public:
	    _Lock() { __NODE_ALLOCATOR_LOCK; }
	    ~_Lock() { __NODE_ALLOCATOR_UNLOCK; }
    };


__PRIVATE:
	// free-list 的节点结构，降低维护链表 list 带来的额外负担
	union _Obj
	{
	    union _Obj* _M_free_list_link;  // 利用联合体特点
	    char _M_client_data[1];    /* The client sees this.*/
	};
  
private:
	#if defined(__SUNPRO_CC) || defined(__GNUC__) || defined(__HP_aCC)
    	static _Obj* __STL_VOLATILE _S_free_list[];  //头地址，动态内存
        // Specifying a size results in duplicate def for 4.1
	#else
   		static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS];  
		// 维护 16 个空闲链表(free list)，初始化为0，即每个链表中都没有空闲数据块  
	#endif 
	  
	#ifdef __STL_THREADS
		static _STL_mutex_lock _S_node_allocator_lock;
	#endif

	// Chunk allocation state.
	static char* _S_start_free;  // 内存池起始位置。只在 _S_chunk_alloc() 中变化
	
	static char* _S_end_free;    // 内存池结束位置。只在 _S_chunk_alloc() 中变化
	
	static size_t _S_heap_size;
	  
private:

	// 将任何小额区块的内存需求量上调至 8 的倍数
	static size_t _S_round_up(size_t __bytes) 
	{ 
		return ( ( (__bytes) + (size_t) _ALIGN - 1 ) & ~( (size_t) _ALIGN - 1) ); 
	}

	//根据申请数据块大小找到相应空闲链表的下标，n 从 0 起算
	static  size_t _S_freelist_index(size_t bytes) 
	{
		return ( ((bytes) + (size_t)_ALIGN-1) / (size_t)_ALIGN - 1 ); // 96 / 8 - 1 
	}

	// Returns an object of size __n, and optionally adds to size __n free list.
	static void* _S_refill(size_t __n);

	// Allocates a chunk for nobjs of size size.  nobjs may be reduced
	// if it is inconvenient to allocate the requested number.
	static char* _S_chunk_alloc(size_t __size, int& __nobjs);
  

public:
	/* __n must be > 0      */
	// 申请大小为n的数据块，返回该数据块的起始地址 
	static void* allocate(size_t __n)
	{
		void* __ret = 0;

		// 如果需求区块大于 128 bytes，就转调用第一级配置
		if (__n > (size_t) _MAX_BYTES) 
		{
		  __ret = malloc_alloc::allocate(__n);
		}
		else 
		{
			// 根据申请空间的大小寻找相应的空闲链表（16个空闲链表中的一个）
			_Obj* __STL_VOLATILE* __my_free_list =  __default_alloc_template<__threads, __inst>::_S_free_list + _S_freelist_index(__n);
			// Acquire the lock here with a constructor call.
			// This ensures that it is released in exit or during stack
			// unwinding.
			#ifndef _NOTHREADS
			  	/*REFERENCED*/
			  	_Lock __lock_instance;
			#endif

			_Obj* __RESTRICT __result = *__my_free_list; //取头节点

			// 空闲链表没有空闲数据块，就将区块大小先调整至 8 倍数边界，然后调用 _S_refill() 重新填充
			if ( __result == 0  )
			{
				__ret = _S_refill( _S_round_up(__n) );
			}
			else 
			{
				// 如果空闲链表中有空闲数据块，则取出一个，并把空闲链表的指针指向下一个数据块  
				*__my_free_list = __result -> _M_free_list_link;

				__ret = __result;
			}
		}
		
		return __ret;
	};

	/* __p may not be 0 */
	// 空间释放函数 deallocate()
	static void deallocate(void* __p, size_t __n)
	{
		if (__n > (size_t) _MAX_BYTES)  // 大于 128 bytes，就调用第一级配置器的释放
		{
			malloc_alloc::deallocate(__p, __n);   
		}
		else 
		{
			_Obj* __STL_VOLATILE*  __my_free_list = __default_alloc_template<__threads, __inst>::_S_free_list + _S_freelist_index(__n);   // 否则将空间回收到相应空闲链表（由释放块的大小决定）中  
			_Obj* __q = (_Obj*)__p;
	
			// acquire lock
			#ifndef _NOTHREADS

				/*REFERENCED*/
				_Lock __lock_instance;
			
			#endif /* _NOTHREADS */

			__q -> _M_free_list_link = *__my_free_list;   // 调整空闲链表，回收数据块 (在头部插入一个区块)

			*__my_free_list = __q;

			// lock is released here
		}
		
	}

	static void* reallocate(void* __p, size_t __old_sz, size_t __new_sz);

} ;

typedef  __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>  alloc;  // 令 alloc 为第二级配置器
typedef __default_alloc_template<false, 0> single_client_alloc;


//多线程下，锁变量初值设定
#ifdef __STL_THREADS
    template <bool __threads, int __inst>
    _STL_mutex_lock \
    __default_alloc_template<__threads, __inst>::_S_node_allocator_lock \
    __STL_MUTEX_INITIALIZER; 
#endif

// 静态成员变量的定义与初值设定
template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_start_free = 0;

template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_end_free = 0;

template <bool __threads, int __inst>
size_t __default_alloc_template<__threads, __inst>::_S_heap_size = 0;


//初始化 S_free_list 变量
template <bool __threads, int __inst>
typename __default_alloc_template<__threads, __inst>::_Obj* __STL_VOLATILE 
__default_alloc_template<__threads, __inst> ::_S_free_list[ 
	#if defined(__SUNPRO_CC) || defined(__GNUC__) || defined(__HP_aCC) 
   		 _NFREELISTS
	#else
   		 __default_alloc_template<__threads, __inst>::_NFREELISTS
	#endif
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
// The 16 zeros are necessary to make version 4.1 of the SunPro
// compiler happy.  Otherwise it appears to allocate too little
// space for the array.



template <bool __threads, int __inst>
inline bool operator==(const __default_alloc_template<__threads, __inst>&,
                       const __default_alloc_template<__threads, __inst>&)
{
  return true;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
	template <bool __threads, int __inst>
	inline bool operator!=(const __default_alloc_template<__threads, __inst>&,
	                       const __default_alloc_template<__threads, __inst>&)
	{
	  return false;
	}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


/**
* @berif _S_chunk_alloc 内存池，管理动态内存
* @author wq 
* @date 2023/07/17
* @param __size  申请单个空间大小（区块大小）
* @param __nobjs 申请空间个数（多少个区块）
* @return 第一个区块的首地址
*/
/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */
// 从内存池中取空间
template <bool __threads, int __inst>
char* __default_alloc_template<__threads, __inst>::_S_chunk_alloc(size_t __size , int& __nobjs)
{
    char* __result;
	
    size_t __total_bytes = __size * __nobjs;  // 需要申请空间的大小  

	size_t __bytes_left = _S_end_free - _S_start_free;  // 计算内存池剩余空间

    if ( __bytes_left >= __total_bytes )  // 内存池剩余空间完全满足申请
	{  
        __result = _S_start_free;
		
        _S_start_free += __total_bytes;

		return(__result);	
    } 
	else if (__bytes_left >= __size)  // 内存池剩余空间不能满足申请，提供一个以上的区块
	{   
        __nobjs = (int)( __bytes_left / __size ); //最多可以提供区块的个数

		__total_bytes = __size * __nobjs;

		__result = _S_start_free;

		_S_start_free += __total_bytes;

		return(__result);
		
    }
	else  // 内存池剩余空间连一个区块的大小都无法提供 
	{  
    	//申请2倍的大小
        size_t __bytes_to_get = 2 * __total_bytes + __default_alloc_template<__threads, __inst>::S_round_up( __default_alloc_template<__threads, __inst>::_S_heap_size >> 4 );

		// Try to make use of the left-over piece.
		// 内存池的剩余空间分给合适的空闲链表
        if (__bytes_left > 0) 
		{
             _Obj* __STL_VOLATILE* __my_free_list = __default_alloc_template<__threads, __inst>::_S_free_list + __default_alloc_template<__threads, __inst>::_S_freelist_index( __bytes_left );

            ( (_Obj*)_S_start_free ) -> _M_free_list_link = *__my_free_list;
		
            *__my_free_list = (_Obj*)_S_start_free;
        }

		_S_start_free = (char*)malloc(__bytes_to_get);  // 配置 heap 空间，用来补充内存池
        
        if ( 0 == _S_start_free ) // heap 空间不足，malloc() 失败
		{  
            size_t __i;
			
            _Obj* __STL_VOLATILE* __my_free_list;

			_Obj* __p;

			/**
			* 从free_list中，取出比size大的空间
			*/
            // Try to make do with what we have.  That can't
            // hurt.  We do not try smaller requests, since that tends
            // to result in disaster on multi-process machines.
			for (__i = __size ;  __i <= (size_t) _MAX_BYTES ; __i += (size_t) _ALIGN)  
			{
				__my_free_list = __default_alloc_template<__threads, __inst>::_S_free_list + __default_alloc_template<__threads, __inst>::_S_freelist_index(__i);
				
				__p = *__my_free_list;
				
				if ( 0 != __p )  //free_list中存在比申请的内存大的区块，挖出一块交出
				{ 
			    	*__my_free_list = __p -> _M_free_list_link;
					
			    	_S_start_free = (char*)__p;

					_S_end_free = _S_start_free + __i;

					return( _S_chunk_alloc(__size, __nobjs ) );
			   		// Any leftover piece will eventually make it to the
			    	// right free list.
				}
			}
				 
	    	_S_end_free = 0;	// In case of exception.

            _S_start_free = (char*)malloc_alloc::allocate(__bytes_to_get);  //如果内存池找不到空闲空间，调用第一级配置器
            
            // This should either throw an
            // exception or remedy the situation.  Thus we assume it
            // succeeded.
        }
		
        _S_heap_size += __bytes_to_get;
		
        _S_end_free = _S_start_free + __bytes_to_get;

		return ( _S_chunk_alloc( __size, __nobjs ) );  // 递归调用自己
		
    }
}


/**
* @berif _S_refill 填充满fill_list 
* @author wq 
* @date 2023/07/17
* @param __n 单个区块的大小
* @return 第一个区块的首地址
*/
/* Returns an object of size __n, and optionally adds to size __n free list.*/
/* We assume that __n is properly aligned.                                */
/* We hold the allocation lock.                                         */
template <bool __threads, int __inst>
void* __default_alloc_template<__threads, __inst>::_S_refill(size_t __n)
{
    int __nobjs = 20;
    // 调用 _S_chunk_alloc()，缺省取 20 个区块作为 free list 的新节点
    // __chunk 为20个区块的 “首地址”（第一个区块 是0~n 第二个区块 ：n ~ 2n ， 第三个区块：2n ~ 3n依次类推 ））
    char* __chunk = _S_chunk_alloc(__n, __nobjs); 
	
    _Obj* __STL_VOLATILE* __my_free_list;
    _Obj* __result;
    _Obj* __current_obj;
    _Obj* __next_obj;
    int __i;

    // 如果只获得一个数据块，那么这个数据块就直接分给调用者，空闲链表中不会增加新节点
    if (1 == __nobjs) 
		return(__chunk);
	
    __my_free_list = _S_free_list + _S_freelist_index(__n);  // 否则根据申请数据块的大小找到相应空闲链表  

    /* Build free list in chunk */
    __result = (_Obj*)__chunk; //这一块返回给客户端(第一块)

	//以下引导free list 指向新的配置空间（取自内存池）
    *__my_free_list = (_Obj*)(__chunk + __n);  // 第0个数据块给调用者，地址访问即chunk~chunk + n - 1 (记录头指针)
    
    __next_obj = (_Obj*)(__chunk + __n)
	  	
	for (__i = 1 ; ; __i++)  //从1开始，因为第0个返回给客端；
	{
		__current_obj = __next_obj;

		__next_obj = (_Obj*)( (char*)__next_obj + __n); //指向下一个区块

		if (__nobjs - 1 == __i) //链表末尾
		{
		    __current_obj -> _M_free_list_link = 0; 
		    break;
		} 
		else 
		{
		    __current_obj -> _M_free_list_link = __next_obj;  //当前_M_free_list_link指向下一个Obj ,构成一个链表
		}
	}
	  
   	return(__result);
	
}


/**
* @berif reallocate 重新分配空间
* @author wq 
* @date 2023/07/17
* @param __p 需要重新分配空间的首地址
* @param __old_sz 旧空间的大小
* @param __new_sz 新空间的大小
* @return
*/

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::reallocate(void* __p , size_t __old_sz , size_t __new_sz )
{
    void* __result;
    size_t __copy_sz;

	//当旧空间的大小 > 128 且 新空间的大小 > 128 时，调用C语言的realloc重新分配空间
    if (__old_sz > (size_t) _MAX_BYTES && __new_sz > (size_t) _MAX_BYTES)
	{
        return (::realloc(__p , __new_sz) );
    }

	//上调至8的倍数后相等，不需要重新分配
    if ( _S_round_up(__old_sz) == _S_round_up(__new_sz) ) 
		return (__p);
	
    __result = __default_alloc_template<threads, inst>::allocate(__new_sz);
	
    __copy_sz = __new_sz > __old_sz ? __old_sz : __new_sz;
	
    memcpy( __result, __p, __copy_sz );

	__default_alloc_template<threads, inst>::deallocate(__p, __old_sz); 
	
    return ( __result );
	
}


#endif /* !__USE_MALLOC  End Pos */ 




// This implements allocators as specified in the C++ standard.  
//
// Note that standard-conforming allocators use many language features
// that are not yet widely implemented.  In particular, they rely on
// member templates, partial specialization, partial ordering of function
// templates, the typename keyword, and the use of the template keyword
// to refer to a template member of a dependent type.

#ifdef __STL_USE_STD_ALLOCATORS
 
/**********************************************上层的空间配置器类 BEGIN****************************************************/
template <class _Tp>
class allocator
{
  typedef  alloc  _Alloc;          // The underlying allocator.(底层分配器 -- 第一级还是第二级)
public:
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;
  typedef _Tp*       pointer;
  typedef const _Tp* const_pointer;
  typedef _Tp&       reference;
  typedef const _Tp& const_reference;
  typedef _Tp        value_type;

  template <class _Tp1> struct rebind
  {
    typedef allocator<_Tp1> other;
  };

  allocator() __STL_NOTHROW {}
  
  allocator(const allocator&) __STL_NOTHROW {}
  
  template <class _Tp1> allocator(const allocator<_Tp1>&) __STL_NOTHROW {}
  
  ~allocator() __STL_NOTHROW {}

  pointer address( reference __x ) const 
  { 
		return &__x; 
  }
  
  const_pointer address(const_reference __x) const 
  {
		return &__x;
  }


 //空间分配
  // __n is permitted to be 0.  The C++ standard says nothing about what
  // the return value is when __n == 0.
  _Tp* allocate(size_type __n, const void* = 0) {
    return __n != 0 ? static_cast<_Tp*>(_Alloc::allocate(__n * sizeof(_Tp))) 
                    : 0;
  }

  //空间归还
  // __p is not permitted to be a null pointer.
  void deallocate(pointer __p, size_type __n)
    { _Alloc::deallocate(__p, __n * sizeof(_Tp)); }

  size_type max_size() const __STL_NOTHROW 
    { return size_t( -1 ) / sizeof(_Tp); }


  //构造_Tp对象
  void construct(pointer __p, const _Tp& __val) 
  {
  	new(__p) _Tp(__val);  //使用的时__p指向的地址，不会重新分配空间
  }
  
  //释放_Tp对象
  void destroy(pointer __p) 
  { 
  	__p->~_Tp(); 
  }
};


//特化版（void） 
template<>
class allocator<void> {
public:
  typedef size_t      size_type;
  typedef ptrdiff_t   difference_type;
  typedef void*       pointer;
  typedef const void* const_pointer;
  typedef void        value_type;

  template <class _Tp1> struct rebind {
    typedef allocator<_Tp1> other;
  };
};


template <class _T1, class _T2>
inline bool operator==(const allocator<_T1>&, const allocator<_T2>&) 
{
  return true;
}

template <class _T1, class _T2>
inline bool operator!=(const allocator<_T1>&, const allocator<_T2>&)
{
  return false;
}

// Allocator adaptor to turn an SGI-style allocator (e.g. alloc, malloc_alloc)
// into a standard-conforming allocator.   Note that this adaptor does
// *not* assume that all objects of the underlying alloc class are
// identical, nor does it assume that all of the underlying alloc's
// member functions are static member functions.  Note, also, that 
// __allocator<_Tp, alloc> is essentially the same thing as allocator<_Tp>. //__allocator<_Tp, alloc> 本质上与  allocator<_Tp>  一样

template <class _Tp, class _Alloc>
struct __allocator {

  _Alloc __underlying_alloc; //底层空间分配器

  typedef size_t    size_type;
  typedef ptrdiff_t difference_type;
  typedef _Tp*       pointer;
  typedef const _Tp* const_pointer;
  typedef _Tp&       reference;
  typedef const _Tp& const_reference;
  typedef _Tp        value_type;

  template <class _Tp1> struct rebind {
    typedef __allocator<_Tp1, _Alloc> other;
  };

  __allocator() __STL_NOTHROW {}
  __allocator(const __allocator& __a) __STL_NOTHROW
    : __underlying_alloc(__a.__underlying_alloc) {}
	
  template <class _Tp1> 
  __allocator(const __allocator<_Tp1, _Alloc>& __a) __STL_NOTHROW
    : __underlying_alloc(__a.__underlying_alloc) {}

  ~__allocator() __STL_NOTHROW {}

  pointer address(reference __x) const { return &__x; }
  const_pointer address(const_reference __x) const { return &__x; }

  // __n is permitted to be 0.
  _Tp* allocate(size_type __n, const void* = 0)
  {
    return __n != 0 ? static_cast<_Tp*>( __underlying_alloc.allocate( __n * sizeof(_Tp) ) ) : 0;
  }

  // __p is not permitted to be a null pointer.
  void deallocate(pointer __p, size_type __n)
    { __underlying_alloc.deallocate(__p, __n * sizeof(_Tp)); }

  size_type max_size() const __STL_NOTHROW 
    { return size_t(-1) / sizeof(_Tp); }

  void construct(pointer __p, const _Tp& __val)
  {
  	new(__p) _Tp(__val); 
  }
  
  void destroy(pointer __p) 
  { 
  	__p->~_Tp(); 
  }
};



template <class _Alloc>
class __allocator<void, _Alloc> 
{
	typedef size_t      size_type;
	typedef ptrdiff_t   difference_type;
	typedef void*       pointer;
	typedef const void* const_pointer;
	typedef void        value_type;

	template <class _Tp1> 
	struct rebind 
	{
		typedef __allocator<_Tp1, _Alloc> other;
	};
};

template < class _Tp, class _Alloc >
inline bool operator==( const __allocator<_Tp, _Alloc>& __a1 , const __allocator<_Tp, _Alloc>& __a2)
{
	return __a1.__underlying_alloc == __a2.__underlying_alloc;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <class _Tp, class _Alloc>
inline bool operator!=( const __allocator<_Tp, _Alloc>& __a1 , const __allocator<_Tp, _Alloc>& __a2)
{
	return __a1.__underlying_alloc != __a2.__underlying_alloc;
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

// Comparison operators for all of the predifined SGI-style allocators.
// This ensures that __allocator<malloc_alloc> (for example) will
// work correctly.

template <int inst>
inline bool operator==(const __malloc_alloc_template<inst>& , const __malloc_alloc_template<inst>&)
{
	return true;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <int __inst>
inline bool operator!=(const __malloc_alloc_template<__inst>& , const __malloc_alloc_template<__inst>&)
{
 	return false;
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */


template <class _Alloc>
inline bool operator==(const debug_alloc<_Alloc>& , const debug_alloc<_Alloc>&)
{
	return true;
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <class _Alloc>
inline bool operator!=(const debug_alloc<_Alloc>& , const debug_alloc<_Alloc>&) 
{
	return false;
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

/**********************************************上层的空间配置器类 END****************************************************/


// Another allocator adaptor: _Alloc_traits.  This serves two
// purposes.  First, make it possible to write containers that can use
// either SGI-style allocators or standard-conforming allocator.
// Second, provide a mechanism so that containers can query whether or
// not the allocator has distinct instances.  If not, the container
// can avoid wasting a word of memory to store an empty object.

// This adaptor uses partial specialization.  The general case of
// _Alloc_traits<_Tp, _Alloc> assumes that _Alloc is a
// standard-conforming allocator, possibly with non-equal instances
// and non-static members.  (It still behaves correctly even if _Alloc
// has static member and if all instances are equal.  Refinements
// affect performance, not correctness.)

// There are always two members: allocator_type, which is a standard-
// conforming allocator type for allocating objects of type _Tp, and
// _S_instanceless, a static const member of type bool.  If
// _S_instanceless is true, this means that there is no difference
// between any two instances of type allocator_type.  Furthermore, if
// _S_instanceless is true, then _Alloc_traits has one additional
// member: _Alloc_type.  This type encapsulates allocation and
// deallocation of objects of type _Tp through a static interface; it
// has two member functions, whose signatures are
//    static _Tp* allocate(size_t)
//    static void deallocate(_Tp*, size_t)

// The fully general version. 常规的版本

template <class _Tp, class _Allocator>
struct _Alloc_traits
{
	static const bool _S_instanceless = false;
	typedef typename _Allocator::__STL_TEMPLATE rebind<_Tp>::other allocator_type; // __STL_TEMPLATE 等同于 template <class _Tp1> 
};

template <class _Tp, class _Allocator>
const bool _Alloc_traits<_Tp, _Allocator>::_S_instanceless;

// The version for the default allocator. 默认构造器
template <class _Tp, class _Tp1>
struct _Alloc_traits< _Tp , allocator<_Tp1> >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, alloc> _Alloc_type; //alloc ：第一级或第二级配置器
	typedef allocator<_Tp> allocator_type;
};

// Versions for the predefined SGI-style allocators.

template <class _Tp, int __inst>
struct _Alloc_traits<_Tp, __malloc_alloc_template<__inst> >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, __malloc_alloc_template<__inst> > _Alloc_type;
	typedef __allocator<_Tp, __malloc_alloc_template<__inst> > allocator_type;
};

template <class _Tp, bool __threads, int __inst>
struct _Alloc_traits<_Tp, __default_alloc_template< __threads , __inst > >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, __default_alloc_template<__threads, __inst> > _Alloc_type;
	typedef __allocator<_Tp, __default_alloc_template<__threads, __inst> > allocator_type;
};

template <class _Tp, class _Alloc>
struct _Alloc_traits< _Tp, debug_alloc<_Alloc> >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, debug_alloc<_Alloc> > _Alloc_type;
	typedef __allocator<_Tp, debug_alloc<_Alloc> > allocator_type;
};

// Versions for the __allocator adaptor used with the predefined
// SGI-style allocators.

template < class _Tp , class _Tp1 , int __inst >
struct _Alloc_traits<_Tp , __allocator<_Tp1, __malloc_alloc_template<__inst> > >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, __malloc_alloc_template<__inst> > _Alloc_type;
	typedef __allocator<_Tp, __malloc_alloc_template<__inst> > allocator_type;
};

template <class _Tp, class _Tp1, bool __thr, int __inst>
struct _Alloc_traits<_Tp , __allocator<_Tp1 , __default_alloc_template<__thr, __inst> > >
{
	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, __default_alloc_template<__thr,__inst> > _Alloc_type;
	typedef __allocator<_Tp, __default_alloc_template<__thr,__inst> > allocator_type;
};

template <class _Tp, class _Tp1, class _Alloc>
struct _Alloc_traits<_Tp, __allocator<_Tp1, debug_alloc<_Alloc> > >
{

	static const bool _S_instanceless = true;
	typedef simple_alloc<_Tp, debug_alloc<_Alloc> > _Alloc_type;
	typedef __allocator<_Tp, debug_alloc<_Alloc> > allocator_type;
	
};


#endif /* __STL_USE_STD_ALLOCATORS */

#if defined(__sgi) && !defined(__GNUC__) && (_MIPS_SIM != _MIPS_SIM_ABI32)
#pragma reset woff 1174
#endif

__STL_END_NAMESPACE

#undef __PRIVATE

#endif /* __SGI_STL_INTERNAL_ALLOC_H */

// Local Variables:
// mode:C++
// End:
