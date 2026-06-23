import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("bench_results.csv")

fig, ax = plt.subplots(figsize=(8, 5))
for size_label in df["size_label"].unique():
    sub = df[df["size_label"] == size_label]
    ax.bar([f"{a}\n({size_label})" for a in sub["algo"]],
           sub["throughput_MBps"],
           yerr=(sub["ci95_high"] - sub["ci95_low"]) / 2)

ax.set_ylabel("Throughput (MB/s)")
ax.set_title("Hash Throughput Comparison (Ubuntu, with 95% CI error bars)")
plt.xticks(rotation=30, ha="right")
plt.tight_layout()
plt.savefig("bench/throughput_chart.png", dpi=150)
print("Da luu: bench/throughput_chart.png")
