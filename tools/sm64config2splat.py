#!/usr/bin/env python3
import sys


def print_usage():
    print("Usage:")
    print("Convert sm64tools yaml config to splat yaml config.")
    print(f"{sys.argv[0]} <in.yaml>")


def blast_get_format(blast_type: int) -> str:
    match blast_type:
        case 1:
            return "rgba16"
        case (2 | 5):
            return "rgba32"
        case (3 | 6):
            return "ia8"
        case 4:
            return "ia16"
        case _:
            return ""


def main():
    if len(sys.argv) != 2:
        print_usage()
        return

    with open(sys.argv[1], "r") as f:
        sm64_yaml = f.read().splitlines()

    range_lines = []
    is_range_line = False
    for line in sm64_yaml:
        stripped = line.strip()
        if line == "ranges:":
            is_range_line = True
        elif stripped and not line.startswith("   "):
            is_range_line = False
        elif stripped and is_range_line and not stripped.startswith("#"):
            range_lines.append(line)

    segments = []
    for line in range_lines:
        if line.startswith("      "):
            is_sub_segment = True
        elif line.startswith("   "):
            is_sub_segment = False
        else:
            raise Exception(f"Unexpected indent in '{line}'")

        if line.endswith(":"):
            assert not is_sub_segment

        split_line = line.\
            split("#")[0].\
            replace(":", "").\
            replace("- [", "").\
            replace("]", "").\
            strip().\
            split(",")

        clean_line = []
        for element in split_line:
            clean_line.append(element.replace('"', "").strip())

        if not is_sub_segment:
            match clean_line[2]:
                case "gzip":
                    segment = {
                        "start": clean_line[0],
                        "end": clean_line[1],
                        "type": "rzip",
                        "name": clean_line[3].replace(".raw", ""),
                        "subsegments": []
                    }
                case "blast":
                    segment = {
                        "start": clean_line[0],
                        "end": clean_line[1],
                        "type": "blast",
                        "blast": int(clean_line[3]),
                        "subsegments": []
                    }
                case _:
                    segment = {
                        "start": clean_line[0],
                        "end": clean_line[1],
                        "type": "bin"
                    }
            segments.append(segment)
        else:
            assert len(clean_line) == 5
            subsegment = {
                "start": clean_line[0],
                "type": clean_line[1].replace("tex.", "") + clean_line[2],
                "width": int(clean_line[3]),
                "height": int(clean_line[4])
            }
            segments[-1]["subsegments"].append(subsegment)

    last_end = ""

    for segment in segments:
        if last_end and last_end != segment["start"]:
            unknown_size = int(segment["start"], 16) - int(last_end, 16)
            print(f"  - [{last_end}] # {unknown_size} bytes")
        last_end = segment["end"]

        if segment["type"] == "rzip":
            if len(segment["subsegments"]) == 0:
                print(f"  - [{segment['start']}, rzip, {segment['name']}]")
            elif len(segment["subsegments"]) == 1:
                t = segment['subsegments'][0]['type']
                w = segment['subsegments'][0]['width']
                h = segment['subsegments'][0]['height']
                seg_str = f"  - [{segment['start']}, rzip, {segment['name']}, {t}, {w}, {h}]"
                print(seg_str)
            elif len(segment["subsegments"]) > 1:
                print(f"  - name:  {segment['name']}")
                print(f"    type:  rzip")
                print(f"    start: {segment['start']}")
                print(f"    subsegments:")
                for subsegment in segment["subsegments"]:
                    s = subsegment['start']
                    t = subsegment['type']
                    w = subsegment['width']
                    h = subsegment['height']
                    print(f"    - [{s}, {t}, {w}, {h}]")
        elif segment["type"] == "blast":
            assert len(segment["subsegments"]) > 0

            if len(segment["subsegments"]) == 1:

                short_address = "%06X" % int(segment['start'], 16)
                file_name = f"{short_address}.blast{segment['blast']}"

                if segment['blast'] == 0:
                    segment_size = int(segment["end"], 16) - int(segment["start"], 16)
                    match segment_size:
                        case 128:
                            print(f"  - [{segment['start']}, bin, {short_address}.lut128]")
                        case 256:
                            print(f"  - [{segment['start']}, bin, {short_address}.lut256]")
                        case _:
                            print(f"  - [{segment['start']}, blast, {file_name}, {segment['blast']}] # {segment_size} bytes")
                else:
                    w = segment['subsegments'][0]['width']
                    h = segment['subsegments'][0]['height']
                    print(f"  - [{segment['start']}, blast, {file_name}, {segment['blast']}, {w}, {h}]")
            elif len(segment["subsegments"]) > 1:
                assert segment["blast"] != 0

                short_address = "%06X" % int(segment['start'], 16)
                file_name = f"{short_address}.blast{segment['blast']}"

                print(f"  - name:  {file_name}")
                print(f"    type:  blast")
                print(f"    start: {segment['start']}")
                print(f"    blast: {segment['blast']}")
                print(f"    subsegments:")
                for subsegment in segment["subsegments"]:
                    assert subsegment['type'] == blast_get_format(segment['blast'])
                    print(f"    - [{subsegment['start']}, {subsegment['width']}, {subsegment['height']}]")
        elif segment["type"] == "bin":
            short_address = "%06X" % int(segment['start'], 16)
            print(f"  - [{segment['start']}, bin, {short_address}.bin]")


main()
