project('blastcorps', 'c',
        version : '0.1',
        default_options : [
          'c_std=c2x',
          'warning_level=3',
          'buildtype=release',
        ],
)

subdir('tools/src')

run_target('format', command : ['clang-format', '-i', all_sources])

run_target('tidy', command : ['clang-tidy', '-p=' + meson.build_root(),
                              all_sources])

