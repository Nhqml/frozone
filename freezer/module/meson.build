mkfile = custom_target(
    'freezer_module_makefile',
    output: 'Makefile',
    command: ['touch', '@OUTPUT@'],
    build_by_default: true
)

kernel_version = run_command('uname', '-r').stdout().strip()
kernel_dir = '/lib/modules/' + kernel_version + '/build/'

freezer_module = custom_target(
    'freezer_module',
    input: ['freezer_hook.c', 'freezer_netlink.c', 'Kbuild'],
    output: 'freezer_module.ko',
    command: [
        'make', '-C', kernel_dir,
        'M=' + meson.current_build_dir(),
        'src=' + meson.current_source_dir(),
        'modules'
    ],
    depends: mkfile,
    build_by_default: true
)