import json
import sys

inp = sys.argv[1]
outp = sys.argv[2]

with open(inp, "r", encoding="utf-8") as f:
    data = json.load(f)  # always an array

with open(outp, "w", encoding="utf-8") as f:
    f.write("[\n")
    for i, obj in enumerate(data):
        line = json.dumps(obj, separators=(",", ":"), ensure_ascii=False)
        f.write(line + (",\n" if i + 1 < len(data) else "\n"))
    f.write("]\n")