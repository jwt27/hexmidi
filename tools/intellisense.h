#pragma once

#ifdef __INTELLISENSE__

#if (__cplusplus < 201703L)
#pragma warning "Intellisense does not support C++17"
#endif

#if (__cplusplus < 201402L)
#pragma error "Intellisense is not parsing in C++14 mode"
#endif

#undef __cplusplus
#undef __cpp_attributes
#undef __cpp_aggregate_nsdmi
#undef __cpp_variable_templates
#undef __cpp_delegating_constructors
#undef __cpp_raw_strings
#undef __cpp_ref_qualifiers
#undef __cpp_initializer_lists
#undef __cpp_lambdas
#undef __cpp_decltype_auto
#undef __cpp_sized_deallocation
#undef __cpp_unicode_literals
#undef __cpp_return_type_deduction
#undef __cpp_decltype
#undef __cpp_alias_templates
#undef __cpp_generic_lambdas
#undef __cpp_user_defined_literals
#undef __cpp_init_captures
#undef __cpp_exceptions
#undef __cpp_enumerator_attributes
#undef __cpp_binary_literals
#undef __cpp_variadic_templates
#undef __STDC_HOSTED__
#undef __cpp_rvalue_references
#undef __cpp_nsdmi
#undef __cpp_rtti
#undef __EXCEPTIONS
#undef __CHAR32_TYPE__

#include "gcc_defines.h"

#undef __SIZE_TYPE__
#define __SIZE_TYPE__ size_t
using size_t = size_t; // yes...

#define __float128 long double
#define __null 0

#endif
