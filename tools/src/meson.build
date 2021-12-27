lib = static_library('blast',
  ['blast.c', 'n64graphics.c', 'utils.c'],
)

executable('gen_level_table', ['gen_level_table.c'], link_whole: lib)
executable('gen_splat_yaml', ['gen_splat_yaml.c'], link_whole: lib)
executable('unblast_rom', ['unblast_rom.c'], link_whole: lib)
executable('unblast', ['unblast.c'], link_whole: lib)
