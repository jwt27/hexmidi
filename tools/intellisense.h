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
#undef __cpp_noexcept_function_type
#undef __cpp_variadic_using
#undef __cpp_capture_star_this
#undef __cpp_if_constexpr
#undef __cpp_aligned_new
#undef __cpp_hex_float
#undef __cpp_nested_namespace_definitions
#undef __cpp_structured_bindings
#undef __cpp_range_based_for
#undef __cpp_nontype_template_args
#undef __cpp_inline_variables
#undef __cpp_namespace_attributes
#undef __cpp_fold_expressions
#undef __cpp_constexpr
#undef __cpp_template_template_args
#undef __cpp_threadsafe_static_init
#undef __cpp_aggregate_bases
#undef __EXCEPTIONS
#undef __CHAR32_TYPE__

#include "gcc_defines.h"

#undef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned int

#define __float128 long double
#define __null 0

#endif
