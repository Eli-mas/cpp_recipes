/*
Iterator that provides a rolling window over a vector.

Example usage:
	std::size_t n = 4;
	std::vector<T> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	
	auto r = RollingWindow<T, n>(v);
	r.data(); // a list with elements {1, 2, 3, 4}
	while( r.can_increment() ){
		++r; // or r++
		std::cout << r << std::endl;
		// iteratively: {2, 3, 4, 5}, {3, 4, 5, 6}, ... {6, 7, 8, 9}
	}
	while( r.can_decrement() ){
		--r; // or r--
		std::cout << r << std::endl;
		// iteratively: {5, 6, 7, 8}, {4, 5, 6, 7}, ..., {1, 2, 3, 4}
	}

To do:
	generalize to other container types
*/
#include <vector>
#include <list>
#include <functional>
#include <iostream>

template<class T, std::size_t N>
class RollingWindow;

template<class T, std::size_t N>
std::ostream& operator<<(std::ostream&, const RollingWindow<T, N>&);

template<class T, std::size_t N>
class RollingWindow{
	private:
		const std::vector<T>& source;
		std::list<T> cycler;
		std::size_t current_end, current_start;
		void _init(){
			std::size_t n = source.size() < N ? source.size() : N;
			auto it = source.begin();
			for(current_end = 0; current_end<n; current_end++, ++it){
				cycler.push_back(*it);
			}
			current_start = 0;
		}
	public:
		bool can_increment() const {return current_end < source.size();}
		bool can_decrement() const {return current_start > 0;}
		
		RollingWindow(const std::vector<T>& v) : 
			source(v), cycler(std::list<T>())
			{_init();}
		RollingWindow(std::vector<T>&& v) :
			source(std::move(v)), cycler(std::list<T>())
			{_init();}
		
		const std::list<T>& data() const {return cycler;}
		
	private:
		void increment(){
			if(!can_increment()){
				throw std::out_of_range("Cannot be further incremented");
			}
			cycler.pop_front();
			cycler.push_back(source[current_end++]);
			++current_start;
		};
		void decrement(){
			if(!can_decrement()){
				throw std::out_of_range("Cannot be further decremented");
			}
			cycler.pop_back();
			cycler.push_front(source[--current_start]);
			--current_end;
		};
		
		template<class U>
		inline U aggregate(std::function<U(const std::list<T>&)> func) const {
			return func(cycler);
		}
		
	public:
		RollingWindow& operator++(){increment(); return *this;}
		RollingWindow operator++(int){
			RollingWindow t = *this;
			increment();
			return t;
		}
		RollingWindow& operator--(){decrement(); return *this;}
		RollingWindow operator--(int){
			RollingWindow t = *this;
			decrement();
			return t;
		}
		
		friend std::ostream& operator<< <> (std::ostream&, const RollingWindow&);
};

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const RollingWindow<T, N>& r){
	os << "<% ";
	for(auto it = r.data().begin(); it!=r.data().end(); ++it){
		os << *it << ' ';
	}
	os << " >%";
	return os;
}