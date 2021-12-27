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


//Process for at least N < 2*CurrentKernel-1
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK < MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
   return i; //error, didn't work, kernel too small
}
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK == MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
	if(CurK <= N)
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK,MinK>(i,N,
			std::forward<Args>(args)...);
		i+=CurK;
	}
	return i;
}
template<size_t CurK,
	size_t MinK=1,
	class KernelStruct,
	class ...Args>
std::enable_if_t<(CurK > MinK),size_t>
_for_n_end(size_t i,size_t N,KernelStruct&& ks,Args&&... args)
{
	if(N >= CurK)
	{
		std::forward<KernelStruct>(ks).
			template kernel<CurK,MinK>(i,N,
			std::forward<Args>(args)...);
		i+=CurK;
		N-=CurK;
	}
	i=_for_n_end<CurK/2,MinK>(i,N,ks,args...);
	return i;
}

struct KernelTest
{
	template<size_t Cur,size_t Min>
	void kernel(size_t i,size_t N)
	{
		std::cout << "Chunk:" << Cur << ",i:" << i << ",N:" << N << std::endl;
	}
};
int main()
{
	_for_n_end<128>(0,129,KernelTest{});

	return 0;
}

