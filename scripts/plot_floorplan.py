#!/usr/bin/env python3
"""Plot floorplans produced by the floorplanner.

The chip outline, fixed modules and nets are read from the input file (ICCAD
2023 Problem D format); the rectilinear soft modules are read from the
check.txt written by the floorplanner (`-o <dir>`).

Examples
--------
Single floorplan::

    python3 scripts/plot_floorplan.py single \
        --input testcase/case01-input.txt --check out/case01/check.txt \
        --out docs/images/case01.png --nets --labels

Gallery of several floorplans (input,check,title triples)::

    python3 scripts/plot_floorplan.py gallery --out docs/images/gallery.png \
        --case testcase/case01-input.txt out/case01/check.txt "case01" ...
"""
import argparse
import re
from dataclasses import dataclass, field

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.patches import Polygon, Rectangle  # noqa: E402

POINT_RE = re.compile(r"\{(-?[\d.e+-]+), (-?[\d.e+-]+)\}")

# Qualitative palette (soft modules); fixed modules are drawn in grey.
PALETTE = [
    "#4C78A8", "#F58518", "#54A24B", "#E45756", "#72B7B2", "#EECA3B",
    "#B279A2", "#FF9DA6", "#9D755D", "#59A14F", "#EDC948", "#76B7B2",
]


@dataclass
class Design:
    width: int = 0
    height: int = 0
    soft_names: list = field(default_factory=list)
    fixed: dict = field(default_factory=dict)  # name -> (x, y, w, h)
    nets: list = field(default_factory=list)  # (names, weight)


@dataclass
class Result:
    polygons: dict = field(default_factory=dict)  # seq index -> [(x, y)]
    legal: dict = field(default_factory=dict)  # seq index -> bool
    wirelength: float = float("nan")


def read_design(path):
    design = Design()
    tokens = open(path).read().split()
    i = 0
    while i < len(tokens):
        key = tokens[i]
        i += 1
        if key == "CHIP":
            design.width, design.height = int(tokens[i]), int(tokens[i + 1])
            i += 2
        elif key == "SOFTMODULE":
            n = int(tokens[i])
            i += 1
            for _ in range(n):
                design.soft_names.append(tokens[i])
                i += 2  # name, minimum area
        elif key == "FIXEDMODULE":
            n = int(tokens[i])
            i += 1
            for _ in range(n):
                name = tokens[i]
                x, y, w, h = (int(v) for v in tokens[i + 1:i + 5])
                design.fixed[name] = (x, y, w, h)
                i += 5
        elif key == "CONNECTION":
            n = int(tokens[i])
            i += 1
            for _ in range(n):
                k = int(tokens[i])
                names = tokens[i + 1:i + 1 + k]
                weight = float(tokens[i + 1 + k])
                design.nets.append((names, weight))
                i += 2 + k
    return design


def read_check(path):
    result = Result()
    lines = [line.rstrip("\n") for line in open(path)]
    for line in lines:
        fields = [f.strip() for f in line.split(",", 2)]
        if len(fields) < 3 or not re.fullmatch(r"s\d+", fields[0]):
            continue
        index = int(fields[0][1:])
        result.legal[index] = fields[1] == "TRUE"
        result.polygons[index] = [(float(x), float(y))
                                  for x, y in POINT_RE.findall(line)]
    numbers = [line.strip() for line in lines
               if re.fullmatch(r"\s*-?[\d.e+]+\s*", line)]
    if numbers:
        result.wirelength = float(numbers[0])
    return result


def centroid(points):
    """Area centroid of a simple polygon (shoelace formula)."""
    a = cx = cy = 0.0
    for (x0, y0), (x1, y1) in zip(points, points[1:] + points[:1]):
        cross = x0 * y1 - x1 * y0
        a += cross
        cx += (x0 + x1) * cross
        cy += (y0 + y1) * cross
    if abs(a) < 1e-9:
        xs, ys = zip(*points)
        return sum(xs) / len(xs), sum(ys) / len(ys)
    return cx / (3 * a), cy / (3 * a)


def add_legend(ax):
    from matplotlib.patches import Patch
    handles = [
        Patch(fc=PALETTE[0], alpha=0.82, ec="white", label="soft module"),
        Patch(fc="#C9CCD1", ec="#6B7078", hatch="////", label="fixed module"),
        Patch(fc="none", ec="#C62828", lw=1.6, label="violates a rule"),
    ]
    ax.legend(handles=handles, loc="upper center", bbox_to_anchor=(0.5, -0.01),
              ncol=3, frameon=False, fontsize=8)


def draw(ax, design, result, title=None, nets=False, labels=False,
         font_size=7):
    ax.add_patch(Rectangle((0, 0), design.width, design.height, fill=False,
                           lw=1.4, ec="#222222", zorder=4))
    centers = {}
    for name, (x, y, w, h) in design.fixed.items():
        ax.add_patch(Rectangle((x, y), w, h, fc="#C9CCD1", ec="#6B7078",
                               hatch="////", lw=0.8, zorder=2))
        centers[name] = (x + w / 2, y + h / 2)
        if labels:
            ax.text(x + w / 2, y + h / 2, name, ha="center", va="center",
                    fontsize=font_size * 0.8, color="#3A3F47", zorder=6)
    for index, points in sorted(result.polygons.items()):
        color = PALETTE[index % len(PALETTE)]
        legal = result.legal.get(index, True)
        ax.add_patch(Polygon(points, closed=True, fc=color, alpha=0.82,
                             ec="#C62828" if not legal else "white",
                             lw=1.6 if not legal else 0.9, zorder=3))
        name = (design.soft_names[index] if index < len(design.soft_names)
                else f"s{index}")
        centers[name] = centroid(points)
        if labels:
            cx, cy = centers[name]
            ax.text(cx, cy, name, ha="center", va="center",
                    fontsize=font_size, color="white", weight="bold",
                    zorder=6)
    if nets and design.nets:
        max_w = max(w for _, w in design.nets)
        for names, weight in design.nets:
            pts = [centers[n] for n in names if n in centers]
            for a, b in zip(pts, pts[1:]):
                ax.plot([a[0], b[0]], [a[1], b[1]], color="#1B1F24",
                        lw=0.3 + 1.6 * weight / max_w, alpha=0.35, zorder=5)
    ax.set_xlim(-design.width * 0.01, design.width * 1.01)
    ax.set_ylim(-design.height * 0.01, design.height * 1.01)
    ax.set_aspect("equal")
    ax.set_xticks([])
    ax.set_yticks([])
    for spine in ax.spines.values():
        spine.set_visible(False)
    if title:
        ax.set_title(title, fontsize=10, pad=6)


def cmd_single(args):
    design, result = read_design(args.input), read_check(args.check)
    aspect = design.height / design.width
    fig, ax = plt.subplots(figsize=(args.size, args.size * aspect))
    draw(ax, design, result, args.title, args.nets, args.labels)
    add_legend(ax)
    fig.tight_layout()
    fig.savefig(args.out, dpi=args.dpi, bbox_inches="tight")


def cmd_gallery(args):
    cases = args.case
    cols = min(args.cols, len(cases))
    rows = (len(cases) + cols - 1) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(3.2 * cols, 3.8 * rows))
    axes = axes.flatten() if hasattr(axes, "flatten") else [axes]
    for ax, (input_path, check_path, title) in zip(axes, cases):
        draw(ax, read_design(input_path), read_check(check_path), title,
             nets=False, labels=False)
    for ax in axes[len(cases):]:
        ax.axis("off")
    fig.tight_layout()
    fig.subplots_adjust(hspace=0.25)
    fig.savefig(args.out, dpi=args.dpi, bbox_inches="tight")


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="command", required=True)

    single = sub.add_parser("single", help="plot one floorplan")
    single.add_argument("--input", required=True, help="input testcase")
    single.add_argument("--check", required=True, help="check.txt")
    single.add_argument("--out", required=True, help="output image")
    single.add_argument("--title")
    single.add_argument("--nets", action="store_true", help="draw the nets")
    single.add_argument("--labels", action="store_true",
                        help="label the modules")
    single.add_argument("--size", type=float, default=6.0,
                        help="figure width in inches")
    single.add_argument("--dpi", type=int, default=160)
    single.set_defaults(func=cmd_single)

    gallery = sub.add_parser("gallery", help="plot several floorplans")
    gallery.add_argument("--case", nargs=3, action="append", required=True,
                         metavar=("INPUT", "CHECK", "TITLE"))
    gallery.add_argument("--out", required=True)
    gallery.add_argument("--cols", type=int, default=5)
    gallery.add_argument("--dpi", type=int, default=140)
    gallery.set_defaults(func=cmd_gallery)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
