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

# Define kernel_version, changes if it's a gitlab job
is_gitlab_job = run_command('env').stdout().contains('IS_GITLAB_JOB=1')

if is_gitlab_job
    kernel_version = run_command('echo','$KERNELDIR').stdout().strip()
else
    kernel_version = run_command('uname', '-r').stdout().strip()
endif

kernel_dir = '/lib/modules/' + kernel_version + '/build/'

# ------------------------------------
# Build module
# ------------------------------------

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