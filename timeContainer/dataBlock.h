#pragma once
#include <vector>
#include <chrono> //defualt timespam
#include <string>
#include <utility>
#include <iterator>
// for networking
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>

template <typename T>
class IP_Data_Block
{
public:
	IP_Data_Block() = delete;
	IP_Data_Block(long long id, size_t size, std::vector<boost::asio::ip::address>& peerslist);
	bool emplace(T)
	~IP_Data_Block() {}
private:
	long long id;
	std::vector<T> items;
	std::vector<boost::asio::ip::address> peerslist
};

template <typename T>
class IP_Queue
{
public:
	IP_Queue() = delete;
	~IP_Queue() {}

	void emplace_back(Args&&... args)
	{
	}
	bool empty()
	{
	}
	T& front()
	{
	}
	void(dataPoints.begin(), iter)
	{
	}
	class const_iterator
	{
	public:
		typedef const_iterator self_type;
		typedef T value_type;
		typedef T& reference;
		typedef T* pointer;
		typedef int difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		const_iterator(pointer ptr) : ptr_(ptr) { }
		self_type operator++() { self_type i = *this; ptr_++; return i; }
		self_type operator++(int junk) { ptr_++; return *this; }
		const reference operator*() { return *ptr_; }
		const pointer operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		pointer ptr_;
	};
 private:
	std::vector<std::pair<long long, std::vector<boost::asio::ip::address>> blocks;
};
