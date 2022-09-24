#include <drm/drm_fourcc.h>
#include <drm/drm.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/cdrom.h>
#include <linux/input.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/ioctl.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>

#include <fs.frigg_bragi.hpp>
#include <posix.frigg_bragi.hpp>

namespace mlibc {

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
//	mlibc::infoLogger() << "mlibc: ioctl with"
//			<< " type: 0x" << frg::hex_fmt(_IOC_TYPE(request))
//			<< ", number: 0x" << frg::hex_fmt(_IOC_NR(request))
//			<< " (raw request: " << frg::hex_fmt(request) << ")"
//			<< " on fd " << fd << frg::endlog;

	SignalGuard sguard;
	auto handle = getHandleForFd(fd);
	if(!handle)
		return EBADF;

	switch(request) {
	case FIONBIO: {
		auto mode = reinterpret_cast<int *>(arg);
		int flags = fcntl(fd, F_GETFL, 0);
		if(*mode) {
		    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
		}else{
		    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
		}
		return 0;
	}
	case FIONREAD: {
		auto argp = reinterpret_cast<int *>(arg);

		auto handle = getHandleForFd(fd);
		if (!handle)
			return EBADF;

		if(!argp)
			return EINVAL;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(FIONREAD);

		auto [offer, send_req, recv_resp] =
		exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*argp = resp.fionread_count();

		return 0;
	}
	case FIOCLEX: {
		managarm::posix::IoctlFioclexRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_fd(fd);

		auto [offer, sendReq, recvResp] = exchangeMsgsSync(
			getPosixLane(),
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(sendReq.error());
		if(recvResp.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recvResp.error());

		managarm::posix::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recvResp.data(), recvResp.length());
		__ensure(resp.error() == managarm::posix::Errors::SUCCESS);
		return 0;
	}
	case DRM_IOCTL_VERSION: {
		auto param = reinterpret_cast<drm_version*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->version_major = resp.drm_version_major();
		param->version_minor = resp.drm_version_minor();
		param->version_patchlevel = resp.drm_version_patchlevel();

		if(param->name)
			memcpy(param->name, resp.drm_driver_name().data(), frg::min(param->name_len,
					resp.drm_driver_name().size()));
		if(param->date)
			memcpy(param->date, resp.drm_driver_date().data(), frg::min(param->date_len,
					resp.drm_driver_date().size()));
		if(param->desc)
			memcpy(param->desc, resp.drm_driver_desc().data(), frg::min(param->desc_len,
					resp.drm_driver_desc().size()));

		param->name_len = resp.drm_driver_name().size();
		param->date_len = resp.drm_driver_date().size();
		param->desc_len = resp.drm_driver_desc().size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_GET_CAP: {
		auto param = reinterpret_cast<drm_get_cap*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_capability(param->capability);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->value = resp.drm_value();
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_SET_CLIENT_CAP: {
		auto param = reinterpret_cast<drm_set_client_cap *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_SET_CLIENT_CAP(" << param->capability << ") ignores its value\e[39m" << frg::endlog;

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_capability(param->capability);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->value = resp.drm_value();
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_GET_MAGIC: {
		auto param = reinterpret_cast<drm_auth *>(arg);
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_GET_MAGIC is not implemented correctly\e[39m"
				<< frg::endlog;
		param->magic = 1;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_AUTH_MAGIC: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_AUTH_MAGIC is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_SET_MASTER: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_SET_MASTER is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_DROP_MASTER: {
		mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_DROP_MASTER is not implemented correctly\e[39m"
				<< frg::endlog;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_GETRESOURCES: {
		auto param = reinterpret_cast<drm_mode_card_res *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		if(recv_resp.error() == kHelErrDismissed) {
			return EINVAL;
		}

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		for(size_t i = 0; i < resp.drm_fb_ids_size(); i++) {
			if(i >= param->count_fbs)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->fb_id_ptr);
			dest[i] = resp.drm_fb_ids(i);
		}
		param->count_fbs = resp.drm_fb_ids_size();

		for(size_t i = 0; i < resp.drm_crtc_ids_size(); i++) {
			if(i >= param->count_crtcs)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->crtc_id_ptr);
			dest[i] = resp.drm_crtc_ids(i);
		}
		param->count_crtcs = resp.drm_crtc_ids_size();

		for(size_t i = 0; i < resp.drm_connector_ids_size(); i++) {
			if(i >= param->count_connectors)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->connector_id_ptr);
			dest[i] = resp.drm_connector_ids(i);
		}
		param->count_connectors = resp.drm_connector_ids_size();

		for(size_t i = 0; i < resp.drm_encoder_ids_size(); i++) {
			if(i >= param->count_encoders)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->encoder_id_ptr);
			dest[i] = resp.drm_encoder_ids(i);
		}
		param->count_encoders = resp.drm_encoder_ids_size();

		param->min_width = resp.drm_min_width();
		param->max_width = resp.drm_max_width();
		param->min_height = resp.drm_min_height();
		param->max_height = resp.drm_max_height();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETCONNECTOR: {
		auto param = reinterpret_cast<drm_mode_get_connector*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_connector_id(param->connector_id);
		req.set_drm_max_modes(param->count_modes);

		auto [offer, send_req, recv_resp, recv_list] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline(),
				helix_ng::recvBuffer(reinterpret_cast<void *>(param->modes_ptr), param->count_modes * sizeof(drm_mode_modeinfo))
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(recv_resp.error() == kHelErrDismissed)
			return EINVAL;

		HEL_CHECK(recv_resp.error());
		HEL_CHECK(recv_list.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		for(size_t i = 0; i < resp.drm_encoders_size(); i++) {
			if(i >= param->count_encoders)
				 continue;
			auto dest = reinterpret_cast<uint32_t *>(param->encoders_ptr);
			dest[i] = resp.drm_encoders(i);
		}

		param->count_props = 0;
		param->encoder_id = resp.drm_encoder_id();
		param->connector_type = resp.drm_connector_type();
		param->connector_type_id = resp.drm_connector_type_id();
		param->connection = resp.drm_connection();
		param->mm_width = resp.drm_mm_width();
		param->mm_height = resp.drm_mm_height();
		param->subpixel = resp.drm_subpixel();
		param->pad = 0;
		param->count_encoders = resp.drm_encoders_size();
		param->count_modes = resp.drm_num_modes();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPROPERTY: {
		auto param = reinterpret_cast<drm_mode_get_property*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_property_id(param->prop_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPERTY(" << param->prop_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		memcpy(param->name, resp.drm_property_name().data(), resp.drm_property_name().size());
		param->count_values = resp.drm_property_vals_size();
		param->flags = resp.drm_property_flags();

		for(size_t i = 0; i < param->count_values && i < resp.drm_property_vals_size() && param->values_ptr; i++) {
			auto dest = reinterpret_cast<uint64_t *>(param->values_ptr);
			dest[i] = resp.drm_property_vals(i);
		}

		__ensure(resp.drm_enum_name_size() == resp.drm_enum_value_size());

		for(size_t i = 0; i < param->count_enum_blobs && i < resp.drm_enum_name_size() && i < resp.drm_enum_value_size(); i++) {
			auto dest = reinterpret_cast<drm_mode_property_enum *>(param->enum_blob_ptr);
			dest[i].value = resp.drm_enum_value(i);
			strncpy(dest[i].name, resp.drm_enum_name(i).data(), DRM_PROP_NAME_LEN);
		}

		param->count_enum_blobs = resp.drm_enum_name_size();

		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_SETPROPERTY: {
		auto param = reinterpret_cast<drm_mode_connector_set_property *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_property_id(param->prop_id);
		req.set_drm_property_value(param->value);
		req.set_drm_obj_id(param->connector_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_SETPROPERTY(" << param->prop_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_get_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_id(param->blob_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() != managarm::fs::Errors::SUCCESS) {
			mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPBLOB(" << param->blob_id << ") error " << (int) resp.error() << "\e[39m"
				<< frg::endlog;
			*result = 0;
			return EINVAL;
		}

		uint8_t *dest = reinterpret_cast<uint8_t *>(param->data);
		for(size_t i = 0; i < resp.drm_property_blob_size(); i++) {
			if(i >= param->length) {
				continue;
			}

			dest[i] = resp.drm_property_blob(i);
		}

		param->length = resp.drm_property_blob_size();

		*result = 0;
		return 0;
	}
	case DRM_IOCTL_MODE_GETPLANE: {
		auto param = reinterpret_cast<drm_mode_get_plane*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_plane_id(param->plane_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->crtc_id = resp.drm_crtc_id();
		param->fb_id = resp.drm_fb_id();
		param->possible_crtcs = resp.drm_possible_crtcs();
		param->gamma_size = resp.drm_gamma_size();

		// FIXME: this should be passed as a buffer with helix, but this has no bounded max size?
		for(size_t i = 0; i < resp.drm_format_type_size(); i+= 4) {
			if(i >= param->count_format_types) {
				break;
			}
			auto dest = reinterpret_cast<uint32_t *>(param->format_type_ptr);
			dest[i] = resp.drm_format_type(i);
		}

		param->count_format_types = resp.drm_format_type_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETPLANERESOURCES: {
		auto param = reinterpret_cast<drm_mode_get_plane_res *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBuffer(ser.data(), ser.size()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		// FIXME: send this via a helix_ng buffer
		for(size_t i = 0; i < resp.drm_plane_res_size(); i++) {
			if(i >= param->count_planes) {
				continue;
			}
			auto dest = reinterpret_cast<uint32_t *>(param->plane_id_ptr);
			dest[i] = resp.drm_plane_res(i);
		}

		param->count_planes = resp.drm_plane_res_size();

		*result = resp.result();

		return 0;
	}
	case DRM_IOCTL_MODE_GETENCODER: {
		auto param = reinterpret_cast<drm_mode_get_encoder*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_encoder_id(param->encoder_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->encoder_type = resp.drm_encoder_type();
		param->crtc_id = resp.drm_crtc_id();
		param->possible_crtcs = resp.drm_possible_crtcs();
		param->possible_clones = resp.drm_possible_clones();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_CREATE_DUMB: {
		auto param = reinterpret_cast<drm_mode_create_dumb*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_bpp(param->bpp);
		req.set_drm_flags(param->flags);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->handle = resp.drm_handle();
		param->pitch = resp.drm_pitch();
		param->size = resp.drm_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ADDFB: {
		auto param = reinterpret_cast<drm_mode_fb_cmd *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitch);
		req.set_drm_bpp(param->bpp);
		req.set_drm_depth(param->depth);
		req.set_drm_handle(param->handle);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fb_id = resp.drm_fb_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ADDFB2: {
		auto param = reinterpret_cast<drm_mode_fb_cmd2 *>(arg);

		if(param->pixel_format != DRM_FORMAT_XRGB8888)
			mlibc::infoLogger() << "mlibc: Unexpected pixel format "
					<< frg::hex_fmt(param->pixel_format) << frg::endlog;
		__ensure(param->pixel_format == DRM_FORMAT_XRGB8888
				|| param->pixel_format == DRM_FORMAT_ARGB8888);
		__ensure(!param->flags || param->flags == DRM_MODE_FB_MODIFIERS);
		__ensure(!param->modifier[0] || param->modifier[0] == DRM_FORMAT_MOD_INVALID);
		__ensure(!param->offsets[0]);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(DRM_IOCTL_MODE_ADDFB);

		req.set_drm_width(param->width);
		req.set_drm_height(param->height);
		req.set_drm_pitch(param->pitches[0]);
		req.set_drm_bpp(32);
		req.set_drm_depth(24);
		req.set_drm_handle(param->handles[0]);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fb_id = resp.drm_fb_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_RMFB: {
		auto param = reinterpret_cast<int *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_fb_id(*param);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_MAP_DUMB: {
		auto param = reinterpret_cast<drm_mode_map_dumb*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_handle(param->handle);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->offset = resp.drm_offset();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_GETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_crtc_id(param->crtc_id);

		auto [offer, send_req, recv_resp, recv_data] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline(),
				helix_ng::recvBuffer(&param->mode, sizeof(drm_mode_modeinfo)))
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());
		HEL_CHECK(recv_data.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fb_id = resp.drm_fb_id();
		param->x = resp.drm_x();
		param->y = resp.drm_y();
		param->gamma_size = resp.drm_gamma_size();
		param->mode_valid = resp.drm_mode_valid();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_SETCRTC: {
		auto param = reinterpret_cast<drm_mode_crtc*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		for(size_t i = 0; i < param->count_connectors; i++) {
			auto dest = reinterpret_cast<uint32_t *>(param->set_connectors_ptr);
			req.add_drm_connector_ids(dest[i]);
		}
		req.set_drm_x(param->x);
		req.set_drm_y(param->y);
		req.set_drm_crtc_id(param->crtc_id);
		req.set_drm_fb_id(param->fb_id);
		req.set_drm_mode_valid(param->mode_valid);

		auto [offer, send_req, send_mode, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::sendBuffer(&param->mode, sizeof(drm_mode_modeinfo)),
				helix_ng::recvInline())
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_mode.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_OBJ_GETPROPERTIES: {
		auto param = reinterpret_cast<drm_mode_obj_get_properties *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_count_props(param->count_props);
		req.set_drm_obj_id(param->obj_id);
		req.set_drm_obj_type(param->obj_type);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		auto props = reinterpret_cast<uint32_t *>(param->props_ptr);
		auto prop_vals = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

		for(size_t i = 0; i < resp.drm_obj_property_ids_size(); i++) {
			if(i >= param->count_props) {
				break;
			}
			props[i] = resp.drm_obj_property_ids(i);
			prop_vals[i] = resp.drm_obj_property_values(i);
		}

		param->count_props = resp.drm_obj_property_ids_size();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_PAGE_FLIP: {
		auto param = reinterpret_cast<drm_mode_crtc_page_flip *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		__ensure(!(param->flags & ~DRM_MODE_PAGE_FLIP_EVENT));
		req.set_drm_crtc_id(param->crtc_id);
		req.set_drm_fb_id(param->fb_id);
		req.set_drm_cookie(param->user_data);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_DIRTYFB: {
		auto param = reinterpret_cast<drm_mode_fb_dirty_cmd*>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_fb_id(param->fb_id);
		req.set_drm_flags(param->flags);
		req.set_drm_color(param->color);
		for(size_t i = 0; i < param->num_clips; i++) {
			auto dest = reinterpret_cast<drm_clip_rect *>(param->clips_ptr);
			managarm::fs::Rect<MemoryAllocator> clip(getSysdepsAllocator());
			clip.set_x1(dest->x1);
			clip.set_y1(dest->y1);
			clip.set_x2(dest->x2);
			clip.set_y2(dest->y2);
			req.add_drm_clips(std::move(clip));
		}

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_MODE_CURSOR: {
		auto param = reinterpret_cast<drm_mode_cursor *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_flags(param->flags);
		req.set_drm_crtc_id(param->crtc_id);

		if (param->flags == DRM_MODE_CURSOR_MOVE) {
			req.set_drm_x(param->x);
			req.set_drm_y(param->y);
		} else if (param->flags == DRM_MODE_CURSOR_BO) {
			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_handle(param->handle);
		} else {
			mlibc::infoLogger() << "\e[35mmlibc: invalid flags in DRM_IOCTL_MODE_CURSOR\e[39m" << frg::endlog;
			return EINVAL;
		}

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		if (resp.error() == managarm::fs::Errors::NO_BACKING_DEVICE) {
			return ENXIO;
		}else if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else{
			*result = resp.result();
			return 0;
		}
	}
	case DRM_IOCTL_MODE_DESTROY_DUMB: {
		auto param = reinterpret_cast<drm_mode_destroy_dumb *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		req.set_drm_handle(param->handle);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_CREATEPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_create_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_size(param->length);

		auto [offer, send_req, blob_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::sendBuffer(reinterpret_cast<void *>(param->data), param->length),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(blob_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->blob_id = resp.drm_blob_id();

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_DESTROYPROPBLOB: {
		auto param = reinterpret_cast<drm_mode_destroy_blob *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_blob_id(param->blob_id);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_ATOMIC: {
		auto param = reinterpret_cast<drm_mode_atomic *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_flags(param->flags);
		req.set_drm_cookie(param->user_data);

		size_t prop_count = 0;
		auto objs_ptr = reinterpret_cast<uint32_t *>(param->objs_ptr);
		auto count_props_ptr = reinterpret_cast<uint32_t *>(param->count_props_ptr);
		auto props_ptr = reinterpret_cast<uint32_t *>(param->props_ptr);
		auto prop_values_ptr = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

		for(size_t i = 0; i < param->count_objs; i++) {
			/* list of modeobjs and their property count */
			req.add_drm_obj_ids(objs_ptr[i]);
			req.add_drm_prop_counts(count_props_ptr[i]);
			prop_count += count_props_ptr[i];
		}

		for(size_t i = 0; i < prop_count; i++) {
			/* array of property IDs */
			req.add_drm_props(props_ptr[i]);
			/* array of property values */
			req.add_drm_prop_values(prop_values_ptr[i]);
		}

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());

		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_MODE_LIST_LESSEES: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_LIST_LESSEES"
				" is not implemented correctly\e[39m" << frg::endlog;
		return EINVAL;
	}
	case DRM_IOCTL_MODE_SETGAMMA: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_SETGAMMA"
				" is not implemented correctly\e[39m" << frg::endlog;
		return 0;
	}
	case DRM_IOCTL_MODE_CREATE_LEASE: {
		auto param = reinterpret_cast<drm_mode_create_lease *>(arg);

		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_CREATE_LEASE"
				" is a noop\e[39m" << frg::endlog;
		param->lessee_id = 1;
		param->fd = fd;
		*result = 0;
		return 0;
	}
	case DRM_IOCTL_GEM_CLOSE: {
		mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_GEM_CLOSE"
				" is a noop\e[39m" << frg::endlog;
		return 0;
	}
	case DRM_IOCTL_PRIME_HANDLE_TO_FD: {
		auto param = reinterpret_cast<drm_prime_handle *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_prime_handle(param->handle);
		req.set_drm_flags(param->flags);

		auto [offer, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::imbueCredentials(),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->fd = resp.drm_prime_fd();
		*result = resp.result();
		return 0;
	}
	case DRM_IOCTL_PRIME_FD_TO_HANDLE: {
		auto param = reinterpret_cast<drm_prime_handle *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_drm_flags(param->flags);

		auto [offer, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::imbueCredentials(getHandleForFd(param->fd)),
				helix_ng::recvInline())
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(send_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::FILE_NOT_FOUND) {
			return EBADF;
		} else {
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		}

		param->handle = resp.drm_prime_handle();
		*result = resp.result();
		return 0;
	}
	case TCGETS: {
		auto param = reinterpret_cast<struct termios *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, recv_resp, recv_attrs] = exchangeMsgsSync(handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline(),
				helix_ng::recvBuffer(param, sizeof(struct termios))
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(recv_resp.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp.error());
		HEL_CHECK(recv_attrs.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		__ensure(recv_attrs.actualLength() == sizeof(struct termios));
		*result = resp.result();
		return 0;
	}
	case TCSETS: {
		auto param = reinterpret_cast<struct termios *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, send_attrs, recv_resp] = exchangeMsgsSync(handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::sendBuffer(param, sizeof(struct termios)),
				helix_ng::recvInline()
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(send_attrs.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(send_attrs.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCSCTTY: {
		HelAction actions[4];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionImbueCredentials;
		actions[2].handle = kHelThisThread;
		actions[2].flags = kHelItemChain;
		actions[3].type = kHelActionRecvInline;
		actions[3].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 4,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto imbue_creds = parseSimple(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		if(imbue_creds->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds->error);
		HEL_CHECK(send_req->error);
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}else if(resp.error() == managarm::fs::Errors::INSUFFICIENT_PERMISSIONS) {
			return EPERM;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCGWINSZ: {
		auto param = reinterpret_cast<struct winsize *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(handle,
			helix_ng::offer(
				helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
				helix_ng::recvInline()
			)
		);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(recv_resp.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::ILLEGAL_OPERATION_TARGET)
			return EINVAL;
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		param->ws_col = resp.pts_width();
		param->ws_row = resp.pts_height();
		param->ws_xpixel = resp.pts_pixel_width();
		param->ws_ypixel = resp.pts_pixel_height();
		return 0;
	}
	case TIOCSWINSZ: {
		auto param = reinterpret_cast<const struct winsize *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_pts_width(param->ws_col);
		req.set_pts_height(param->ws_row);
		req.set_pts_pixel_width(param->ws_xpixel);
		req.set_pts_pixel_height(param->ws_ypixel);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionRecvInline;
		actions[2].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		*result = resp.result();
		return 0;
	}
	case TIOCGPTN: {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionRecvInline;
		actions[2].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*param = resp.pts_index();
		*result = resp.result();
		return 0;
	}
	case TIOCGPGRP: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		*static_cast<int *>(arg) = resp.pid();
		return 0;
	}
	case TIOCSPGRP: {
		auto param = reinterpret_cast<int *>(arg);

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_pgid((long int)param);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::INSUFFICIENT_PERMISSIONS) {
			return EPERM;
		} else if(resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
			return EINVAL;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	case TIOCGSID: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, imbue_creds, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
							helix_ng::sendBuffer(ser.data(), ser.size()),
							helix_ng::imbueCredentials(),
							helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		if(imbue_creds.error() == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(imbue_creds.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		*static_cast<int *>(arg) = resp.pid();
		return 0;
	}
	case CDROM_GET_CAPABILITY: {
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);

		auto [offer, send_req, recv_resp] =
			exchangeMsgsSync(
					handle,
					helix_ng::offer(
						helix_ng::sendBuffer(ser.data(), ser.size()),
						helix_ng::recvInline()
					)
			);

		HEL_CHECK(offer.error());
		if(send_req.error())
			return EINVAL;
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if(resp.error() == managarm::fs::Errors::NOT_A_TERMINAL) {
			return ENOTTY;
		}
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}
	} // end of switch()


	if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGVERSION)) {
		*reinterpret_cast<int *>(arg) = 0x010001; // should be EV_VERSION
		*result = 0;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGID)) {
		memset(arg, 0, sizeof(struct input_id));
		*result = 0;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGNAME(0))) {
		const char *s = "Managarm generic evdev";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPHYS(0))) {
		// Returns the sysfs path of the device.
		const char *s = "input0";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGUNIQ(0))) {
		// Returns a unique ID for the device.
		const char *s = "0";
		auto chunk = frg::min(_IOC_SIZE(request), strlen(s) + 1);
		memcpy(arg, s, chunk);
		*result = chunk;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGPROP(0))) {
		// Returns a bitmask of properties of the device.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGKEY(0))) {
		// Returns the current key state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGLED(0))) {
		// Returns the current LED state.
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOCGSW(0))) {
		auto size = _IOC_SIZE(request);
		memset(arg, 0, size);
		*result = size;
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) >= _IOC_NR(EVIOCGBIT(0, 0))
			&& _IOC_NR(request) <= _IOC_NR(EVIOCGBIT(EV_MAX, 0))) {
		// Returns a bitmask of capabilities of the device.
		// If type is zero, return a mask of supported types.
		// As EV_SYN is zero, this implies that it is impossible
		// to get the mask of supported synthetic events.
		auto type = _IOC_NR(request) - _IOC_NR(EVIOCGBIT(0, 0));
		if(!type) {
			// TODO: Check with the Linux ABI if we have to do this.
			memset(arg, 0, _IOC_SIZE(request));

			HelAction actions[4];
			globalQueue.trim();

			managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(0, 0));
			req.set_size(_IOC_SIZE(request));

			frg::string<MemoryAllocator> ser(getSysdepsAllocator());
			req.SerializeToString(&ser);
			actions[0].type = kHelActionOffer;
			actions[0].flags = kHelItemAncillary;
			actions[1].type = kHelActionSendFromBuffer;
			actions[1].flags = kHelItemChain;
			actions[1].buffer = ser.data();
			actions[1].length = ser.size();
			actions[2].type = kHelActionRecvInline;
			actions[2].flags = kHelItemChain;
			actions[3].type = kHelActionRecvToBuffer;
			actions[3].flags = 0;
			actions[3].buffer = arg;
			actions[3].length = _IOC_SIZE(request);
			HEL_CHECK(helSubmitAsync(handle, actions, 4,
					globalQueue.getQueue(), 0, 0));

			auto element = globalQueue.dequeueSingle();
			auto offer = parseHandle(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			if(recv_resp->error == kHelErrDismissed)
				return EINVAL;
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = recv_data->length;
			return 0;
		}else{
			// TODO: Check with the Linux ABI if we have to do this.
			memset(arg, 0, _IOC_SIZE(request));

			HelAction actions[4];
			globalQueue.trim();

			managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
			req.set_command(EVIOCGBIT(1, 0));
			req.set_input_type(type);
			req.set_size(_IOC_SIZE(request));

			frg::string<MemoryAllocator> ser(getSysdepsAllocator());
			req.SerializeToString(&ser);
			actions[0].type = kHelActionOffer;
			actions[0].flags = kHelItemAncillary;
			actions[1].type = kHelActionSendFromBuffer;
			actions[1].flags = kHelItemChain;
			actions[1].buffer = ser.data();
			actions[1].length = ser.size();
			actions[2].type = kHelActionRecvInline;
			actions[2].flags = kHelItemChain;
			actions[3].type = kHelActionRecvToBuffer;
			actions[3].flags = 0;
			actions[3].buffer = arg;
			actions[3].length = _IOC_SIZE(request);
			HEL_CHECK(helSubmitAsync(handle, actions, 4,
					globalQueue.getQueue(), 0, 0));

			auto element = globalQueue.dequeueSingle();
			auto offer = parseHandle(element);
			auto send_req = parseSimple(element);
			auto recv_resp = parseInline(element);
			auto recv_data = parseLength(element);

			HEL_CHECK(offer->error);
			HEL_CHECK(send_req->error);
			if(recv_resp->error == kHelErrDismissed)
				return EINVAL;
			HEL_CHECK(recv_resp->error);
			HEL_CHECK(recv_data->error);

			managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp->data, recv_resp->length);
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			*result = recv_data->length;
			return 0;
		}
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) == _IOC_NR(EVIOSCLOCKID)) {
		auto param = reinterpret_cast<int *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(request);
		req.set_input_clock(*param);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionRecvInline;
		actions[2].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
		*result = resp.result();
		return 0;
	}else if(_IOC_TYPE(request) == 'E'
			&& _IOC_NR(request) >= _IOC_NR(EVIOCGABS(0))
			&& _IOC_NR(request) <= _IOC_NR(EVIOCGABS(ABS_MAX))) {
		auto param = reinterpret_cast<struct input_absinfo *>(arg);
		HelAction actions[3];
		globalQueue.trim();

		auto type = _IOC_NR(request) - _IOC_NR(EVIOCGABS(0));
		managarm::fs::CntRequest<MemoryAllocator> req(getSysdepsAllocator());
		req.set_req_type(managarm::fs::CntReqType::PT_IOCTL);
		req.set_command(EVIOCGABS(0));
		req.set_input_type(type);

		frg::string<MemoryAllocator> ser(getSysdepsAllocator());
		req.SerializeToString(&ser);
		actions[0].type = kHelActionOffer;
		actions[0].flags = kHelItemAncillary;
		actions[1].type = kHelActionSendFromBuffer;
		actions[1].flags = kHelItemChain;
		actions[1].buffer = ser.data();
		actions[1].length = ser.size();
		actions[2].type = kHelActionRecvInline;
		actions[2].flags = 0;
		HEL_CHECK(helSubmitAsync(handle, actions, 3,
				globalQueue.getQueue(), 0, 0));

		auto element = globalQueue.dequeueSingle();
		auto offer = parseHandle(element);
		auto send_req = parseSimple(element);
		auto recv_resp = parseInline(element);

		HEL_CHECK(offer->error);
		HEL_CHECK(send_req->error);
		if(recv_resp->error == kHelErrDismissed)
			return EINVAL;
		HEL_CHECK(recv_resp->error);

		managarm::fs::SvrResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp->data, recv_resp->length);
		__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

		param->value = resp.input_value();
		param->minimum = resp.input_min();
		param->maximum = resp.input_max();
		param->fuzz = resp.input_fuzz();
		param->flat = resp.input_flat();
		param->resolution = resp.input_resolution();

		*result = resp.result();
		return 0;
	}else if(request == KDSETMODE) {
		auto param = reinterpret_cast<unsigned int *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_SETMODE(" << frg::hex_fmt(param) << ") is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == KDGETMODE) {
		auto param = reinterpret_cast<unsigned int *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_GETMODE is a no-op" << frg::endlog;
		*param = 0;

		*result = 0;
		return 0;
	}else if(request == KDSKBMODE) {
		auto param = reinterpret_cast<long>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: KD_SKBMODE(" << frg::hex_fmt(param) << ") is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_SETMODE) {
		// auto param = reinterpret_cast<struct vt_mode *>(arg);
		mlibc::infoLogger() << "\e[35mmlibc: VT_SETMODE is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_GETSTATE) {
		auto param = reinterpret_cast<struct vt_stat *>(arg);

		param->v_active = 0;
		param->v_signal = 0;
		param->v_state = 0;

		mlibc::infoLogger() << "\e[35mmlibc: VT_GETSTATE is a no-op" << frg::endlog;

		*result = 0;
		return 0;
	}else if(request == VT_ACTIVATE || request == VT_WAITACTIVE) {
		mlibc::infoLogger() << "\e[35mmlibc: VT_ACTIVATE/VT_WAITACTIVE are no-ops" << frg::endlog;
		*result = 0;
		return 0;
	}else if(request == TIOCSPTLCK) {
		mlibc::infoLogger() << "\e[35mmlibc: TIOCSPTLCK is a no-op" << frg::endlog;
		*result = 0;
		return 0;
	}

	mlibc::infoLogger() << "mlibc: Unexpected ioctl with"
			<< " type: 0x" << frg::hex_fmt(_IOC_TYPE(request))
			<< ", number: 0x" << frg::hex_fmt(_IOC_NR(request))
			<< " (raw request: " << frg::hex_fmt(request) << ")" << frg::endlog;
	__ensure(!"Illegal ioctl request");
	__builtin_unreachable();
}

} //namespace mlibc
