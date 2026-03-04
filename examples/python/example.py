#!/usr/bin/env python3
"""
Example: Load the CMS sample database and demonstrate common queries.

Usage:
    python example.py
    python example.py --url https://raw.githubusercontent.com/<user>/cms-sample-db/main/data/samples_2017UL.json
"""

import argparse
from sample_loader import SampleDB

def main():
    parser = argparse.ArgumentParser(description="CMS Sample DB example")
    parser.add_argument("--json", default="../data/samples_2017UL.json",
                        help="Path to the JSON file")
    parser.add_argument("--url", default=None,
                        help="Load from a remote URL instead of local file")
    args = parser.parse_args()

    # ── 1. Load ──────────────────────────────────────────────
    if args.url:
        db = SampleDB.from_url(args.url)
        print(f"Loaded from URL: {args.url}")
    else:
        db = SampleDB.from_json(args.json)
        print(f"Loaded from file: {args.json}")

    print(db.summary())
    print()

    # ── 2. Access a single sample ────────────────────────────
    tt = db["TTTo2L2Nu_TuneCP5_powheg"]
    print("[Single sample access]")
    print(f"  Name    : {tt.name}")
    print(f"  DAS     : {tt.das_path}")
    print(f"  xsec    : {tt.cross_section_pb} pb")
    print(f"  N_evt   : {tt.nevents:,}")
    print(f"  N_eff   : {tt.effective_nevents:,.0f}")
    print(f"  accuracy: {tt.accuracy}")
    if tt.comment:
        print(f"  comment : {tt.comment}")
    print()

    # ── 3. Filter by substring ───────────────────────────────
    dy_samples = db.filter("DYJetsToLL")
    print(f"[Filter: DYJetsToLL] — {len(dy_samples)} matches")
    print(f"  {'Name':<50} {'xsec [pb]':>12} {'N events':>14}")
    print("-" * 78)
    for s in dy_samples:
        xsec = f"{s.cross_section_pb:>12.1f}" if s.cross_section_pb else f"{'(pending)':>12}"
        nev  = f"{s.nevents:>14,}" if s.nevents else f"{'(pending)':>14}"
        print(f"  {s.name:<50} {xsec} {nev}")
    print()

    # ── 4. Filter by process prefix ──────────────────────────
    top_samples = db.filter_by_process("TT", "ST_")
    print(f"[Top samples (TT* + ST_*)] — {len(top_samples)} matches")
    for s in top_samples:
        xsec = f"{s.cross_section_pb:.1f} pb" if s.cross_section_pb else "pending"
        print(f"  {s.name:<55} {xsec}")
    print()

    # ── 5. Safe access ───────────────────────────────────────
    missing = db.get("NonExistentSample")
    print(f"[Safe access] db.get('NonExistentSample') = {missing}")

    # ── 6. Typical analysis loop ─────────────────────────────
    print("\n[Analysis-style loop: compute luminosity weight]")
    target_lumi = 41.5  # fb^-1 for 2017
    for name in ["WJetsToLNu_TuneCP5_amcatnloFXFX", "TTTo2L2Nu_TuneCP5_powheg"]:
        s = db[name]
        if s.cross_section_pb and s.nevents:
            # xsec is in pb, lumi in fb^-1, so multiply lumi by 1000
            neff = s.effective_nevents or s.nevents
            weight = (s.cross_section_pb * target_lumi * 1000) / neff
            print(f"  {name}")
            print(f"    weight = ({s.cross_section_pb} × {target_lumi}×1000) / {neff:.0f} = {weight:.6f}")

if __name__ == "__main__":
    main()
