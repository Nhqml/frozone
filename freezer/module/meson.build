# ------------------------------------
# Setup
# ------------------------------------

# Dummy makefile required for the freezer_module make command
mkfile = custom_target(
    'freezer_module_makefile',
    output: 'Makefile',
    command: ['touch', '@OUTPUT@'],
    build_by_default: true
)

meson_get_kversion_sh = find_program('meson_get_kversion.sh', dirs:meson.current_build_dir())

kernel_version = run_command(meson_get_kversion_sh, check: false).stdout().strip()

kernel_dir = '/lib/modules/' + kernel_version + '/build/'

# ------------------------------------
# Build module
# ------------------------------------

freezer_module = custom_target(
    'freezer_module',
    input: ['freezer_hook.c', 'freezer_netlink.c', 'array.c', 'freezer_hook_utils.c', 'Kbuild'],
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