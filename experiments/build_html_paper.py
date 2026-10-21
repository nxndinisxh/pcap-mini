"""
Compile research_paper.md to an academic, publication-styled standalone HTML document.
Optimized for 6 to 8 pages print length (Ctrl+P -> Save as PDF) with zero overflowing tables.
"""

import mistune
import os

base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
md_file = os.path.join(base_dir, "paper", "research_paper.md")
html_file = os.path.join(base_dir, "paper", "research_paper.html")

with open(md_file, "r", encoding="utf-8") as f:
    md_content = f.read()

# Render Markdown to HTML using mistune
html_body = mistune.html(md_content)

template = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Hybrid MPI-OpenMP Based Intelligent Waste Collection Route Optimization</title>
<script>
MathJax = {
  tex: {
    inlineMath: [['$', '$'], ['\\\\(', '\\\\)']],
    displayMath: [['$$', '$$'], ['\\\\[', '\\\\]']]
  }
};
</script>
<script id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"></script>
<style>
    @page {
        size: A4;
        margin: 14mm 16mm;
    }
    body {
        font-family: 'Times New Roman', Times, 'Liberation Serif', serif;
        font-size: 9.5pt;
        line-height: 1.38;
        color: #111;
        max-width: 820px;
        margin: 0 auto;
        padding: 20px 15px;
        background-color: #fff;
    }
    h1 {
        text-align: center;
        font-size: 16pt;
        font-weight: bold;
        line-height: 1.22;
        margin-bottom: 8px;
    }
    h2 {
        font-size: 12pt;
        border-bottom: 1px solid #333;
        padding-bottom: 2px;
        margin-top: 20px;
        margin-bottom: 8px;
        text-transform: uppercase;
        letter-spacing: 0.4px;
    }
    h3 {
        font-size: 10.2pt;
        margin-top: 14px;
        margin-bottom: 6px;
        font-weight: bold;
    }
    p {
        text-align: justify;
        margin-top: 0;
        margin-bottom: 8px;
    }
    .table-container {
        width: 100%;
        overflow-x: auto;
        margin: 12px 0;
    }
    table {
        width: 100% !important;
        max-width: 100% !important;
        table-layout: fixed;
        border-collapse: collapse;
        margin: 10px 0;
        font-size: 7.5pt;
        page-break-inside: avoid;
        word-wrap: break-word;
        overflow-wrap: break-word;
    }
    th, td {
        border: 1px solid #666;
        padding: 4px 5px;
        text-align: left;
        word-break: break-word;
    }
    th {
        background-color: #f0f0f0;
        font-weight: bold;
        text-align: center;
    }
    pre, code {
        font-family: 'Consolas', 'Courier New', monospace;
        background-color: #f8f8f8;
        font-size: 8pt;
    }
    pre {
        border: 1px solid #ccc;
        padding: 8px 10px;
        overflow-x: auto;
        line-height: 1.3;
        border-radius: 2px;
        margin: 8px 0;
    }
    img {
        max-width: 78%;
        height: auto;
        display: block;
        margin: 12px auto 4px auto;
        border: 1px solid #ddd;
        page-break-inside: avoid;
    }
    em {
        font-size: 8pt;
        color: #333;
        text-align: center;
        display: block;
        margin-bottom: 10px;
    }
    ul, ol {
        margin-top: 2px;
        margin-bottom: 8px;
        padding-left: 20px;
    }
    li {
        margin-bottom: 3px;
    }
    hr {
        border: 0;
        border-top: 1px solid #bbb;
        margin: 14px 0;
    }
    @media print {
        body {
            max-width: 100%;
            padding: 0;
            font-size: 9pt;
            line-height: 1.32;
        }
        h1 { font-size: 14pt; }
        h2 { font-size: 11pt; margin-top: 14px; }
        h3 { font-size: 9.5pt; margin-top: 10px; }
        table { font-size: 7pt; }
        th, td { padding: 3px 4px; }
        img { max-width: 70%; }
        a { text-decoration: none; color: #000; }
    }
</style>
</head>
<body>
""" + html_body + """
</body>
</html>
"""

# Wrap all tables in a responsive container if not already wrapped
template = template.replace("<table>", '<div class="table-container"><table>').replace("</table>", '</table></div>')

with open(html_file, "w", encoding="utf-8") as f:
    f.write(template)

print(f"Generated streamlined 6-8 page academic HTML paper: {html_file}")
