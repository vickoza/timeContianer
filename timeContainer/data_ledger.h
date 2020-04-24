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
#include <ranges>
#include <concepts>

template<class T>
concept Addable = requires (T a, T b)
{
	{ a + b }->T;
	{ a += b }->T&;
};

template<class T, class U, class V, class  Container>
concept time_contianer = requires (T a, U, V v1, V v2, Container c)
{
	{U::now()}->V;
	v1 > v2;
	//c.push_back(a, v1);
	c.emplace_back(a, v1);
	c.cbegin();
	c.cend();
	c.erase(c.begin(), c.end());
};


template <Addable T, class U = std::chrono::steady_clock, class V = std::chrono::time_point<U>, class Container = std::deque<std::pair<T, V>>> requires time_contianer<T, U, V, Container>
class data_ledger
{
public:
	data_ledger() = delete;
	data_ledger(const data_ledger&) = delete;
	data_ledger(data_ledger&& old) : intialState(std::move(old.intialState)), currentState(std::move(old.currentState)), dataPoints(std::move(old.dataPoints)) {}
	data_ledger(T intSt) : intialState(intSt), currentState(intSt){}
	template <typename... Args>
	data_ledger(Args&&... args) : intialState(std::forward<Args>(args)...), currentState(intialState) {}
	void update(const T& val)
	{
		dataPoints.emplace_back(val, U::now());
		currentState += val;
	}

	const T& getCurrent() const { return currentState;  }
	const T& getIntial() const { return intialState; }

	template<class Contianer1 = Container> //untestible except in live code
	data_ledger<T, U, V, Contianer1> partition(const V& time)
	{
		data_ledger<T, U, V, Contianer1> temp{ intialState };
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = std::find_if(dataPoints.begin(), dataPoints.end(), [&temp, &time, &intialState](const auto& pr)
			{
				if (pr.first > time)
					return true;
				temp.dataPoints.emplace_back(pr.first, pr.second);
				intialState += pr.first;
				temp.currentState += pr.first;

			});
			dataPoints.erase(dataPoints.begin(), iter);
		}
		return temp;
	}

	template<class Contianer1 = Container>
	data_ledger<T, U, V, Contianer1> partition(const long double& percentKeep)
	{
		data_ledger<T, U, V, Contianer1> temp{ intialState };
		auto value = (size_t)(((100.0 - percentKeep) / 100.0) * dataPoints.size());
		auto iter = dataPoints.begin();
		for(auto i = (size_t)0; i <value ; ++i)
		{
			temp.dataPoints.emplace_back(iter->first, iter->second);
			intialState += iter->first;
			temp.currentState += iter->first;
			++iter;
		}
		dataPoints.erase(dataPoints.begin(), iter);
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

template <typename T, typename S, typename U = std::chrono::steady_clock, typename V = std::chrono::time_point<U>, typename Container = std::deque<std::pair<S, V>>>  requires time_contianer<S, U, V, Container>
class state_data_ledger
{
public:
	state_data_ledger() = delete;
	state_data_ledger(const state_data_ledger&) = delete;
	template <typename... Args>
	state_data_ledger(std::function<void(T&, const S&)>& alterFunction, Args&&... args): intialState(std::forward<Args>(args)...), currentState(intialState), fun(alterFunction) {}
	state_data_ledger(state_data_ledger&& old) : intialState(std::move(old.intialState)), currentState(std::move(old.currentState)), dataPoints(std::move(old.dataPoints)), fun(std::move(old.fun)) {}
	state_data_ledger(std::function<void(T&, const S&)> alterFunction, T intSt) : intialState(intSt), currentState(intSt), fun(std::move(alterFunction)){}
	template <typename... Args>
	void update(Args&&... args )
	{
		dataPoints.emplace_back(std::forward<Args>(args)..., U::now());
		fun(currentState, dataPoints.back().first);
	}
	void update(const S& val)
	{
		dataPoints.emplace_back(val, U::now());
		fun(currentState, val);
	}
	const T& getCurrent() const { return currentState; }
	const T& getIntial() const { return intialState; }

	state_data_ledger partition(const V& time)
	{
		state_data_ledger temp{ intialState, fun };
		if (!dataPoints.empty() && time > dataPoints.front())
		{
			auto iter = std::find_if(dataPoints.begin(), dataPoints.end(), [&temp, &time, &intialState](const auto& pr)
			{
				if (pr.first > time)
					return true;
				temp.dataPoints.emplace_back(pr.first, pr.second);
				fun(intialState, pr.first);
				fun(temp.currentState, pr.first);

			});
			dataPoints.erase(dataPoints.begin(), iter);
		}
		return temp;
	}

	state_data_ledger partition(const long double& percentKeep)
	{
		state_data_ledger temp{ fun, intialState };
		auto value = (size_t)(std::ceil(((100.0 - percentKeep) / 100.0) * dataPoints.size()));
		auto iter = dataPoints.begin();
		for (auto i = (size_t)0; i < value; ++i, ++iter)
		{
			temp.dataPoints.emplace_back(iter->first, iter->second);
			fun(intialState, iter->first);
			fun(temp.currentState, iter->first);
		}

		dataPoints.erase(dataPoints.begin(), iter);
		return temp;
	}
	bool validate() const
	{
		auto i2 = std::adjacent_find(dataPoints.begin(), dataPoints.end(), [](const auto& x, const auto& y) { return x.second > y.second; });
		return (currentState == std::accumulate(dataPoints.begin(), dataPoints.end(), intialState, [&](auto a, auto pr) { fun(a, pr.first); return a; })) && i2 == dataPoints.end();
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

