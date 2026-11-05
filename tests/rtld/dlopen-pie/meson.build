pie = executable('pie', 'pie.c',
	pie: true,
	override_options: ['b_sanitize=none'],
	link_args: ['-nostartfiles', '-nostdlib'],
)
test_depends = [pie]

pie_native = executable('native-pie', 'pie.c',
	pie: true,
	link_args: ['-nostartfiles', '-nostdlib'],
	native: true,
)
test_native_depends = [pie_native]
