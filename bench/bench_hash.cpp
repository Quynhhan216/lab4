#include "../src/hash_engine.h"
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

static void EnsureTestFile(const std::string& path, size_t sizeBytes) {
    std::ifstream check(path, std::ios::binary | std::ios::ate);
    if (check.is_open() && (size_t)check.tellg() == sizeBytes) {
        check.close();
        return;
    }
    check.close();

    std::ofstream out(path, std::ios::binary);
    std::vector<char> buffer(1 << 20);
    std::srand(42); // seed CO DINH chi de tao du lieu test deterministic, KHONG dung cho key/nonce
    size_t written = 0;
    while (written < sizeBytes) {
        size_t chunk = std::min(buffer.size(), sizeBytes - written);
        for (size_t i = 0; i < chunk; ++i) buffer[i] = (char)std::rand();
        out.write(buffer.data(), chunk);
        written += chunk;
    }
}

struct Stats { double mean, median, stdev, ci_low, ci_high; };

static Stats ComputeStats(std::vector<double>& samples) {
    Stats s{};
    size_t n = samples.size();
    double sum = 0;
    for (double v : samples) sum += v;
    s.mean = sum / n;

    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    s.median = (n % 2 == 0) ? (sorted[n/2 - 1] + sorted[n/2]) / 2.0 : sorted[n/2];

    double sq = 0;
    for (double v : samples) sq += (v - s.mean) * (v - s.mean);
    s.stdev = std::sqrt(sq / (n - 1));

    double margin = 1.96 * s.stdev / std::sqrt((double)n); // 95% CI, xap xi voi n>=30
    s.ci_low = s.mean - margin;
    s.ci_high = s.mean + margin;
    return s;
}

int main() {
    struct TestCase { std::string algo; size_t sizeBytes; std::string label; };
    std::vector<TestCase> cases = {
        {"sha256",   1ull << 20,   "1MiB"},
        {"sha512",   1ull << 20,   "1MiB"},
        {"sha3-256", 1ull << 20,   "1MiB"},
        {"sha3-512", 1ull << 20,   "1MiB"},
        {"sha256",   100ull << 20, "100MiB"},
        {"sha512",   100ull << 20, "100MiB"},
        {"sha3-256", 100ull << 20, "100MiB"},
        {"sha3-512", 100ull << 20, "100MiB"},
    };
    const int N_TRIALS = 30;

    std::cout << "algo,size_label,size_bytes,mean_sec,median_sec,stdev_sec,ci95_low,ci95_high,throughput_MBps\n";

    for (auto& tc : cases) {
        std::string path = "bench_data_" + tc.label + ".bin";
        EnsureTestFile(path, tc.sizeBytes);

        for (int w = 0; w < 2; ++w) hashtool::HashFileHex(tc.algo, path); // warm-up

        std::vector<double> times;
        times.reserve(N_TRIALS);
        for (int i = 0; i < N_TRIALS; ++i) {
            auto t0 = std::chrono::high_resolution_clock::now();
            hashtool::HashFileHex(tc.algo, path);
            auto t1 = std::chrono::high_resolution_clock::now();
            times.push_back(std::chrono::duration<double>(t1 - t0).count());
        }

        Stats s = ComputeStats(times);
        double throughputMBps = (tc.sizeBytes / (1024.0 * 1024.0)) / s.mean;

        std::cout << tc.algo << "," << tc.label << "," << tc.sizeBytes << ","
                   << s.mean << "," << s.median << "," << s.stdev << ","
                   << s.ci_low << "," << s.ci_high << "," << throughputMBps << "\n";
        std::cerr << "[done] " << tc.algo << " " << tc.label
                   << " mean=" << s.mean << "s  ~" << throughputMBps << " MB/s\n";
    }
    return 0;
}
