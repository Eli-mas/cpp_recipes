#include "Fraction.h"
#include "RollingWindow.h"

#include <iostream>
using std::cout, std::endl;

#include <functional>
#include <array>
#include <vector>
using std::vector;

template<typename T>
std::ostream& operator << (std::ostream&, const Fraction<T>&);

typedef long long longlong;
#define DECLARE_FOR_INT_TYPES(MACRO) \
	MACRO(short) MACRO(int) MACRO(long) MACRO(longlong)

template<class T>
void test_GCD(T a, T b, T expected){
	T r = fracmath::gcd(a,b);
	if (r != expected){
		cout << "gcd(" << a << ", " << b << "): expected "
		     << expected << ", got" << r << endl;
	}
	
	if (a==b) return;
	
	// gcd is symmetric
	r = fracmath::gcd(b, a);
	if (r != expected){
		cout << "gcd(" << b << ", " << a << "): expected "
		     << expected << ", got" << r << endl;
	}
}


template<class T>
T product(const vector<T>& v){
	if(!v.size()) return 0;
	auto it {v.begin()};
	T p {*it};
	for(++it; it<v.end(); ++it){
		p *= *it;
	}
	return p;
}

void test_GCDs(){
	vector<int> primes = {1, 2, 3, 5, 7, 11, 13, 17, 19, 23};
	
	vector<int>::iterator it, it2;
	
	for(it = primes.begin(); it != primes.end(); it++){
		// gcd(value, value) = value
		test_GCD(*it, *it, *it);
		// gcd(prime1, prime2) = 1 , with prime1!=prime2
		for(it2 = it+1; it2 != primes.end(); it2++){
			test_GCD(*it, *it2, 1);
		}
	}
}

int main(){
	
	test_GCDs();
	
	
	Fraction<int> f {1,2};
	Fraction<long> g {1L,2L}; // {1,2} results in linker error
	Fraction<int> h {1,3};
	
	// the 3.L and 3.f casts are required (can't use double 3.)
	// otherwise, due to floating-point limitation,
	// they are not considered equal
	assert(double(h) == 1/3. && (long double)h==1/3.L && float(h)==1/3.f);
	assert(short(f) == 0 && int(f) == 0 && long(f) == 0 && (long long)f == 0);
	
	cout << "g: " << g << " = " << static_cast<double>(g) << endl;
	cout << "f: " << f << " = " << static_cast<double>(f) << endl;
	cout << "h: " << h << " = " << static_cast<double>(h) << endl;
	cout << "f==g: " << (f==g) << endl;
	cout << "f==h: " << (f==h) << endl;
	
	f = {2,1};
	assert(int(f)==2 && short(f)==2 && long(f)==2 && (long long)f==2);
	assert(float(f)==2 && double(f)==2 && (long double)f == 2);
	assert(std::to_string(f) == "2/1");
	
	
	g = {-2L,1L};
	cout << "g: " << std::to_string(g) << endl;
	
	// tests for operating Fractions with Fractions
		// test negative in numerator, denominator
		assert(Fraction(1, -1) == Fraction(-1, 1));
		assert(Fraction(-1, -1) == Fraction(1, 1));
		
		// test 0 equality
		assert(Fraction(0, 1) == Fraction(0, 2));
		assert(Fraction(0,-1) == Fraction(0, 1));
		
		// test addition
		assert(Fraction(1, -1) + Fraction(1, 1) == Fraction(0, 1));
		assert(Fraction(1, 2) + Fraction(1, 4) == Fraction(3, 4));
		
		// test subtraction
		assert(Fraction(1, 2) - Fraction(1, 4) == Fraction(  1,  4));
		assert(Fraction(1, 4) - Fraction(1, 2) == Fraction( -1,  4));
		assert(Fraction(1, 4) - Fraction(1, 2) == Fraction(  1, -4));
		assert(Fraction(1, 3) - Fraction(5, 4) == Fraction(-11,  12));
		
		// test multiplication: reduction
		assert(Fraction(2, 3) * Fraction(3, 2) == Fraction(1,1));
		// test multiplication: negation
		assert(Fraction(2, 3) * Fraction(2, -3) == Fraction(-4, 9));
		assert(Fraction(2, 3) * Fraction(-2, 3) == Fraction(-4, 9));
		assert(Fraction(2, -3) * Fraction(2, 3) == Fraction(-4, 9));
		assert(Fraction(-2, 3) * Fraction(2, 3) == Fraction(-4, 9));
		
		Fraction<int> z {0,0} /*nan*/, ip {1,0} /*+inf*/, in {-1,0} /*-inf*/;
		cout << "g = " << g << ", f = " << f << endl;
		auto A = std::array<Fraction<int>,3>({z,ip,in});
		for(auto frac:A){
			cout << "frac: " << frac << "; +g = " << frac+g
				 << "; +f = " << frac+f << "; +z = " << frac+z
				 << "; +ip = " << frac+ip << "; +in = " << frac+in
				 << endl;
		}
	
	
	int LIM (50);
	#define TEST_OP_INT_FRAC(MACRO, i, n1, d1) \
		MACRO(short, i, n1, d1); \
		MACRO(int, i, n1, d1); \
		MACRO(long, i, n1, d1); \
		MACRO(longlong, i, n1, d1);
	
	#define TEST_OVER_LIM(MACRO) \
		for(int i=-LIM; i<=LIM; i++){ \
			for(int j=-LIM; j<=LIM; j++){ \
				for(int k=-LIM; k<=LIM; k++){ \
					TEST_OP_INT_FRAC(MACRO,i,j,k) \
				} \
			} \
		}
	
	#define TEST_ADD_INT_FRAC(type, i, n1, d1) \
		if(i + Fraction<type>((type)n1,(type)d1) != Fraction<type>(type(i*d1+n1),(type)d1)) {\
			printf("(%d, %d, %d): ",i,n1,d1);\
			cout << i << " + " << Fraction<type>((type)n1,(type)d1) << \
			" != " << Fraction<type>(type(n1+i*d1),(type)d1) << endl; \
			assert(0 == 1);\
		}
	
	#define TEST_SUB_INT_FRAC(type, i, n1, d1) \
		if(i - Fraction<type>((type)n1,(type)d1) != Fraction<type>(type(i*d1-n1),(type)d1)) {\
			printf("(%d, %d, %d): ",i,n1,d1);\
			cout << i << " - " << Fraction<type>((type)n1,(type)d1) << \
			" != " << Fraction<type>(type(i*d1-n1),(type)d1) << endl; \
			assert(0 == 1);\
		}
	
	#define TEST_MUL_INT_FRAC(type, i, n1, d1) \
		if(i * Fraction<type>((type)n1,(type)d1) != Fraction<type>(type(i*n1),(type)d1)) {\
			printf("(%d, %d, %d): ",i,n1,d1);\
			cout << i << " * " << Fraction<type>((type)n1,(type)d1) << \
			" != " << Fraction<type>(type(i*n1),(type)d1) << endl; \
			assert(0 == 1);\
		}
	
	#define TEST_DIV_INT_FRAC(type, i, n1, d1) \
		if(i / Fraction<type>((type)n1,(type)d1) != Fraction<type>(type(i*d1),(type)n1)) {\
			printf("(%d, %d, %d): ",i,n1,d1);\
			cout << i << " / " << Fraction<type>((type)n1,(type)d1) << \
			" != " << Fraction<type>(type(i*d1),(type)n1) << endl; \
			assert(0 == 1);\
		}
	
	TEST_OVER_LIM(TEST_ADD_INT_FRAC);
	TEST_OVER_LIM(TEST_SUB_INT_FRAC);
	TEST_OVER_LIM(TEST_MUL_INT_FRAC);
	TEST_OVER_LIM(TEST_DIV_INT_FRAC);
}