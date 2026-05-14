"""
plot_scaling.py
---------------
Loads strong and weak scaling CSVs for OpenMP, MPI, and CUDA, fits Amdahl's
and Gustafson's laws to each dataset, and produces two figures:
  - strong_scaling.png  (3 subplots: OpenMP | MPI | CUDA)
  - weak_scaling.png    (3 subplots: OpenMP | MPI | CUDA)

Expected input files in --input_dir:
  strong_openmp.csv, weak_openmp.csv
  strong_mpi.csv,    weak_mpi.csv
  strong_cuda.csv,   weak_cuda.csv

Strong CSV columns : Dim, Threads/Blocks[, Threads], Execution_Time, Speedup
Weak CSV columns   : Dim, Threads/Blocks[, Threads], Execution_Time, Scaled_Speedup

For CUDA files both Threads and Blocks are present; the x-axis uses Blocks and
the thread count is shown in the axis label.

Usage:
    python plot.py --input_dir results --output_dir plots --sm-count 20 --max-threads-mp 16 --max-threads-mpi 6
    python plot.py --help
"""

from dataclasses import dataclass
from pathlib import Path
from typing import Literal

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tyro
from matplotlib.axes import Axes
from scipy.optimize import curve_fit



LAWS = Literal["amdahl","gustafson"]


# ---------------------------------------------------------------------------
# CLI configuration
# ---------------------------------------------------------------------------

@dataclass
class Config:
    """Configuration for the scaling plot generator.

    Attributes:
        sm_count (int): The streaming multiprocessor count for your GPU
        max_threads_mp (int): Max number of threads to use for fitting strong or weak scaline of openmp data
        max_threads_mpi (int): Max number of threads to use for fitting strong or weak scaline of mpi data
        input_dir (Path):  Directory containing the six scaling CSV files
                    (strong/weak x openmp/mpi/cuda).
        output_dir (Path): Directory where the two output figures are saved.
    """
    sm_count: int
    max_threads_mp: int
    max_threads_mpi: int
    input_dir: Path
    output_dir: Path = Path(".")


# ---------------------------------------------------------------------------
# Amdahl's and Gustafson's law
# ---------------------------------------------------------------------------

def amdahl(N: np.ndarray, s: float) -> np.ndarray:
    """Amdahl's law: speedup = 1 / (s + (1-s)/N).

    Args:
        N (np.ndarray): Array of processor counts.
        s (float): Serial fraction of execution time (0 < s < 1).

    Returns:
        Predicted speedup for each value in N.
    """
    return 1.0 / (s + (1.0 - s) / N)


def gustafson(N: np.ndarray, s: float) -> np.ndarray:
    """Gustafson's law: scaled_speedup = s + (1-s)*N.

    Args:
        N (np.ndarray): Array of processor counts.
        s (float): Serial fraction of execution time (0 < s < 1).

    Returns:
        Predicted scaled speedup for each value in N.
    """
    return s + (1.0 - s) * N


def fit_serial_fraction(N: np.ndarray, speedup: np.ndarray, law: LAWS) -> float:
    """Fit the serial fraction s to the measured speedup data.

    Args:
        N (np.ndarray):       Processor counts.
        speedup (np.ndarray): Measured speedup or scaled speedup values.
        law (Literal[str]):   amdahl or gustafson

    Returns:
        Best-fit serial fraction s in [0, 1].
    """
    fn = amdahl if law == "amdahl" else gustafson
    popt, _ = curve_fit(fn, N, speedup, p0=[0.05], bounds=(1e-9, 1.0 - 1e-9))
    return float(popt[0])


# ---------------------------------------------------------------------------
# CSV loading helpers
# ---------------------------------------------------------------------------

#: Type alias for a loaded dataset: (N, speedup, x_col, fixed_threads).
Dataset = tuple[np.ndarray, np.ndarray, str, int | None]


def _x_column(df: pd.DataFrame) -> str:
    """Return the name of the parallelism column (Threads, Ranks, or Blocks).

    For CUDA files both Threads and Blocks are present; Blocks is the x-axis.

    Args:
        df: DataFrame loaded from a scaling CSV.

    Returns:
        Column name to use as the x-axis.

    Raises:
        ValueError: If none of the expected column names are found.
    """
    for col in ("Blocks", "Ranks", "Threads"):
        if col in df.columns:
            return col
    raise ValueError(f"No parallelism column found in {list(df.columns)}")


def load_strong(path: Path) -> Dataset:
    """Load a strong-scaling CSV.

    Args:
        path: Path to the CSV file.

    Returns:
        A tuple ``(N, speedup, x_col, fixed_threads)`` where:
          - N             - array of parallelism counts used as x-axis,
          - speedup       - array of measured speedups,
          - x_col         - name of the parallelism column,
          - fixed_threads - thread count for CUDA files (else ``None``).
    """
    df = pd.read_csv(path)
    x_col = _x_column(df)
    fixed_threads = int(df["Threads"].iloc[0]) if x_col == "Blocks" else None
    return df[x_col].to_numpy(float), df["Speedup"].to_numpy(float), x_col, fixed_threads


def load_weak(path: Path) -> Dataset:
    """Load a weak-scaling CSV.

    Args:
        path: Path to the CSV file.

    Returns:
        Same structure as :func:`load_strong` but with scaled speedup values.
    """
    df = pd.read_csv(path)
    x_col = _x_column(df)
    fixed_threads = int(df["Threads"].iloc[0]) if x_col == "Blocks" else None
    return df[x_col].to_numpy(float), df["Scaled_Speedup"].to_numpy(float), x_col, fixed_threads


def try_load(
    loader: callable,
    path: Path,
) -> Dataset | None:
    """Attempt to load a scaling CSV, returning None if the file is missing or malformed.

    Args:
        loader: One of :func:`load_strong` or :func:`load_weak`.
        path:   Path to the CSV file.

    Returns:
        The loader's return value on success, or ``None`` on any error.
    """
    try:
        return loader(path)
    except FileNotFoundError:
        print(f"Warning: file not found, skipping - {path}")
    except Exception as exc:
        print(f"Warning: could not load {path} ({exc}), skipping.")
    return None


# ---------------------------------------------------------------------------
# Per-subplot plotting
# ---------------------------------------------------------------------------

def _x_label(x_col: str, fixed_threads: int | None) -> str:
    """Build the x-axis label.

    Args:
        x_col:         Name of the parallelism column.
        fixed_threads: Fixed thread count for CUDA (``None`` otherwise).

    Returns:
        Human-readable x-axis label string.
    """
    if x_col == "Blocks":
        return f"Number of Blocks (T = {fixed_threads} threads)"
    return f"Number of {x_col}"


def plot_subplot(
    ax: Axes,
    N: np.ndarray,
    speedup: np.ndarray,
    x_col: str,
    fixed_threads: int | None,
    law: LAWS,
    limit: int,
    title: str,
    y_label: str,
) -> None:
    """Render measured data and fitted theoretical curve onto a single Axes.

    Args:
        ax:                         Matplotlib Axes to draw on.
        N (np.ndarray):             Measured parallelism counts.
        speedup (np.ndarray):       Measured speedup or scaled speedup values.
        x_col (str):                Name of the parallelism column (for axis label).
        fixed_threads (int | None): Fixed CUDA thread count (``None`` if not CUDA).
        law (Literal[str]):         amdahl for strong scaling, gustafson for weak scaling.
        title (str):                Subplot title (e.g. OpenMP).
        y_label (str):              Y-axis label string.
    """

    idx = np.argmax(N > limit)
    if idx == 0:
        idx = N.size

    s = fit_serial_fraction(N[:idx], speedup[:idx], law)

    N_fine = np.linspace(N.min(), N[idx - 1], 500)
    fn = amdahl if law == "amdahl" else gustafson
    fitted = fn(N_fine, s)

    color_dots = '#2ca02c'
    color_line = '#7f7f7f'

    ax.plot(N, speedup, "o", zorder=3, color = color_dots)
    ax.plot(N_fine, fitted, "--", zorder=2, color = color_line)
    ax.set_title(title)
    ax.set_xlabel(_x_label(x_col, fixed_threads))
    ax.set_ylabel(y_label)
    ax.grid(True, linestyle=":", alpha=0.5)


# ---------------------------------------------------------------------------
# Figure builder
# ---------------------------------------------------------------------------

def build_figure(
    datasets: list[Dataset | None],
    subtitles: list[str],
    law: LAWS,
    config: Config,
    fig_title: str,
    y_label: str,
    out_path: Path,
) -> None:
    """Create and save a figure with one subplot per method.

    Args:
        datasets (list[Dataset | None]):   List of (N, speedup, x_col, fixed_threads) tuples one per method [OpenMP, MPI, CUDA].
                                           None if the data is not available.
        subtitles (list[str]):             Subplot titles, e.g. ["OpenMP", "MPI", "CUDA"].
        law (Literal[str]):                amdahl for strong scaling, gustafson for weak scaling.
        fig_title (str):                   Overall figure suptitle.
        y_label (str):                     Shared y-axis label for all subplots.
        out_path (Path):                   File path where the figure is saved.
    """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    fig.suptitle(fig_title, fontsize=14, fontweight="bold")

    for ax, dataset, subtitle in zip(axes, datasets, subtitles):
        if dataset is None:
            ax.set_title(subtitle)
            ax.text(
                0.5, 0.5, "No data available",
                ha="center", va="center",
                transform=ax.transAxes,
                fontsize=12, color="grey",
            )
            ax.set_axis_off()
        else:
            N, speedup, x_col, fixed_threads = dataset
            limits = {"CUDA": config.sm_count, "OpenMP": config.max_threads_mp, "MPI": config.max_threads_mpi}
            limit = limits[subtitle]
            plot_subplot(ax, N, speedup, x_col, fixed_threads, law, limit, subtitle, y_label)

    fig.tight_layout()
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved: {out_path}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main(cfg: Config) -> None:
    """Load CSVs, fit scaling laws, and save strong_scaling.png / weak_scaling.png.

    Args:
        cfg (Config):
    """
    cfg.output_dir.mkdir(parents=True, exist_ok=True)

    methods   = ["openmp", "mpi", "cuda"]
    subtitles = ["OpenMP", "MPI", "CUDA"]

    strong_data = [try_load(load_strong, cfg.input_dir / f"strong_{m}.csv") for m in methods]
    weak_data   = [try_load(load_weak,   cfg.input_dir / f"weak_{m}.csv")   for m in methods]

    build_figure(
        strong_data,
        subtitles,
        law="amdahl",
        config = cfg,
        fig_title="Strong Scaling (Amdahl's Law)",
        y_label="Speedup",
        out_path=cfg.output_dir / "strong_scaling.png",
    )

    build_figure(
        weak_data,
        subtitles,
        law="gustafson",
        config = cfg,
        fig_title="Weak Scaling (Gustafson's Law)",
        y_label="Scaled Speedup",
        out_path=cfg.output_dir / "weak_scaling.png",
    )


if __name__ == "__main__":
    main(tyro.cli(Config))
