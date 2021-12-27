blast_sources = files([
  'blast.c', 'n64graphics.c', 'utils.c'
])
lib = static_library('blast', blast_sources)

gen_level_table_sources = files([
  'gen_level_table.c'
])
executable('gen_level_table', gen_level_table_sources, link_whole: lib)

gen_splat_yaml_sources = files([
  'gen_splat_yaml.c'
])
executable('gen_splat_yaml', gen_splat_yaml_sources, link_whole: lib)

unblast_rom_sources = files([
  'unblast_rom.c'
])
executable('unblast_rom', unblast_rom_sources, link_whole: lib)

unblast_sources = files([
  'unblast.c'
])
executable('unblast', unblast_sources, link_whole: lib)
