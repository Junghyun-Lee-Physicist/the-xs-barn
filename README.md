# CMS MC Sample Database

Centralized cross-section table and metadata for CMS Monte Carlo samples.  
**Campaign**: RunIISummer20UL17 NanoAODv9 (2017 Ultra-Legacy)

> **Live table**: [https://\<username\>.github.io/cms-sample-db/](https://<username>.github.io/cms-sample-db/)

---

## Repository Structure

```
cms-sample-db/
├── data/
│   └── samples_2017UL.json          ← single source of truth
├── python/
│   ├── sample_loader.py             ← Python library (import해서 사용)
│   └── example.py                   ← Python 사용 예시
├── cpp/
│   ├── include/
│   │   ├── cms/
│   │   │   └── sample_loader.h      ← C++ library (#include해서 사용)
│   │   └── nlohmann/
│   │       └── (json.hpp 여기에)     ← 직접 다운로드
│   ├── src/
│   │   └── example.cpp              ← C++ 사용 예시
│   └── Makefile
├── index.html                        ← GitHub Pages 웹 뷰어
└── README.md
```

### 역할 정리

| 파일 | 역할 | 설명 |
|------|------|------|
| `sample_loader.py` | **라이브러리** | `from sample_loader import SampleDB` 로 import |
| `sample_loader.h` | **라이브러리** | `#include "cms/sample_loader.h"` 로 include |
| `example.py` | 예시 코드 | Python loader 사용법 데모 |
| `example.cpp` | 예시 코드 | C++ loader 사용법 데모 |
| `index.html` | 웹 뷰어 | GitHub Pages로 브라우저에서 테이블 조회 |

---

## Quick Start — Python

```bash
cd python/
python example.py
python example.py --url https://raw.githubusercontent.com/<user>/cms-sample-db/main/data/samples_2017UL.json
```

```python
from sample_loader import SampleDB

# 로컬 파일에서 로드
db = SampleDB.from_json("data/samples_2017UL.json")

# GitHub raw URL에서 직접 fetch (LXPLUS, CONDOR 등 어디서든)
db = SampleDB.from_url(
    "https://raw.githubusercontent.com/<user>/cms-sample-db/main/data/samples_2017UL.json"
)

# 샘플 접근
tt = db["TTTo2L2Nu_TuneCP5_powheg"]
print(tt.cross_section_pb)    # 833900.0
print(tt.das_path)
print(tt.effective_nevents)   # N × (1 − 2f_neg)

# 필터
qcd = db.filter("QCD_HT")
dy  = db.filter_by_process("DYJetsToLL")
```

---

## Quick Start — C++

### 1. nlohmann/json 다운로드

```bash
cd cpp/
wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp \
     -O include/nlohmann/json.hpp
```

### 2. 빌드 & 실행

```bash
make          # bin/example 생성
make run      # 빌드 후 바로 실행
```

또는 수동으로:

```bash
g++ -std=c++17 -O2 -Iinclude src/example.cpp -o bin/example
./bin/example ../data/samples_2017UL.json
```

### 3. 자기 코드에서 사용

```cpp
#include "cms/sample_loader.h"
#include <iostream>

int main() {
    auto db = cms::SampleDB::from_json("path/to/samples_2017UL.json");

    // 단일 샘플 접근
    const auto& s = db["TTTo2L2Nu_TuneCP5_powheg"];
    std::cout << s.cross_section_pb.value() << " pb\n";

    // 패턴 필터
    for (const auto* qcd : db.filter("QCD_HT")) {
        std::cout << qcd->name << "\n";
    }

    // 전체 순회
    for (const auto& [name, info] : db) {
        if (info.cross_section_pb) {
            // ... analysis logic
        }
    }
}
```

컴파일 시 `-Ipath/to/include`만 추가하면 됩니다. 링크(link)할 `.so`나 `.a`는 없습니다 — header-only입니다.

---

## Web Viewer (GitHub Pages)

### 활성화 방법

1. GitHub 레포 **Settings → Pages**
2. Source: **Deploy from a branch → `main` / `/ (root)`**
3. `https://<username>.github.io/cms-sample-db/` 에서 접속

### 기능

- 실시간 검색 (샘플 이름 / DAS path)
- "All / Has xsec / Pending" 필터
- 컬럼 클릭으로 정렬
- 💬 아이콘 hover 시 코멘트 표시

---

## JSON Schema

`data/samples_2017UL.json`의 각 엔트리 구조:

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

| Field              | Type             | Description                                   |
|--------------------|------------------|-----------------------------------------------|
| `das_path`         | `string`         | Full DAS dataset path                         |
| `nevents`          | `int \| null`    | Total generated events                        |
| `nfiles`           | `int \| null`    | Number of files                               |
| `cross_section_pb` | `float \| null`  | Cross section in picobarns                    |
| `accuracy`         | `string \| null` | Calculation order (LO, NLO, NNLO+NNLL, etc.) |
| `frac_neg_weight`  | `float \| null`  | Negative-weight fraction (from XSDB)          |
| `xsec_ref`         | `string \| null` | Source (XSDB, Twiki, paper, …)               |
| `comment`          | `string \| null` | Free-form notes                               |

---

## Tips

- **LXPLUS / CERN firewall**: `export https_proxy=http://lxplus-cloud.cern.ch:8443` 후 URL fetch
- **HTCondor**: JSON 파일을 `transfer_input_files`에 넣거나, job wrapper에서 `wget`으로 fetch
- **Negative weights**: NLO generator (aMC@NLO 등) 사용 시 반드시 `effective_nevents` = N × (1 − 2f) 로 정규화(normalization)
- **Version pinning**: raw URL에 commit SHA를 넣으면 특정 스냅샷(snapshot) 고정 가능  
  `https://raw.githubusercontent.com/<user>/cms-sample-db/<SHA>/data/samples_2017UL.json`
- **새 캠페인 추가**: `samples_2018UL.json` 등 같은 스키마(schema)로 새 JSON 파일 생성. Loader는 아무 경로나 받음
