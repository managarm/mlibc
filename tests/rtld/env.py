import os
import sys

def main():
    args = sys.argv[1:]
    env = os.environ.copy()

    # parse environment variable assignments
    i = 0
    while i < len(args) and '=' in args[i]:
        var, val = args[i].split('=', 1)
        env[var] = val
        i += 1

    if i < len(args):
        exe_wrapper = os.environ.get('MESON_EXE_WRAPPER')
        if exe_wrapper:
            cmd = [exe_wrapper] + args[i:]
        else:
            cmd = args[i:]
        os.execvpe(cmd[0], cmd, env)
    else:
        print("No command provided to execute.", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
