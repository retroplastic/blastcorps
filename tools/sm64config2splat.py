#!/usr/bin/env python3
import pprint
import sys


def print_usage():
    print("Usage:")
    print("Convert sm64tools yaml config to splat yaml config.")
    print(f"{sys.argv[0]} <in.yaml> <out.yaml>")


def main():
    if len(sys.argv) != 3:
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

        # print(is_sub_segment, clean_line)

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
                    segments.append(segment)
                case "blast":
                    segment = {
                        "start": clean_line[0],
                        "end": clean_line[1],
                        "type": "blast",
                        "blast": clean_line[2],
                        "subsegments": []
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

    pprint.pprint(segments)


main()
