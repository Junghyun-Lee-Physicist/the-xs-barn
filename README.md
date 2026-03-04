# The Xs Barn

> *Open the barn — CMS MC cross sections, stored and served.*

Centralized cross-section table and metadata for CMS Monte Carlo samples.  
**Campaign**: RunIISummer20UL17 NanoAODv9 (2017 Ultra-Legacy)

**Live table**: https://\<username\>.github.io/the-barn/

---

## Structure

```
the-barn/
├── data/
│   └── samples_2017UL.json        ← single source of truth
│
├── include/                        ← C++ headers (외부에서 -I 로 잡는 곳)
│   ├── cms/
│   │   └── sample_loader.h         ← C++ library (header-only)
│   └── nlohmann/
│       └── (json.hpp)              ← 직접 다운로드
│
├── python/
│   └── sample_loader.py            ← Python library
│
├── examples/
│   ├── cpp/
│   │   ├── basic_usage.cpp
│   │   └── Makefile
│   └── python/
│       └── basic_usage.py
│
├── index.html                      ← GitHub Pages 웹 뷰어
├── LICENSE
└── README.md
```

| 파일 | 역할 |
|------|------|
| `data/*.json` | 데이터 (source of truth) |
| `include/cms/sample_loader.h` | C++ 라이브러리 — `#include "cms/sample_loader.h"` |
| `python/sample_loader.py` | Python 라이브러리 — `from sample_loader import SampleDB` |
| `examples/` | 사용 예시 코드 |
| `index.html` | 브라우저에서 볼 수 있는 테이블 뷰어 |

---

## Quick Start — Python

```python
import sys
sys.path.insert(0, "the-barn/python")
from sample_loader import SampleDB

# 로컬 파일
db = SampleDB.from_json("the-barn/data/samples_2017UL.json")

# 또는 GitHub raw URL (LXPLUS, CONDOR 등 어디서든)
db = SampleDB.from_url(
    "https://raw.githubusercontent.com/<user>/the-barn/main/data/samples_2017UL.json"
)

tt = db["TTTo2L2Nu_TuneCP5_powheg"]
print(tt.cross_section_pb)    # 833900.0
print(tt.effective_nevents)   # N × (1 − 2f_neg)

qcd = db.filter("QCD_HT")
dy  = db.filter_by_process("DYJetsToLL")
```

예시 실행:
```bash
cd examples/python/
python basic_usage.py
```

---

## Quick Start — C++

### 1. nlohmann/json 다운로드

```bash
wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \
     -O include/nlohmann/json.hpp
```

### 2. 예시 빌드 & 실행

```bash
cd examples/cpp/
make run
```

### 3. 자기 프로젝트에서 사용

```cpp
#include "cms/sample_loader.h"

auto db = cms::SampleDB::from_json("path/to/samples_2017UL.json");
const auto& s = db["TTTo2L2Nu_TuneCP5_powheg"];
std::cout << s.cross_section_pb.value() << " pb\n";

for (const auto* qcd : db.filter("QCD_HT")) {
    std::cout << qcd->name << "\n";
}
```

컴파일:
```bash
g++ -std=c++17 -O2 -Ipath/to/the-barn/include your_code.cpp -o your_code
```

Header-only이므로 링크할 `.so`나 `.a`는 없습니다.

---

## Submodule로 가져오기

분석 프로젝트에서 이 레포를 외부 모듈로 사용할 때:

```bash
cd my-analysis/
git submodule add https://github.com/<user>/the-barn.git
git commit -m "Add the-barn as submodule"
```

```
my-analysis/
├── the-barn/          ← submodule
├── src/
│   └── main.cpp       # -Ithe-barn/include
├── python/
│   └── analyze.py     # sys.path.insert(0, "the-barn/python")
└── Makefile           # CXXFLAGS += -Ithe-barn/include
```

---

## Web Viewer (GitHub Pages)

### 활성화

1. GitHub 레포 **Settings → Pages**
2. Source: **Deploy from a branch → `main` / `/ (root)`**
3. https://\<username\>.github.io/the-barn/

### 기능

- 실시간 검색 (샘플 이름 / DAS path)
- All / Has σ / Pending 필터
- 컬럼 클릭 정렬
- 📝 hover 시 코멘트 표시

---

## JSON Schema

```json
"SAMPLE_SHORT_NAME": {
  "das_path":         "/Full/DAS/Path/NANOAODSIM",
  "nevents":          12345678,
  "nfiles":           42,
  "cross_section_pb": 831.76,
  "accuracy":         "NNLO+NNLL",
  "frac_neg_weight":  0.0041,
  "xsec_ref":         "XSDB",
  "comment":          "optional note"
}
```

| Field | Type | Description |
|-------|------|-------------|
| `das_path` | `string` | Full DAS dataset path |
| `nevents` | `int \| null` | Total generated events |
| `nfiles` | `int \| null` | Number of files |
| `cross_section_pb` | `float \| null` | Cross section [pb] |
| `accuracy` | `string \| null` | Calculation order (LO, NLO, NNLO+NNLL, …) |
| `frac_neg_weight` | `float \| null` | Negative-weight fraction (from XSDB) |
| `xsec_ref` | `string \| null` | Source (XSDB, Twiki, paper, …) |
| `comment` | `string \| null` | Free-form notes |

---

## Tips

- **LXPLUS**: `export https_proxy=http://lxplus-cloud.cern.ch:8443` 후 URL fetch
- **HTCondor**: JSON을 `transfer_input_files`에 넣거나 job wrapper에서 `wget`
- **Negative weights**: NLO generator 사용 시 `effective_nevents` = N × (1 − 2f) 로 정규화
- **Version pinning**: raw URL에 commit SHA 사용 → 스냅샷 고정
- **새 캠페인**: `samples_2018UL.json` 등 같은 스키마로 추가. Loader는 아무 경로나 받음

---

## License

[MIT](LICENSE)