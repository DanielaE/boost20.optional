// Copyright (C) 2020, Daniela Engert
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef OPTIONAL_NAMED_MODULE
#pragma once

#define OPTIONAL_EXPORT
#define OPTIONAL_NOEXPORT_BEGIN namespace {
#define OPTIONAL_NOEXPORT_END }

#else // OPTIONAL_NAMED_MODULE

#define OPTIONAL_EXPORT export
#define OPTIONAL_NOEXPORT_BEGIN
#define OPTIONAL_NOEXPORT_END

#endif // OPTIONAL_NAMED_MODULE

#if __cplusplus <= 201703L
#  error insufficient language support!
#endif

#if !defined(OPTIONAL_NAMED_MODULE) || !defined(OPTIONAL_NAMED_MODULE_PURVIEW)

// global module fragment

#include <optional>
#include <type_traits>
#include <functional> // for std::hash
#include <concepts>
#include <compare>

namespace boost {
class in_place_factory_base;
class typed_in_place_factory_base;

template <typename T>
struct optional_swap_should_use_default_constructor;
} // namespace boost

#endif // !OPTIONAL_NAMED_MODULE_PURVIEW

#if !defined(OPTIONAL_NAMED_MODULE) || defined(OPTIONAL_NAMED_MODULE_PURVIEW)

// module purview

#ifdef OPTIONAL_USE_OPTIONAL
#define OPTIONAL_BASE_OPTIONAL OPTIONAL_USE_OPTIONAL
#else
#define OPTIONAL_BASE_OPTIONAL std::optional
#endif

#ifdef __cpp_consteval
#define OPTIONAL_CONSTEVAL consteval
#else
#define OPTIONAL_CONSTEVAL constexpr
#endif

#if !defined(OPTIONAL_NO_THREE_WAY) && defined(__cpp_impl_three_way_comparison)
#define OPTIONAL_THREE_WAY
#endif

#ifndef OPTIONAL_NAMESPACE
#define OPTIONAL_NAMESPACE boost
#endif
#ifndef OPTIONAL_DEPRECATED
#define OPTIONAL_DEPRECATED [[deprecated]]
#endif

OPTIONAL_EXPORT namespace OPTIONAL_NAMESPACE {

struct in_place_init_t : std::in_place_t {
	constexpr explicit in_place_init_t(std::in_place_t) {}
};
inline constexpr in_place_init_t in_place_init{ std::in_place };

struct in_place_init_if_t : std::in_place_t {
	constexpr explicit in_place_init_if_t(std::in_place_t) {}
};
inline constexpr in_place_init_if_t in_place_init_if{ std::in_place };

#ifndef OPTIONAL_NONE_T_CPP20
#define OPTIONAL_NONE_T_CPP20
struct none_t : std::nullopt_t {
	constexpr none_t(std::nullopt_t) : std::nullopt_t{std::nullopt} {}
};
#endif
#ifndef OPTIONAL_NONE_CPP20
#define OPTIONAL_NONE_CPP20
inline constexpr none_t none{ std::nullopt };
#endif

// [optional.nullopt]
using nullopt_t = std::nullopt_t;
inline constexpr nullopt_t nullopt{ std::nullopt };

// [optional.bad.access]
using bad_optional_access = std::bad_optional_access;

} // exported namespace OPTIONAL_NAMESPACE

namespace OPTIONAL_NAMESPACE {
OPTIONAL_NOEXPORT_BEGIN
namespace dtl {

template <typename T>
using base_optional = OPTIONAL_BASE_OPTIONAL<T>;

template <typename T>
inline constexpr bool dependent_false = false;

OPTIONAL_CONSTEVAL std::false_type optional_tag(...);
template <typename T>
OPTIONAL_CONSTEVAL std::true_type optional_tag(const volatile dtl::base_optional<T> *);

template <typename T>
concept optional_type = decltype(optional_tag((std::remove_reference_t<T> *)nullptr))::value;
template <typename T>
concept nullopt_type = std::is_base_of_v<std::nullopt_t, std::remove_cvref_t<T>>;
template <typename T>
concept inplace_type = std::is_base_of_v<std::in_place_t, std::remove_cvref_t<T>>;
template <typename T>
concept optional_related = nullopt_type<T> || inplace_type<T> || optional_type<T>;
template <typename T>
concept inplace_factory_type =
	std::is_base_of_v<in_place_factory_base, std::decay_t<T>> ||
	std::is_base_of_v<typed_in_place_factory_base, std::decay_t<T>>;

template <typename T, typename U>
concept compatible_optional_type =
	(std::is_constructible_v<T, dtl::base_optional<U> &> ||
	 std::is_constructible_v<T, const dtl::base_optional<U> &> ||
	 std::is_constructible_v<T, const dtl::base_optional<U>> ||
	 std::is_constructible_v<T, dtl::base_optional<U>> ||
	 std::is_convertible_v<dtl::base_optional<U> &, T> ||
	 std::is_convertible_v<const dtl::base_optional<U> &, T> ||
	 std::is_convertible_v<const dtl::base_optional<U>, T> ||
	 std::is_convertible_v<dtl::base_optional<U>, T>);

template <typename T>
using unwrap_t = std::conditional_t<optional_type<T>,
	typename std::remove_reference_t<T>::value_type,
	typename std::remove_reference_t<T>>;

template <typename T, typename U>
concept constructible =
	std::is_same_v<T, std::remove_reference_t<U>> ||
	std::is_same_v<T, const std::remove_reference_t<U>>;

template <typename T>
concept _bool_testable = std::convertible_to<T, bool>;
template <typename T>
concept bool_testable = _bool_testable<T> && requires(T && t) {
	{ !static_cast<T &&>(t) } -> _bool_testable;
};

template <typename T, typename U>
concept eq_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs == rhs } -> bool_testable;
	};

template <typename T, typename U>
concept ne_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs != rhs } -> bool_testable;
	};

template <typename T, typename U>
concept lt_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs < rhs } -> bool_testable;
	};

template <typename T, typename U>
concept gt_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs > rhs } -> bool_testable;
	};

template <typename T, typename U>
concept le_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs <= rhs } -> bool_testable;
	};

template <typename T, typename U>
concept ge_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ lhs >= rhs } -> bool_testable;
	};

template <typename T, typename U>
concept tw_comparable =
	requires(const std::remove_cvref_t<T> & lhs, const std::remove_cvref_t<U> & rhs) {
		{ (lhs <=> rhs) == 0 } -> bool_testable;
	};

template <typename T, typename U>
constexpr bool eq_v(const T & lhs, const U & rhs) {
	if constexpr (eq_comparable<T, U>) {
		return lhs == rhs;
	} else if constexpr (ne_comparable<T, U>) {
		return !(lhs != rhs);
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs == rhs");
	}
}

template <typename T, typename U>
constexpr bool ne_v(const T & lhs, const U & rhs) {
	if constexpr (ne_comparable<T, U>) {
		return lhs != rhs;
	} else if constexpr (eq_comparable<T, U>) {
		return !(lhs == rhs);
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs != rhs");
	}
}

template <typename T, typename U>
constexpr bool lt_v(const T & lhs, const U & rhs) {
	if constexpr (lt_comparable<T, U>) {
		return lhs < rhs;
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs < rhs");
	}
}

template <typename T, typename U>
constexpr bool gt_v(const T & lhs, const U & rhs) {
	if constexpr (gt_comparable<T, U>) {
		return lhs > rhs;
	} else if constexpr (lt_comparable<U, T>) {
		return rhs < lhs;
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs > rhs");
	}
}

template <typename T, typename U>
constexpr bool le_v(const T & lhs, const U & rhs) {
	if constexpr (le_comparable<T, U>) {
		return lhs <= rhs;
	} else if constexpr (lt_comparable<U, T>) {
		return !(rhs < lhs);
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs <= rhs");
	}
}

template <typename T, typename U>
constexpr bool ge_v(const T & lhs, const U & rhs) {
	if constexpr (ge_comparable<T, U>) {
		return lhs >= rhs;
	} else if constexpr (lt_comparable<T, U>) {
		return !(lhs < rhs);
	} else {
		static_assert(dependent_false<T>, "invalid expression lhs >= rhs");
	}
}

} // non-exported namespace dtl
OPTIONAL_NOEXPORT_END
} // namespace OPTIONAL_NAMESPACE

OPTIONAL_EXPORT namespace OPTIONAL_NAMESPACE {

// [optional.optional]
template <typename T>
class optional : public dtl::base_optional<T> {
	using base = dtl::base_optional<T>;

	template <typename Factory>
	void construct_at(void * storage, Factory && f) {
		if constexpr (std::is_convertible_v<Factory *, ::boost::in_place_factory_base *>)
			f.template apply<T>(storage);
		else
			f.apply(storage);
	}

	template <typename Factory>
	T make_from(Factory && f) {
		alignas(T) char storage[sizeof(T)];
		construct_at(storage, static_cast<Factory &&>(f));
		return static_cast<T &&>(*std::launder(reinterpret_cast<T*>(storage)));
	}

	template <typename Factory>
	void replace_from(Factory && f) {
		T * pstorage = this->operator->();
		pstorage->~T();
		construct_at(pstorage, static_cast<Factory &&>(f));
	}

public:
	using value_type = T;

	// [optional.object.ctor]
	[[nodiscard]] constexpr optional() noexcept = default;
	[[nodiscard]] constexpr optional(std::nullopt_t) noexcept {};

	template <typename... Args>
		requires std::is_constructible_v<T, Args...>
	[[nodiscard]] constexpr explicit optional(std::in_place_t, Args &&... args)
	: base{ std::in_place, static_cast<Args &&>(args)... } {}

	template <typename U, typename... Args>
		requires std::is_constructible_v<T, std::initializer_list<U> &, Args &&...>
	[[nodiscard]] constexpr optional(std::in_place_t, std::initializer_list<U> il, Args &&... args)
	: base{ std::in_place, il, static_cast<Args &&>(args)... } {}

	template <typename U>
		requires (!dtl::optional_related<U> &&
		          !std::is_same_v<T, std::decay_t<U>> &&
		          !dtl::inplace_factory_type<U> &&
		           std::is_constructible_v<T, U>)
	[[nodiscard]] constexpr explicit(!std::is_convertible_v<U, T>)
	optional(U && other) : base{ static_cast<U &&>(other) } {}

	template <typename U>
		requires std::is_constructible_v<T, U>
	[[nodiscard]] constexpr explicit(!std::is_convertible_v<U, T>)
	optional(const optional<U &> & other) : base{ other ? base{*other} : base{} } {}
	template <typename U>
		requires (!std::is_reference_v<U> &&
		          !std::is_same_v<T, U> &&
		          !dtl::compatible_optional_type<T, U>)
	[[nodiscard]] explicit(!std::is_convertible_v<U, T>)
	optional(const dtl::base_optional<U> & other) : base{ other } {}

	template <typename U>
		requires (!std::is_reference_v<U> &&
		          !std::is_same_v<T, U> &&
		          !dtl::compatible_optional_type<T, U>)
	[[nodiscard]] explicit(!std::is_convertible_v<U, T>)
	optional(dtl::base_optional<U> && other) : base{ static_cast<dtl::base_optional<U> &&>(other) } {}

	// [optional.assign]
	optional & operator=(std::nullopt_t) noexcept {
		return static_cast<optional &>(base::operator=(std::nullopt));
	}

	template <typename U = T>
		requires (!std::is_same_v<optional, std::remove_cvref_t<U>> &&
		          !dtl::nullopt_type<U> &&
		          !(std::is_scalar_v<T> && std::is_same_v<T, std::decay_t<U>>) &&
		          std::is_constructible_v<T, U> && std::is_assignable_v<T&, U>)
	optional & operator=(U && rhs) {
		return static_cast<optional &>(base::operator=(static_cast<U &&>(rhs)));
	}

	template <typename U>
		requires (!std::is_reference_v<U> &&
		           std::is_assignable_v<dtl::base_optional<T>, const dtl::base_optional<U> &>)
	optional & operator=(const dtl::base_optional<U> & rhs) {
		return static_cast<optional &>(
			base::operator=(rhs));
	}

	template <typename U>
		requires (!std::is_reference_v<U> &&
		           std::is_assignable_v<dtl::base_optional<T>, dtl::base_optional<U> &&>)
	optional & operator=(dtl::base_optional<U> && rhs) {
		return static_cast<optional &>(
			base::operator=(static_cast<dtl::base_optional<U> &&>(rhs)));
	}

	template <typename U>
		requires requires(base lhs, const U & rhs) { lhs = rhs; }
	optional & operator=(const optional<U &> & rhs) {
		return static_cast<optional &>(rhs ? base::operator=(*rhs) : base::operator=(std::nullopt));
	}

	template <typename U>
		requires requires(base lhs, const U & rhs) { lhs = rhs; }
	optional & operator=(optional<U &> && rhs) {
		return static_cast<optional &>( rhs ? base::operator=(*rhs) : base::operator=(std::nullopt));
	}

	// conversion from base
	[[nodiscard]] constexpr optional(const base & from) : base(from) {}
	[[nodiscard]] constexpr optional(base && from) noexcept : base(static_cast<base &&>(from)) {}

	// non-standard additional Boost interfaces

	using reference_type       = T &;
	using reference_const_type = const T &;
	using rval_reference_type  = T &&;
	using pointer_type         = T *;
	using pointer_const_type   = const T *;
	using argument_type        = T const &;

	// construction
	[[nodiscard]] constexpr optional(const T & other) : base{ other } {}
	[[nodiscard]] constexpr optional(T && other)
		noexcept(std::is_nothrow_move_constructible_v<T>)
		requires std::is_move_constructible_v<T>
	: base{ static_cast<T &&>(other) } {}

	[[nodiscard]] constexpr optional(bool condition, const T & other)
	: base{ condition ? base(other) : base{} } {}
	[[nodiscard]] constexpr optional(bool condition, T && other)
		noexcept(std::is_nothrow_move_constructible_v<T>)
		requires std::is_move_constructible_v<T>
	: base{ condition ? base(static_cast<T &&>(other)) : base{} } {}

	template <typename... Args>
	constexpr optional(in_place_init_if_t, bool condition, Args &&... args)
	: base{} { if (condition) this->emplace(static_cast<Args &&>(args)...); }

	template <typename Factory>
		requires (dtl::inplace_factory_type<Factory> && std::is_default_constructible_v<T>)
	explicit optional(Factory && f)
	: base(std::in_place) {
		replace_from(static_cast<Factory &&>(f));
	}
	template <typename Factory>
		requires (dtl::inplace_factory_type<Factory> && !std::is_default_constructible_v<T>)
	explicit optional(Factory && f)
	: base(std::in_place, make_from(static_cast<Factory &&>(f))) {}

	// assignment
	template <typename Factory>
		requires dtl::inplace_factory_type<Factory>
	optional<T> & operator=(Factory && f) {
		if (*this) {
			replace_from(static_cast<Factory &&>(f));
		} else {
			if constexpr (std::is_default_constructible_v<T>) {
				this->emplace();
				replace_from(static_cast<Factory &&>(f));
			} else {
				this->emplace(make_from(static_cast<Factory &&>(f)));
			}
		}
		return *this;
	}

	// observers
	[[nodiscard]] constexpr const T & get() const { return **this; }
	[[nodiscard]] constexpr T & get() { return **this; }

	[[nodiscard]] constexpr const T * get_ptr() const {
		return this->has_value() ? std::addressof(**this) : nullptr;
	}
	[[nodiscard]] constexpr T * get_ptr() {
		return this->has_value() ? std::addressof(**this) : nullptr;
	}

	[[nodiscard]] constexpr bool operator!() const noexcept {
		return !static_cast<bool>(*this);
	}

	template <typename Func>
	[[nodiscard]] constexpr optional<std::invoke_result_t<Func, T &>>
	map(Func f) & {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] constexpr optional<std::invoke_result_t<Func, const T &>>
	map(Func f) const & {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] constexpr optional<std::invoke_result_t<Func, T &&>>
	map(Func f) && {
		if (this->has_value())
			return f(static_cast<T &&>(**this));
		return none;
	}

	template <typename Func>
	[[nodiscard]] constexpr
		optional<dtl::unwrap_t<std::invoke_result_t<Func, T &>>>
	flat_map(Func f) & {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] constexpr
		optional<dtl::unwrap_t<std::invoke_result_t<Func, const T &>>>
	flat_map(Func f) const & {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] constexpr
		optional<dtl::unwrap_t<std::invoke_result_t<Func, T &&>>>
	flat_map(Func f) && {
		if (this->has_value())
			return f(static_cast<T &&>(**this));
		return none;
	}

	[[nodiscard]] constexpr const T & get_value_or(const T & replacement) const {
		return this->has_value() ? **this : replacement;
	}
	[[nodiscard]] constexpr T & get_value_or(T & replacement) {
		return this->has_value() ? **this : replacement;
	}

	template <typename Func>
	[[nodiscard]] constexpr T value_or_eval(Func f) const & {
		return this->has_value()? **this : f();
	}
	template <typename Func>
	[[nodiscard]] constexpr T value_or_eval(Func f) && {
		return this->has_value()? static_cast<T &&>(**this) : f();
	}

	// modifiers
	constexpr void reset() { base::reset(); }

	// deprecated
	// observers
	OPTIONAL_DEPRECATED [[nodiscard]] constexpr bool is_initialized() const noexcept {
		return this->has_value();
	}
	// modifiers
	OPTIONAL_DEPRECATED constexpr void reset(T const & rhs) { *this = rhs; }
}; // class optional<T>

template <typename T>
class optional<T &> {
	template <typename U>
	friend OPTIONAL_CONSTEVAL std::true_type optional_tag(const volatile optional<U &> *);

	template <typename U>
	struct taint_rvalue {
		[[maybe_unused]] constexpr taint_rvalue() = default;
		static_assert(std::is_lvalue_reference_v<U>, "binding rvalues is ill-formed");
	};

	template <typename U>
	constexpr decltype(auto) forward_reference(U && r) {
		taint_rvalue<U>{};
		return static_cast<U &&>(r);
	}

	T * p_ = nullptr;

public:
	using value_type = T &;

	// [optional.object.ctor]
	[[nodiscard]] constexpr optional() noexcept = default;
	[[nodiscard]] constexpr optional(none_t) noexcept {}
	[[nodiscard]] constexpr optional(std::nullopt_t) noexcept {}
	[[nodiscard]] constexpr optional(const optional & other) noexcept = default;
	[[nodiscard]] constexpr optional(optional && other) noexcept      = default;
	optional(T &&) { taint_rvalue<T>{}; }

	template <typename U>
	[[nodiscard]] constexpr explicit optional(const optional<U &> & other) noexcept : p_(other.get_ptr()) {}

	template <typename U>
		requires dtl::constructible<T, U>
	[[nodiscard]] constexpr optional(U & other) noexcept : p_(std::addressof(other)) {}

	template <typename U>
		requires(!dtl::optional_type<U> && !dtl::constructible<T, U>)
	[[nodiscard]] constexpr optional(U && other) noexcept : p_(std::addressof(other)) {
		taint_rvalue<U>{};
	}

	// [optional.assign]
	constexpr optional & operator=(const optional & rhs) noexcept = default;
	constexpr optional & operator=(optional && rhs) noexcept = default;

	constexpr optional & operator=(std::nullopt_t) noexcept {
		p_ = nullptr;
		return *this;
	}

	template <typename U>
	constexpr optional & operator=(const optional<U &> & rhs) noexcept {
		p_ = rhs.get_ptr();
		return *this;
	}

	template <typename U>
		requires (!dtl::optional_related<U>)
	constexpr optional & operator=(U && rhs) noexcept {
		taint_rvalue<U>{};
		p_ = std::addressof(rhs);
		return *this;
	}

	template <typename U>
		requires (!dtl::optional_related<U>)
	constexpr void emplace(U && rhs) noexcept {
		p_ = std::addressof(rhs);
		taint_rvalue<U>{};
	}

	// [optional.observe]
	constexpr T * operator->() const noexcept { return p_; }
	constexpr T & operator*() const { return *p_; }
	[[nodiscard]] constexpr explicit operator bool() const noexcept { return p_ != nullptr; }
	[[nodiscard]] constexpr bool has_value() const noexcept { return p_ != nullptr; }

	[[nodiscard]] constexpr T & value() const {
		return p_ ? *p_ : throw bad_optional_access();
	}
	template <typename U>
		requires (!dtl::optional_type<U>)
	[[nodiscard]] constexpr T & value_or(U && replacement) const noexcept {
		taint_rvalue<U>{};
		return p_ ? *p_ : replacement;
	}

	// [optional.swap]
	constexpr void swap(optional & rhs) noexcept { std::swap(p_, rhs.p_); }

	// [optional.mod]
	constexpr void reset() noexcept { p_ = nullptr; }

	// non-standard additional Boost interfaces

	using reference_type       = T &;
	using reference_const_type = T &;
	using rval_reference_type  = T &;
	using pointer_type         = T *;
	using pointer_const_type   = T *;

	// construction
	template <typename U>
		requires (!dtl::optional_related<U>)
	constexpr optional(bool condition, U && rhs) noexcept
	: p_(condition ? std::addressof(rhs) : nullptr) {
		taint_rvalue<U>{};
	}

	// observers
	[[nodiscard]] constexpr T & get() const { return *p_; }
	[[nodiscard]] constexpr T * get_ptr() const noexcept { return p_; }
	constexpr bool operator!() const noexcept { return p_ == nullptr; }

	template <typename Func>
	[[nodiscard]] optional<std::invoke_result_t<Func, T &>>
	map(Func f) const {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] optional<dtl::unwrap_t<std::invoke_result_t<Func, T &>>>
	flat_map(Func f) const {
		if (this->has_value())
			return f(**this);
		return none;
	}

	template <typename Func>
	[[nodiscard]] T & value_or_eval(Func f) const {
		taint_rvalue<std::invoke_result_t<Func>>{};
		return p_ ? *p_ : f();
	}

	// deprecated
	// observers
	OPTIONAL_DEPRECATED [[nodiscard]] constexpr bool is_initialized() const noexcept {
		return p_ != nullptr;
	}

	template <typename U>
		requires (!dtl::optional_type<U>)
	OPTIONAL_DEPRECATED [[nodiscard]] constexpr T &
	get_value_or(U && replacement) const noexcept {
		taint_rvalue<U>{};
		return p_ ? *p_ : replacement;
	}

	// modifiers
	template <typename U>
		requires (!dtl::optional_type<U>)
	OPTIONAL_DEPRECATED constexpr void reset(U && rhs) noexcept {
		taint_rvalue<U>{};
		p_ = std::addressof(rhs);
	}
}; // class optional<T &>

// [optional.relops]
template <typename T, typename U>
[[nodiscard]] constexpr bool operator==(const optional<T> & lhs, const optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv == rhs.has_value() && (!lhv || dtl::eq_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator==(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv == rhs.has_value() && (!lhv || dtl::eq_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator==(const dtl::base_optional<U> & lhs, const optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv == rhs.has_value() && (!lhv || dtl::eq_v(*lhs, *rhs));
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator!=(const optional<T> & lhs, const optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv != rhs.has_value() || (lhv && dtl::ne_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator!=(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv != rhs.has_value() || (lhv && dtl::ne_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator!=(const dtl::base_optional<T> & lhs, const optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	return lhv != rhs.has_value() || (lhv && dtl::ne_v(*lhs, *rhs));
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator<(const optional<T> & lhs, const optional<U> & rhs) {
	return rhs.has_value() && (!lhs.has_value() || dtl::lt_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator<(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	return rhs.has_value() && (!lhs.has_value() || dtl::lt_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator<(const dtl::base_optional<T> & lhs, const optional<U> & rhs) {
	return rhs.has_value() && (!lhs.has_value() || dtl::lt_v(*lhs, *rhs));
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator>(const optional<T> & lhs, const optional<U> & rhs) {
	return lhs.has_value() && (!rhs.has_value() || dtl::gt_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator>(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	return lhs.has_value() && (!rhs.has_value() || dtl::gt_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator>(const dtl::base_optional<T> & lhs, const optional<U> & rhs) {
	return lhs.has_value() && (!rhs.has_value() || dtl::gt_v(*lhs, *rhs));
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator<=(const optional<T> & lhs, const optional<U> & rhs) {
	return !lhs.has_value() || (rhs.has_value() && dtl::le_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator<=(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	return !lhs.has_value() || (rhs.has_value() && dtl::le_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator<=(const dtl::base_optional<T> & lhs, const optional<U> & rhs) {
	return !lhs.has_value() || (rhs.has_value() && dtl::le_v(*lhs, *rhs));
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator>=(const optional<T> & lhs, const optional<U> & rhs) {
	return !rhs.has_value() || (lhs.has_value() && dtl::ge_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator>=(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	return !rhs.has_value() || (lhs.has_value() && dtl::ge_v(*lhs, *rhs));
}
template <typename T, typename U>
[[nodiscard]] constexpr bool operator>=(const dtl::base_optional<T> & lhs, const optional<U> & rhs) {
	return !rhs.has_value() || (lhs.has_value() && dtl::ge_v(*lhs, *rhs));
}

#ifdef OPTIONAL_THREE_WAY
template <typename T,  typename U>
	requires dtl::tw_comparable<T, U>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U>
operator<=>(const optional<T> & lhs, const optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	const bool rhv = rhs.has_value();
	return lhs && rhv ? *lhs <=> *rhs : lhv <=> rhv;
}
template <typename T,  typename U>
	requires dtl::tw_comparable<T, U>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U>
operator<=>(const optional<T> & lhs, const dtl::base_optional<U> & rhs) {
	const bool lhv = lhs.has_value();
	const bool rhv = rhs.has_value();
	return lhs && rhv ? *lhs <=> *rhs : lhv <=> rhv;
}
#endif

// [optional.nullops]
template <typename T>
[[nodiscard]] constexpr bool operator==(const optional<T> & o, std::nullopt_t) noexcept {
	return !o.has_value();
}
template <typename T>
[[nodiscard]] constexpr bool operator==(std::nullopt_t, const optional<T> & o) noexcept {
	return !o.has_value();
}

template <typename T>
[[nodiscard]] constexpr bool operator!=(const optional<T> & o, std::nullopt_t) noexcept {
	return o.has_value();
}
template <typename T>
[[nodiscard]] constexpr bool operator!=(std::nullopt_t, const optional<T> & o) noexcept {
	return o.has_value();
}

template <typename T>
[[nodiscard]] constexpr bool operator<(const optional<T> &, std::nullopt_t) noexcept {
	return false;
}
template <typename T>
[[nodiscard]] constexpr bool operator<(std::nullopt_t, const optional<T> & o) noexcept {
	return o.has_value();
}

template <typename T>
[[nodiscard]] constexpr bool operator>(const optional<T> & o, std::nullopt_t) noexcept {
	return o.has_value();
}
template <typename T>
[[nodiscard]] constexpr bool operator>(std::nullopt_t, const optional<T> &) noexcept {
	return false;
}

template <typename T>
[[nodiscard]] constexpr bool operator<=(const optional<T> & o, std::nullopt_t) noexcept {
	return !o.has_value();
}
template <typename T>
[[nodiscard]] constexpr bool operator<=(std::nullopt_t, const optional<T> &) noexcept {
	return true;
}

template <typename T>
[[nodiscard]] constexpr bool operator>=(const optional<T> &, std::nullopt_t) noexcept {
	return true;
}
template <typename T>
[[nodiscard]] constexpr bool operator>=(std::nullopt_t, const optional<T> & o) noexcept {
	return !o.has_value();
}

template <typename T>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const optional<T> & o, std::nullopt_t) noexcept {
	return o.has_value() <=> false;
}

// [optional.comp_with_t]
template <typename T, typename U>
	requires (dtl::eq_comparable<T, U> || dtl::ne_comparable<T, U>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator==(const optional<T> & lhs, const U & rhs) {
	return lhs.has_value() && dtl::eq_v(*lhs, rhs);
}
template <typename U, typename T>
	requires (dtl::eq_comparable<U, T> || dtl::ne_comparable<U, T>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator==(const U & lhs, const optional<T> & rhs) {
	return rhs.has_value() && dtl::eq_v(lhs, *rhs);
}

template <typename T, typename U>
	requires (dtl::ne_comparable<T, U> || dtl::eq_comparable<T, U>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator!=(const optional<T> & lhs, const U & rhs) {
	return !lhs.has_value() || dtl::ne_v(*lhs, rhs);
}
template <typename U, typename T>
	requires (dtl::ne_comparable<U, T> || dtl::eq_comparable<U, T>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator!=(const U & lhs, const optional<T> & rhs) {
	return !rhs.has_value() || dtl::ne_v(lhs, *rhs);
}

template <typename T, typename U>
	requires dtl::lt_comparable<T, U> && (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator<(const optional<T> & lhs, const U & rhs) {
	return !lhs.has_value() || dtl::lt_v(*lhs, rhs);
}
template <typename U, typename T>
	requires dtl::lt_comparable<U, T> && (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator<(const U & lhs, const optional<T> & rhs) {
	return rhs.has_value() && dtl::lt_v(lhs, *rhs);
}

template <typename T, typename U>
	requires (dtl::gt_comparable<T, U> || dtl::lt_comparable<U, T>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator>(const optional<T> & lhs, const U & rhs) {
	return lhs.has_value() && dtl::gt_v(*lhs, rhs);
}
template <typename U, typename T>
	requires (dtl::gt_comparable<U, T> || dtl::lt_comparable<T, U>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator>(const U & lhs, const optional<T> & rhs) {
	return !rhs.has_value() || dtl::gt_v(lhs, *rhs);
}

template <typename T, typename U>
	requires (dtl::le_comparable<T, U> || dtl::lt_comparable<U, T>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator<=(const optional<T> & lhs, const U & rhs) {
	return !lhs.has_value() || dtl::le_v(*lhs, rhs);
}
template <typename U, typename T>
	requires (dtl::le_comparable<U, T> || dtl::lt_comparable<T, U>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator<=(const U & lhs, const optional<T> & rhs) {
	return rhs.has_value() && dtl::le_v(lhs, *rhs);
}

template <typename T, typename U>
	requires (dtl::ge_comparable<T, U> || dtl::lt_comparable<T, U>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator>=(const optional<T> & lhs, const U & rhs) {
	return lhs.has_value() && dtl::ge_v(*lhs, rhs);
}
template <typename U, typename T>
	requires (dtl::ge_comparable<U, T> || dtl::lt_comparable<U, T>) &&
	         (!dtl::optional_related<U>)
[[nodiscard]] constexpr bool operator>=(const U & lhs, const optional<T> & rhs) {
	return !rhs.has_value() || dtl::ge_v(lhs, *rhs);
}

#ifdef OPTIONAL_THREE_WAY
template <typename T,  typename U>
	requires dtl::tw_comparable<T, U>
[[nodiscard]] constexpr std::compare_three_way_result_t<T, U>
operator<=>(const optional<T> & lhs, const U & rhs) {
	return lhs.has_value() ? *lhs <=> rhs : std::strong_ordering::less;
}
#endif

// [optional.specalg]
template <typename T>
constexpr void swap(optional<T &> & lhs, optional<T &> & rhs) noexcept {
	lhs.swap(rhs);
}
template <typename T>
constexpr void swap(optional<T &> & lhs, dtl::base_optional<T &> & rhs) noexcept {
	lhs.swap(rhs);
}
template <typename T>
constexpr void swap(dtl::base_optional<T &> & lhs, optional<T &> & rhs) noexcept {
	lhs.swap(rhs);
}

template <typename T>
	requires (!dtl::optional_type<T>)
[[nodiscard]] constexpr optional<std::decay_t<T>> make_optional(T && value) {
	return optional<std::decay_t<T>>{ static_cast<T &&>(value) };
}
template <typename T>
	requires dtl::optional_type<T>
[[nodiscard]] constexpr optional<std::decay_t<T>> make_optional(T && value) {
	return optional<std::decay_t<T>>{ static_cast<T &&>(value) };
}
template <typename T, typename... Args>
[[nodiscard]] constexpr optional<T> make_optional(Args &&... args) {
	return optional<T>{ std::in_place, static_cast<Args &&>(args)... };
}
template <typename T, typename Elem, typename... Args>
[[nodiscard]] constexpr optional<T>
make_optional(std::initializer_list<Elem> list, Args &&... args) {
	return optional<T>{ std::in_place, list, static_cast<Args &&>(args)... };
}

template <typename T>
optional(T) -> optional<T>;

} // exported namespace OPTIONAL_NAMESPACE

// non-standard additional Boost interfaces

namespace OPTIONAL_NAMESPACE {
OPTIONAL_NOEXPORT_BEGIN
namespace dtl {

template <typename T>
using reference_t = typename optional<T>::reference_type;
template <typename T>
using const_reference_t = typename optional<T>::reference_const_type;
template <typename T>
using pointer_t = typename optional<T>::pointer_type;
template <typename T>
using const_pointer_t = typename optional<T>::pointer_const_type;

} // non-exported namespace dtl
OPTIONAL_NOEXPORT_END
} // namespace OPTIONAL_NAMESPACE

OPTIONAL_EXPORT namespace OPTIONAL_NAMESPACE {

template <typename T>
[[nodiscard]] constexpr optional<std::decay_t<T>>
make_optional(bool condition, T && v) {
	return optional<std::decay_t<T>>(condition, static_cast<T &&>(v));
}

template <typename T>
constexpr dtl::const_reference_t<T> get(const optional<T> & o) {
	return o.get();
}

template <typename T>
constexpr dtl::reference_t<T> get(optional<T> & o) {
	return o.get();
}

template <typename T>
constexpr dtl::const_pointer_t<T> get(const optional<T> * o) {
	return o->get_ptr();
}

template <typename T>
constexpr dtl::pointer_t<T> get(optional<T> * o) {
	return o->get_ptr();
}

template <typename T>
constexpr dtl::const_reference_t<T>
get_optional_value_or(const optional<T> & o, dtl::const_reference_t<T> v) {
	return o.get_value_or(v);
}

template <typename T>
constexpr dtl::reference_t<T>
get_optional_value_or(optional<T> & o, dtl::reference_t<T> v) {
	return o.get_value_or(v);
}

template <typename T>
constexpr dtl::const_pointer_t<T> get_pointer(const optional<T> & o) {
	return o.get_ptr();
}

template <typename T>
constexpr dtl::pointer_t<T> get_pointer(optional<T> & o) {
	return o.get_ptr();
}

} // exported namespace OPTIONAL_NAMESPACE

// [optional.hash]
namespace std {

OPTIONAL_EXPORT
template <typename T>
	requires is_default_constructible_v<
		hash<::OPTIONAL_NAMESPACE::dtl::base_optional<T>>>
struct hash<::OPTIONAL_NAMESPACE::optional<T>> {
	using argument_type OPTIONAL_DEPRECATED = ::OPTIONAL_NAMESPACE::optional<T>;
	using result_type OPTIONAL_DEPRECATED   = size_t;
	using _h = hash<::OPTIONAL_NAMESPACE::dtl::base_optional<T>>;
	[[nodiscard]] size_t operator()(const ::OPTIONAL_NAMESPACE::optional<T> & o) const noexcept(
		noexcept(_h{}(o))) {
		return _h{}(o);
	}
};

OPTIONAL_EXPORT
template <typename T>
	requires is_default_constructible_v<hash<remove_const_t<T>>>
struct hash<::OPTIONAL_NAMESPACE::optional<T &>> {
	using argument_type OPTIONAL_DEPRECATED = ::OPTIONAL_NAMESPACE::optional<T &>;
	using result_type OPTIONAL_DEPRECATED   = size_t;
	using _h = hash<remove_const_t<T>>;
	[[nodiscard]] size_t operator()(const ::OPTIONAL_NAMESPACE::optional<T &> & o) const noexcept(
		noexcept(_h{}(declval<const remove_const_t<T> &>()))) {
		return o.has_value() ? _h{}(*o) : 0;
	}
};

} // namespace std

#undef OPTIONAL_THREE_WAY
#undef OPTIONAL_CONSTEVAL
#undef OPTIONAL_DEPRECATED
#undef OPTIONAL_NAMESPACE
#undef OPTIONAL_BASE_OPTIONAL

#endif // OPTIONAL_NAMED_MODULE_PURVIEW

#if defined(OPTIONAL_NAMED_MODULE) && !defined(OPTIONAL_NAMED_MODULE_PURVIEW)
#define OPTIONAL_NAMED_MODULE_PURVIEW
#endif

#undef OPTIONAL_EXPORT
#undef OPTIONAL_NOEXPORT_BEGIN
#undef OPTIONAL_NOEXPORT_END
