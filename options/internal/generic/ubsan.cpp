#include <limits.h>
#include <mlibc/debug.hpp>

#define FMT(obj) format_object((obj), opts, formatter)

#define LOG_NAME_LOC(name, loc) "ubsan: " name " at " << loc << "\n  "
#define LOG_LHS_RHS(lhs, rhs) "LHS = " << (lhs) << ", RHS = " << (rhs)

struct SourceLocation {
	const char *filename;
	uint32_t line;
	uint32_t column;
};

template<class F>
void format_object(const SourceLocation &loc, frg::format_options opts, F &formatter) {
	FMT(loc.filename);
	FMT(":");
	FMT(loc.line);
	FMT(":");
	FMT(loc.column);
}

using ValueHandle = uintptr_t;

struct TypeDescriptor {
	enum class Kind : uint16_t {
		Integer = 0x0000,
		Float = 0x0001,
		Unknown = 0xffff
	} kind;

	uint16_t info;
	char name[];

	unsigned bitWidthInt() const {
		return 1 << (info >> 1);
	}

	bool isInlineInt() const {
		if (kind != Kind::Integer)
			return false;

		auto inlineBits = sizeof(ValueHandle) * CHAR_BIT;
		auto valueBits = bitWidthInt();
		return inlineBits <= valueBits;
	}

	bool isSigned() const {
		return info & 1;
	}
};

template<class F>
void format_object(const TypeDescriptor &type, frg::format_options opts, F &formatter) {
	FMT(type.name);
}

struct Value {
	const TypeDescriptor &type;
	ValueHandle val;

	Value(const TypeDescriptor &type, ValueHandle val) : type(type), val(val) {}
};

template<class F>
void format_object(const Value &val, frg::format_options opts, F &formatter) {
	if (val.type.isInlineInt() && val.type.isSigned()) {
		auto signedValue = static_cast<int64_t>(val.val);
		FMT(signedValue);
	} else if (val.type.isInlineInt() && !val.type.isSigned()) {
		auto unsignedValue = static_cast<uint64_t>(val.val);
		FMT(unsignedValue);
	}

	FMT(" (");
	FMT(val.type);
	FMT(")");
}


// --- Hook implementations ---

struct TypeMismatch {
	SourceLocation loc;
	const TypeDescriptor &type;
	unsigned char logAlignment;
	unsigned char kind;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_type_mismatch_v1(TypeMismatch *tm, ValueHandle pointer) {
	// TODO: Make this print more information.
	mlibc::panicLogger()
		<< LOG_NAME_LOC("type mismatch", tm->loc)
		<< "accessed address " << (void *)pointer << " but type "
		<< tm->type << " requires alignment " << (1 << tm->logAlignment)
		<< frg::endlog;
}

struct PointerOverflowData {
	SourceLocation loc;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_pointer_overflow(PointerOverflowData *pod, ValueHandle base, ValueHandle result) {
	(void)base;
	(void)result;
	mlibc::panicLogger()
		<< LOG_NAME_LOC("pointer overflow", pod->loc)
		<< frg::endlog;
}

struct InvalidValueData {
	SourceLocation loc;
	const TypeDescriptor &type;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_load_invalid_value(InvalidValueData *ivd, ValueHandle value) {
	(void)value;
	mlibc::panicLogger()
		<< LOG_NAME_LOC("load of invalid value", ivd->loc)
		<< frg::endlog;
}

struct OverflowData {
	SourceLocation loc;
	const TypeDescriptor &type;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_add_overflow(OverflowData *od, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("add overflowed ", od->loc)
		<< LOG_LHS_RHS(Value(od->type, lhs), Value(od->type, rhs))
		<< frg::endlog;
}

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_sub_overflow(OverflowData *od, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("sub overflowed", od->loc)
		<< LOG_LHS_RHS(Value(od->type, lhs), Value(od->type, rhs))
		<< frg::endlog;
}

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_mul_overflow(OverflowData *od, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("mul overflowed", od->loc)
		<< LOG_LHS_RHS(Value(od->type, lhs), Value(od->type, rhs))
		<< frg::endlog;
}

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_divrem_overflow(OverflowData *od, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("divrem overflowed", od->loc)
		<< LOG_LHS_RHS(Value(od->type, lhs), Value(od->type, rhs))
		<< frg::endlog;
}

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_negate_overflow(OverflowData *od, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("negate overflowed", od->loc)
		<< LOG_LHS_RHS(Value(od->type, lhs), Value(od->type, rhs))
		<< frg::endlog;
}

struct ShiftOutOfBoundsData {
	SourceLocation loc;
	const TypeDescriptor &lhsType;
	const TypeDescriptor &rhsType;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_shift_out_of_bounds(ShiftOutOfBoundsData *soob, ValueHandle lhs, ValueHandle rhs) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("shift out of bounds", soob->loc)
		<< LOG_LHS_RHS(Value(soob->lhsType, lhs), Value(soob->rhsType, rhs))
		<< frg::endlog;
}

struct OutOfBoundsData {
	SourceLocation loc;
	const TypeDescriptor &arrayType;
	const TypeDescriptor &indexType;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_out_of_bounds(OutOfBoundsData *oobd, ValueHandle data) {
	(void)data;
	mlibc::panicLogger()
		<< LOG_NAME_LOC("out of bounds access", oobd->loc)
		<< frg::endlog;
}

struct UnreachableData {
	SourceLocation loc;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_builtin_unreachable(UnreachableData *ubd) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("reached __builtin_unreachable()", ubd->loc)
		<< frg::endlog;
}

struct InvalidBuiltinData {
	SourceLocation loc;
	unsigned char kind;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_invalid_builtin(InvalidBuiltinData *ibd) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("reached invalid builtin", ibd->loc)
		<< frg::endlog;
}

struct VLABoundData {
	SourceLocation loc;
	const TypeDescriptor &type;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_vla_bound_not_positive(VLABoundData *vlabd) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("VLA bound not positive", vlabd->loc)
		<< frg::endlog;
}

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_missing_return(UnreachableData *data) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("reached end of a value-returning function without returning a value", data->loc)
		<< frg::endlog;
}

struct NonNullArgData {
	SourceLocation loc;
	SourceLocation attr_loc;
	int arg_index;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_nonnull_arg(NonNullArgData *data) {
	mlibc::panicLogger()
		<< LOG_NAME_LOC("null pointer passed to non-null argument", data->loc)
		<< "argument " << data->arg_index << " is required to be non-null in "
		<< data->attr_loc << frg::endlog;
}

struct FloatCastOverflowData {
	SourceLocation loc;
	const TypeDescriptor &from_type;
	const TypeDescriptor &to_type;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_float_cast_overflow(FloatCastOverflowData *data, ValueHandle from) {
	(void) from;
	mlibc::panicLogger()
		<< LOG_NAME_LOC("float cast overflow", data->loc)
		<< "from " << data->from_type << " to "
		<< data->to_type << frg::endlog;
}

struct FunctionTypeMismatchData {
	SourceLocation loc;
	const TypeDescriptor &type;
};

extern "C" [[gnu::visibility("hidden")]]
void __ubsan_handle_function_type_mismatch(FunctionTypeMismatchData *data, ValueHandle from) {
	(void) from;
	mlibc::panicLogger()
		<< LOG_NAME_LOC("function type mismatch", data->loc)
		<< frg::endlog;
}
