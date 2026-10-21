# Conference Submission and Publication Guide

## Target Conference Selection & Submission Package
**Project:** Hybrid MPI-OpenMP Based Intelligent Waste Collection Route Optimization  
**Authors:** Nandini Sah (240905552), Prisha Gupta (240905612)  
**Affiliation:** Department of Computer Science & Engineering, Manipal Institute of Technology, Manipal Academy of Higher Education (MAHE)

---

## 1. Selected Target Conferences (Scopus & Web of Science Indexed)

To maximize the probability of acceptance and fulfill the 10-mark submission and acceptance evaluation criteria, we have evaluated three premier peer-reviewed conferences:

### Option A: IEEE International Conference on High Performance Computing, Data, and Analytics (HiPC)
- **Publisher / Indexing:** IEEE Computer Society / IEEE Xplore (Scopus, Web of Science, Ei Compendex).
- **Track:** High-Performance Computing Algorithms, Hybrid Parallel Programming, and Systems.
- **Format:** IEEE Conference 2-column format (`IEEEtran.cls`), strictly 6–8 pages.
- **Review Mechanism:** Double-blind peer review via Microsoft CMT / EasyChair.
- **Why It Fits:** Our dual-layer MPI-OpenMP architectural evaluation, Amdahl scalability benchmarks, and empirical comparison directly align with the core focus of HiPC.

### Option B: International Conference on Parallel Computing Technologies (PaCT / Springer LNCS)
- **Publisher / Indexing:** Springer Nature, Lecture Notes in Computer Science (LNCS) / CCIS (Scopus, DBLP).
- **Track:** Distributed Memory Programming, Hybrid Parallel Computing, Island Models.
- **Format:** Springer LNCS single-column format (`llncs.cls`), 10–12 pages.
- **Review Mechanism:** Single-blind peer review via Springer EquinOCS / EasyChair.
- **Why It Fits:** Strong theoretical and empirical focus on island-model metaheuristics, distributed memory load-balancing, and hybrid CPU architectures.

### Option C: IEEE INDICON / IEEE CONECCT (Flagship IEEE Section Conferences)
- **Publisher / Indexing:** IEEE Xplore, Scopus-indexed.
- **Track:** Smart Cities, Intelligent Transportation & High-Performance Computing.
- **Format:** IEEE standard 2-column template, 6 pages.
- **Review Mechanism:** Microsoft CMT peer review.
- **Why It Fits:** Combines practical smart-city application (IoT waste management) with rigorous parallel computing engineering.

---

## 2. Manuscript Formatting Specifications

Per course guidelines, the manuscript is prepared in **both** official formats:

### 1. Springer Nature Format (LNCS / CCIS)
- **Template Repository:** [Overleaf Springer Conference Proceedings Template](https://www.overleaf.com/latex/templates/springer-conference-proceedings-template)
- **Class File:** `llncs.cls` or `sn-jnl.cls`
- **Structure:**
  - Front matter: `\title`, `\author`, `\institute`, `\email`
  - Structured Abstract (150–250 words)
  - 4–6 Keywords
  - Sections: 1. Introduction, 2. Related Work, 3. Problem Formulation, 4. System Architecture, 5. Parallel Implementation, 6. Experimental Setup, 7. Results & Analysis, 8. Conclusion & Future Work
  - References: Numbered Springer LNCS format via BibTeX (`splncs04.bst`).

### 2. IEEE Conference Format (IEEEtran)
- **Template Repository:** [Overleaf IEEE Conference Template](https://www.overleaf.com/latex/templates/ieee-conference-template/grfzhhncsfqn)
- **Class File:** `IEEEtran.cls` (conference option, two-column)
- **Structure:**
  - Standard IEEE headers, two-column layout
  - Mathematical equations in LaTeX `align` / `equation` environments
  - Captions centered, figures spanning single column or double column (`figure*`)
  - References in IEEE format (`IEEEtran.bst`).

---

## 3. Submission Preparation Checklist

- [x] Verified that all citations are authentic and published in peer-reviewed venues (IEEE, Elsevier, Springer, MDPI).
- [x] Mathematical equations are verified and match the code implementation.
- [x] All experimental figures are rendered at 300+ DPI in vector/raster formats without text clipping.
- [x] Both LaTeX packages (`paper/ieee/` and `paper/springer/`) include all `.tex`, `.bib`, and image files ready to zip and upload to Overleaf or submission portals.
- [x] Code repository structured with CMakeLists.txt and Dockerfile for full submission reproducibility.
- [ ] Final user confirmation before submitting to the chosen conference portal (EasyChair / Microsoft CMT).
