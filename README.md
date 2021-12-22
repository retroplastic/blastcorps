# Blast Corps

- This repo is about to contain a full decompilation of Blast Corps `(Japan)`, `(USA)`, `(USA) (Rev 1)` and `(Europe) (En,De)`.
- Naming and documentation of the source code and data structures are in progress.

It uses the following ROMs:

| no-intro                       | Location             | sha1                                       |
| ---                            | ---                  | ---                                        |
| `Blast Corps (USA)`            | `baserom.us.v10.z64` | `185a6ef7ba1adb243278062c81a7d4e119bda58c` |
| `Blast Corps (USA) (Rev 1)`    | `baserom.us.v11.z64` | `483f7161aea39de8b45c9fbc70a2c3883c4dea8c` |
| `Blastdozer (Japan)`           | `baserom.jp.z64`     | `b147fdbeb661c89107c440b00dc4810508f58636` |
| `Blast Corps (Europe) (En,De)` | `baserom.eu.z64`     | `460212600f8b9f0da95219c4c7330f2e626d9a7e` |

This repo does not include all assets necessary for compiling the ROMs.
A prior copy of the game is required to extract the assets.

# Clone the repo

Clone recursivley to initialize the splat submodule.

```
git clone https://github.com/retroplastic/blastcorps.git --recursive
```

If you cloned it without `--recursive`, you can initialize the submodule later.

```
git submodule init
git submodule update
```

# Build

This is a two-stage build; first stage is to extract the compressed section from the ROM, second stage is to extract/compile them.

Place a US Rev 1.0 ROM at the base of this repo.

## Stage 1

**Extract init, hd_code and hd_front_end code from ROM**
```
make extract
```
**Decompress hd_code and hd_front_end .text and .data sections**
```
make decompress
```
**Build ROM**
```
make
```

## Stage 2 (Optional)

**Extract `init` + `hd_code` (TODO: `hd_front_end`):**
```
make -C blastcorps extract
```
**Compile ASM/C**
```
make -C blastcorps
```
**Compress compiled code and replace**
```
make -C blastcorps compress
```
**(re)Build ROM**
```
make
```

## C tools

C tools from queueRAM's `blast_corps_docs`, `sm64tools` and other places.
They can be found in the `tools/src` subdirectory of this repo.

### Build the tools

```
meson build-tools
ninja -C build-tools
```

### Run the tools

```
./build-tools/tools/src/blast_textures
./build-tools/tools/src/gen_level_table
```

# Related

* mkst's [blastcorps](https://github.com/mkst/blastcorps)

  Initial set up of splat and Makefile build this repo is based on.

* queueRAM's [blast_corps_docs](https://github.com/queueRAM/blast_corps_docs)

  The original repository this is based upon. The content can be found in `docs` and `tools`.

* queueRAM's [BlastCorpsEditor](https://github.com/queueRAM/BlastCorpsEditor)

  A C# level editor for blast corps.

* queueRAM's [sm64tools](https://github.com/queueRAM/sm64tools)

  A N64 rom manipulation tool silimar to splat, written in C.

* mkst's [gzip](https://github.com/mkst/gzip) branch

  Backport of the pre-1.5 bug behaviour of gzip to support the rare gzip format.

* ethteck's [splat](https://github.com/ethteck/splat)

  A binary splitting tool, used as subrepository in this project.

* [n64decomp](https://github.com/n64decomp)

  A collection of N64 decompilation projects.
