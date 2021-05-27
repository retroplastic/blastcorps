# blastcorps

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
