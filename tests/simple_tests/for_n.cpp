#include<array>
#include<vector>
#include<cstdint>
#include<cstdlib>
#include<type_traits>

#include<experimental/simd>
namespace stdx = std::experimental;

#include<array>
#include<vector>
#include<cstdint>
#include<cstdlib>
#include<type_traits>


#include<iostream>
#include<numeric>


//Process for at least N < 2*CurrentK-1
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
static inline
std::enable_if_t<((CurK/2) < MinK),size_t>
_for_n_end(size_t i,size_t e,KernelStruct&& ks,Args&&... args)
{
   return i; //error, didn't work, kernel too small
}

template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
static inline
std::enable_if_t<((CurK/2) >= MinK),size_t>
_for_n_end(size_t i,size_t e,KernelStruct&& ks,Args&&... args)
{
	e &= (CurK-1);
	if(e >= (CurK/2))
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK/2>(i,
			std::forward<Args>(args)...);
		i+=CurK/2;
	}
	i=_for_n_end<CurK/2>(i,e,ks,args...);
	return i;
}

template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
static inline
std::enable_if_t<((CurK/2) < MinK),size_t>
_for_n_begin(size_t i,KernelStruct&& ks,Args&&... args)
{
	return i;
}

template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
static inline
std::enable_if_t<((CurK/2) >= MinK),size_t>
_for_n_begin(size_t i,KernelStruct&& ks,Args&&... args)
{
	i=_for_n_begin<CurK/2>(i,ks,args...);
	if(i & (CurK/2))
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK/2>(i,
			std::forward<Args>(args)...);
		i+=CurK/2;
	}
	return i;
}


//select a max size based on max value that N straddles.
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK > MinK),size_t>
static inline
_for_n(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
	if((i % MinK) || (N % MinK)) return 0;
	if(CurK <= N)
		return _for_n_internal<CurK,MinK>(i,N,ks,args...);
	else
		return _for_n<CurK/2,MinK>(i,N,ks,args...);
}



struct KernelTest
{
	template<size_t Cur>
	void kernel(size_t i)
	{
		std::cout << "Chunk:" << i << "-" << i+Cur << "(" << Cur << ")" << std::endl;
	}
};
int main()
{
	_for_n_begin<64>(3,KernelTest{});

	return 0;
}

