import subprocess
import sys
import os
from pyexpect import expect

exe_wrapper = os.environ.get("MESON_EXE_WRAPPER")
wrapper = exe_wrapper.split(" ") if exe_wrapper else []

def check_tz(tz, expected):
	environ = os.environ.copy()
	environ["TZ"] = tz

	output = subprocess.check_output(
		wrapper + sys.argv[1:],
		stderr=subprocess.STDOUT,
		env=environ,
	)

	expect(expected).to_equal(output)

if len(sys.argv) < 2:
	print("Usage: python3 tz.py <path to test program>", file=sys.stderr)
	sys.exit(1)

check_tz("UTC0", b"UTC UTC 0 0\nUTC offset: 0:00\n")
check_tz("GMT0", b"GMT GMT 0 0\nUTC offset: 0:00\n")
check_tz(":UTC0", b"UTC UTC 0 0\nUTC offset: 0:00\n")
check_tz("EST+5", b"EST EST 18000 0\nUTC offset: -5:00\n")
check_tz("EET-2", b"EET EET -7200 0\nUTC offset: 2:00\n")
check_tz("<+03>-03", b"+03 +03 -10800 0\nUTC offset: 3:00\n")
check_tz("NPT-05:45", b"NPT NPT -20700 0\nUTC offset: 5:45\n")
check_tz("NST+03:30NDT", b"NST NDT 12600 1\nUTC offset: -2:30\n")
check_tz("NZST-12NZDT-13,M9.5.0,M4.1.0/3", b"NZST NZDT -43200 1\nUTC offset: 12:00\n")
check_tz("<PST-8>+8<PDT-7>+7", b"PST-8 PDT-7 28800 1\nUTC offset: -7:00\n")
check_tz("<PST-8>+8<PDT-7>+7,M3.2.0,M11.1.0", b"PST-8 PDT-7 28800 1\nUTC offset: -7:00\n")

# These depend on tzdata, which may or may not exist.
# Useful to check every now and then, but these also present subtle differences
# between the host libc and mlibc, so we can't expect these to always hold up.
# E.g. glibc will set `tzname` from the TZ environment variable even if it
# cannot find the tzinfo file, however mlibc will use the name of the timezone
# as read from the tzinfo file or the timezone that it defaults to (UTC).

# Would be b"Universal  0 0\n" on glibc.
# check_tz("", b"UTC UTC 0 0\n")

# Would be b"UTC  0 0\n" on glibc.
# check_tz("UTC", b"UTC UTC 0 0\n")
# check_tz(":UTC", b"UTC UTC 0 0\n")

# Would be b"Universal  0 0\n" on glibc.
# check_tz("Universal", b"UTC UTC 0 0\n")
# check_tz(":Universal", b"UTC UTC 0 0\n")

# Would be b"ATimeZoneThatShouldHopefullyNeverBeInTzdata  0 0\n" on glibc.
# check_tz("ATimeZoneThatShouldHopefullyNeverBeInTzdata", b"UTC UTC 0 0\n")
# check_tz(":ATimeZoneThatShouldHopefullyNeverBeInTzdata", b"UTC UTC 0 0\n")
