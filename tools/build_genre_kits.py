#!/usr/bin/env python3
"""125A Drumator 1.5: reproducible CC0 eight-genre sample kit audition build.

Only INPUTS from two commit-pinned, source-recorded CC0 sample collections.
No commercial/proprietary sample instruments, no capture of other plugin output.
This builds an audition *artifact*, NOT a silently released VST3 update.
"""
from __future__ import annotations

import hashlib
import json
import math
import shutil
import subprocess
import sys
import tempfile
import urllib.parse
import urllib.request
import wave
from array import array
from pathlib import Path

SOURCES = {
    "808": ("tidalcycles/sounds-tr808-fischer", "85fbecf1bec32553395625ea659e2a56dfd7c0e1"),
    "rusty": ("sfzinstruments/karoryfer.big-rusty-drums", "f07ce00df34a46b6b08375be56fe116cf15782bc"),
}
ROLE_NAMES = ("BD", "Snare", "CHH", "OHH", "Low_Tom", "High_Tom", "Crash", "Clap")
ROLE_SECONDS = (0.85, 0.7, 0.23, 1.35, 1.00, 0.95, 3.4, 0.7)
ROLE_PEAKS = (0.90, 0.83, 0.64, 0.65, 0.70, 0.70, 0.56, 0.71)

def E(path: str):
    return ("808", path)

def R(path: str):
    return ("rusty", "Samples/" + path + ".flac")

def k(n: int, rr: int = 1):
    return R(f"kick_24/kick/kick/k_vl{n}_rr{rr}")
def sn(n: int, rr: int = 1):
    return R(f"snare_14/center/top/sn_center_vl{n}_rr{rr}")
def rim(n: int, rr: int = 1):
    return R(f"snare_14/rimshot/top/sn_rims_vl{n}_rr{rr}")
def hc(n: int, rr: int = 1):
    return R(f"hihat_14/cl/cl/ht_cl_vl{n}_rr{rr}")
def ho(n: int, rr: int = 1):
    return R(f"hihat_14/open/oh/ht_open_vl{n}_rr{rr}")
def tl(n: int, rr: int = 1):
    return R(f"tom_22/center/cl/t22_vl{n}_rr{rr}")
def th(n: int, rr: int = 1):
    return R(f"tom_14/center/cl/t14_vl{n}_rr{rr}")
def cr(n: int, rr: int = 1):
    return R(f"crash_17/cr/oh/cr_vl{n}_rr{rr}")

# Role order is hardwired to existing BeatCreator (BD/Snare/CHH/OHH/Toms/Crash/Clap).
# Recorded source hits vary by genre, strike intensity and, where possible,
# round-robin. Audio coloring is modest; every kit is made from recordings.
KITS = {
    "01_Industrial_NDH": [
        k(13), rim(5), E("ch8/CH.WAV"), ho(5), tl(6), th(5), cr(4), E("cp8/CP.WAV")],
    "02_EBM": [
        E("bd8/BD2550.WAV"), E("sd8/SD5050.WAV"), E("ch8/CH.WAV"),
        E("oh8/OH25.WAV"), E("lt8/LT50.WAV"), E("ht8/HT50.WAV"),
        E("cy8/CY2550.WAV"), E("cp8/CP.WAV")],
    "03_Techno": [
        E("bd8/BD7550.WAV"), E("sd8/SD5010.WAV"), E("ch8/CH.WAV"),
        E("oh8/OH75.WAV"), E("lt8/LT25.WAV"), E("ht8/HT50.WAV"),
        E("cy8/CY7550.WAV"), E("cp8/CP.WAV")],
    "04_House": [
        E("bd8/BD5010.WAV"), E("sd8/SD2525.WAV"), E("ch8/CH.WAV"),
        E("oh8/OH50.WAV"), E("lt8/LT25.WAV"), E("ht8/HT25.WAV"),
        E("cy8/CY5050.WAV"), E("cp8/CP.WAV")],
    "05_Rock": [
        k(10, 2), sn(8, 2), hc(5, 2), ho(4, 2),
        tl(5, 2), th(5, 2), cr(3, 2), E("cp8/CP.WAV")],
    "06_Metal": [
        k(14, 3), rim(6, 3), hc(6, 3), ho(6, 3),
        tl(7, 3), th(6, 3), cr(5, 3), E("cp8/CP.WAV")],
    "07_Hip_Hop": [
        E("bd8/BD0075.WAV"), E("sd8/SD2550.WAV"), E("ch8/CH.WAV"),
        E("oh8/OH10.WAV"), E("lt8/LT25.WAV"), E("ht8/HT25.WAV"),
        E("cy8/CY0010.WAV"), E("cp8/CP.WAV")],
    "08_Drum_and_Bass": [
        E("bd8/BD2500.WAV"), sn(10, 4), E("ch8/CH.WAV"),
        E("oh8/OH25.WAV"), tl(6, 4), th(6, 4),
        cr(4, 4), E("cp8/CP.WAV")],
}

# No preset-specific destructive distortion. Peaks are controlled per role;
# genre-dependent EQ/compander is limited to gentle accent shaping.
COLOUR = {
    "01_Industrial_NDH": "highpass=f=30,acompressor=threshold=0.20:ratio=2.1:attack=4:release=90",
    "02_EBM": "highpass=f=28,lowpass=f=14500",
    "03_Techno": "highpass=f=28,equalizer=f=85:t=q:w=1:g=2",
    "04_House": "highpass=f=28,equalizer=f=150:t=q:w=1:g=-1",
    "05_Rock": "highpass=f=28",
    "06_Metal": "highpass=f=30,acompressor=threshold=0.17:ratio=2.6:attack=3:release=80",
    "07_Hip_Hop": "highpass=f=23,lowpass=f=13500",
    "08_Drum_and_Bass": "highpass=f=32,acompressor=threshold=0.19:ratio=2.0:attack=3:release=75",
}

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def fetch(provider: str, path: str, cache: Path) -> tuple[Path, str]:
    repo, commit = SOURCES[provider]
    url = f"https://raw.githubusercontent.com/{repo}/{commit}/{urllib.parse.quote(path, safe='/')}"
    suffix = Path(path).suffix.lower()
    destination = cache / f"{hashlib.sha256(url.encode()).hexdigest()}{suffix}"
    if not destination.exists():
        req = urllib.request.Request(url, headers={"User-Agent": "125A-Drumator-CC0-kit-builder/1.0"})
        with urllib.request.urlopen(req, timeout=45) as response:
            data = response.read()
        if not data or len(data) > 12_000_000 or data.startswith(b"version https://git-lfs"):
            raise RuntimeError(f"unavailable source recording or LFS pointer: {provider}/{path}")
        destination.write_bytes(data)
    data = destination.read_bytes()
    if len(data) < 1000:
        raise RuntimeError(f"source file unexpectedly short: {provider}/{path}")
    return destination, sha(data)

def process(source: Path, target: Path, colour: str, role: int) -> tuple[int, float, float]:
    target.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run([
        "ffmpeg", "-hide_banner", "-nostdin", "-loglevel", "error", "-y",
        "-i", str(source), "-t", str(ROLE_SECONDS[role]),
        "-af", colour, "-ar", "44100", "-ac", "1",
        "-c:a", "pcm_s16le", str(target)], check=True)
    with wave.open(str(target), "rb") as f:
        if (f.getnchannels(), f.getframerate(), f.getsampwidth()) != (1, 44100, 2):
            raise RuntimeError(f"unexpected sample encoding: {target}")
        n = f.getnframes()
        samples = array("h")
        samples.frombytes(f.readframes(n))
    if sys.byteorder != "little":
        samples.byteswap()
    if not samples:
        raise RuntimeError(f"empty recording: {target}")
    maximum = max(abs(int(v)) for v in samples)
    if maximum <= 50:
        raise RuntimeError(f"silent recording: {target}")
    gain = ROLE_PEAKS[role] * 32767.0 / maximum
    # Normalize peak without hard clipping; preserve the shape and real transient.
    for i in range(len(samples)):
        v = int(samples[i] * gain)
        # 1ms anti-click attack and a 10ms fade-out.
        fade = min(1.0, i / 44.1, (len(samples) - 1 - i) / 441.0)
        samples[i] = max(-32767, min(32767, round(v * max(0.0, fade))))
    energy = sum(float(v) * v for v in samples) / (32768.0**2 * len(samples))
    peak = max(abs(int(v)) for v in samples) / 32768.0
    rms = math.sqrt(energy)
    if not (0.002 < rms < 0.8 and 0.03 < peak < 0.97):
        raise RuntimeError(f"sample dynamic QA failed: {target}: peak={peak} rms={rms}")
    with wave.open(str(target), "wb") as f:
        f.setnchannels(1)
        f.setframerate(44100)
        f.setsampwidth(2)
        if sys.byteorder != "little":
            samples.byteswap()
        f.writeframes(samples.tobytes())
    return len(samples), round(peak, 4), round(rms, 4)

def build(root: Path):
    if shutil.which("ffmpeg") is None:
        raise RuntimeError("ffmpeg is required")
    root.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="drumator-cc0-") as td:
        cache = Path(td)
        manifest = {"type": "DEVELOPMENT_CANDIDATE_NOT_RELEASED",
                    "format": "44.1kHz PCM16 mono",
                    "sources": {name: {"repo": a, "commit": b, "license": "CC0-1.0"}
                                for name, (a, b) in SOURCES.items()},
                    "kits": {}}
        all_processed = []
        for style, roles in KITS.items():
            if len(roles) != 8:
                raise RuntimeError("eight instrument roles required")
            entries = []
            for i, (provider, path) in enumerate(roles):
                source, source_hash = fetch(provider, path, cache)
                wav = root / style / f"{i + 1:02d}_{ROLE_NAMES[i]}.wav"
                frames, peak, rms = process(source, wav, COLOUR[style], i)
                entries.append({
                    "role": ROLE_NAMES[i], "filename": wav.name,
                    "source": provider, "source_path": path,
                    "source_sha256": source_hash, "wav_sha256": sha(wav.read_bytes()),
                    "frames": frames, "peak": peak, "rms": rms})
                all_processed.append(wav)
            manifest["kits"][style] = entries
            print(f"PASS: {style} (8 recorded drum hits)", flush=True)
        if len(all_processed) != 64 or len({p.parent for p in all_processed}) != 8:
            raise RuntimeError("64 outputs in 8 kits expected")
        if len({sha(p.read_bytes()) for p in all_processed}) < 30:
            raise RuntimeError("too few distinct recorded/audio-processed sounds")
        (root / "SOURCE-MANIFEST.json").write_text(
            json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        (root / "START-HERE.txt").write_text(
            "125A DRUMATOR 1.5: EIGHT GENRE KITS - DEVELOPMENT CANDIDATES\n\n"
            "64 WAVs: 8 kits x 8 roles, 44.1kHz/16-bit/mono.\n"
            "These are recorded, CC0-licensed hits, not the previous C++ synthesizer kit.\n"
            "Sources: Michael Fischer's real TR-808 recordings (TidalCycles CC0)\n"
            "and Karoryfer Big Rusty Drums CC0 (physical acoustic drum recordings).\n"
            "No WAVs are embedded in the plugin by this preview-only workflow.\n"
            "Inspect the per-sample provenance and SHA-256 hashes in SOURCE-MANIFEST.json.\n"
            "No modifications to main, or the user's factory/project kits.\n"
            "Do not advertise these as approved without listening/DAW verification.\n",
            encoding="utf-8")
        print("64 WAVs / 8 kits validated; copyright provenance manifest produced.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: python tools/build_genre_kits.py OUTPUT_DIRECTORY")
    build(Path(sys.argv[1]).resolve())
