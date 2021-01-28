#include <iostream>
#include <string>
// #include <utility>


typedef long long longlong;
#define DECLARE_FOR_INT_TYPES(MACRO) \
	MACRO(short) MACRO(int) MACRO(long) MACRO(longlong)







// template<class T>
// Fraction<T> operator+(const int&, const Fraction<T>&);






namespace fracmath{
	template<typename T>
	inline void make_abs(T& v){if(v < 0) v = -v;}
	
	template<typename T>
	// greatest common divisor
	T gcd(T n, T d){
		make_abs(n); // negative inputs are allowed, but
		make_abs(d); // algorithm requires non-negatives
		
		T temp;
		
		while(d){
			temp = d;
			d = std::move(n) % std::move(d);
			n = std::move(temp);
		}
		return n;
	}
	
	template<typename T>
	// least common multiple
	T lcm(T a, T b){
		if(!(a && b)) return 0;
		
		T r (a * (b / gcd(a,b)));
		make_abs(r);
		return r;
	}
	
	template<typename T>
	void gcd_r(T& a, T& b){
		if(!(a||b)) return;
		T g = gcd(a, b);
		a /= g;
		b /= g;
	}
	
	template<typename T>
	// pretty neat trick: https://stackoverflow.com/questions/1903954
	T sgn(T n){return (T(0) < n) - (n < T(0));}
}

/* The procedure for specifying a friend operator
of a templated class is somewhat elaborate if we do not
define the function inside the class definition

First the function must be declared BEFORE the class,
which in turn requires before that a forward declaration
of the class.
*/
template<typename T>
class Fraction; // forward-declare templated Fraction

template<typename T> // forward-declare templated friend operator
std::ostream& operator<<(std::ostream&, const Fraction<T>&);

namespace std{
	//function overload within std namespace
	template<typename T>
	string to_string(const Fraction<T>&);
}

#define OP_INT_LEFT_DECLARE(IntType, op) \
	template<class T> Fraction<T> operator op(const IntType&, const Fraction<T>&);
// addition
#define OP_ADD_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, +)
#define OP_SUB_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, -)
#define OP_MUL_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, *)
#define OP_DIV_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, /)

DECLARE_FOR_INT_TYPES(OP_ADD_LEFT_DECLARE)
DECLARE_FOR_INT_TYPES(OP_SUB_LEFT_DECLARE)
DECLARE_FOR_INT_TYPES(OP_MUL_LEFT_DECLARE)
DECLARE_FOR_INT_TYPES(OP_DIV_LEFT_DECLARE)

#undef OP_ADD_LEFT_DECLARE
#undef OP_SUB_LEFT_DECLARE
#undef OP_MUL_LEFT_DECLARE
#undef OP_DIV_LEFT_DECLARE

#undef OP_INT_LEFT_DECLARE




template<typename T>
class Fraction{
	/* Fraction type intended for application to native integral types,
	BUT it may be used with other types as well. The only requirement
	is that the type support arithmetic operations and can be initialized
	with single, integral values.*/
	private:
		T num, denom;
			// numerator and denominator
		
		Fraction(T, T, bool);
			/* Optimized private constructor for the case
			where numerator and denominator do not have to
			be reduced via gcd. Used internally.*/
		
		void _init(T&, T&);
			/* Internal initializer function. Receives values by reference
			because those values will have been received by value via a public
			constructor, so no need to recopy.*/
		
		Fraction _add(const T&, const T&) const;
			// handles the logic of addition
		
		// for addition with integer, use three-arg constructor, since
		// fraction added with integer results in a fraction in reduced form
		template <typename I>
		Fraction _addInt(I& i) const{
			return Fraction(static_cast<T>(i*denom)+num, denom, true);
		}
		template <typename I>
		Fraction _addInt(I&& i) const{
			return Fraction(static_cast<T>(i*denom)+num, denom, true);
		}
		
		// for * or / operations can't use three-arg constructor, as it is
		// possible that the integer is not co-prime with denominator/numerator,
		template <typename I>
		Fraction _mulInt(I& i) const{
			return Fraction(static_cast<T>(i*num), denom);
		}
		template <typename I>
		Fraction _mulInt(I&& i) const{
			return Fraction(static_cast<T>(i*num), denom);
		}
		
		template <typename I>
		Fraction _divInt(I& i) const{
			return Fraction(num, static_cast<T>(i*denom));
		}
		template <typename I>
		Fraction _divInt(I&& i) const{
			return Fraction(num, static_cast<T>(i*denom));
		}
	public:
		using value_type = T;
		
		Fraction() : num(0), denom(1) {}
		Fraction(T n) : num(n), denom(1) {}
		Fraction(T n, T d) { _init(n, d); }
		
		// public read-only member access
		inline T getnum() const {return num;}
		inline T getdenom() const {return denom;}
		
		// typecasts are defined for native numeric types
		#define FRAC_CONV(type) explicit operator type() const \
			{return static_cast<type>(num)/static_cast<type>(denom);}
		
		FRAC_CONV(int) FRAC_CONV(short) FRAC_CONV(long) FRAC_CONV(long long)
		FRAC_CONV(float) FRAC_CONV(double) FRAC_CONV(long double)
		
		#undef FRAC_CONV
		
		// copy constructor across types
		template<typename U>
		Fraction(const Fraction<U>& f): num(f.getnum()), denom(f.getdenom()) {}
		// move constructor across types
		template<typename U>
		Fraction(Fraction<U>&& f) :
		    num{std::move(f.num)},
		    denom{std::move(static_cast<U>(f.denom))} {}
		
		// copy assignment across types
		template<typename U>
		Fraction& operator=(const Fraction<U>& f){
			this->num = static_cast<U>(f.num);
			this->denom = static_cast<U>(f.denom);
			return *this;
		}
		// move assignment across types
		template<typename U>
		Fraction& operator=(Fraction&& f){
			this->num = std::move(static_cast<U>(f.num));
			this->denom = std::move(static_cast<U>(f.denom));
		}
		
		// simple member comparison, Fraction is in reduced form
		bool operator==(const Fraction& f) const {
			return this->num==f.num && this->denom==f.denom;
		}
		
		bool operator!=(const Fraction& f) const {
			return (this->num!=f.num) || (this->denom!=f.denom);
		}
		
		
		
		/* Declaring external friend operators
		
		Two possible forms:
			friend ... <> (...)
			friend ... <T> (...)
		
		If this carat pair is missing, a linker error results.
		*/
		friend std::ostream& operator<< <> (std::ostream&, const Fraction&);
		friend std::string std::to_string <> (const Fraction&);
		
		// operations with left-side integral types
		// using macros, the following code declares functions
		// to implement i (op) f for some integer i and fraction f
		// for ops (+, -, *, /) x integer types (short, int, long, long long)
		
		#define OP_INT_LEFT_DECLARE(IntType, op) \
			friend Fraction operator op <> (const IntType&, const Fraction&);
		// addition
		#define OP_ADD_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, +)
		#define OP_SUB_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, -)
		#define OP_MUL_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, *)
		#define OP_DIV_LEFT_DECLARE(IntType) OP_INT_LEFT_DECLARE(IntType, /)
		
		DECLARE_FOR_INT_TYPES(OP_ADD_LEFT_DECLARE)
		DECLARE_FOR_INT_TYPES(OP_SUB_LEFT_DECLARE)
		DECLARE_FOR_INT_TYPES(OP_MUL_LEFT_DECLARE)
		DECLARE_FOR_INT_TYPES(OP_DIV_LEFT_DECLARE)
		
		#undef OP_ADD_LEFT_DECLARE
		#undef OP_SUB_LEFT_DECLARE
		#undef OP_MUL_LEFT_DECLARE
		#undef OP_DIV_LEFT_DECLARE
		
		#undef OP_INT_LEFT_DECLARE
		/*An issue to be resolved
		
		This works -- BUT if the above operators are instead declared
		AFTER the declarations involving DEF_ADD_INT and like macros below,
		the program will not compile; why?
		*/
		
		
		
		// flip numerator, denominator; already in reduced form,
		// so use the private constructor
		Fraction reciprocal() const {return Fraction(denom, num, true);}
		
		// operations with other fractions
		Fraction operator+(const Fraction& f) const{
			return _add(f.num, f.denom);
		}
		Fraction operator-(const Fraction& f) const{
			return _add(-f.num, f.denom);
		}
		Fraction operator*(const Fraction& f) const{
			return Fraction(num*f.num, denom*f.denom);
		}
		Fraction operator/(const Fraction& f) const{
			return Fraction(num*f.denom, denom*f.num);
		}
		
		// operations with right-side integral types
		// addition
		#define DEF_ADD_INT(itype) \
			Fraction operator+(const itype& i) const{return _addInt(i);}
			
			DECLARE_FOR_INT_TYPES(DEF_ADD_INT)
			#undef DEF_ADD_INT
		// subtraction
		#define DEF_SUB_INT(itype) \
			Fraction operator-(const itype& i) const{return _addInt(-i);}
			
			DECLARE_FOR_INT_TYPES(DEF_SUB_INT)
			#undef DEF_SUB_INT
		// multiplication
		#define DEF_MUL_INT(itype) \
			Fraction operator*(const itype& i) const{return _mulInt(i);}
		
			DECLARE_FOR_INT_TYPES(DEF_MUL_INT)
			#undef DEF_MUL_INT
		// division
		#define DEF_DIV_INT(itype) \
			Fraction operator/(const itype& i) const{return _divInt(i);}
		
			DECLARE_FOR_INT_TYPES(DEF_DIV_INT)
			#undef DEF_DIV_INT
};

#define ADD_INT_LEFT_IMPLEMENT(IntType) \
	template<typename T> Fraction<T> \
	operator+(const IntType& i, const Fraction<T>& f) \
	{return f + i;}
	
	DECLARE_FOR_INT_TYPES(ADD_INT_LEFT_IMPLEMENT)
	#undef ADD_INT_LEFT_IMPLEMENT

#define SUB_INT_LEFT_IMPLEMENT(IntType) \
	template<typename T> Fraction<T> \
	operator-(const IntType& i, const Fraction<T>& f) \
	{return (f-i) * -1;}
	
	DECLARE_FOR_INT_TYPES(SUB_INT_LEFT_IMPLEMENT)
	#undef SUB_INT_LEFT_IMPLEMENT

#define MUL_INT_LEFT_IMPLEMENT(IntType) \
	template<typename T> Fraction<T> \
	operator*(const IntType& i, const Fraction<T>& f) \
	{return f * i;}
	
	DECLARE_FOR_INT_TYPES(MUL_INT_LEFT_IMPLEMENT)
	#undef MUL_INT_LEFT_IMPLEMENT

#define DIV_INT_LEFT_IMPLEMENT(IntType) \
	template<typename T> Fraction<T> \
	operator/(const IntType& i, const Fraction<T>& f) \
	{return f.reciprocal() * i;}
	
	DECLARE_FOR_INT_TYPES(DIV_INT_LEFT_IMPLEMENT)
	#undef DIV_INT_LEFT_IMPLEMENT



template<typename T>
Fraction<T>::Fraction(T n, T d, bool bypass){
	if(bypass){
		if(d < 0) num = -n, denom = -d;
		else num = n, denom = d;
	}
	else _init(n, d);
}

template<typename T>
inline Fraction<T> Fraction<T>::_add(const T& fn, const T& fd) const{
	// fn, fd = numerator, denominator of other fraction
	using namespace fracmath;
	
	T new_denom = lcm(denom, fd);
	if (new_denom==0){
		if(num && fn){
			if(fd) return *this;
			return Fraction(sgn(sgn(num) + sgn(fn)), 0);
		}
		return {0,0};
	}
	
	return Fraction(
		num * (new_denom/denom) + fn * (new_denom/fd),
		new_denom
	);
}

template<typename T>
inline void Fraction<T>::_init(T& n, T& d){
	// denominator is always positive, numerator stores sign
	if(d<0){n=-n; d=-d;}
	fracmath::gcd_r(n, d);
	num = n;
	denom = d;
}

namespace std{
	template<typename T>
	string to_string(const Fraction<T>& f){
		return to_string(f.num)+'/'+to_string(f.denom);
	}
}

template<typename T>
std::ostream& operator << (std::ostream& os, const Fraction<T>& f){
	return os << '<' << f.num << '/' << f.denom << '>';
}
// template<typename SType, typename FType>
// SType operator+(SType value, const Fraction<FType>& f){
// 	return value+static_cast<double>(f);
// }


#undef DECLARE_FOR_INT_TYPES
