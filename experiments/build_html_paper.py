"""
Compile research_paper.md to an academic, publication-styled standalone HTML document.
Can be opened in any web browser and printed directly to PDF (15-20 formatted pages).
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
<style>
    @page {
        size: A4;
        margin: 20mm;
    }
    body {
        font-family: 'Times New Roman', Times, 'Liberation Serif', serif;
        font-size: 11pt;
        line-height: 1.55;
        color: #111;
        max-width: 900px;
        margin: 0 auto;
        padding: 40px 25px;
        background-color: #fff;
    }
    h1 {
        text-align: center;
        font-size: 18pt;
        font-weight: bold;
        line-height: 1.25;
        margin-bottom: 12px;
    }
    h2 {
        font-size: 13.5pt;
        border-bottom: 1.5px solid #222;
        padding-bottom: 3px;
        margin-top: 30px;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }
    h3 {
        font-size: 11.5pt;
        margin-top: 20px;
        font-weight: bold;
    }
    table {
        width: 100%;
        border-collapse: collapse;
        margin: 22px 0;
        font-size: 9pt;
        page-break-inside: avoid;
    }
    th, td {
        border: 1px solid #777;
        padding: 6px 8px;
        text-align: left;
    }
    th {
        background-color: #eaeaea;
        font-weight: bold;
        text-align: center;
    }
    pre, code {
        font-family: 'Consolas', 'Courier New', monospace;
        background-color: #f7f7f7;
        font-size: 8.5pt;
    }
    pre {
        border: 1px solid #ccc;
        padding: 10px 14px;
        overflow-x: auto;
        line-height: 1.35;
        border-radius: 3px;
    }
    img {
        max-width: 90%;
        height: auto;
        display: block;
        margin: 25px auto 10px auto;
        border: 1px solid #ddd;
    }
    p {
        text-align: justify;
        margin-bottom: 10px;
    }
    blockquote {
        border-left: 4px solid #0056b3;
        padding-left: 12px;
        margin-left: 0;
        color: #444;
        font-style: italic;
    }
    ul, ol {
        margin-bottom: 12px;
        padding-left: 25px;
    }
    li {
        margin-bottom: 4px;
    }
    .header-box {
        text-align: center;
        margin-bottom: 35px;
        border-bottom: 2px solid #333;
        padding-bottom: 20px;
    }
</style>
</head>
<body>
""" + html_body + """
</body>
</html>
"""

with open(html_file, "w", encoding="utf-8") as f:
    f.write(template)

print(f"Generated standalone academic HTML paper: {html_file}")
