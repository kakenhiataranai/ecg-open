#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <numeric>

std::mt19937_64 mt(time(0));

double randuniform(double in){
 std::uniform_real_distribution<> d(0.0, in);
 return d(mt);
}

double hokan(double xmin, double xmax, std::vector<double> *y, double xin){
 if(xin<xmin || xin >xmax){std::cout << "out of range at hokan." <<std::endl;}
 if(xmax==xmin){std::cout << "xmax=xmin at hokan." <<std::endl;}
 double ystep=(xmax-xmin)/((double)y->size()-1.);
 double alpha=(xin-xmin)/ystep;
 double alphaminusintalpha=alpha-(int)alpha;
 return(
  (1.-alphaminusintalpha)*(*y)[(int)alpha]
  +alphaminusintalpha*(*y)[(int)alpha+1]
 );
}


std::vector<double> get_T_const(int size, double rr){
 std::vector<double> o(size, rr);
 return o;
}

std::vector<double> get_RR_delay_s(int size, double rr, int dim, double stdev){
 //averaged dim is 5.8375
 //averaged R-R is 0.8609
 //averaged stdev of R-R is 0.0767

 //Error
 if(size<dim*2){std::cout << "size < dim*2 \n";exit(-1);}

 //Init
 std::vector<double> t(int(size*1.2)); //discarding rate is 0.2
 for(int i=0;i<dim;i++){
  t[i]=rr;
 }
 //Main
 for(int i=dim; i<t.size(); i++){
  if(fabs(t[i-dim]-rr)>stdev){t[i]=t[i-dim];}
  else{
   double b=sqrt(stdev*stdev-(t[i-dim]-rr)*(t[i-dim]-rr));
   t[i]=rr+randuniform(2.*b)-b;//assumed a circle
  }
 }

 //Discard
 std::vector<double> o(size);
 auto tfirst=t.end();
 tfirst-=size;
 std::copy(tfirst, t.end(), o.begin());
 return o;
}

std::vector<double> RR2T(std::vector<double> &in, double xaxis_adding){
 std::vector<double> xaxis(in.size());
 exclusive_scan(in.begin(),in.end(),xaxis.begin(),0.);

 std::vector<double> o;
 std::vector<double> y(2);

 for(int i=1;i<in.size();i++){
  double t=xaxis[i-1];
  y[0]=in[i-1];
  y[1]=in[i];
  while(t<xaxis[i]){
   o.push_back(hokan(xaxis[i-1], xaxis[i], &y ,t));
   t+=xaxis_adding;
  }
 }
 return o;
}

//int main(){
// //std::vector<double> get_T_delay_s(10000, double rr, int dim, double stdev){
// std::vector<double> tmp=get_RR_delay_s(600, 0.86, 6, 0.07);
// std::vector<double> kekka=RR2T(tmp, 0.002);
// for(auto i:kekka){std::cout << i << std::endl;}
//}
