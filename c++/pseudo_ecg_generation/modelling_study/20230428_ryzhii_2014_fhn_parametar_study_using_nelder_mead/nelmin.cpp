//#include <iostream>
//#include <iomanip>
//#include <cmath>
//#include <vector>
//#include "nelmin.h"
//
//using namespace std;
//
//double rosenbrock (vector<double> &x){
// double fx;
// double fx1;
// double fx2;
// fx1 = x[1] - x[0] * x[0];
// fx2 = 1.0 - x[0];
// fx = 100.0 * fx1 * fx1
//   +     fx2 * fx2;
// return fx;
//}
//
//
//double quartic(vector<double> &x){
// double fx;
// int i;
// fx = 0.0;
// for (i = 0; i < 10; i++){
//   fx = fx + x[i] * x[i] * x[i] * x[i];
// }
// return fx;
//}
//
//
//
//
////----------------------------------------------------
//void test01 (){
//
// Data<double> in(2);
//
// //std::cout << in.n<<","<< in.eps <<","<<in.ccoeff <<","<<in.ecoeff <<","<<in.rcoeff <<"\n";
//
// in.start[0] = -1.2;
// in.start[1] = 1.0;
// //in.reqmin = 1.0E-08;
// //in.step[0] = 1.0;
// //in.step[1] = 1.0;
// //in.konvge = 10;
// //in.kcount = 500;
//
// //cout <<in.eps<<"\n";
//
// nelmin (rosenbrock, &in);
//
// cout << " Return code IFAULT = " << in.ifault << "\n";
// cout << "\n";
// cout << " Estimate of minimizing value X*:\n";
// cout << "\n";
// for (int i = 0; i < in.n; i++){
//  cout << " " << setw(14) << in.xmin[i] << "\n";
// }
// cout << "\n";
// cout << " F(X*) = " << in.ynewlo << "\n";
// cout << "\n";
// cout << " Number of iterations = " << in.icount << "\n";
// cout << " Number of restarts =  " << in.numres << "\n";
// return;
//}
//
//
//
//
//void test04(){
// int n=10, i;
// Data<double> in(n);
//
// for (i = 0; i < n; i++){
//  in.start[i] = 1.0;
// }
// //in.reqmin = 1.0E-08;
// //for (i = 0; i < n; i++){
// // in.step[i] = 1.0;
// //}
// //in.konvge = 10;
// //in.kcount = 500;
//
// //in.ynewlo = quartic (in.start);
// //cout << " F(X) = " << in.ynewlo << "\n";
// nelmin (quartic, &in);
//
// cout << " Return code IFAULT = " << in.ifault << "\n";
// cout << "\n";
// cout << " Estimate of minimizing value X*:\n";
// cout << "\n";
// for (i = 0; i < n; i++){
//  cout << " " << setw(14) << in.xmin[i] << "\n";
// }
// cout << "\n";
// cout << " F(X*) = " << in.ynewlo << "\n";
// cout << "\n";
// cout << " Number of iterations = " << in.icount << "\n";
// cout << " Number of restarts =  " << in.numres << "\n";
// return;
//}
//
//
//
//
//
//
//
//
//
//int main (){
// test01();
// test04();
// return 0;
//}
