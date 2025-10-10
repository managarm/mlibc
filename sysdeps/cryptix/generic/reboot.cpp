#include <bits/ensure.h>
#include <cryptix/reboot.hpp>
#include <cryptix/syscall.h>
#include <errno.h>
#include <sys/reboot.h>

using namespace cryptix;

RebootCmd cryptix_reboot_cmd(unsigned int what) {
	switch (what) {
		case 0x01234567:
			return RebootCmd::eRestart;
		case 0xCDEF0123:
			return RebootCmd::eHalt;
		case 0x4321FEDC:
			return RebootCmd::ePowerOff;
		case 0xA1B2C3D4:
			return RebootCmd::eRestart2;
		case 0xD000FCE2:
			return RebootCmd::eSuspend;
		case 0x45584543:
			return RebootCmd::eKexec;

		default:
			break;
	}

	return RebootCmd::eUndefined;
}

int reboot(int what) {
	auto cmd = cryptix_reboot_cmd(what);

	auto ret = Syscall(SYS_REBOOT, cmd);
	if (auto e = syscall_error(ret); e) {
		errno = e;
		return -1;
	}

	return 0;
}
