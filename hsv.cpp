#include <iostream>
#include <cmath>

void rgb_to_hsv( double* H, double* S, double* V,
                 double  R, double  G, double  B)
{
  const double M = std::max(std::max(R,G),B);
  const double m = std::min(std::min(R,G),B);
  const double C = M - m;
  double H_prime = 0;
  if( C == 0 ) {
    //const unsigned long long raw_nan = 0x7ff8000000000000;
    H_prime = NAN;// *( double* )&raw_nan;
  }
  else if( M == R ) {
    H_prime = fmod((G-B)/C,6.0);
  }
  else if( M == G ) {
    H_prime = (B-R)/C+2;
  }
  else if( M == B ) {
    H_prime = (R-G)/C+4;
  }
  *H = 60*H_prime; // H is in degrees now
  *V = M;
  if( C == 0 ) {
    *S = 0;
  }
  else {
    *S = C/(*V);
  }
}

void hsv_to_rgb( double* R, double* G, double* B,
                 double  H, double  S, double  V )
{
  const double C = V*S;
  const double H_prime = H/60;
  const double X = C*(1.0 - fabs(fmod(H_prime,2.0) - 1.0));
  double R_1=0, G_1=0, B_1=0;
  if( isnan(H_prime) ) {
  }
  else if( H_prime < 1 ) {
    R_1 = C;
    G_1 = X;
  }
  else if( H_prime < 2 ) {
    R_1 = X;
    G_1 = C;
  }
  else if( H_prime < 3 ) {
    G_1 = C;
    B_1 = X;
  }
  else if( H_prime < 4 ) {
    G_1 = X;
    B_1 = C;
  }
  else if( H_prime < 5 ) {
    R_1 = X;
    B_1 = C;
  }
  else if( H_prime < 6 ) {
    R_1 = C;
    B_1 = X;
  }
  const double m = V - C;
  *R = R_1 + m;
  *G = G_1 + m;
  *B = B_1 + m;
}

int main()
{
  double H=0, S=0, V=0;
  //double R=212/256.0, G=225/256.0, B=242/256.0;
  double R=254.0/256.0, G=255/256.0, B=0.0/256.0;
  rgb_to_hsv(&H,&S,&V,
             R, G, B);
  std::cout << "H="<<H<<",S="<<(S*256)<<",V="<<(V*256)<<std::endl;
  double dS = S/4.0;
  for( double S_1 = S; S_1 >= 0; S_1 -= dS )//, V += 0.01 )
  {
    double R_1=0,G_1=0,B_1=0;
    hsv_to_rgb(&R_1,&G_1,&B_1, H, S_1, V);
    //std::cout << "H="<<H<<",S_1="<<S_1<<",V="<<V<<std::endl;
    std::cout << "R="<<(int)(256*R_1)<<",G="<<(int)(256*G_1)<<",B="<<(int)(256*B_1)<<std::endl;
  }
}
