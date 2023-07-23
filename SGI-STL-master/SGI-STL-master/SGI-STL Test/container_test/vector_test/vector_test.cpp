#include <iostream>
#include <vector>


/**
 * vector 的操作
 */
template <typename T>
void printVector(const std::vector<T>& vec)
{
	for (auto x : vec) {
		std::cout << x << " ";
	}

	std::cout << std::endl;
}

int main()
{
	// 构造函数:四种形式
	std::vector<std::string> iv(2, "hi");
	printVector(iv);

	std::vector<std::string> sv {"why", "always", "me"} ; //列表初始化
	printVector(sv);

	std::vector<std::string> sv2(sv.begin(), sv.end());
	printVector(sv2);

	std::vector<std::string> sv3(sv); //拷贝构造
	printVector(sv3);

	// // operator= 赋值运算符
	std::vector<std::string> sv4;
	sv4 = sv;
	printVector(sv4);

	// // assign 
	std::vector<char> cv;
	cv.assign(5 , 'x');
	printVector(cv);
	
	std::vector<char> cv1;
	cv1.assign(cv.begin(), cv.end());
	printVector(cv1);

	// at(索引)
	std::cout << cv1.at(1) << std::endl;

	// operator[] 
	cv1[1] = 'a';
	printVector(cv1);

	// // front 容器首元素
	std::cout << sv.front() << std::endl;
	
	// // back 容器最后一个元素
	std::cout << sv.back() << std::endl;

	// begin 返回指向容器第一个元素的迭代器
	// end 返回指向容器尾端的迭代器
	for (std::vector<std::string>::iterator it = sv.begin(); it != sv.end(); it++) {
		std::cout << *it << " ";	
	}
	std::cout << std::endl;

	// rbegin 返回一个指向容器最后一个元素的反向迭代器 
	// rend 返回一个指向容器前端的反向迭代器
	for (std::vector<std::string>::reverse_iterator it = sv.rbegin(); it != sv.rend(); it++) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	// empty 若容器为空则为 true ，否则为 false
	if (sv.empty()) {
		std::cout << "container is null." << std::endl;
	} else {
		std::cout << "container is not null." << std::endl;
	}

	// size	容器中的元素个数
	std::cout << sv.size() << std::endl;

	// max_size 元素数量的最大值
	std::cout << sv.max_size() << std::endl;

	// capacity 当前分配存储的容量
	std::cout << sv.capacity() << std::endl;

	// resize 改变容器中可存储元素的个数 
	sv.push_back("abc");
	std::cout << sv.capacity() << std::endl;
	
	// // shrink_to_fit 请求移除未使用的容量
	sv.shrink_to_fit();
	std::cout << sv.capacity() << std::endl;

  	// clear 从容器移除所有元素
	iv.clear();
	printVector(iv);

	// insert:三种形式
	auto it = sv.begin();
	it = sv.insert(it, "YES");
	printVector(sv);
	
	sv.insert(it, 2, "NO");
	printVector(sv);

	it = sv.begin();
	std::vector<std::string> sv5(2, "xx");
	sv.insert(it+2, sv5.begin(), sv5.end());
	printVector(sv);

	// erase 从容器移除指定的元素
	sv.erase(sv.begin());
	printVector(sv);
	
	sv.erase(sv.begin() + 2, sv.begin() + 4);
	printVector(sv);
	
	// push_back 向容器尾部插入元素
	std::cout << sv.size() << std::endl;
	sv.push_back("add");
	printVector(sv);

	// pop_back 移除容器的最末元素
	sv.pop_back();
	printVector(sv);
	
	// swap
	sv.swap(sv5);
	printVector(sv);
	printVector(sv5);

	return 0;
}

