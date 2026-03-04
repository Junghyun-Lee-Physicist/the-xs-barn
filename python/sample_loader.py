"""
CMS Sample Database Loader (Python)

Usage:
    from sample_loader import SampleDB

    db = SampleDB.from_json("data/samples_2017UL.json")
    # or directly from a GitHub raw URL:
    db = SampleDB.from_url("https://raw.githubusercontent.com/<user>/<repo>/main/data/samples_2017UL.json")

    xsec = db["TTTo2L2Nu_TuneCP5_powheg"].cross_section_pb
    path = db["TTTo2L2Nu_TuneCP5_powheg"].das_path
    
    # Filter samples by pattern
    tt_samples = db.filter("TT")
"""

from __future__ import annotations

import json
import urllib.request
from dataclasses import dataclass
from pathlib import Path
from typing import Optional, Iterator


@dataclass(frozen=True, slots=True)
class SampleInfo:
    """Immutable record for a single MC sample."""
    name:             str
    das_path:         str
    nevents:          Optional[int]   = None
    nfiles:           Optional[int]   = None
    cross_section_pb: Optional[float] = None
    accuracy:         Optional[str]   = None
    frac_neg_weight:  Optional[float] = None
    xsec_ref:         Optional[str]   = None
    comment:          Optional[str]   = None

    @property
    def effective_nevents(self) -> Optional[float]:
        """N_eff = N * (1 - 2f), where f is the negative-weight fraction."""
        if self.nevents is None or self.frac_neg_weight is None:
            return None
        return self.nevents * (1.0 - 2.0 * self.frac_neg_weight)


class SampleDB:
    """
    In-memory sample database backed by a JSON file.
    
    Supports dict-like access by sample short name,
    iteration, and simple substring filtering.
    """

    __slots__ = ("_samples",)

    def __init__(self, samples: dict[str, SampleInfo]):
        self._samples = samples

    # ---- constructors ----

    @classmethod
    def from_json(cls, path: str | Path) -> SampleDB:
        """Load from a local JSON file."""
        with open(path, encoding="utf-8") as f:
            raw = json.load(f)
        return cls._build(raw)

    @classmethod
    def from_url(cls, url: str, timeout: int = 30) -> SampleDB:
        """Load from a remote URL (e.g. GitHub raw)."""
        req = urllib.request.Request(url, headers={"User-Agent": "cms-sample-db/1.0"})
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            raw = json.loads(resp.read().decode("utf-8"))
        return cls._build(raw)

    @classmethod
    def _build(cls, raw: dict) -> SampleDB:
        samples = {
            name: SampleInfo(name=name, **fields)
            for name, fields in raw.items()
        }
        return cls(samples)

    # ---- access ----

    def __getitem__(self, name: str) -> SampleInfo:
        return self._samples[name]

    def __contains__(self, name: str) -> bool:
        return name in self._samples

    def __len__(self) -> int:
        return len(self._samples)

    def __iter__(self) -> Iterator[str]:
        return iter(self._samples)

    def get(self, name: str, default: SampleInfo | None = None) -> SampleInfo | None:
        return self._samples.get(name, default)

    def items(self):
        return self._samples.items()

    def values(self):
        return self._samples.values()

    # ---- query helpers ----

    def filter(self, pattern: str) -> list[SampleInfo]:
        """Return samples whose short name contains `pattern`."""
        pat = pattern.lower()
        return [s for s in self._samples.values() if pat in s.name.lower()]

    def filter_by_process(self, *prefixes: str) -> list[SampleInfo]:
        """Return samples whose name starts with any of the given prefixes."""
        return [
            s for s in self._samples.values()
            if any(s.name.startswith(p) for p in prefixes)
        ]

    def summary(self) -> str:
        """Print a quick summary table."""
        filled = sum(1 for s in self._samples.values() if s.cross_section_pb is not None)
        total  = len(self._samples)
        return f"SampleDB: {total} samples ({filled} with xsec, {total - filled} pending)"


# ---- convenience ----

def load(path_or_url: str) -> SampleDB:
    """Auto-detect local path vs URL and load."""
    if path_or_url.startswith(("http://", "https://")):
        return SampleDB.from_url(path_or_url)
    return SampleDB.from_json(path_or_url)
