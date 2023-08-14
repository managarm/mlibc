import subprocess
import sys
import os
from pyexpect import expect

wrapper = os.getenv("MESON_EXE_WRAPPER")
wrapper = [x for x in (wrapper,) if x]

output = subprocess.check_output(wrapper + [sys.argv[1]], stderr=subprocess.STDOUT)

expect(bytes(sys.argv[1], 'utf-8') + b':error_at_line:5: test: error: Invalid argument (EINVAL)\n').to_equal(output)
