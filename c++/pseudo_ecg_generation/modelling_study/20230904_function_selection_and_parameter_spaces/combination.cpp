
#include "combination.h"


combination::combination(){}

void combination::generate_combination_matrix(std::vector<std::vector<int>>& combination_matrix, std::vector<int>& current_row, int m, int n, int row_index) {
    if (row_index == m) {
     combination_matrix.push_back(current_row);
        return;
    }

    for (int i = 0; i < n; i++) {
        current_row.push_back(i);
        generate_combination_matrix(combination_matrix, current_row, m, n, row_index + 1);
        current_row.pop_back();
    }
}




//int main() {
//    int m = 5;
//    int n = 4;
//    int total_rows = pow(n, m);
//
//    std::vector<std::vector<int>> combination_matrix;
//    std::vector<int> current_row;
//
//    generate_combination_matrix(combination_matrix, current_row, m, n, 0);
//
//    // matrixの中身を表示する例
//    for (int i = 0; i < total_rows; i++) {
//        for (int j = 0; j < m; j++) {
//            std::cout << combination_matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//
//    return 0;
//}
