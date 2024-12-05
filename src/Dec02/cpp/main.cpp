#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using int_t = std::int16_t;
using lvl_int_t = std::int8_t;
using lvl_vec_t = std::vector<lvl_int_t>;
using rpt_vec_t = std::vector<lvl_vec_t>;

constexpr int_t MAX_FILE_LINE_LENTH = 25;
constexpr int_t N_FILE_LINES = 1000;
constexpr lvl_int_t SAFE_LVL_ABS_DIFF_MIN = 1;
constexpr lvl_int_t SAFE_LVL_ABS_DIFF_MAX = 3;
constexpr char DELIMITER = ' ';
constexpr lvl_int_t N_MAX_DAMPS = 1;

[[nodiscard]] static lvl_vec_t parse_report(const std::string &line) noexcept {
    lvl_vec_t report;
    std::size_t delim_idx;
    std::size_t search_begin = 0;
    while ((delim_idx = line.find(DELIMITER, search_begin)) !=
           std::string::npos) {
        const std::size_t len = delim_idx - search_begin;
        const lvl_int_t level = std::stoi(line.substr(search_begin, len));
        report.push_back(level);
        search_begin = delim_idx + 1;
    }
    // gotta get the last level separated by the
    // last delimiter
    const lvl_int_t level = std::stoi(line.substr(search_begin));
    report.push_back(level);
    return report;
}

[[nodiscard]] static int parse_file(rpt_vec_t &reports) {
    std::ifstream input_file("./input.txt", std::ios::in);
    std::string line;

    if (input_file.is_open()) {
        line.reserve(MAX_FILE_LINE_LENTH);
        std::size_t idx = 0;
        while ((std::getline(input_file, line)) && (idx < N_FILE_LINES)) {
            const lvl_vec_t rpt = parse_report(line);
            reports.push_back(rpt);
            ++idx;
        }
        return 1;
    } else {
        printf("Failed to open file.\n");
        input_file.close();
        return 0;
    }
}

[[nodiscard]] static inline bool is_safe(const lvl_int_t lvl1,
                                         const lvl_int_t lvl2,
                                         const lvl_int_t lvl3) noexcept {
    const lvl_int_t diff_left = lvl1 - lvl2;
    const lvl_int_t diff_right = lvl2 - lvl3;
    const bool is_neg1 = std::signbit(diff_left);
    const bool is_neg2 = std::signbit(diff_right);
    const lvl_int_t abs_diff_left = std::abs(diff_left);
    const lvl_int_t abs_diff_right = std::abs(diff_right);

    const bool is_monotonic = (is_neg1 == is_neg2);
    const bool unsafe_rate = ((abs_diff_left > SAFE_LVL_ABS_DIFF_MAX) ||
                              (abs_diff_left < SAFE_LVL_ABS_DIFF_MIN) ||
                              (abs_diff_right > SAFE_LVL_ABS_DIFF_MAX) ||
                              (abs_diff_right < SAFE_LVL_ABS_DIFF_MIN));
    if (!is_monotonic || unsafe_rate) {
        return 0;
    }
    return 1;
}

[[nodiscard]] static inline bool check_rpt_sign(const lvl_vec_t &rpt) noexcept {
    float sum = 0;
    for (std::size_t idx = 1; idx < rpt.size(); ++idx) {
        sum += std::signbit(rpt[idx] - rpt[idx - 1]) ? -1 : 1;
    }
    return std::signbit(sum);
}

[[nodiscard]] static bool check_report_safety_pt1(
    const lvl_vec_t &rpt) noexcept {
    for (std::size_t idx = 2; idx < rpt.size(); ++idx) {
        const lvl_int_t lvl1 = rpt[idx - 2];
        const lvl_int_t lvl2 = rpt[idx - 1];
        const lvl_int_t lvl3 = rpt[idx];

        if (!is_safe(lvl1, lvl2, lvl3)) return 0;
    }
    return 1;
}

[[nodiscard]] static bool check_report_safety_pt2(
    const lvl_vec_t &rpt) noexcept {
    const bool sign_rpt = check_rpt_sign(rpt);
    lvl_vec_t rpt_copy(rpt);

    std::size_t idx = 1;
    std::size_t rpt_len = rpt_copy.size();
    lvl_int_t n_fails = 0;
    while (idx < rpt_len) {
        if (n_fails > 1) return 0;
        lvl_int_t lvl1 = rpt_copy[idx - 1];
        lvl_int_t lvl2 = rpt_copy[idx];

        lvl_int_t diff = lvl2 - lvl1;
        lvl_int_t abs_diff = std::abs(diff);

        bool is_monotonic = (sign_rpt == std::signbit(diff));
        bool valid_rate = ((abs_diff >= SAFE_LVL_ABS_DIFF_MIN) &&
                           (abs_diff <= SAFE_LVL_ABS_DIFF_MAX));

        if (is_monotonic & valid_rate) {
            ++idx;
        } else {
            if ((!valid_rate) && (is_monotonic)) {
                if (sign_rpt) {
                    rpt_copy.erase(rpt_copy.begin() + idx - 1);
                } else {
                    rpt_copy.erase(rpt_copy.begin() + idx);
                }
            } else if ((!valid_rate) && (!is_monotonic)) {
                if (std::abs(lvl2) > std::abs(lvl1)) {
                    rpt_copy.erase(rpt_copy.begin() + idx);
                } else {
                    rpt_copy.erase(rpt_copy.begin() + idx - 1);
                }
            } else if ((valid_rate) && (!is_monotonic)) {
                if (sign_rpt) {
                    if (std::abs(lvl1) > std::abs(lvl2)) {
                        rpt_copy.erase(rpt_copy.begin() + idx - 1);
                    } else {
                        rpt_copy.erase(rpt_copy.begin() + idx);
                    }
                }
                if (std::abs(lvl2) > std::abs(lvl1)) {
                    rpt_copy.erase(rpt_copy.begin() + idx);
                } else {
                    rpt_copy.erase(rpt_copy.begin() + idx - 1);
                }
            }
            rpt_len = rpt_copy.size();
            ++n_fails;
        }
    }
    if (n_fails > 1) return 0;

    return 1;
}

int main(int argc, const char **argv) {
    rpt_vec_t reports;
    int_t n_safe = 0;
    reports.reserve(N_FILE_LINES);
    if (parse_file(reports)) {
        for (const auto &rpt : reports) {
            n_safe += check_report_safety_pt1(rpt);
        }
        printf("Number of safe reports (no Problem Damper): %d\n", n_safe);

        n_safe = 0;
        for (const auto &rpt : reports) {
            n_safe += check_report_safety_pt2(rpt);
        }
        printf("Number of safe reports (Problem Damper): %d\n", n_safe);
    }
    return 0;
}
