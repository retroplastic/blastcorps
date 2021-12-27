project('blastcorps', 'c',
        version : '0.1',
        default_options : [
          'c_std=c2x',
          'warning_level=3',
          'buildtype=release',
        ],
)

subdir('tools/src')

run_target('format', command : ['clang-format', '-i',
                                blast_sources, gen_level_table_sources,
                                gen_splat_yaml_sources, unblast_rom_sources,
                                unblast_sources])
