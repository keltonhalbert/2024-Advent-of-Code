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

[[nodiscard]] static inline lvl_int_t is_safe(const lvl_int_t lvl1, 
                                              const lvl_int_t lvl2, 
                                              const lvl_int_t lvl3) {
    const lvl_int_t diff1 = lvl1 - lvl2;
    const lvl_int_t diff2 = lvl2 - lvl3;
    const lvl_int_t diff3 = lvl1 - lvl3;
    const bool is_neg1 = std::signbit(diff1);
    const bool is_neg2 = std::signbit(diff2);
    const lvl_int_t abs_diff1 = std::abs(diff1);
    const lvl_int_t abs_diff2 = std::abs(diff2);
    const lvl_int_t abs_diff3 = std::abs(diff3);
    lvl_int_t n_errs = 0;
    
    const bool is_monotonic = (is_neg1 == is_neg2);
    const bool is_spikey = (
        (abs_diff1 > SAFE_LVL_ABS_DIFF_MAX) || 
        (abs_diff1 < SAFE_LVL_ABS_DIFF_MIN) || 
        (abs_diff2 > SAFE_LVL_ABS_DIFF_MAX) || 
        (abs_diff2 < SAFE_LVL_ABS_DIFF_MIN) 
    );
    const bool is_spikey_no_mid = (
        (abs_diff3 > SAFE_LVL_ABS_DIFF_MAX) ||
        (abs_diff3 < SAFE_LVL_ABS_DIFF_MIN) 
    );

    printf("%d %d %d\t%d %d %d\n", lvl1, lvl2, lvl3, is_monotonic, is_spikey, is_spikey_no_mid);
    if (is_spikey & is_spikey_no_mid){
        n_errs += 1;
    }
    else if (!is_monotonic) {
        n_errs += !is_monotonic + (abs_diff3 == 0);
    }
    return n_errs;
}

[[nodiscard]] static bool check_report_safety(const lvl_vec_t& rpt) noexcept {
    lvl_int_t n_fails = 0;
    for (std::size_t idx = 2; idx < rpt.size(); ++idx) {
        const lvl_int_t lvl1 = rpt[idx-2];
        const lvl_int_t lvl2 = rpt[idx-1];
        const lvl_int_t lvl3 = rpt[idx];

        lvl_int_t fails =  is_safe(lvl1, lvl2, lvl3);
        n_fails += fails;
        printf("%d\n", n_fails);
        if ((n_fails > 1)) {
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
    /*        n_safe += check_report_safety(rpt);*/
    /*    }*/
    /*}*/
    /*printf("Number of safe reports: %d\n", n_safe);*/
    /*const lvl_vec_t rpt = {9, 7, 6, 2, 1};*/
    /*const lvl_vec_t rpt = {1, 2, 7, 8, 9};*/
    /*const lvl_vec_t rpt = {94, 96, 94, 93, 92, 89, 91};*/
    /*const lvl_vec_t rpt = {8, 6, 4, 4, 1};*/
    // I've been trying so hard to do this without popping 
    // an element from an array and re-checking the result... but 
    // at this point I need to stop being stubborn
    const lvl_vec_t rpt = {1, 3, 2, 4, 5};
    bool safe = check_report_safety(rpt);
    printf("%d\n", safe);
    return 0;
}
