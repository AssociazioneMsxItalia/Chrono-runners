#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ROM Usage Analyzer
==================
Walks every commit on the current branch (vs a base branch, default: main) in
chronological order, runs a full compilation at each commit, captures ROM
segment sizes, and prints a per-commit delta report + global branch summary.

Results are cached in a JSON file keyed by git commit hash, so subsequent runs
skip the build for any commit that has already been analyzed.

Usage:
    py analyze_rom.py [--base main] [--out rom_analysis]
    py analyze_rom.py --no-cache          # force rebuild of all commits
    py analyze_rom.py --cache-only        # only use cache, skip uncached commits
    py analyze_rom.py --dry-run           # list commits without building
"""

import argparse
import json
import os
import re
import subprocess
import sys
import time
from datetime import datetime

# ── Configuration ────────────────────────────────────────────────────────────

SEGMENT_RE = re.compile(
    r"Seg\[(\d+)\]: Lower=([0-9A-Fa-f]+)h Higher=([0-9A-Fa-f]+)h Size=(\d+)"
)
NUM_SEGMENTS = 8 # ASCII-16


# ── Helpers ──────────────────────────────────────────────────────────────────

def run(cmd: list[str], **kwargs) -> subprocess.CompletedProcess:
    """Run a command, raise on failure."""
    return subprocess.run(cmd, check=True, text=True, **kwargs)


def run_git(cmd: list[str], **kwargs) -> str:
    """Run a git command, return stripped stdout."""
    return run(["git"] + cmd, capture_output=True, **kwargs).stdout.strip()


def parse_args():
    p = argparse.ArgumentParser(description="Analyze ROM usage across git commits")
    p.add_argument("--base", default="main", help="Base branch (default: main)")
    p.add_argument("--out", default="rom_analysis", help="Output directory (default: rom_analysis)")
    p.add_argument("--dry-run", action="store_true", help="Just list commits, don't build")
    p.add_argument("--no-cache", action="store_true", help="Ignore cache, rebuild all commits")
    p.add_argument("--cache-only", action="store_true",
                   help="Only use cached data; skip builds for uncached commits")
    return p.parse_args()


# ── Segment parsing ─────────────────────────────────────────────────────────

def parse_segments(text: str) -> list[dict]:
    """Extract segment data from build output.

    Returns a list of dicts: [{idx, lower, higher, size}, ...]
    """
    segments = []
    for m in SEGMENT_RE.finditer(text):
        segments.append({
            "idx":    int(m.group(1)),
            "lower":  int(m.group(2), 16),
            "higher": int(m.group(3), 16),
            "size":   int(m.group(4)),
        })
    return segments


# ── Cache management ─────────────────────────────────────────────────────────

def load_cache(cache_path: str) -> dict:
    """Load the segment cache from JSON. Returns empty dict if not found."""
    if os.path.isfile(cache_path):
        try:
            with open(cache_path, "r", encoding="utf-8") as f:
                return json.load(f)
        except (json.JSONDecodeError, OSError) as e:
            print(f"WARNING: Could not read cache ({e}), starting fresh.")
    return {}


def save_cache(cache_path: str, cache: dict):
    """Persist the cache to disk."""
    os.makedirs(os.path.dirname(cache_path), exist_ok=True)
    with open(cache_path, "w", encoding="utf-8") as f:
        json.dump(cache, f, indent=2, sort_keys=True)


# ── Main ─────────────────────────────────────────────────────────────────────

def main():
    args = parse_args()

    # Safety: ensure we're in a git repo
    try:
        run_git(["rev-parse", "--git-dir"])
    except subprocess.CalledProcessError:
        print("ERROR: Not inside a git repository.", file=sys.stderr)
        sys.exit(1)

    current_branch = run_git(["branch", "--show-current"])
    if not current_branch:
        print("ERROR: Detached HEAD. Please checkout a branch first.", file=sys.stderr)
        sys.exit(1)

    # Verify base branch exists
    try:
        run_git(["rev-parse", "--verify", args.base])
    except subprocess.CalledProcessError:
        print(f"ERROR: Base branch '{args.base}' not found.", file=sys.stderr)
        sys.exit(1)

    # Warn about uncommitted changes
    status = run_git(["status", "--porcelain"])
    stashed = False
    if status:
        print("WARNING: Uncommitted changes detected. They will be stashed during analysis.")
        if not args.dry_run and not args.cache_only:
            print("         Press Enter to continue, or Ctrl+C to abort.")
            try:
                input()
            except (KeyboardInterrupt, EOFError, OSError):
                print("         (proceeding automatically)")

    commits_raw = run_git([
        "log", "--reverse", "--format=%H\t%s",
        f"{args.base}..HEAD"
    ])

    if not commits_raw:
        print(f"No commits on '{current_branch}' vs '{args.base}'. Nothing to do.")
        return

    commits = []
    for line in commits_raw.splitlines():
        sha, msg = line.split("\t", 1)
        commits.append({"sha": sha, "short": sha[:7], "msg": msg})

    print("=" * 70)
    print("  ROM Usage Analysis")
    print("=" * 70)
    print(f"  Branch : {current_branch}")
    print(f"  Base   : {args.base}")
    print(f"  Commits: {len(commits)}")
    print(f"  Output : {args.out}")
    print()

    # ── Load cache ────────────────────────────────────────────────────────
    cache_path = os.path.join(args.out, "cache.json")
    cache = {} if args.no_cache else load_cache(cache_path)

    cached_count = sum(1 for c in commits if c["sha"] in cache)
    if cached_count and not args.no_cache:
        print(f"  Cache hits: {cached_count}/{len(commits)} (use --no-cache to force rebuild)")
    print()

    for i, c in enumerate(commits, 1):
        marker = " (cached)" if c["sha"] in cache else ""
        print(f"  [{i:2d}] {c['short']}  {c['msg']}{marker}")
    print()

    if args.dry_run:
        print("Dry run — stopping here.")
        return

    # ── Prepare output directories ─────────────────────────────────────────
    os.makedirs(args.out, exist_ok=True)
    logs_dir = os.path.join(args.out, "build_logs")
    os.makedirs(logs_dir, exist_ok=True)
    data_path = os.path.join(args.out, "raw_data.tsv")
    report_path = os.path.join(args.out, "report.txt")

    # Determine build command — use absolute path: Python 3.14 on Windows
    # requires a path separator for CreateProcess to resolve .bat files
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)  # ensure we're always in the right directory

    script = "build.bat" if os.name == "nt" else "build.sh"
    if not os.path.isfile(script):
        print(f"ERROR: Build script '{script}' not found.", file=sys.stderr)
        sys.exit(1)
    build_cmd = [os.path.abspath(script)]

    # Stash local changes (only if we're going to actually build something)
    needs_build = any(c["sha"] not in cache for c in commits)
    if status and needs_build and not args.cache_only:
        run_git(["stash", "push", "-m", "rom_analyzer_autostash"])
        stashed = True

    built = 0
    skipped = 0

    for i, c in enumerate(commits, 1):
        sha = c["sha"]

        # ── Cache hit: reuse ───────────────────────────────────────────
        if sha in cache:
            entry = cache[sha]
            nsegs = len(entry.get("segs", []))
            print(f"[{i}/{len(commits)}] {c['short']}  CACHED ({nsegs} segments, {entry.get('total', 0)} bytes)")
            skipped += 1
            continue

        # ── Cache miss but cache-only mode: skip ───────────────────────
        if args.cache_only:
            print(f"[{i}/{len(commits)}] {c['short']}  SKIP (not cached)")
            skipped += 1
            continue

        # ── Cache miss: build ──────────────────────────────────────────
        print(f"[{i}/{len(commits)}] Building {c['short']} ... ", end="", flush=True)

        # Checkout the commit
        try:
            run_git(["checkout", "-q", sha])
        except subprocess.CalledProcessError:
            print("SKIP (checkout failed)")
            skipped += 1
            continue

        # Run build
        t0 = time.monotonic()
        try:
            result = subprocess.run(
                build_cmd,
                capture_output=True,
                timeout=180  # 3 minutes
            )
        except subprocess.TimeoutExpired:
            print("SKIP (timeout)")
            skipped += 1
            continue
        except Exception as e:
            print(f"SKIP ({e})")
            skipped += 1
            continue

        elapsed = time.monotonic() - t0

        # Save full build output
        log_path = os.path.join(logs_dir, f"{c['short']}.log")
        with open(log_path, "wb") as lf:
            lf.write(b"=== STDOUT ===\n")
            lf.write(result.stdout)
            lf.write(b"\n=== STDERR ===\n")
            lf.write(result.stderr)
            lf.write(f"\n=== BUILD TIME: {elapsed:.1f}s ===\n".encode())

        # Parse output
        build_output = result.stdout.decode("utf-8", errors="replace")
        build_output += result.stderr.decode("utf-8", errors="replace")

        if result.returncode != 0:
            print(f"SKIP (build failed, log: build_logs/{c['short']}.log)")
            skipped += 1
            continue

        segs = parse_segments(build_output)
        if not segs:
            print(f"SKIP (no segment data, log: build_logs/{c['short']}.log)")
            skipped += 1
            continue

        # ── Store in cache ─────────────────────────────────────────────
        sizes = [0] * NUM_SEGMENTS
        for seg in segs:
            idx = seg["idx"]
            if idx < NUM_SEGMENTS:
                sizes[idx] = seg["size"]
        total = sum(sizes)

        cache[sha] = {
            "msg": c["msg"],
            "segs": sizes,
            "total": total,
        }
        save_cache(cache_path, cache)  # persist after every build

        print(f"OK ({len(segs)} segments, {elapsed:.1f}s)")
        built += 1

    # ── Restore repo state ────────────────────────────────────────────────
    if current_branch:
        run_git(["checkout", "-q", current_branch])
    if stashed:
        try:
            run_git(["stash", "pop"])
        except subprocess.CalledProcessError:
            pass

    print(f"\nBuilt: {built}  Cached: {skipped}  Total: {len(commits)}")

    # ── Regenerate raw_data.tsv from cache ─────────────────────────────────
    with open(data_path, "w", encoding="utf-8") as df:
        df.write("commit_sha\tcommit_msg\tseg_idx\tlower\thigher\tsize\n")
        for c in commits:
            sha = c["sha"]
            if sha not in cache:
                continue
            entry = cache[sha]
            for idx, size in enumerate(entry.get("segs", [])):
                if size > 0:
                    df.write(f"{sha}\t{entry['msg']}\t{idx}\t0\t0\t{size}\n")
    print(f"Raw data saved to: {data_path}")

    # ── Generate report ───────────────────────────────────────────────────
    generate_report(cache, commits, report_path, current_branch, args.base)

    print(f"Report saved to:   {report_path}")
    print(f"Cache saved to:    {cache_path}  ({len(cache)} entries)")
    print("Done.")


# ── Report generation ────────────────────────────────────────────────────────

def generate_report(cache: dict, commits: list[dict], report_path: str,
                    branch: str, base: str):
    """Read from cache and produce a human-readable report."""

    # Filter to commits present in cache
    valid = [c for c in commits if c["sha"] in cache]
    if not valid:
        print("No data to report.")
        return

    first = valid[0]
    last = valid[-1]

    first_total = cache[first["sha"]]["total"]
    last_total = cache[last["sha"]]["total"]
    global_delta = last_total - first_total

    lines = []

    # Calculate required width
    header_width = 2 + 8 + 1 + 40
    for s in range(NUM_SEGMENTS):
        header_width += 7
    header_width += 2 + 7 + 2 + 7
    W = max(header_width + 2, 90)

    def w(s):
        lines.append(s)

    w("╔" + "═" * (W - 2) + "╗")
    w("║" + "  ROM Usage Analysis Report".ljust(W - 2) + "║")
    w("╠" + "═" * (W - 2) + "╣")
    w(f"║  {'Branch:':12s} {branch:<{W - 17}s}║")
    w(f"║  {'Base:':12s} {base:<{W - 17}s}║")
    w(f"║  {'Commits:':12s} {len(valid)} of {len(commits)} in cache".ljust(W - 2) + "║")
    w(f"║  {'Date:':12s} {datetime.now().strftime('%Y-%m-%d %H:%M:%S'):<{W - 17}s}║")
    w("╠" + "═" * (W - 2) + "╣")
    w("║  Per-Commit Segment Sizes (bytes)" + " " * (W - 2 - 35) + "║")
    w("╟" + "─" * (W - 2) + "╢")

    # Header row
    header = f"  {'Commit':8s} {'Message':40s}"
    for s in range(NUM_SEGMENTS):
        header += f" {'S'+str(s):>6s}"
    header += f"  {'Total':>7s}  {'Δ Step':>7s}"
    w("║" + header[:W-2].ljust(W-2) + "║")
    w("║" + " " * (W - 2) + "║")

    prev_total = None

    for c in valid:
        entry = cache[c["sha"]]
        sizes = entry.get("segs", [0] * NUM_SEGMENTS)
        t = entry["total"]

        step_delta = t - prev_total if prev_total is not None else 0
        prev_total = t

        row = f"  {c['short']:8s} {entry['msg'][:40]:40s}"
        for s in range(NUM_SEGMENTS):
            sz = sizes[s] if s < len(sizes) else 0
            row += f" {sz:6d}"
        row += f"  {t:7d}  {step_delta:+7d}"
        w("║" + row[:W-2].ljust(W-2) + "║")

    w("╠" + "═" * (W - 2) + "╣")
    w("║  Global Delta (first → last commit)" + " " * (W - 2 - 38) + "║")
    w("╟" + "─" * (W - 2) + "╢")
    w(f"║  First ({first['short']}): {first_total:>6d} bytes".ljust(W - 2) + "║")
    w(f"║  Last  ({last['short']}): {last_total:>6d} bytes".ljust(W - 2) + "║")
    w(f"║  Delta:                {global_delta:+7d} bytes  ({global_delta/1024:+.1f} KB)".ljust(W - 2) + "║")
    w("║" + " " * (W - 2) + "║")
    w("╟" + "─" * (W - 2) + "╢")
    w("║  Per-Segment Delta (first → last)" + " " * (W - 2 - 36) + "║")
    w("╟" + "─" * (W - 2) + "╢")

    first_sizes = cache[first["sha"]].get("segs", [0] * NUM_SEGMENTS)
    last_sizes = cache[last["sha"]].get("segs", [0] * NUM_SEGMENTS)

    for s in range(NUM_SEGMENTS):
        first_sz = first_sizes[s] if s < len(first_sizes) else 0
        last_sz = last_sizes[s] if s < len(last_sizes) else 0
        delta_s = last_sz - first_sz
        w(f"║  Seg[{s}]: {first_sz:6d} → {last_sz:6d}  ({delta_s:+d} bytes)".ljust(W - 2) + "║")

    w("╚" + "═" * (W - 2) + "╝")

    report = "\n".join(lines)

    # Write UTF-8 report to file
    with open(report_path, "w", encoding="utf-8") as f:
        f.write(report + "\n")

    print()
    # Print to console with ASCII fallback for Windows cp1252 terminals
    ascii_report = (report
        .replace("╔", "+").replace("╗", "+").replace("╚", "+").replace("╝", "+")
        .replace("╠", "+").replace("╣", "+").replace("╟", "+").replace("╢", "+")
        .replace("═", "=").replace("─", "-").replace("║", "|")
        .replace("→", "->").replace("Δ", "D"))
    try:
        print(ascii_report)
    except UnicodeEncodeError:
        print(ascii_report.encode("ascii", errors="replace").decode("ascii"))


if __name__ == "__main__":
    main()
