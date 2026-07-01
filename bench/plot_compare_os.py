import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df_ubuntu = pd.read_csv("bench_results.csv")
df_ubuntu["os"] = "Ubuntu"
df_windows = pd.read_csv("bench_results_windows.csv")
df_windows["os"] = "Windows"

df = pd.concat([df_ubuntu, df_windows], ignore_index=True)

fig, axes = plt.subplots(1, 2, figsize=(13, 5.5), sharey=True)

for ax, size_label in zip(axes, ["1MiB", "100MiB"]):
    sub = df[df["size_label"] == size_label]
    algos = sub["algo"].unique()
    x = np.arange(len(algos))
    width = 0.35

    ubuntu_vals = [sub[(sub["algo"] == a) & (sub["os"] == "Ubuntu")]["throughput_MBps"].values[0] for a in algos]
    windows_vals = [sub[(sub["algo"] == a) & (sub["os"] == "Windows")]["throughput_MBps"].values[0] for a in algos]

    ax.bar(x - width/2, ubuntu_vals, width, label="Ubuntu", color="#1f77b4")
    ax.bar(x + width/2, windows_vals, width, label="Windows", color="#ff7f0e")

    ax.set_xticks(x)
    ax.set_xticklabels(algos, rotation=20, ha="right")
    ax.set_title(f"File size: {size_label}")
    ax.set_ylabel("Throughput (MB/s)")
    ax.legend()

fig.suptitle("Hash Throughput: Ubuntu vs Windows Comparison", fontsize=14)
plt.tight_layout()
plt.savefig("bench/throughput_compare_os.png", dpi=150)
print("Da luu: bench/throughput_compare_os.png")