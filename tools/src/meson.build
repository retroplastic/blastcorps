blast_sources = files([
  'blast.h',
  'blast.c',
  'n64graphics.h',
  'n64graphics.c',
  'utils.h',
  'utils.c'
])

stb_includes = include_directories('stb', is_system: true)

lib = static_library('blast', [blast_sources, 'stb.c'],
                     include_directories: [stb_includes])

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

all_sources = [
  blast_sources, gen_level_table_sources, gen_splat_yaml_sources,
  unblast_rom_sources, unblast_sources
]
