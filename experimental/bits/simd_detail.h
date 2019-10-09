// Internal macros for the simd implementation -*- C++ -*-

// Copyright © 2015-2019 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                       Matthias Kretz <m.kretz@gsi.de>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the names of contributing organizations nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _GLIBCXX_EXPERIMENTAL_SIMD_DETAIL_H_
#define _GLIBCXX_EXPERIMENTAL_SIMD_DETAIL_H_

#if __cplusplus >= 201703L

#include <cstddef>
#include <cstdint>

#define Vir_SIMD_VERSION_STRING "2.0.0"

#define _GLIBCXX_SIMD_BEGIN_NAMESPACE                                                    \
    namespace std _GLIBCXX_VISIBILITY(default)                                           \
    {                                                                                    \
    _GLIBCXX_BEGIN_NAMESPACE_VERSION namespace experimental                              \
    {                                                                                    \
    inline namespace parallelism_v2                                                      \
    {
#define _GLIBCXX_SIMD_END_NAMESPACE                                                      \
    }                                                                                    \
    }                                                                                    \
    _GLIBCXX_END_NAMESPACE_VERSION                                                       \
    }

_GLIBCXX_SIMD_BEGIN_NAMESPACE
// __next_power_of_2{{{
/**
 * \internal
 * Returns the next power of 2 larger than or equal to \p __x.
 */
constexpr std::size_t __next_power_of_2(std::size_t __x)
{
  return (__x & (__x - 1)) == 0 ? __x
				: __next_power_of_2((__x | (__x >> 1)) + 1);
}

// }}}
namespace simd_abi  // {{{
{
// implementation details:
struct _Scalar;
template <int _N> struct _Fixed;

template <int _UsedBytes>
struct _VecBuiltin;

template <typename _Tp, int _N>
using _VecN = _VecBuiltin<sizeof(_Tp) * _N>;

template <int _UsedBytes = 16>
using _Sse = _VecBuiltin<_UsedBytes>;
template <int _UsedBytes = 32>
using _Avx = _VecBuiltin<_UsedBytes>;
template <int _UsedBytes = 64>
struct _Avx512;
template <int _UsedBytes = 16>
using _Neon = _VecBuiltin<_UsedBytes>;

template <int _N, typename _Abi> struct _Combine;

// implementation-defined:
template <int _NRegisters> using __sse_x = _Combine<_NRegisters, _Sse<>>;
template <int _NRegisters> using __avx_x = _Combine<_NRegisters, _Avx<>>;
template <int _NRegisters> using __avx512_x = _Combine<_NRegisters, _Avx512<>>;
template <int _NRegisters> using __neon_x = _Combine<_NRegisters, _Neon<>>;

template <typename _Tp, int _N> using __sse_n = _Sse<sizeof(_Tp) * _N>;
template <typename _Tp, int _N> using __avx_n = _Avx<sizeof(_Tp) * _N>;
template <typename _Tp, int _N> using __avx512_n = _Avx512<sizeof(_Tp) * _N>;
template <typename _Tp, int _N> using __neon_n = _Neon<sizeof(_Tp) * _N>;

using __sse = _Sse<>;
using __avx = _Avx<>;
using __avx512 = _Avx512<>;
using __neon = _Neon<>;

using __neon128 = _Neon<16>;
using __neon64 = _Neon<8>;

// standard:
template <typename _Tp, size_t _N, typename... > struct deduce;
template <int _N> using fixed_size = _Fixed<_N>;
using scalar = _Scalar;
}  // namespace simd_abi }}}
// forward declarations is_simd(_mask), simd(_mask), simd_size {{{
template <typename _Tp> struct is_simd;
template <typename _Tp> struct is_simd_mask;
template <typename _Tp, typename _Abi> class simd;
template <typename _Tp, typename _Abi> class simd_mask;
template <typename _Tp, typename _Abi> struct simd_size;
// }}}

// ISA extension detection. The following defines all the _GLIBCXX_SIMD_HAVE_XXX macros
// ARM{{{
#if defined __ARM_NEON
#define _GLIBCXX_SIMD_HAVE_NEON 1
#else
#define _GLIBCXX_SIMD_HAVE_NEON 0
#endif
#if defined __ARM_NEON && (__ARM_ARCH >= 8 || defined __aarch64__)
#define _GLIBCXX_SIMD_HAVE_NEON_A32 1
#else
#define _GLIBCXX_SIMD_HAVE_NEON_A32 0
#endif
#if defined __ARM_NEON && defined __aarch64__
#define _GLIBCXX_SIMD_HAVE_NEON_A64 1
#else
#define _GLIBCXX_SIMD_HAVE_NEON_A64 0
#endif
//}}}
// x86{{{
#ifdef __MMX__
#define _GLIBCXX_SIMD_HAVE_MMX 1
#else
#define _GLIBCXX_SIMD_HAVE_MMX 0
#endif
#if defined __SSE__ || defined __x86_64__
#define _GLIBCXX_SIMD_HAVE_SSE 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE 0
#endif
#if defined __SSE2__ || defined __x86_64__
#define _GLIBCXX_SIMD_HAVE_SSE2 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE2 0
#endif
#ifdef __SSE3__
#define _GLIBCXX_SIMD_HAVE_SSE3 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE3 0
#endif
#ifdef __SSSE3__
#define _GLIBCXX_SIMD_HAVE_SSSE3 1
#else
#define _GLIBCXX_SIMD_HAVE_SSSE3 0
#endif
#ifdef __SSE4_1__
#define _GLIBCXX_SIMD_HAVE_SSE4_1 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE4_1 0
#endif
#ifdef __SSE4_2__
#define _GLIBCXX_SIMD_HAVE_SSE4_2 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE4_2 0
#endif
#ifdef __XOP__
#define _GLIBCXX_SIMD_HAVE_XOP 1
#else
#define _GLIBCXX_SIMD_HAVE_XOP 0
#endif
#ifdef __AVX__
#define _GLIBCXX_SIMD_HAVE_AVX 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX 0
#endif
#ifdef __AVX2__
#define _GLIBCXX_SIMD_HAVE_AVX2 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX2 0
#endif
#ifdef __BMI__
#define _GLIBCXX_SIMD_HAVE_BMI1 1
#else
#define _GLIBCXX_SIMD_HAVE_BMI1 0
#endif
#ifdef __BMI2__
#define _GLIBCXX_SIMD_HAVE_BMI2 1
#else
#define _GLIBCXX_SIMD_HAVE_BMI2 0
#endif
#ifdef __LZCNT__
#define _GLIBCXX_SIMD_HAVE_LZCNT 1
#else
#define _GLIBCXX_SIMD_HAVE_LZCNT 0
#endif
#ifdef __SSE4A__
#define _GLIBCXX_SIMD_HAVE_SSE4A 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE4A 0
#endif
#ifdef __FMA__
#define _GLIBCXX_SIMD_HAVE_FMA 1
#else
#define _GLIBCXX_SIMD_HAVE_FMA 0
#endif
#ifdef __FMA4__
#define _GLIBCXX_SIMD_HAVE_FMA4 1
#else
#define _GLIBCXX_SIMD_HAVE_FMA4 0
#endif
#ifdef __F16C__
#define _GLIBCXX_SIMD_HAVE_F16C 1
#else
#define _GLIBCXX_SIMD_HAVE_F16C 0
#endif
#ifdef __POPCNT__
#define _GLIBCXX_SIMD_HAVE_POPCNT 1
#else
#define _GLIBCXX_SIMD_HAVE_POPCNT 0
#endif
#ifdef __AVX512F__
#define _GLIBCXX_SIMD_HAVE_AVX512F 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX512F 0
#endif
#ifdef __AVX512DQ__
#define _GLIBCXX_SIMD_HAVE_AVX512DQ 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX512DQ 0
#endif
#ifdef __AVX512VL__
#define _GLIBCXX_SIMD_HAVE_AVX512VL 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX512VL 0
#endif
#ifdef __AVX512BW__
#define _GLIBCXX_SIMD_HAVE_AVX512BW 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX512BW 0
#endif

#if _GLIBCXX_SIMD_HAVE_SSE
#define _GLIBCXX_SIMD_HAVE_SSE_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_SSE_ABI 0
#endif
#if _GLIBCXX_SIMD_HAVE_SSE2
#define _GLIBCXX_SIMD_HAVE_FULL_SSE_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_FULL_SSE_ABI 0
#endif

#if _GLIBCXX_SIMD_HAVE_AVX
#define _GLIBCXX_SIMD_HAVE_AVX_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX_ABI 0
#endif
#if _GLIBCXX_SIMD_HAVE_AVX2
#define _GLIBCXX_SIMD_HAVE_FULL_AVX_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_FULL_AVX_ABI 0
#endif

#if _GLIBCXX_SIMD_HAVE_AVX512F
#define _GLIBCXX_SIMD_HAVE_AVX512_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_AVX512_ABI 0
#endif
#if _GLIBCXX_SIMD_HAVE_AVX512BW
#define _GLIBCXX_SIMD_HAVE_FULL_AVX512_ABI 1
#else
#define _GLIBCXX_SIMD_HAVE_FULL_AVX512_ABI 0
#endif

#if defined __x86_64__ && !_GLIBCXX_SIMD_HAVE_SSE2
#error "Use of SSE2 is required on AMD64"
#endif
//}}}

#define _GLIBCXX_SIMD_NORMAL_MATH [[__gnu__::__optimize__("finite-math-only,no-signed-zeros")]]
#define _GLIBCXX_SIMD_NEVER_INLINE [[__gnu__::__noinline__]]
#define _GLIBCXX_SIMD_INTRINSIC [[__gnu__::__always_inline__, __gnu__::__artificial__]] inline
#define _GLIBCXX_SIMD_CONST __attribute__((__const__))
#define _GLIBCXX_SIMD_PURE __attribute__((__pure__))
#define _GLIBCXX_SIMD_ALWAYS_INLINE [[__gnu__::__always_inline__]] inline
#define _GLIBCXX_SIMD_IS_UNLIKELY(__x) __builtin_expect(__x, 0)
#define _GLIBCXX_SIMD_IS_LIKELY(__x) __builtin_expect(__x, 1)

#define _GLIBCXX_SIMD_LIST_BINARY(macro) macro(|) macro(&) macro(^)
#define _GLIBCXX_SIMD_LIST_SHIFTS(macro) macro(<<) macro(>>)
#define _GLIBCXX_SIMD_LIST_ARITHMETICS(macro) macro(+) macro(-) macro(*) macro(/) macro(%)

#define _GLIBCXX_SIMD_ALL_BINARY(macro) _GLIBCXX_SIMD_LIST_BINARY(macro) static_assert(true)
#define _GLIBCXX_SIMD_ALL_SHIFTS(macro) _GLIBCXX_SIMD_LIST_SHIFTS(macro) static_assert(true)
#define _GLIBCXX_SIMD_ALL_ARITHMETICS(macro) _GLIBCXX_SIMD_LIST_ARITHMETICS(macro) static_assert(true)

#ifdef _GLIBCXX_SIMD_NO_ALWAYS_INLINE
#undef _GLIBCXX_SIMD_ALWAYS_INLINE
#define _GLIBCXX_SIMD_ALWAYS_INLINE inline
#undef _GLIBCXX_SIMD_INTRINSIC
#define _GLIBCXX_SIMD_INTRINSIC inline
#endif

#if _GLIBCXX_SIMD_HAVE_SSE || _GLIBCXX_SIMD_HAVE_MMX
#define _GLIBCXX_SIMD_X86INTRIN 1
#else
#define _GLIBCXX_SIMD_X86INTRIN 0
#endif

// workaround macros {{{
// use aliasing loads to help GCC understand the data accesses better
// This also seems to hide a miscompilation on swap(x[i], x[i + 1]) with
// fixed_size_simd<float, 16> x.
#define _GLIBCXX_SIMD_USE_ALIASING_LOADS 1

// vector conversions on x86 not optimized:
#if _GLIBCXX_SIMD_X86INTRIN
#define _GLIBCXX_SIMD_WORKAROUND_PR85048 1
#endif

// zero extension from xmm to zmm not optimized:
#if __GNUC__ < 9
#define _GLIBCXX_SIMD_WORKAROUND_PR85480 1
#endif

// incorrect use of k0 register for _kortestc_mask64_u8 and _kortestc_mask32_u8:
#define _GLIBCXX_SIMD_WORKAROUND_PR85538 1

// Invalid instruction mov from xmm16-31
#define _GLIBCXX_SIMD_WORKAROUND_PR89229 1

// integer division not optimized
#define _GLIBCXX_SIMD_WORKAROUND_PR90993 1

// very bad codegen for extraction and concatenation of 128/256 "subregisters" with
// sizeof(element type) < 8: https://godbolt.org/g/mqUsgM
#if _GLIBCXX_SIMD_X86INTRIN
#define _GLIBCXX_SIMD_WORKAROUND_XXX_1 1
#endif

// bad codegen for 8 Byte memcpy to __vector_type_t<char, 16>
#define _GLIBCXX_SIMD_WORKAROUND_PR90424 1

// bad codegen for zero-extend using simple concat(__x, 0)
#if _GLIBCXX_SIMD_X86INTRIN
#define _GLIBCXX_SIMD_WORKAROUND_XXX_3 1
#endif

// bad codegen for integer division
#define _GLIBCXX_SIMD_WORKAROUND_XXX_4 1

// abs pattern may generate MMX instructions without EMMS cleanup (This only happens with SSSE3
// because pabs[bwd] is part of SSSE3.)
#if __GNUC__ < 10 && defined __SSSE3__ && _GLIBCXX_SIMD_X86INTRIN
#define _GLIBCXX_SIMD_WORKAROUND_PR91533 1
#endif

#if __GNUC__ < 10 && defined __aarch64__
#define _GLIBCXX_SIMD_WORKAROUND_XXX_5 1
#endif

// https://github.com/cplusplus/parallelism-ts/issues/65 (incorrect return type of
// static_simd_cast)
#define _GLIBCXX_SIMD_FIX_P2TS_ISSUE65 1

// https://github.com/cplusplus/parallelism-ts/issues/66 (incorrect SFINAE constraint on
// (static)_simd_cast)
#define _GLIBCXX_SIMD_FIX_P2TS_ISSUE66 1
// }}}
_GLIBCXX_SIMD_END_NAMESPACE

#endif  // __cplusplus >= 201703L
#endif  // _GLIBCXX_EXPERIMENTAL_SIMD_DETAIL_H_
// vim: foldmethod=marker
