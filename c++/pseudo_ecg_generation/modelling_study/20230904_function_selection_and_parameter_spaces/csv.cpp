
#include "csv.h"



csv::csv() {}

void csv::create_output_folder(const std::string& folder_name) {

 if (!std::filesystem::exists(folder_name)) {
  try {
   std::filesystem::create_directory(folder_name);
   std::cout << "フォルダが作成されました: " << folder_name << std::endl;
  }
  catch (const std::filesystem::filesystem_error& e) {
   std::cerr << "フォルダの作成中にエラーが発生しました: " << e.what() << std::endl;
  }
 }
 else {
  std::cout << "フォルダは既に存在します: " << folder_name << std::endl;
 }

}


std::vector<std::vector<double>> csv::read_csv(const std::string& file_name, const std::string& folder_name) {
 std::vector<std::vector<double>> data;
 std::ifstream file(folder_name + "/" + file_name);

 if (!file.is_open()) {
  std::cerr << "Failed to open file: " << file_name << std::endl;
  return data;
 }

 std::string line;
 while (std::getline(file, line)) {
  std::vector<double> row;
  std::istringstream iss(line);
  std::string value;  

  while (std::getline(iss, value, ',')) {
   try {
	double number = std::stod(value);
	row.push_back(number);
   }
   catch (const std::exception& e) {
	std::cerr << "Error converting value to double: " << value << std::endl;
   }
  }
  data.push_back(row);  
 }

 file.close();
 return data;
}


std::vector<std::string> csv::get_csv_file_name_vector(const std::string& folder_path) {

 std::vector<std::string> res;
 for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {

  if (entry.is_regular_file() && entry.path().extension() == ".csv") {
   std::string filename = entry.path().filename().string();
   res.push_back(filename);
  }
 }
 return res;

}


void csv::one_dimension_vector_double_to_csv(std::vector<double> vector_name, const std::string& file_name) {

 int row_num = vector_name.size();

 /* Open a file stream for writing */
 std::ofstream file(file_name);

 /*  Write the data to the file */
 for (int i = 0; i < row_num; i++) {
   /* Write the element to the file */
   file << vector_name[i];
   /* Add a comma after the element, unless it's the last element in the row */
   //file << ",";
   /* Add a new line after each row */
   file << "\n";
  } 
 file.close();
}


void csv::two_dimension_vector_double_to_csv(std::vector<std::vector<double>> vector_name, const std::string& file_name) {

 int row_num = vector_name.size();
 int column_num = vector_name.at(0).size();

 /* Open a file stream for writing */
 std::ofstream file(file_name);

 /*  Write the data to the file */
 for (int i = 0; i < row_num; i++) {
  for (int j = 0; j < column_num; j++) {
   /* Write the element to the file */
   file << vector_name[i][j];
   /* Add a comma after the element, unless it's the last element in the row */
   if (j < column_num - 1) {
	file << ",";
   }
  }
  /* Add a new line after each row */
  file << "\n";
 }
 file.close();

}

void csv::two_dimension_vector_double_to_csv(std::vector<std::vector<double>> vector_name, const std::string& file_name, const std::string& folder_name) {

 int row_num = vector_name.size();
 int column_num = vector_name.at(0).size();

 /* Open a file stream for writing */
 std::ofstream file(folder_name + "/" + file_name);

 /*  Write the data to the file */
 for (int i = 0; i < row_num; i++) {
  for (int j = 0; j < column_num; j++) {
   /* Write the element to the file */
   file << vector_name[i][j];
   /* Add a comma after the element, unless it's the last element in the row */
   if (j < column_num - 1) {
	file << ",";
   }
  }
  /* Add a new line after each row */
  file << "\n";
 }
 file.close();

}