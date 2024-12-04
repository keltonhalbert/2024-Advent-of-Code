#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
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

void print_arr(const lvl_vec_t& rpt) {
    printf("{ ");
    for (std::size_t idx = 0; idx < rpt.size(); ++idx) {
        printf("%d, ", rpt[idx]);
    }
    printf("}\n");
}

[[nodiscard]] static lvl_vec_t parse_report(const std::string& line) noexcept {
    lvl_vec_t report;
    std::size_t delim_idx;
    std::size_t search_begin = 0;
    while ((delim_idx = line.find(DELIMITER, search_begin)) != std::string::npos) {
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

[[nodiscard]] static int parse_file(rpt_vec_t& reports) {
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
    }
    else {
        printf("Failed to open file.\n");
        input_file.close();
        return 0;
    }
}

[[nodiscard]] static inline bool is_safe(const lvl_int_t lvl1, 
                                         const lvl_int_t lvl2, 
                                         const lvl_int_t lvl3) {
    const lvl_int_t diff_left = lvl1 - lvl2;
    const lvl_int_t diff_right = lvl2 - lvl3;
    const bool is_neg1 = std::signbit(diff_left);
    const bool is_neg2 = std::signbit(diff_right);
    const lvl_int_t abs_diff_left = std::abs(diff_left);
    const lvl_int_t abs_diff_right = std::abs(diff_right);
    
    const bool is_monotonic = (is_neg1 == is_neg2);
    const bool unsafe_rate = (
        (abs_diff_left > SAFE_LVL_ABS_DIFF_MAX) || 
        (abs_diff_left < SAFE_LVL_ABS_DIFF_MIN) || 
        (abs_diff_right > SAFE_LVL_ABS_DIFF_MAX) || 
        (abs_diff_right < SAFE_LVL_ABS_DIFF_MIN) 
    );
    /*printf("%d %d %d\t%d %d\t%d %d\n", lvl1, lvl2, lvl3, diff1, diff2, is_monotonic, is_spikey);*/
    if (!is_monotonic || unsafe_rate){
        return 0;
    }
    return 1;
}

[[nodiscard]] static inline bool check_rate(const lvl_int_t abs_diff) {
    return (abs_diff <= SAFE_LVL_ABS_DIFF_MAX) && (abs_diff >= SAFE_LVL_ABS_DIFF_MIN);
}

[[nodiscard]] static inline bool check_rpt_sign(const lvl_vec_t& rpt) {
    float sum = 0;
    for (std::size_t idx = 1; idx < rpt.size(); ++idx) {
        sum += std::signbit(rpt[idx] - rpt[idx-1]) ? -1 : 1;
    }
    return std::signbit(sum);
}

[[nodiscard]] static inline bool check_monotonicity(const bool sign1, const bool sign2) {
    return (sign1 == sign2);
}

[[nodiscard]] static bool check_report_safety_pt1(const lvl_vec_t& rpt) {
    for (std::size_t idx = 2; idx < rpt.size(); ++idx) {
        const lvl_int_t lvl1 = rpt[idx-2];
        const lvl_int_t lvl2 = rpt[idx-1];
        const lvl_int_t lvl3 = rpt[idx];

        if (!is_safe(lvl1, lvl2, lvl3)) return 0;
    }
    return 1;
}

[[nodiscard]] static bool check_report_safety_pt2(const lvl_vec_t& rpt) noexcept {
    const bool sign_rpt = check_rpt_sign(rpt);
    lvl_int_t n_damps = 0; 
    for (std::size_t idx = 1; idx < rpt.size(); ++idx) {
        const lvl_int_t lvl1 = rpt[idx-1];
        const lvl_int_t lvl2 = rpt[idx];
        const lvl_int_t diff = lvl2 - lvl1;
        const lvl_int_t abs_diff = std::abs(diff);

        // We need to grab an element ahead or behind, depending
        // where we are in the moving window. This is how we check 
        // if removing a report from the array causes it to pass 
        // safety. 
        const std::size_t idx_aux = (idx < rpt.size() - 1) ? idx + 1 : idx - 2;
        const lvl_int_t diff_aux1 = (idx_aux > idx) ? rpt[idx_aux] - lvl2 : lvl1 - rpt[idx_aux];
        const lvl_int_t diff_aux2 = (idx_aux > idx) ? rpt[idx_aux] - lvl1 : lvl2 - rpt[idx_aux];  
        const lvl_int_t abs_diff_aux1 = std::abs(diff_aux1);
        const lvl_int_t abs_diff_aux2 = std::abs(diff_aux2);

        const bool valid_rate = check_rate(abs_diff);
        const bool valid_rate_aux1 = check_rate(abs_diff_aux1);
        const bool valid_rate_aux2 = check_rate(abs_diff_aux2);

        const bool is_neg = std::signbit(diff);
        const bool is_neg_aux1 = std::signbit(diff_aux1);
        const bool is_neg_aux2 = std::signbit(diff_aux2);
        const bool is_monotonic = check_monotonicity(is_neg, sign_rpt); 
        const bool is_monotonic_aux1 = check_monotonicity(is_neg_aux1, sign_rpt);
        const bool is_monotonic_aux2 = check_monotonicity(is_neg_aux2, sign_rpt);
        if (n_damps > N_MAX_DAMPS) {
            return 0;
        }

        if (valid_rate && is_monotonic && valid_rate_aux1 && is_monotonic_aux1) {
            continue; 
        }
        else if ((!valid_rate || !is_monotonic) && (valid_rate_aux1 && is_monotonic_aux1)) {
            ++n_damps;
            continue;
        }
        else if ((lvl1 == lvl2) && 
                 (valid_rate_aux1 && is_monotonic_aux1) 
        ) {
            ++n_damps;
            continue;
        }
        else {
            return 0;
        }

    }
    return 1;
}

int main(int argc, const char** argv) {
    /*rpt_vec_t reports;*/
    /*int_t n_safe = 0;*/
    /*reports.reserve(N_FILE_LINES);*/
    /*if (parse_file(reports)) {*/
    /*    for (const auto& rpt : reports) {*/
    /*        n_safe += check_report_safety_pt1(rpt);*/
    /*    }*/
    /*}*/
    /*printf("Number of safe reports (no Problem Damper): %d\n", n_safe);*/
    /**/
    /*n_safe = 0;*/
    /*for (const auto& rpt : reports) {*/
    /*    n_safe += check_report_safety_pt2(rpt);*/
    /*}*/
    /*printf("Number of safe reports (Problem Damper): %d\n", n_safe);*/
    const rpt_vec_t reports = {
        {7, 6, 4, 2, 1},
        {1, 2, 7, 8, 9},
        {9, 7, 6, 2, 1},
        {1, 3, 2, 4, 5},
        {8, 6, 4, 4, 1},
        {1, 3, 6, 7, 9},
        {4, 10, 3, 2, 1},
        {10, 5, 4, 3, 2},
        {5, 4, 3, 2, 10}
    };
    for (const auto& rpt : reports) {
        bool safe = check_report_safety_pt2(rpt);
        printf("%d ", safe);
        print_arr(rpt);
    }
    // I've been trying so hard to do this without popping 
    // an element from an array and re-checking the result... but 
    // at this point I need to stop being stubborn
    return 0;
}
