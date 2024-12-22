#pragma once

#include <cstddef>
#include <type_traits>
#include <tuple>

namespace ext {
namespace detail {
template <typename Res, typename... Args>
struct function_traits_base {
    using result_type = Res;
    using args_type = std::tuple<Args...>;
    using arity = std::tuple_size<args_type>;
};

template<typename Class>
struct member_traits {
    using class_type = std::decay_t<Class>;
    using is_const = std::is_const<Class>;
    using is_volatile = std::is_volatile<Class>;
    using is_rvalue_reference = std::is_rvalue_reference<Class>;
    using is_lvalue_reference = std::is_lvalue_reference<Class>;
};

template <typename Signature>
struct function_traits {
    using is_invocable = std::false_type;
};

#define FUNCTION_TRAITS_HELPER_HELPER(CV, REF, NOEXCEPT) \
template <typename Res, typename... Args> \
struct function_traits<Res(Args...) CV REF NOEXCEPT> \
: function_traits_base<Res, Args...> { \
    using is_invocable = std::true_type; \
    using vararg = std::false_type; \
    using is_nothrow_invocable = std::is_nothrow_invocable<Res (*)(Args...) CV REF NOEXCEPT, Args...>; \
}; \
template <typename Res, typename... Args> \
struct function_traits<Res(Args......) CV REF NOEXCEPT> \
: function_traits_base<Res, Args...> { \
    using is_invocable = std::true_type; \
    using vararg = std::true_type; \
    using is_nothrow_invocable = std::is_nothrow_invocable<Res (*)(Args......) CV REF NOEXCEPT, Args...>; \
};

#define FUNCTION_TRAITS_HELPER(REF, NOEXCEPT, NOEXCEPT_IGNORED) \
FUNCTION_TRAITS_HELPER_HELPER( , REF, NOEXCEPT) \
FUNCTION_TRAITS_HELPER_HELPER(const, REF, NOEXCEPT) \
FUNCTION_TRAITS_HELPER_HELPER(volatile, REF, NOEXCEPT) \
FUNCTION_TRAITS_HELPER_HELPER(const volatile, REF, NOEXCEPT)

#define FUNCTION_TRAITS(REF, RVAL_IGNORED, LVAL_IGNORED) \
FUNCTION_TRAITS_HELPER(REF,  , std::false_type) \
FUNCTION_TRAITS_HELPER(REF, noexcept, std::true_type)

FUNCTION_TRAITS(  , std::false_type, std::false_type)
FUNCTION_TRAITS(&, std::false_type, std::true_type)
FUNCTION_TRAITS(&&, std::true_type, std::false_type)

#undef FUNCTION_TRAITS_HELPER_HELPER
#undef FUNCTION_TRAITS_HELPER
#undef FUNCTION_TRAITS

template<typename Signature>
struct member_function_traits {
    using is_invocable = std::false_type;
};

#define MEMBER_FUNCTION_TRAITS_HELPER_HELPER(CV, REF, NOEXCEPT) \
template<typename Res, typename Class, typename... Args> \
struct member_function_traits<Res (Class::*)(Args...) CV REF NOEXCEPT> \
: member_traits<CV Class REF> \
, function_traits<Res(Args...) CV REF NOEXCEPT> {}; \
template<typename Res, typename Class, typename... Args> \
struct member_function_traits<Res (Class::*)(Args......) CV REF NOEXCEPT> \
: member_traits<CV Class REF> \
, function_traits<Res(Args......) CV REF NOEXCEPT> {};

#define MEMBER_FUNCTION_TRAITS_HELPER(REF, NOEXCEPT, NOEXCEPT_IGNORED) \
MEMBER_FUNCTION_TRAITS_HELPER_HELPER(  , REF, NOEXCEPT) \
MEMBER_FUNCTION_TRAITS_HELPER_HELPER(const, REF, NOEXCEPT) \
MEMBER_FUNCTION_TRAITS_HELPER_HELPER(volatile, REF, NOEXCEPT) \
MEMBER_FUNCTION_TRAITS_HELPER_HELPER(const volatile, REF, NOEXCEPT)

#define MEMBER_FUNCTION_TRAITS(REF, RVAL_IGNORED, LVAL_IGNORED) \
MEMBER_FUNCTION_TRAITS_HELPER(REF,  , std::false_type) \
MEMBER_FUNCTION_TRAITS_HELPER(REF, noexcept, std::true_type)

MEMBER_FUNCTION_TRAITS(  , std::false_type, std::false_type)
MEMBER_FUNCTION_TRAITS(&, std::false_type, std::true_type)
MEMBER_FUNCTION_TRAITS(&&, std::true_type, std::false_type)

#undef MEMBER_FUNCTION_TRAITS_HELPER_HELPER
#undef MEMBER_FUNCTION_TRAITS_HELPER
#undef MEMBER_FUNCTION_TRAITS


template <typename Signature>
struct weak_function_object_traits {
    using is_invocable = std::false_type;
};

template <typename Signature>
struct weak_function_object_traits : member_function_traits<decltype(&Signature::operator())> {};

enum class weak_invocable {
    FUNCTION,
    MEMBER_FUNCTION,
    FUNCTION_OBJECT,
};

template <typename Signature, typename Weak_invocable_type>
struct weak_invocable_traits;

template <typename Signature>
struct weak_invocable_traits<Signature, std::integral_constant<weak_invocable, weak_invocable::FUNCTION>> : function_traits<Signature> {};

template <typename Signature>
struct weak_invocable_traits<Signature, std::integral_constant<weak_invocable, weak_invocable::MEMBER_FUNCTION>> : member_function_traits<Signature> {};

template <typename Signature>
struct weak_invocable_traits<Signature, std::integral_constant<weak_invocable, weak_invocable::FUNCTION_OBJECT>> : weak_function_object_traits<Signature> {};


template <typename Signature, typename = decltype(&Signature::operator())>
std::integral_constant<weak_invocable, weak_invocable::FUNCTION_OBJECT> get_weak_invocable_type(int);

template <typename Signature>
std::conditional_t<std::is_member_function_pointer_v<Signature>, std::integral_constant<weak_invocable, weak_invocable::MEMBER_FUNCTION>, std::integral_constant<weak_invocable, weak_invocable::FUNCTION>> get_weak_invocable_type(...);
}

template <typename Signature>
struct weak_invocable_traits : detail::weak_invocable_traits<Signature, decltype(detail::get_weak_invocable_type<Signature>(0))> {};
}


  /// If we have found a result_type, extract it.
  template<typename _Functor, typename = __void_t<>>
    struct _Maybe_get_result_type
    { };

  template<typename _Functor>
    struct _Maybe_get_result_type<_Functor,
				  __void_t<typename _Functor::result_type>>
    { typedef typename _Functor::result_type result_type; };

  /**
   *  Base class for any function object that has a weak result type, as
   *  defined in 20.8.2 [func.require] of C++11.
  */
  template<typename _Functor>
    struct _Weak_result_type_impl
    : _Maybe_get_result_type<_Functor>
    { };

  /// Retrieve the result type for a function type.
  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) _GLIBCXX_NOEXCEPT_QUAL>
    { typedef _Res result_type; };

  /// Retrieve the result type for a varargs function type.
  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) _GLIBCXX_NOEXCEPT_QUAL>
    { typedef _Res result_type; };

  /// Retrieve the result type for a function pointer.
  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct _Weak_result_type_impl<_Res(*)(_ArgTypes...) _GLIBCXX_NOEXCEPT_QUAL>
    { typedef _Res result_type; };

  /// Retrieve the result type for a varargs function pointer.
  template<typename _Res, typename... _ArgTypes _GLIBCXX_NOEXCEPT_PARM>
    struct
    _Weak_result_type_impl<_Res(*)(_ArgTypes......) _GLIBCXX_NOEXCEPT_QUAL>
    { typedef _Res result_type; };

  // Let _Weak_result_type_impl perform the real work.
  template<typename _Functor,
	   bool = is_member_function_pointer<_Functor>::value>
    struct _Weak_result_type_memfun
    : _Weak_result_type_impl<_Functor>
    { };

  // A pointer to member function has a weak result type.
  template<typename _MemFunPtr>
    struct _Weak_result_type_memfun<_MemFunPtr, true>
    {
      using result_type = typename _Mem_fn_traits<_MemFunPtr>::__result_type;
    };

  // A pointer to data member doesn't have a weak result type.
  template<typename _Func, typename _Class>
    struct _Weak_result_type_memfun<_Func _Class::*, false>
    { };

  /**
   *  Strip top-level cv-qualifiers from the function object and let
   *  _Weak_result_type_memfun perform the real work.
  */
  template<typename _Functor>
    struct _Weak_result_type
    : _Weak_result_type_memfun<typename remove_cv<_Functor>::type>
    { };

#if __cplusplus <= 201703L
  // Detect nested argument_type.
  template<typename _Tp, typename = __void_t<>>
    struct _Refwrap_base_arg1
    { };

  // Nested argument_type.
  template<typename _Tp>
    struct _Refwrap_base_arg1<_Tp,
			      __void_t<typename _Tp::argument_type>>
    {
      typedef typename _Tp::argument_type argument_type;
    };

  // Detect nested first_argument_type and second_argument_type.
  template<typename _Tp, typename = __void_t<>>
    struct _Refwrap_base_arg2
    { };

  // Nested first_argument_type and second_argument_type.
  template<typename _Tp>
    struct _Refwrap_base_arg2<_Tp,
			      __void_t<typename _Tp::first_argument_type,
				       typename _Tp::second_argument_type>>
    {
      typedef typename _Tp::first_argument_type first_argument_type;
      typedef typename _Tp::second_argument_type second_argument_type;
    };

  /**
   *  Derives from unary_function or binary_function when it
   *  can. Specializations handle all of the easy cases. The primary
   *  template determines what to do with a class type, which may
   *  derive from both unary_function and binary_function.
  */
  template<typename _Tp>
    struct _Reference_wrapper_base
    : _Weak_result_type<_Tp>, _Refwrap_base_arg1<_Tp>, _Refwrap_base_arg2<_Tp>
    { };

// Ignore warnings about unary_function and binary_function.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

  // - a function type (unary)
  template<typename _Res, typename _T1 _GLIBCXX_NOEXCEPT_PARM>
    struct _Reference_wrapper_base<_Res(_T1) _GLIBCXX_NOEXCEPT_QUAL>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) volatile>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const volatile>
    : unary_function<_T1, _Res>
    { };

  // - a function type (binary)
  template<typename _Res, typename _T1, typename _T2 _GLIBCXX_NOEXCEPT_PARM>
    struct _Reference_wrapper_base<_Res(_T1, _T2) _GLIBCXX_NOEXCEPT_QUAL>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  // - a function pointer type (unary)
  template<typename _Res, typename _T1 _GLIBCXX_NOEXCEPT_PARM>
    struct _Reference_wrapper_base<_Res(*)(_T1) _GLIBCXX_NOEXCEPT_QUAL>
    : unary_function<_T1, _Res>
    { };

  // - a function pointer type (binary)
  template<typename _Res, typename _T1, typename _T2 _GLIBCXX_NOEXCEPT_PARM>
    struct _Reference_wrapper_base<_Res(*)(_T1, _T2) _GLIBCXX_NOEXCEPT_QUAL>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Tp, bool = is_member_function_pointer<_Tp>::value>
    struct _Reference_wrapper_base_memfun
    : _Reference_wrapper_base<_Tp>
    { };

  template<typename _MemFunPtr>
    struct _Reference_wrapper_base_memfun<_MemFunPtr, true>
    : _Mem_fn_traits<_MemFunPtr>::__maybe_type
    {
      using result_type = typename _Mem_fn_traits<_MemFunPtr>::__result_type;
    };
#pragma GCC diagnostic pop
#endif // ! C++20

  /// @endcond

  /**
   *  @brief Primary class template for reference_wrapper.
   *  @ingroup functors
   */
  template<typename _Tp>
    class reference_wrapper
#if __cplusplus <= 201703L
    // In C++20 std::reference_wrapper<T> allows T to be incomplete,
    // so checking for nested types could result in ODR violations.
    : public _Reference_wrapper_base_memfun<typename remove_cv<_Tp>::type>
#endif
    {
      _Tp* _M_data;

      _GLIBCXX20_CONSTEXPR
      static _Tp* _S_fun(_Tp& __r) noexcept { return std::__addressof(__r); }

      static void _S_fun(_Tp&&) = delete;

      template<typename _Up, typename _Up2 = __remove_cvref_t<_Up>>
	using __not_same
	  = typename enable_if<!is_same<reference_wrapper, _Up2>::value>::type;

    public:
      typedef _Tp type;

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 2993. reference_wrapper<T> conversion from T&&
      // 3041. Unnecessary decay in reference_wrapper
      template<typename _Up, typename = __not_same<_Up>, typename
		= decltype(reference_wrapper::_S_fun(std::declval<_Up>()))>
	_GLIBCXX20_CONSTEXPR
	reference_wrapper(_Up&& __uref)
	noexcept(noexcept(reference_wrapper::_S_fun(std::declval<_Up>())))
	: _M_data(reference_wrapper::_S_fun(std::forward<_Up>(__uref)))
	{ }

      reference_wrapper(const reference_wrapper&) = default;

      reference_wrapper&
      operator=(const reference_wrapper&) = default;

      _GLIBCXX20_CONSTEXPR
      operator _Tp&() const noexcept
      { return this->get(); }

      _GLIBCXX20_CONSTEXPR
      _Tp&
      get() const noexcept
      { return *_M_data; }

      template<typename... _Args>
	_GLIBCXX20_CONSTEXPR
	typename __invoke_result<_Tp&, _Args...>::type
	operator()(_Args&&... __args) const
	noexcept(__is_nothrow_invocable<_Tp&, _Args...>::value)
	{
#if __cplusplus > 201703L
	  if constexpr (is_object_v<type>)
	    static_assert(sizeof(type), "type must be complete");
#endif
	  return std::__invoke(get(), std::forward<_Args>(__args)...);
	}

#if __glibcxx_reference_wrapper >= 202403L // >= C++26
      // [refwrap.comparisons], comparisons
      [[nodiscard]]
      friend constexpr bool
      operator==(reference_wrapper __x, reference_wrapper __y)
      requires requires { { __x.get() == __y.get() } -> convertible_to<bool>; }
      { return __x.get() == __y.get(); }

      [[nodiscard]]
      friend constexpr bool
      operator==(reference_wrapper __x, const _Tp& __y)
      requires requires { { __x.get() == __y } -> convertible_to<bool>; }
      { return __x.get() == __y; }

      [[nodiscard]]
      friend constexpr bool
      operator==(reference_wrapper __x, reference_wrapper<const _Tp> __y)
      requires (!is_const_v<_Tp>)
	&& requires { { __x.get() == __y.get() } -> convertible_to<bool>; }
      { return __x.get() == __y.get(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 4071. reference_wrapper comparisons are not SFINAE-friendly

      [[nodiscard]]
      friend constexpr auto
      operator<=>(reference_wrapper __x, reference_wrapper __y)
      requires requires (const _Tp __t) {
	{ __t < __t } -> __detail::__boolean_testable;
      }
      { return __detail::__synth3way(__x.get(), __y.get()); }

      [[nodiscard]]
      friend constexpr auto
      operator<=>(reference_wrapper __x, const _Tp& __y)
      requires requires { { __y < __y } -> __detail::__boolean_testable; }
      { return __detail::__synth3way(__x.get(), __y); }

      [[nodiscard]]
      friend constexpr auto
      operator<=>(reference_wrapper __x, reference_wrapper<const _Tp> __y)
      requires (!is_const_v<_Tp>) && requires (const _Tp __t) {
	{ __t < __t } -> __detail::__boolean_testable;
      }
      { return __detail::__synth3way(__x.get(), __y.get()); }
#endif
    };

#if __cpp_deduction_guides
  template<typename _Tp>
    reference_wrapper(_Tp&) -> reference_wrapper<_Tp>;
#endif

  /// @relates reference_wrapper @{

  /// Denotes a reference should be taken to a variable.
  template<typename _Tp>
    _GLIBCXX20_CONSTEXPR
    inline reference_wrapper<_Tp>
    ref(_Tp& __t) noexcept
    { return reference_wrapper<_Tp>(__t); }

  /// Denotes a const reference should be taken to a variable.
  template<typename _Tp>
    _GLIBCXX20_CONSTEXPR
    inline reference_wrapper<const _Tp>
    cref(const _Tp& __t) noexcept
    { return reference_wrapper<const _Tp>(__t); }

  template<typename _Tp>
    void ref(const _Tp&&) = delete;

  template<typename _Tp>
    void cref(const _Tp&&) = delete;

  /// std::ref overload to prevent wrapping a reference_wrapper
  template<typename _Tp>
    _GLIBCXX20_CONSTEXPR
    inline reference_wrapper<_Tp>
    ref(reference_wrapper<_Tp> __t) noexcept
    { return __t; }

  /// std::cref overload to prevent wrapping a reference_wrapper
  template<typename _Tp>
    _GLIBCXX20_CONSTEXPR
    inline reference_wrapper<const _Tp>
    cref(reference_wrapper<_Tp> __t) noexcept
    { return { __t.get() }; }

  /// @}

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // C++11

#endif // _GLIBCXX_REFWRAP_H
