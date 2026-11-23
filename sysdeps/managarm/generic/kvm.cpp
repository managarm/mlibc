#include <linux/kvm.h>

#include <bits/errors.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>

#include <fs.frigg_bragi.hpp>

namespace mlibc {

namespace {

template <typename Request>
int kvmRequest(
    HelHandle handle, Request &req, uint64_t *value, bool imbueCreds = false
) CAP_REQUIRES(sysdepAllocatorCapability) {
	managarm::fs::IoctlRequest<SysdepsAllocator> ioctlReq(getSysdepsAllocator());
	managarm::fs::KvmResponse<SysdepsAllocator> resp(getSysdepsAllocator());

	if (imbueCreds) {
		auto [offer, sendIoctlReq, sendReq, sendCreds, recvResp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(ioctlReq, getSysdepsAllocator()),
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
		        helix_ng::imbueCredentials(),
		        helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		if (dismissed(sendIoctlReq, sendReq, sendCreds, recvResp))
			return ENOTTY;
		HEL_CHECK(sendIoctlReq.error());
		HEL_CHECK(sendReq.error());
		HEL_CHECK(sendCreds.error());
		HEL_CHECK(recvResp.error());
		resp.ParseFromArray(recvResp.data(), recvResp.length());
	} else {
		auto [offer, sendIoctlReq, sendReq, recvResp] = exchangeMsgsSync(
		    handle,
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(ioctlReq, getSysdepsAllocator()),
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
		        helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		if (dismissed(sendIoctlReq, sendReq, recvResp))
			return ENOTTY;
		HEL_CHECK(sendIoctlReq.error());
		HEL_CHECK(sendReq.error());
		HEL_CHECK(recvResp.error());
		resp.ParseFromArray(recvResp.data(), recvResp.length());
	}

	*value = resp.value();
	return resp.error() | toErrno;
}

} // namespace

int ioctl_kvm(int fd, unsigned long request, void *arg, int *result, HelHandle handle) CAP_REQUIRES(
    sysdepAllocatorCapability
) {
	(void)fd;

	uint64_t value = 0;
	switch (request) {
		case KVM_GET_API_VERSION: {
			managarm::fs::KvmGetApiVersionRequest<SysdepsAllocator> req(getSysdepsAllocator());
			if (int e = kvmRequest(handle, req, &value); e)
				return e;
			*result = value;
			return 0;
		}
		case KVM_CHECK_EXTENSION: {
			managarm::fs::KvmCheckExtensionRequest<SysdepsAllocator> req(getSysdepsAllocator());
			req.set_extension(reinterpret_cast<uintptr_t>(arg));
			if (int e = kvmRequest(handle, req, &value); e)
				return e;
			*result = value;
			return 0;
		}
		case KVM_CREATE_VM: {
			managarm::fs::KvmCreateVmRequest<SysdepsAllocator> req(getSysdepsAllocator());
			req.set_machine_type(reinterpret_cast<uintptr_t>(arg));
			if (int e = kvmRequest(handle, req, &value, true); e)
				return e;
			*result = value;
			return 0;
		}
	}

	mlibc::infoLogger() << "mlibc: Unexpected KVM ioctl with number: 0x"
	                    << frg::hex_fmt(_IOC_NR(request))
	                    << " (raw request: " << frg::hex_fmt(request) << ")" << frg::endlog;
	return ENOTTY;
}

} // namespace mlibc
