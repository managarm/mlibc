import subprocess
import sys
from pyexpect import expect

output = subprocess.check_output([sys.argv[1]], stderr=subprocess.STDOUT)

expect(bytes(sys.argv[1], 'utf-8') + b':error_at_line:5: test: error: Invalid argument (EINVAL)\n').to_equal(output)
