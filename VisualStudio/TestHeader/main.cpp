#include <optional/optional.hpp>


using namespace boost;

static_assert(dtl::optional_type<boost::optional<int>>);

class nohash {};

int main() {
	int i = __cplusplus;

	boost::optional<int> oi = i;
	boost::optional<int&> oir{ i };
	oi = oir;
	oi = i;
	oi = 42;
	bool ok = oi == i;
	ok = oi == 42;

	std::optional<int> os;
	os = oi;
	ok = os == oi;
	ok = os == os;
	ok = ::boost::dtl::eq_v(i, 42);

	auto h = std::hash<optional<int>>{}(oi);
	(void)h;
	optional<nohash> onh;
	static_assert(!std::is_default_constructible_v<std::hash<optional<nohash>>>);
	static_assert(!std::is_default_constructible_v<std::hash<optional<const nohash>>>);
	static_assert(!std::is_default_constructible_v<std::hash<optional<nohash &>>>);
	static_assert(!std::is_default_constructible_v<std::hash<optional<const nohash &>>>);
	return *oi + ok;
}
