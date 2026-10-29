#!/usr/bin/env python3
"""Recolor only the silver cap in all 101 frames of the original 125A knob.

Every pixel outside the cap and the complete alpha channel remain untouched.
This reproducible, one-shot transformation does not change the artwork's
size, frame structure, pointer, outer ring, shadows, or highlight geometry.
"""
from hashlib import sha1
from pathlib import Path
import sys

import numpy as np
from PIL import Image

source = Path(sys.argv[1])
original_bytes = source.read_bytes()
original_git_sha = sha1(
    ("blob %d\0" % len(original_bytes)).encode("ascii") + original_bytes
).hexdigest()
if original_git_sha != "f1bc5941851814b19bd7a381aac58d68eb2dc47b":
    raise SystemExit("STOP: original resource changed; no recoloring performed")

original = np.asarray(Image.open(source).convert("RGBA"))
assert original.shape == (101 * 125, 125, 4)
frames = original.reshape((101, 125, 125, 4))
output = frames.copy()

yy, xx = np.mgrid[0:125, 0:125]
radius = np.hypot(xx - 62.0, yy - 62.0)
mask = np.clip((20.65 - radius) / (20.65 - 19.20), 0.0, 1.0)
mask = mask * mask * (3.0 - 2.0 * mask)

# A restrained titanium-blue tint; original gray-level highlight texture stays.
rgb = frames[:, :, :, :3].astype(np.float32)
tint = np.array([0.875, 0.965, 1.105], dtype=np.float32)
result_rgb = rgb + ((rgb * tint) - rgb) * mask[None, :, :, None]
output[:, :, :, :3] = np.clip(np.rint(result_rgb), 0, 255).astype(np.uint8)

assert np.array_equal(output[:, :, :, 3], frames[:, :, :, 3])
assert np.array_equal(output[:, mask == 0], frames[:, mask == 0])
changed = (output[:, :, :, :3] != frames[:, :, :, :3]).any(axis=3)
counts = changed.sum(axis=(1, 2))
assert len(counts) == 101 and counts.min() == counts.max() == 1309
Image.fromarray(output.reshape((12625, 125, 4)), "RGBA").save(source, optimize=True)
print(f"PASS: 101 original frames, {counts[0]} cap pixels recolored/frame; outer ring and pointer unchanged.")
