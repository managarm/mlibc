#ifndef _LINUX_BPF_H
#define _LINUX_BPF_H

#include <linux/types.h>

enum bpf_attach_type {
	BPF_CGROUP_INET_INGRESS,
	BPF_CGROUP_INET_EGRESS,
	BPF_CGROUP_INET_SOCK_CREATE,
	BPF_CGROUP_SOCK_OPS,
	BPF_SK_SKB_STREAM_PARSER,
	BPF_SK_SKB_STREAM_VERDICT,
	BPF_CGROUP_DEVICE,
	BPF_SK_MSG_VERDICT,
	BPF_CGROUP_INET4_BIND,
	BPF_CGROUP_INET6_BIND,
	BPF_CGROUP_INET4_CONNECT,
	BPF_CGROUP_INET6_CONNECT,
	BPF_CGROUP_INET4_POST_BIND,
	BPF_CGROUP_INET6_POST_BIND,
	BPF_CGROUP_UDP4_SENDMSG,
	BPF_CGROUP_UDP6_SENDMSG,
	BPF_LIRC_MODE2,
	BPF_FLOW_DISSECTOR,
	BPF_CGROUP_SYSCTL,
	BPF_CGROUP_UDP4_RECVMSG,
	BPF_CGROUP_UDP6_RECVMSG,
	BPF_CGROUP_GETSOCKOPT,
	BPF_CGROUP_SETSOCKOPT,
	BPF_TRACE_RAW_TP,
	BPF_TRACE_FENTRY,
	BPF_TRACE_FEXIT,
	BPF_MODIFY_RETURN,
	BPF_LSM_MAC,
	BPF_TRACE_ITER,
	BPF_CGROUP_INET4_GETPEERNAME,
	BPF_CGROUP_INET6_GETPEERNAME,
	BPF_CGROUP_INET4_GETSOCKNAME,
	BPF_CGROUP_INET6_GETSOCKNAME,
	BPF_XDP_DEVMAP,
	BPF_CGROUP_INET_SOCK_RELEASE,
	BPF_XDP_CPUMAP,
	BPF_SK_LOOKUP,
	BPF_XDP,
	BPF_SK_SKB_VERDICT,
	BPF_SK_REUSEPORT_SELECT,
	BPF_SK_REUSEPORT_SELECT_OR_MIGRATE,
	BPF_PERF_EVENT,
	BPF_TRACE_KPROBE_MULTI,
	BPF_LSM_CGROUP,
	__MAX_BPF_ATTACH_TYPE
};

#define MAX_BPF_ATTACH_TYPE __MAX_BPF_ATTACH_TYPE

union bpf_attr {
	struct {
		uint32_t map_type;
		uint32_t key_size;
		uint32_t value_size;
		uint32_t max_entries;
		uint32_t map_flags;
		uint32_t inner_map_fd;
		uint32_t numa_node;
		char map_name[BPF_OBJ_NAME_LEN];
		uint32_t map_ifindex;
		uint32_t btf_fd;
		uint32_t btf_key_type_id;
		uint32_t btf_value_type_id;
		uint32_t btf_vmlinux_value_type_id;
		uint64_t map_extra;
	};

	struct {
		uint32_t map_fd;
		__aligned_u64 key;
		union {
			__aligned_u64 value;
			__aligned_u64 next_key;
		};
		uint64_t flags;
	};

	struct {
		__aligned_u64 in_batch;;
		__aligned_u64 out_batch;
		__aligned_u64 keys;
		__aligned_u64 values;
		uint32_t count;
		uint32_t map_fd;
		uint64_t elem_flags;
		uint64_t flags;
	} batch;

	struct {
		uint32_t prog_type;
		uint32_t insn_cnt;
		__aligned_u64 insns;
		__aligned_u64 license;
		uint32_t log_level;
		uint32_t log_size;
		__aligned_u64 log_buf;
		uint32_t kern_version;
		uint32_t prog_flags;
		char prog_name[BPF_OBJ_NAME_LEN];
		uint32_t prog_ifindex;
		uint32_t expected_attach_type;
		uint32_t prog_btf_fd;
		uint32_t func_info_rec_size;
		__aligned_u64 func_info;
		uint32_t func_info_cnt;
		uint32_t line_info_rec_size;
		__aligned_u64 line_info;
		uint32_t line_info_cnt;
		uint32_t attach_btf_id;
		union {
			uint32_t attach_prog_fd;
			uint32_t attach_btf_obj_fd;
		};
		uint32_t core_relo_cnt;
		__aligned_u64 fd_array;
		__aligned_u64 core_relos;
		uint32_t core_relo_rec_size;
	};

	struct {
		__aligned_u64 pathname;
		uint32_t bpf_fd;
		uint32_t file_flags;
	};

	struct {
		uint32_t target_fd;
		uint32_t attach_bpf_fd;
		uint32_t attach_type;
		uint32_t attach_flags;
		uint32_t replace_bpf_fd;
	};

	struct {
		uint32_t prog_fd;
		uint32_t retval;
		uint32_t data_size_in;
		uint32_t data_size_out;
		__aligned_u64 data_in;
		__aligned_u64 data_out;
		uint32_t repeat;
		uint32_t duration;
		uint32_t ctx_size_in;
		uint32_t ctx_size_out;
		__aligned_u64 ctx_in;
		__aligned_u64 ctx_out;
		uint32_t flags;
		uint32_t cpu;
		uint32_t batch_size;
	} test;

	struct {
		union {
			uint32_t start_id;
			uint32_t prog_id;
			uint32_t map_id;
			uint32_t btf_id;
			uint32_t link_id;
		};
		uint32_t next_id;
		uint32_t open_flags;
	};

	struct {
		uint32_t bpf_fd;
		uint32_t info_len;
		__aligned_u64 info;
	} info;

	struct {
		uint32_t target_fd;
		uint32_t attach_type;
		uint32_t query_flags;
		uint32_t attach_flags;
		__aligned_u64 prog_ids;
		uint32_t prog_cnt;
		__aligned_u64 prog_attach_flags;
	} query;

	struct {
		uint64_t name;
		uint32_t prog_fd;
	} raw_tracepoint;

	struct {
		__aligned_u64 btf;
		__aligned_u64 btf_log_buf;
		uint32_t btf_size;
		uint32_t btf_log_size;
		uint32_t btf_log_level;
	};

	struct {
		uint32_t pid;
		uint32_t fd;
		uint32_t flags;
		uint32_t buf_len;
		__aligned_u64 buf;
		uint32_t prog_id;
		uint32_t fd_type;
		uint64_t probe_offset;
		uint64_t probe_addr;
	} task_fd_query;

	struct {
		uint32_t prog_fd;
		union {
			uint32_t target_fd;
			uint32_t target_ifindex;
		};
		uint32_t attach_type;
		uint32_t flags;
		union {
			uint32_t target_btf_id;
			struct {
				__aligned_u64 iter_info;
				uint32_t iter_info_len;
			};
			struct {
				uint64_t bpf_cookie;
			} perf_event;
			struct {
				uint32_t flags;
				uint32_t cnt;
				__aligned_u64 syms;
				__aligned_u64 addrs;
				__aligned_u64 cookies;
			} kprobe_multi;
			struct {
				uint32_t target_btf_id;
				uint64_t cookie;
			} tracing;
		};
	} link_create;

	struct {
		uint32_t link_fd;
		uint32_t new_prog_fd;
		uint32_t flags;
		uint32_t old_prog_fd;
	} link_update;

	struct {
		uint32_t link_fd;
	} link_detach;

	struct {
		uint32_t type;
	} enable_stats;

	struct {
		uint32_t link_fd;
		uint32_t flags;
	} iter_create;

	struct {
		uint32_t prog_fd;
		uint32_t map_fd;
		uint32_t flags;
	} prog_bind_map;

} __attribute__((aligned(8)));

#endif // _LINUX_BPF_H
