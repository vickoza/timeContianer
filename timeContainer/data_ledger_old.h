#pragma once
#include <chrono> //defualt timespam
#include <algorithm>
#include <numeric>
#include <deque>
#include <complex>
#include <utility>
#include <type_traits>
#include <functional>
#include <iterator>

template<class T> struct is_complex : std::false_type {};
template<class T> struct is_complex<std::complex<T>> : std::true_type {};
namespace detail {
	template<class, class = std::void_t<>>
	struct is_clock_impl : std::false_type {};
	template<class T>
	struct is_clock_impl<T, std::void_t<typename T::rep, typename T::period,
		typename T::duration, typename T::time_point,
		decltype(T::is_steady), decltype(T::now())>>
		: std::true_type {};
}

template<class T>
struct is_clock : detail::is_clock_impl<T> {};

template <typename T, typename U = std::chrono::steady_clock, typename V = std::chrono::time_point<U>, typename Container = std::deque<std::pair<T, V>>>
class data_ledger
{
	static_assert(std::is_arithmetic<T>::value || is_complex<T>::value, "T must be a numeric value");
	//static_assert(is_clock<U>, "U must be a clocktype");
public:
	data_ledger() = delete;
	data_ledger(const data_ledger&) = delete;
	data_ledger(data_ledger&& old) : intialState(std::move(old.intialState)), currentState(std::move(old.currentState)), dataPoints(std::move(old.dataPoints)) {}
	data_ledger(T intSt) : intialState(intSt), currentState(intSt) {}
	template <typename... Args>
	data_ledger(Args&&... args) : intialState(std::forward<Args>(args)...), currentState(intialState) {}
	void update(const T& val)
	{
		dataPoints.emplace_back(val, U::now());
		currentState += val;
	}
	const T& getCurrent() const { return currentState; }
	data_ledger partition(const V& time)
	{
		data_ledger temp{ intialState };
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = std::find_if(dataPoints.begin(), dataPoints.end(), [&temp, &time, &intialState = this->intialState](const auto& pr)
				{
					if (pr.first > time)
						return true;
					temp.dataPoints.emplace_back(pr.first, pr.second);
					intialState += pr.first;
					temp.currentState += pr.first;

				});
			dataPoints.remove(dataPoints.begin(), iter);
		}
		return temp;
	}

	data_ledger partition(const long double& percentKeep)
	{
		data_ledger temp{ intialState };
		auto value = (size_t)(((100.0 - percentKeep) / 100.0) * dataPoints.size() + 1);
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = dataPoints.begin();
			for (auto i = (size_t)0; i < value; ++i, ++iter)
			{
				temp.dataPoints.emplace_back(iter->first, iter->second);
				intialState += iter->first;
				temp.currentState += iter->first;

			}
			dataPoints.remove(dataPoints.begin(), iter);
		}
		return temp;
	}
	bool validate() const
	{
		auto i2 = std::adjacent_find(dataPoints.begin(), dataPoints.end(), [](const auto& x, const auto& y) { return x.second > y.second; });
		return (currentState == std::accumulate(dataPoints.begin(), dataPoints.end(), intialState, [](auto a, auto pr) { return a + pr.first; })) && i2 == dataPoints.end();
	}
	auto begin() { return dataPoints.cbegin(); }
	auto cbegin() { return dataPoints.cbegin(); }
	auto end() { return dataPoints.cend(); }
	auto cend() { return dataPoints.cend(); }
private:
	T intialState;
	T currentState;
	Container dataPoints;
};

template <typename T, typename S, typename U = std::chrono::steady_clock, typename V = std::chrono::time_point<U>, typename Container = std::deque<std::pair<S, V>>>
class state_data_ledger
{
	//static_assert(is_clock<U>, "U must be a clocktype");
public:
	state_data_ledger() = delete;
	state_data_ledger(const state_data_ledger&) = delete;
	template <typename... Args>
	state_data_ledger(std::function<void(T&, const S&)>& alterFunction, Args&&... args) : intialState(std::forward<Args>(args)...), currentState(intialState), fun(std::move(alterFunction)) {}
	state_data_ledger(state_data_ledger&& old) : intialState(std::move(old.intialState)), currentState(std::move(old.currentState)), dataPoints(std::move(old.dataPoints)), fun(std::move(old.fun)) {}
	state_data_ledger(T intSt, std::function<void(T&, const S&)>& alterFunction) : intialState(intSt), currentState(intSt), fun(std::move(alterFunction)) {}
	template <typename... Args>
	void update(Args&&... args)
	{
		dataPoints.emplace_back(std::forward<Args>(args)..., U::now());
		fun(currentState, dataPoints.first);
	}
	void update(const S& val)
	{
		dataPoints.emplace_back(val, U::now());
		fun(currentState, val);
	}
	const T& getCurrent() const { return currentState; }
	state_data_ledger partition(const V& time)
	{
		state_data_ledger temp{ intialState, fun };
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = std::find_if(dataPoints.begin(), dataPoints.end(), [&temp, &time, &intialState = this->intialState, &fun = this->fun](const auto& pr)
				{
					if (pr.first > time)
						return true;
					temp.dataPoints.emplace_back(pr.first, pr.second);
					fun(intialState, pr.first);
					fun(temp.currentState, pr.first);

				});
			dataPoints.remove(dataPoints.begin(), iter);
		}
		return temp;
	}

	state_data_ledger partition(const long double& percentKeep)
	{
		state_data_ledger temp{ intialState, fun };
		auto value = (size_t)(((100.0 - percentKeep) / 100.0) * dataPoints.size() + 1);
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = dataPoints.begin();
			for (auto i = (size_t)0; i < value; ++i, ++iter)
			{
				temp.dataPoints.emplace_back(iter->first, iter->second);
				fun(intialState, iter->first);
				fun(temp.currentState, iter->first);
			}
			dataPoints.remove(dataPoints.begin(), iter);
		}
		return temp;
	}
	bool validate() const
	{
		auto i2 = std::adjacent_find(dataPoints.begin(), dataPoints.end(), [](const auto& x, const auto& y) { return x.second > y.second; });
		return (currentState == std::accumulate(dataPoints.begin(), dataPoints.end(), intialState, [&fun = this->fun](auto a, auto pr) { fun(a, pr.first); return a; })) && i2 == dataPoints.end();
	}
	auto begin() { return dataPoints.cbegin(); }
	auto cbegin() { return dataPoints.cbegin(); }
	auto end() { return dataPoints.cend(); }
	auto cend() { return dataPoints.cend(); }

private:
	T intialState;
	T currentState;
	Container dataPoints;
	std::function<void(T&, const S&)> fun;
};

