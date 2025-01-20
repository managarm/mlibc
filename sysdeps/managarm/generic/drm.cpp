#include <drm/drm.h>
#include <drm/drm_fourcc.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>

#include <fs.frigg_bragi.hpp>

namespace mlibc {

int ioctl_drm(int fd, unsigned long request, void *arg, int *result, HelHandle handle) {
	managarm::fs::IoctlRequest<MemoryAllocator> ioctl_req(getSysdepsAllocator());

	switch (request) {
		case DRM_IOCTL_VERSION: {
			auto param = reinterpret_cast<drm_version *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->version_major = resp.drm_version_major();
			param->version_minor = resp.drm_version_minor();
			param->version_patchlevel = resp.drm_version_patchlevel();

			if (param->name)
				memcpy(
				    param->name,
				    resp.drm_driver_name().data(),
				    frg::min(param->name_len, resp.drm_driver_name().size())
				);
			if (param->date)
				memcpy(
				    param->date,
				    resp.drm_driver_date().data(),
				    frg::min(param->date_len, resp.drm_driver_date().size())
				);
			if (param->desc)
				memcpy(
				    param->desc,
				    resp.drm_driver_desc().data(),
				    frg::min(param->desc_len, resp.drm_driver_desc().size())
				);

			param->name_len = resp.drm_driver_name().size();
			param->date_len = resp.drm_driver_date().size();
			param->desc_len = resp.drm_driver_desc().size();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_GET_CAP: {
			auto param = reinterpret_cast<drm_get_cap *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_capability(param->capability);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
				return EINVAL;
			} else {
				__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

				param->value = resp.drm_value();
				*result = resp.result();
				return 0;
			}
		}
		case DRM_IOCTL_SET_CLIENT_CAP: {
			auto param = reinterpret_cast<drm_set_client_cap *>(arg);
			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_SET_CLIENT_CAP(" << param->capability
			                    << ") ignores its value\e[39m" << frg::endlog;

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_capability(param->capability);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
				return EINVAL;
			} else {
				__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

				param->value = resp.drm_value();
				*result = resp.result();
				return 0;
			}
		}
		case DRM_IOCTL_GET_MAGIC: {
			auto param = reinterpret_cast<drm_auth *>(arg);
			mlibc::infoLogger(
			) << "\e[31mmlibc: DRM_IOCTL_GET_MAGIC is not implemented correctly\e[39m"
			  << frg::endlog;
			param->magic = 1;
			*result = 0;
			return 0;
		}
		case DRM_IOCTL_AUTH_MAGIC: {
			mlibc::infoLogger(
			) << "\e[31mmlibc: DRM_IOCTL_AUTH_MAGIC is not implemented correctly\e[39m"
			  << frg::endlog;
			*result = 0;
			return 0;
		}
		case DRM_IOCTL_SET_MASTER: {
			mlibc::infoLogger(
			) << "\e[31mmlibc: DRM_IOCTL_SET_MASTER is not implemented correctly\e[39m"
			  << frg::endlog;
			*result = 0;
			return 0;
		}
		case DRM_IOCTL_DROP_MASTER: {
			mlibc::infoLogger(
			) << "\e[31mmlibc: DRM_IOCTL_DROP_MASTER is not implemented correctly\e[39m"
			  << frg::endlog;
			*result = 0;
			return 0;
		}
		case DRM_IOCTL_MODE_GETRESOURCES: {
			auto param = reinterpret_cast<drm_mode_card_res *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			if (recv_resp.error() == kHelErrDismissed) {
				return EINVAL;
			}

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			for (size_t i = 0; i < resp.drm_fb_ids_size(); i++) {
				if (i >= param->count_fbs)
					break;
				auto dest = reinterpret_cast<uint32_t *>(param->fb_id_ptr);
				dest[i] = resp.drm_fb_ids(i);
			}
			param->count_fbs = resp.drm_fb_ids_size();

			for (size_t i = 0; i < resp.drm_crtc_ids_size(); i++) {
				if (i >= param->count_crtcs)
					break;
				auto dest = reinterpret_cast<uint32_t *>(param->crtc_id_ptr);
				dest[i] = resp.drm_crtc_ids(i);
			}
			param->count_crtcs = resp.drm_crtc_ids_size();

			for (size_t i = 0; i < resp.drm_connector_ids_size(); i++) {
				if (i >= param->count_connectors)
					break;
				auto dest = reinterpret_cast<uint32_t *>(param->connector_id_ptr);
				dest[i] = resp.drm_connector_ids(i);
			}
			param->count_connectors = resp.drm_connector_ids_size();

			for (size_t i = 0; i < resp.drm_encoder_ids_size(); i++) {
				if (i >= param->count_encoders)
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
			auto param = reinterpret_cast<drm_mode_get_connector *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_connector_id(param->connector_id);
			req.set_drm_max_modes(param->count_modes);

			auto [offer, send_ioctl_req, send_req, recv_resp, recv_list] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline(),
			        helix_ng::recvBuffer(
			            reinterpret_cast<void *>(param->modes_ptr),
			            param->count_modes * sizeof(drm_mode_modeinfo)
			        )
			    )
			);

			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			if (recv_resp.error() == kHelErrDismissed)
				return EINVAL;

			HEL_CHECK(recv_resp.error());
			HEL_CHECK(recv_list.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			for (size_t i = 0; i < resp.drm_encoders_size(); i++) {
				if (i >= param->count_encoders)
					continue;
				auto dest = reinterpret_cast<uint32_t *>(param->encoders_ptr);
				dest[i] = resp.drm_encoders(i);
			}

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

			if (param->props_ptr) {
				auto id_ptr = reinterpret_cast<uint32_t *>(param->props_ptr);
				auto val_ptr = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

				for (size_t i = 0;
				     i < frg::min(
				         static_cast<size_t>(param->count_props), resp.drm_obj_property_ids_size()
				     );
				     i++) {
					id_ptr[i] = resp.drm_obj_property_ids(i);
					val_ptr[i] = resp.drm_obj_property_values(i);
				}
			}

			param->count_props = resp.drm_obj_property_ids_size();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_GETPROPERTY: {
			auto param = reinterpret_cast<drm_mode_get_property *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_property_id(param->prop_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() != managarm::fs::Errors::SUCCESS) {
				mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPERTY(" << param->prop_id
				                    << ") error " << (int)resp.error() << "\e[39m" << frg::endlog;
				*result = 0;
				return EINVAL;
			}

			memcpy(param->name, resp.drm_property_name().data(), resp.drm_property_name().size());
			param->count_values = resp.drm_property_vals_size();
			param->flags = resp.drm_property_flags();

			for (size_t i = 0;
			     i < param->count_values && i < resp.drm_property_vals_size() && param->values_ptr;
			     i++) {
				auto dest = reinterpret_cast<uint64_t *>(param->values_ptr);
				dest[i] = resp.drm_property_vals(i);
			}

			__ensure(resp.drm_enum_name_size() == resp.drm_enum_value_size());

			for (size_t i = 0; i < param->count_enum_blobs && i < resp.drm_enum_name_size()
			                   && i < resp.drm_enum_value_size();
			     i++) {
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

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_property_id(param->prop_id);
			req.set_drm_property_value(param->value);
			req.set_drm_obj_id(param->connector_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() != managarm::fs::Errors::SUCCESS) {
				mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_SETPROPERTY(" << param->prop_id
				                    << ") error " << (int)resp.error() << "\e[39m" << frg::endlog;
				*result = 0;
				return EINVAL;
			}

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_GETPROPBLOB: {
			auto param = reinterpret_cast<drm_mode_get_blob *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_blob_id(param->blob_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() != managarm::fs::Errors::SUCCESS) {
				mlibc::infoLogger() << "\e[31mmlibc: DRM_IOCTL_MODE_GETPROPBLOB(" << param->blob_id
				                    << ") error " << (int)resp.error() << "\e[39m" << frg::endlog;
				*result = 0;
				return EINVAL;
			}

			uint8_t *dest = reinterpret_cast<uint8_t *>(param->data);
			for (size_t i = 0; i < resp.drm_property_blob_size(); i++) {
				if (i >= param->length) {
					continue;
				}

				dest[i] = resp.drm_property_blob(i);
			}

			param->length = resp.drm_property_blob_size();

			*result = 0;
			return 0;
		}
		case DRM_IOCTL_MODE_GETPLANE: {
			auto param = reinterpret_cast<drm_mode_get_plane *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_plane_id(param->plane_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->crtc_id = resp.drm_crtc_id();
			param->fb_id = resp.drm_fb_id();
			param->possible_crtcs = resp.drm_possible_crtcs();
			param->gamma_size = resp.drm_gamma_size();

			// FIXME: this should be passed as a buffer with helix, but this has no bounded max
			// size?
			for (size_t i = 0; i < resp.drm_format_type_size(); i++) {
				if (i >= param->count_format_types) {
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

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			frg::string<MemoryAllocator> ser(getSysdepsAllocator());
			req.SerializeToString(&ser);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBuffer(ser.data(), ser.size()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			// FIXME: send this via a helix_ng buffer
			for (size_t i = 0; i < resp.drm_plane_res_size(); i++) {
				if (i >= param->count_planes) {
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
			auto param = reinterpret_cast<drm_mode_get_encoder *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_encoder_id(param->encoder_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
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
			auto param = reinterpret_cast<drm_mode_create_dumb *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_bpp(param->bpp);
			req.set_drm_flags(param->flags);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
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

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_pitch(param->pitch);
			req.set_drm_bpp(param->bpp);
			req.set_drm_depth(param->depth);
			req.set_drm_handle(param->handle);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->fb_id = resp.drm_fb_id();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_GETFB2: {
			auto param = reinterpret_cast<drm_mode_fb_cmd2 *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(DRM_IOCTL_MODE_GETFB2);
			req.set_drm_fb_id(param->fb_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->width = resp.drm_width();
			param->height = resp.drm_height();
			param->pixel_format = resp.pixel_format();
			param->modifier[0] = resp.modifier();
			memcpy(
			    param->handles,
			    resp.drm_handles().data(),
			    sizeof(uint32_t) * resp.drm_handles_size()
			);
			memcpy(
			    param->pitches,
			    resp.drm_pitches().data(),
			    sizeof(uint32_t) * resp.drm_pitches_size()
			);
			memcpy(
			    param->offsets,
			    resp.drm_offsets().data(),
			    sizeof(uint32_t) * resp.drm_offsets_size()
			);

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_ADDFB2: {
			auto param = reinterpret_cast<drm_mode_fb_cmd2 *>(arg);

			__ensure(!param->flags || param->flags == DRM_MODE_FB_MODIFIERS);
			__ensure(!param->modifier[0] || param->modifier[0] == DRM_FORMAT_MOD_INVALID);
			__ensure(!param->offsets[0]);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(DRM_IOCTL_MODE_ADDFB2);

			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_pitch(param->pitches[0]);
			req.set_drm_fourcc(param->pixel_format);
			req.set_drm_handle(param->handles[0]);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->fb_id = resp.drm_fb_id();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_RMFB: {
			auto param = reinterpret_cast<int *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_fb_id(*param);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_MAP_DUMB: {
			auto param = reinterpret_cast<drm_mode_map_dumb *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_handle(param->handle);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->offset = resp.drm_offset();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_GETCRTC: {
			auto param = reinterpret_cast<drm_mode_crtc *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_crtc_id(param->crtc_id);

			auto [offer, send_ioctl_req, send_req, recv_resp, recv_data] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline(),
			        helix_ng::recvBuffer(&param->mode, sizeof(drm_mode_modeinfo))
			    )
			);

			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());
			HEL_CHECK(recv_data.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
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
			auto param = reinterpret_cast<drm_mode_crtc *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			for (size_t i = 0; i < param->count_connectors; i++) {
				auto dest = reinterpret_cast<uint32_t *>(param->set_connectors_ptr);
				req.add_drm_connector_ids(dest[i]);
			}
			req.set_drm_x(param->x);
			req.set_drm_y(param->y);
			req.set_drm_crtc_id(param->crtc_id);
			req.set_drm_fb_id(param->fb_id);
			req.set_drm_mode_valid(param->mode_valid);

			auto [offer, send_ioctl_req, send_req, send_mode, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::sendBuffer(&param->mode, sizeof(drm_mode_modeinfo)),
			        helix_ng::recvInline()
			    )
			);

			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(send_mode.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_OBJ_GETPROPERTIES: {
			auto param = reinterpret_cast<drm_mode_obj_get_properties *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_count_props(param->count_props);
			req.set_drm_obj_id(param->obj_id);
			req.set_drm_obj_type(param->obj_type);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			auto props = reinterpret_cast<uint32_t *>(param->props_ptr);
			auto prop_vals = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

			for (size_t i = 0; i < resp.drm_obj_property_ids_size(); i++) {
				if (i >= param->count_props) {
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

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			__ensure(!(param->flags & ~DRM_MODE_PAGE_FLIP_EVENT));
			req.set_drm_crtc_id(param->crtc_id);
			req.set_drm_fb_id(param->fb_id);
			req.set_drm_cookie(param->user_data);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_DIRTYFB: {
			auto param = reinterpret_cast<drm_mode_fb_dirty_cmd *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_fb_id(param->fb_id);
			req.set_drm_flags(param->flags);
			req.set_drm_color(param->color);
			for (size_t i = 0; i < param->num_clips; i++) {
				auto dest = reinterpret_cast<drm_clip_rect *>(param->clips_ptr);
				managarm::fs::Rect<MemoryAllocator> clip(getSysdepsAllocator());
				clip.set_x1(dest->x1);
				clip.set_y1(dest->y1);
				clip.set_x2(dest->x2);
				clip.set_y2(dest->y2);
				req.add_drm_clips(std::move(clip));
			}

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
				return EINVAL;
			} else {
				__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
				*result = resp.result();
				return 0;
			}
		}
		case DRM_IOCTL_MODE_CURSOR: {
			auto param = reinterpret_cast<drm_mode_cursor *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
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
				mlibc::infoLogger()
				    << "\e[35mmlibc: invalid flags in DRM_IOCTL_MODE_CURSOR\e[39m" << frg::endlog;
				return EINVAL;
			}

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() == managarm::fs::Errors::NO_BACKING_DEVICE) {
				return ENXIO;
			} else if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
				return EINVAL;
			} else {
				*result = resp.result();
				return 0;
			}
		}
		case DRM_IOCTL_MODE_CURSOR2: {
			auto param = reinterpret_cast<drm_mode_cursor2 *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_flags(param->flags);
			req.set_drm_crtc_id(param->crtc_id);
			req.set_drm_x(param->x);
			req.set_drm_y(param->y);
			req.set_drm_width(param->width);
			req.set_drm_height(param->height);
			req.set_drm_handle(param->handle);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			if (resp.error() == managarm::fs::Errors::NO_BACKING_DEVICE) {
				return ENXIO;
			} else if (resp.error() == managarm::fs::Errors::ILLEGAL_ARGUMENT) {
				return EINVAL;
			} else {
				*result = resp.result();
				return 0;
			}
		}
		case DRM_IOCTL_MODE_DESTROY_DUMB: {
			auto param = reinterpret_cast<drm_mode_destroy_dumb *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);

			req.set_drm_handle(param->handle);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_CREATEPROPBLOB: {
			auto param = reinterpret_cast<drm_mode_create_blob *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_blob_size(param->length);

			auto [offer, send_ioctl_req, send_req, blob_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::sendBuffer(reinterpret_cast<void *>(param->data), param->length),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(blob_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->blob_id = resp.drm_blob_id();

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_DESTROYPROPBLOB: {
			auto param = reinterpret_cast<drm_mode_destroy_blob *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_blob_id(param->blob_id);

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_ATOMIC: {
			auto param = reinterpret_cast<drm_mode_atomic *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_flags(param->flags);
			req.set_drm_cookie(param->user_data);

			size_t prop_count = 0;
			auto objs_ptr = reinterpret_cast<uint32_t *>(param->objs_ptr);
			auto count_props_ptr = reinterpret_cast<uint32_t *>(param->count_props_ptr);
			auto props_ptr = reinterpret_cast<uint32_t *>(param->props_ptr);
			auto prop_values_ptr = reinterpret_cast<uint64_t *>(param->prop_values_ptr);

			for (size_t i = 0; i < param->count_objs; i++) {
				/* list of modeobjs and their property count */
				req.add_drm_obj_ids(objs_ptr[i]);
				req.add_drm_prop_counts(count_props_ptr[i]);
				prop_count += count_props_ptr[i];
			}

			for (size_t i = 0; i < prop_count; i++) {
				/* array of property IDs */
				req.add_drm_props(props_ptr[i]);
				/* array of property values */
				req.add_drm_prop_values(prop_values_ptr[i]);
			}

			auto [offer, send_ioctl_req, send_req, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());

			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_MODE_LIST_LESSEES: {
			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_LIST_LESSEES"
			                       " is not implemented correctly\e[39m"
			                    << frg::endlog;
			return EINVAL;
		}
		case DRM_IOCTL_MODE_SETGAMMA: {
			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_SETGAMMA"
			                       " is not implemented correctly\e[39m"
			                    << frg::endlog;
			return 0;
		}
		case DRM_IOCTL_MODE_CREATE_LEASE: {
			auto param = reinterpret_cast<drm_mode_create_lease *>(arg);

			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_MODE_CREATE_LEASE"
			                       " is a noop\e[39m"
			                    << frg::endlog;
			param->lessee_id = 1;
			param->fd = fd;
			*result = 0;
			return 0;
		}
		case DRM_IOCTL_GEM_CLOSE: {
			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_GEM_CLOSE"
			                       " is a noop\e[39m"
			                    << frg::endlog;
			return 0;
		}
		case DRM_IOCTL_WAIT_VBLANK: {
			mlibc::infoLogger() << "\e[35mmlibc: DRM_IOCTL_WAIT_VBLANK"
			                       " is a noop\e[39m"
			                    << frg::endlog;
			return 0;
		}
		case DRM_IOCTL_PRIME_HANDLE_TO_FD: {
			auto param = reinterpret_cast<drm_prime_handle *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_prime_handle(param->handle);
			req.set_drm_flags(param->flags);

			auto [offer, send_ioctl_req, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::imbueCredentials(),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(send_creds.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			__ensure(resp.error() == managarm::fs::Errors::SUCCESS);

			param->fd = resp.drm_prime_fd();
			*result = resp.result();
			return 0;
		}
		case DRM_IOCTL_PRIME_FD_TO_HANDLE: {
			auto param = reinterpret_cast<drm_prime_handle *>(arg);

			managarm::fs::GenericIoctlRequest<MemoryAllocator> req(getSysdepsAllocator());
			req.set_command(request);
			req.set_drm_flags(param->flags);

			auto [offer, send_ioctl_req, send_req, send_creds, recv_resp] = exchangeMsgsSync(
			    handle,
			    helix_ng::offer(
			        helix_ng::sendBragiHeadOnly(ioctl_req, getSysdepsAllocator()),
			        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()),
			        helix_ng::imbueCredentials(getHandleForFd(param->fd)),
			        helix_ng::recvInline()
			    )
			);
			HEL_CHECK(offer.error());
			HEL_CHECK(send_ioctl_req.error());
			HEL_CHECK(send_req.error());
			HEL_CHECK(send_creds.error());
			HEL_CHECK(recv_resp.error());

			managarm::fs::GenericIoctlReply<MemoryAllocator> resp(getSysdepsAllocator());
			resp.ParseFromArray(recv_resp.data(), recv_resp.length());
			if (resp.error() == managarm::fs::Errors::FILE_NOT_FOUND) {
				return EBADF;
			} else {
				__ensure(resp.error() == managarm::fs::Errors::SUCCESS);
			}

			param->handle = resp.drm_prime_handle();
			*result = resp.result();
			return 0;
		}
	}

	mlibc::infoLogger() << "mlibc: Unexpected DRM ioctl with"
	                    << ", number: 0x" << frg::hex_fmt(_IOC_NR(request))
	                    << " (raw request: " << frg::hex_fmt(request) << ")" << frg::endlog;
	__ensure(!"Illegal ioctl request");
	__builtin_unreachable();
}

} // namespace mlibc
