#pragma once

#include <algorithm>
#include <bits/ensure.h>
#include <bits/nl_item.h>
#include <frg/scope_exit.hpp>
#include <frg/small_vector.hpp>
#include <frg/span.hpp>
#include <mlibc/locale.hpp>
#include <ranges>
#include <stdint.h>
#include <string.h>
#include <utility>

namespace mlibc {

namespace coll {

// bitflags set in LC_COLLATE rulesets.
constexpr uint8_t sort_forward = 1;
constexpr uint8_t sort_backward = 2;
constexpr uint8_t sort_position = 4;
[[maybe_unused]]
constexpr uint8_t sort_mask = sort_forward | sort_backward | sort_position;

} // namespace coll

template <typename Char>
struct CollationPolicy {};

template <>
struct CollationPolicy<char> {
	static constexpr int TABLE = _NL_COLLATE_TABLEMB;
	static constexpr int INDIRECT = _NL_COLLATE_INDIRECTMB;

	using StringType = char;
	using UCharType = unsigned char;

	static frg::span<const uint8_t> getWeight(const mlibc::localeinfo *l) {
		return l->collate.get(_NL_COLLATE_WEIGHTMB).asByteSpan();
	}

	static frg::span<const uint8_t> getExtra(const mlibc::localeinfo *l) {
		return l->collate.get(_NL_COLLATE_EXTRAMB).asByteSpan();
	}
};

template <>
struct CollationPolicy<wchar_t> {
	static constexpr int TABLE = _NL_COLLATE_TABLEWC;
	static constexpr int INDIRECT = _NL_COLLATE_INDIRECTWC;

	using StringType = wchar_t;
	using UCharType = wint_t;

	static frg::span<const wint_t> getWeight(const mlibc::localeinfo *l) {
		return l->collate.get(_NL_COLLATE_WEIGHTWC).asUint32Span();
	}

	static frg::span<const wint_t> getExtra(const mlibc::localeinfo *l) {
		return l->collate.get(_NL_COLLATE_EXTRAWC).asUint32Span();
	}
};

template <typename Char>
struct coll_context {
	using P = CollationPolicy<Char>;

private:
	// Represents a single entry in the LC_COLLATE extra table.
	// This table is used to resolve a sequence to its offset into the weights table if the regular
	// table defers here by not having an index >= 0.
	// Its layout is as follows:
	//     int32_t index;
	//     Char nhere;
	//     Char sequence[nhere];
	//     Char end_sequence[nhere]; // only present if `index` is negative
	// The `index` member determines if we are looking at an entry for an exact sequence or rather
	// a range of sequences between `sequence` and `end_sequence`. The `nhere` member determines
	// the length of the sequence.
	struct extra_table_entry {
		extra_table_entry(const P::UCharType *entry)
		: entry_{entry} {

		}

		// Safely extract the index from the first four bytes of the entry.
		int32_t index() const {
			int32_t i;
			memcpy(&i, entry_, sizeof(i));
			return i;
		}

		// Obtain `nhere` from its correct offset.
		size_t nhere() const {
			if constexpr (std::is_same_v<Char, char>)
				return entry_[4];
			else
				return entry_[1];
		}

		// Obtain a span to the `sequence`. For indices >= 0, this is the exact sequence to match;
		// otherwise, this is the inclusive lower bound of the range.
		frg::span<const typename P::UCharType> sequence() const {
			if constexpr (std::is_same_v<Char, char>)
				return frg::span{entry_ + 5, nhere()};
			else
				return frg::span{entry_ + 2, nhere()};
		}

		// Obtain a span to the `end_sequence`. Only valid if `index` is negative. Represents the
		// inclusive upper bound of the range.
		frg::span<const typename P::UCharType> end_sequence() const {
			__ensure(index() < 0);
			if constexpr (std::is_same_v<Char, char>)
				return frg::span{entry_ + 5 + nhere(), nhere()};
			else
				return frg::span{entry_ + 2 + nhere(), nhere()};
		}

		// Spaceship operator implemented to compare as equal if the span falls into the range
		// of this entry.
		constexpr std::weak_ordering operator<=>(const frg::span<const typename P::UCharType> str) const {
			if (index() >= 0) {
				auto seq = sequence();
				if (str.size() >= seq.size() && std::ranges::equal(str.subspan(0, seq.size()), seq))
					return std::weak_ordering::equivalent;
				return std::ranges::lexicographical_compare(str, seq)
					? std::weak_ordering::less : std::weak_ordering::greater;
			} else {
				if (std::ranges::lexicographical_compare(str, sequence()))
					return std::weak_ordering::less;
				if (std::ranges::lexicographical_compare(end_sequence(), str))
					return std::weak_ordering::greater;
				return std::weak_ordering::equivalent;
			}
		}

		// As the spaceship operator, compares as equal if the span falls into the range of this
		// entry.
		constexpr bool operator==(const frg::span<const typename P::UCharType> str) const {
			return (*this <=> str) == 0;
		}

		inline extra_table_entry operator++(int) {
			auto old = *this;
			// Entries are always 4-byte aligned (char) or already implicitly aligned (wchar_t).
			entry_ = reinterpret_cast<P::UCharType *>((reinterpret_cast<uintptr_t>(entry_) + (3 + len())) & ~3);
			__ensure(((uintptr_t) entry_ & 3) == 0);
			return old;
		}

	private:
		size_t len() const {
			size_t len = 4 + (1 + nhere()) * sizeof(Char);
			if (index() < 0)
				len += nhere() * sizeof(Char);
			return len;
		}

		const P::UCharType *entry_;
	};

	// Perform the lookup step for `Char c` in the first-order lookup table `table`.
	inline int32_t table_lookup(const P::UCharType c) const {
		if constexpr (std::is_same_v<Char, char>) {
			return table[c];
		} else {
			uint32_t shift1 = table[0];
			uint32_t index1 = c >> shift1;
			uint32_t bound = table[1];
			if (index1 < bound) {
				uint32_t lookup1 = table[5 + index1] / sizeof(table[0]);
				if (lookup1) {
					uint32_t shift2 = table[2];
					uint32_t mask2 = table[3];
					uint32_t index2 = (c >> shift2) & mask2;
					uint32_t lookup2 = table[lookup1 + index2] / sizeof(table[0]);
					if (lookup2) {
						uint32_t mask3 = table[4];
						uint32_t index3 = c & mask3;
						return table[lookup2 + index3];
					}
				}
			}
			return 0;
		}
	}

public:
	// Helper to construct `coll_context` from `localeinfo`.
	static coll_context<Char> from_localeinfo(const mlibc::localeinfo *l) {
		return {
			.nrules = l->collate.get(_NL_COLLATE_NRULES).asUint32(),
			.rulesets = l->collate.get(_NL_COLLATE_RULESETS).asByteSpan(),
			.table = l->collate.get(P::TABLE).asInt32Span(),
			.extra = P::getExtra(l),
			.indirect = l->collate.get(P::INDIRECT).asInt32Span(),
			.weights = P::getWeight(l),
		};
	}

	// For the sequence `cpp`, find the matching indices into the ruleset and weights table.
	// This walks the tables (`table`, `extra`, `indirect`) until the match is found.
	// The return is a pair of (rule index, weight index).
	inline std::pair<uint8_t, int32_t> get_index(const P::UCharType *&cpp) const {
		auto index = table_lookup(*cpp++);

		// Negative indices refer to the next level of tables.
		if (index < 0) {
			extra_table_entry entry{&extra[-index]};

			while (true) {
				frg::span view{cpp, entry.nhere()};

				// Check if the `view` falls into the range described by the extra table entry.
				if (entry == view) {
					index = entry.index();
					cpp += entry.nhere();

					if (index >= 0)
						return {static_cast<uint8_t>(index >> 24), index & 0xFFFFFF};

					size_t indirect_table_off = [&] {
						if constexpr (std::is_same_v<Char, char>) {
							// Skip over the matching portions of the sequences.
							auto [mismatch_v, mismatch_s] = std::ranges::mismatch(view, entry.sequence());
							auto tail = std::views::zip(
								std::ranges::subrange(mismatch_v, view.end()),
								std::ranges::subrange(mismatch_s, entry.sequence().end())
							);

							return std::ranges::fold_left(tail, size_t{0}, [](size_t acc, auto pair) {
								auto [v, s] = pair;
								return (acc << 8) + (v - s);
							});
						} else {
							return view[entry.nhere() - 1] - entry.sequence()[entry.nhere() - 1];
						}
					}();

					int32_t v = indirect[-index + indirect_table_off];
					return {static_cast<uint8_t>(v >> 24), v & 0xFFFFFF};
				}
				entry++;
			}
		}

		return {static_cast<uint8_t>(index >> 24), index & 0xFFFFFF};
	}

	// Number of passes used in this collation table set.
	const size_t nrules;
	// Rulesets hold the `coll::sort_*` flags for each pass respectively.
	// The layout for each ruleset is:
	//     uint8_t rules[nrules];
	const frg::span<const uint8_t> rulesets;

	// Contains the first level of the lookup tables that maps a sequence to its rule and weight
	// indices. Indices in this table are signed; zero or positive values indicate that this is
	// a result, negative indices indicate the its absolute value is an index into the next lookup
	// level. The logic for this is implemented in `table_lookup`.
	// For a table for narrow characters the layout is a flat array of uint32_t indexed by
	// the first char.
	// For a table for wide characters, the layout is:
	//     uint32_t shift1; // number of bits `wc` should be right-shifted by for level 1
	//     uint32_t bound1; // exclusive upper bound for the shifted value
	//     uint32_t shift2; // number of bits `wc` should be right-shifted by for level 2
	//     uint32_t mask2; // mask for level 2
	//     uint32_t mask3; // mask for level 3
	//     uint32_t data[]; // indices data; base for the level 1 index
	const frg::span<const int32_t> table;
	// See `extra_table_entry` for a description of this table.
	const frg::span<const typename P::UCharType> extra;
	// Flat array of int32_t weight table indices. Entries from the extra table with a negative
	// index point here, with the index determined by the absolute value of the index result
	// from the extra table and an offset derived from the sequence.
	const frg::span<const int32_t> indirect;

	// Table of deduplicated weight sequences. For a sequence, the other tables are used to
	// determine the index into this table. The indexing into this table is in units of
	// `unsigned Char`, as produced by the other tables.
	// The layout for each weight sequence is:
	//     struct {
	//         unsigned Char weight_len;
	//         unsigned Char weight[weight_len];
	//     } weights[nrules];
	const frg::span<const typename P::UCharType> weights;
};

template <typename Char>
struct coll_seq {
	using P = CollationPolicy<Char>;

	// The string as `unsigned Char *` to avoid unintended behavior around the sign bit.
	const P::UCharType *us = nullptr;

	// Position of the sequence relative to the previous non-ignored sequence.
	size_t offset_from_previous = 0;
	// Highest code unit index we traversed to.
	size_t highest_index = 0;
	// Current forward sequence code unit index.
	size_t forward_index = 0;
	// Current backward sequence code unit index.
	std::optional<size_t> backward_index = std::nullopt;
	// Index of last code unit of backward sequence.
	std::optional<size_t> backward_stop = std::nullopt;

	// Saved rule index for the first sequence.
	uint8_t rule = 0;

	// View for the weights for the current pass.
	frg::span<const typename P::UCharType> weights;
	// Index to the weight of the current sequence.
	uint32_t idx = 0;
	// Save looked up index of a forward sequence after the last backward sequence.
	uint32_t save_idx = 0;

	// Beginning of the backward sequence.
	const P::UCharType *backward_us = nullptr;
	// Buffer for reversed indices of a backward sequence block.
	frg::small_vector<int32_t, 32, MemoryAllocator> backward_buffer{getAllocator()};

	int compare(coll_seq &seq2, bool position) {
		auto &seq1 = *this;

		if (position) {
			if (auto cmp = seq1.offset_from_previous <=> seq2.offset_from_previous; cmp != std::strong_ordering::equal)
				return cmp == std::strong_ordering::greater ? 1 : -1;
		}

		auto [it1, it2] = std::ranges::mismatch(seq1.weights, seq2.weights);

		frg::scope_exit updateSeqs([&] {
			seq1.weights = seq1.weights.subspan(std::distance(seq1.weights.begin(), it1));
			seq2.weights = seq2.weights.subspan(std::distance(seq2.weights.begin(), it2));
		});

		if (it1 != seq1.weights.end() && it2 != seq2.weights.end())
			return static_cast<int>(*it1) - static_cast<int>(*it2);

		if (position)
			return static_cast<int>(seq1.weights.size()) - static_cast<int>(seq2.weights.size());

		return 0;
	}

	void pass_reset() {
		weights = {};
		idx = 0;
		forward_index = 0;
		backward_index = std::nullopt;
		backward_stop = std::nullopt;
		// This has little cost as the statically allocated part of the small_vector is preserved.
		// Effectively, we just reset the size.
		backward_buffer.resize(0);
	}

	// Walk the sequence pointed to by the context `ctx` until a sequence with an associated weight
	// is encountered.
	std::optional<frg::span<const typename P::UCharType>> next(const coll_context<Char> &ctx, const size_t pass) {
		__ensure(pass < ctx.nrules);
		offset_from_previous = 0;
		auto cur_us = us;

		while (weights.empty()) {
			offset_from_previous++;

			if (backward_stop) {
				if (backward_index == backward_stop) {
					if (forward_index < highest_index) {
						idx = std::exchange(save_idx, 0);
						backward_stop = std::nullopt;
					} else {
						idx = 0;
						return std::nullopt;
					}
				} else {
					idx = backward_buffer.back();
					backward_buffer.pop_back();
					--(*backward_index);
				}
			} else {
				backward_stop = highest_index;
				auto prev_idx = idx;

				while (*cur_us) {
					const auto [r, next_idx] = ctx.get_index(cur_us);
					prev_idx = std::exchange(idx, next_idx);
					forward_index = highest_index++;

					if (forward_index == 0)
						rule = r;

					if (!(ctx.rulesets[r * ctx.nrules + pass] & coll::sort_backward))
						break;
					forward_index++;
				}

				if (backward_stop >= forward_index) {
					if (forward_index == highest_index || backward_stop > forward_index)
						return std::nullopt;

					backward_stop = std::nullopt;
				} else {
					backward_us = std::exchange(us, cur_us);
					backward_index = forward_index;
					if (highest_index > forward_index) {
						--(*backward_index);
						save_idx = std::exchange(idx, prev_idx);
					}

					if (backward_index > backward_stop)
						--(*backward_index);

					backward_buffer.resize(*backward_index - *backward_stop);
					auto backw_us_ptr = backward_us;
					for (auto &val : backward_buffer)
						val = ctx.get_index(backw_us_ptr).second;
				}
			}

			// Skip over `pass` weights (i.e. the length field + the weight of the length)
			// to get to the desired weight for the pass.
			const auto [idx_off, weight_len] = std::ranges::fold_left(
				std::views::repeat(0, pass),
				std::pair{idx + 1, static_cast<size_t>(ctx.weights[idx])},
				[&ctx](auto acc, auto) {
					auto [i, l] = acc;
					return std::pair{i + l + 1, static_cast<size_t>(ctx.weights[i + l])};
				}
			);

			if (weight_len > 0)
				weights = ctx.weights.subspan(idx_off, weight_len);
		}

		us = cur_us;
		return weights;
	}
};

template <typename Char>
int strcoll(const Char *a, const Char *b, const mlibc::localeinfo *l) {
	using P = CollationPolicy<Char>;

	const auto nrules = l->collate.get(_NL_COLLATE_NRULES).asUint32();
	if (!nrules)
		return frg::generic_strcmp(a, b);

	if (*a == '\0' || *b == '\0')
		return (*a != '\0') - (*b != '\0');

	const auto ctx = mlibc::coll_context<Char>::from_localeinfo(l);

	mlibc::coll_seq<Char> seq1{};
	mlibc::coll_seq<Char> seq2{};
	int result = 0;
	uint8_t rule = 0;

	for (const auto pass : std::views::iota(0u, nrules)) {
		seq1.pass_reset();
		seq2.pass_reset();

		seq1.us = reinterpret_cast<const P::UCharType *>(a);
		seq2.us = reinterpret_cast<const P::UCharType *>(b);

		while (true) {
			const auto w1 = seq1.next(ctx, pass);
			const auto w2 = seq2.next(ctx, pass);

			if (!w1 || !w2) {
				if (auto cmp = static_cast<int>(seq1.weights.size()) - static_cast<int>(seq2.weights.size()); cmp)
					return (cmp > 0) - (cmp < 0); // same as std::clamp(cmp, -1, 1)
				if (pass == 0 && !frg::generic_strcmp(a, b))
					return result;
				break;
			}

			result = seq1.compare(seq2, ctx.rulesets[rule * nrules + pass] & mlibc::coll::sort_position);
			if (result)
				return result;
		}

		rule = seq1.rule;
	}

	return result;
}

template <typename Char>
size_t do_xfrm(
    const typename CollationPolicy<Char>::UCharType *usrc,
    typename CollationPolicy<Char>::StringType *dest,
    const size_t n,
    const coll_context<Char> &ctx
) {
	size_t needed = 0;
	size_t pass_start_needed = 0;

	coll_seq<Char> seq{};

	// Helper to write out data if there is space and to keep track of the `needed` counter.
	auto write = [&](const auto &range) {
		const auto sz = std::ranges::size(range);
		if (needed + sz < n)
			std::ranges::copy(range, dest + needed);
		needed += sz;
	};

	// Encode a uint32_t into a UTF-8 sequence
	auto encode_as_utf8 = [](frg::span<char> buf, uint32_t val) -> frg::span<char> {
		if (val < 0x80) {
			buf[0] = static_cast<char>(val);
			return buf.subspan(0, 1);
		}

		const int bytes = *std::ranges::find_if(std::views::iota(2, 6), [&](int b) {
			return (val >> (5 * b + 1)) == 0;
		});

		std::ranges::for_each(buf.subspan(1, bytes - 1) | std::views::reverse, [&](char &b) {
			b = static_cast<char>(0x80 | (val & 0x3F));
			val >>= 6;
		});

		buf[0] = static_cast<char>((0xFF << (8 - bytes)) | val);
		return buf.subspan(0, bytes);
	};

	for (const auto pass : std::views::iota(0u, ctx.nrules)) {
		pass_start_needed = needed;
		seq.pass_reset();
		seq.us = usrc;

		const typename CollationPolicy<Char>::UCharType *pos_us = usrc;
		const auto [rule, idx] = ctx.get_index(pos_us);
		const bool position = ctx.rulesets[rule * ctx.nrules + pass] & coll::sort_position;

		while (auto w = seq.next(ctx, pass)) {
			if (position) {
				if constexpr (std::is_same_v<Char, char>) {
					char buf[7];
					write(encode_as_utf8(frg::span<char>{buf, 7}, seq.offset_from_previous));
				} else {
					write(frg::span(&seq.offset_from_previous, 1));
				}
			}

			write(*w);
			seq.weights = {};
		}

		// Insert pass separator.
		if (needed < n)
			dest[needed] = (pass + 1 < ctx.nrules) ? '\1' : '\0';
		needed++;
	}

	// glibc trims "\1\0" to just '\0' for the final pass.
	if (needed > 2 && needed == pass_start_needed + 1) {
		if (--needed <= n)
			dest[needed - 1] = '\0';
	}

	// Return `needed` excluding the mandatory null termination.
	return needed - 1;
}

} // namespace mlibc
