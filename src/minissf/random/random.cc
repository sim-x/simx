#include <math.h>
#include <time.h>
#include "random/random.h"
#include "kernel/throwable.h"

namespace minissf {

#define PI 3.1415926535897932384626433832795

int Random::global_seed = 0;

double Random::uniform(double a, double b)
{
  if(a > b) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  if(a == b) return a;
  else return a+(b-a)*(*this)();
}

double Random::exponential(double x)
{
  if(x <= 0) SSF_THROW("invalid argument: x=" << x);
  double result = -1.0/x*log(1.0-(*this)());
  return result;
}

double Random::erlang(long n, double x)
{  
  if(n <= 0 || x <= 0) 
    SSF_THROW("invalid arguments: n=" << n << ", x=" << x);
  double y = 0.0;
  for(long i=0; i<n; i++) y += exponential(x);
  return y;
}

double Random::pareto(double k, double a)
{
  if(k <= 0 || a <= 0) 
    SSF_THROW("invalid arguments: k=" << k << ", a=" << a);
  return k*pow(1.0-(*this)(), -1.0/a);
}

double Random::normal(double m, double s)
{
  if(s <= 0) SSF_THROW("invalid arguments: m=" << m << ", s=" << s);

  /* Uses a very accurate approximation of the normal idf due to Odeh & Evans, */
  /* J. Applied Statistics, 1974, vol 23, pp 96-97.                            */
  double p0 = 0.322232431088;        double q0 = 0.099348462606;
  double p1 = 1.0;                   double q1 = 0.588581570495;
  double p2 = 0.342242088547;        double q2 = 0.531103462366;
  double p3 = 0.204231210245e-1;     double q3 = 0.103537752850;
  double p4 = 0.453642210148e-4;     double q4 = 0.385607006340e-2;
  double u, t, p, q, z;

  u = (*this)();
  if(u < 0.5) t = sqrt(-2.0*log(u));
  else t = sqrt(-2.0*log(1.0-u));
  p = p0+t*(p1+t*(p2+t*(p3+t*p4)));
  q = q0+t*(q1+t*(q2+t*(q3+t*q4)));
  if(u < 0.5) z = (p/q)-t;
  else z = t-(p/q);
  return m+s*z;
}

double Random::lognormal(double a, double b)
{
  if(b <= 0) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  double result = exp(a+b*normal(0.0, 1.0));
  return result;
}

double Random::chisquare(long n)
{ 
  if(n <= 0) SSF_THROW("invalid argument: n=" << n);
  double y = 0.0;
  for(long i=0; i<n; i++) {
    double z = normal(0.0, 1.0);
    y += z*z;
  }
  return y;
}

double Random::student(long n)
{
  if(n <= 0) SSF_THROW("invalid argument: n=" << n);
  double result = normal(0.0, 1.0)/sqrt(chisquare(n)/n);
  return result;
}

long Random::bernoulli(double p)
{
  if(p < 0 || p > 1) 
    SSF_THROW("invalid argument: p=" << p);
  long result = (*this)()<p ? 1 : 0;
  return result;
}

long Random::equilikely(long a, long b)
{
  if(a > b) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  double x = (*this)();
  long v = a+(long)floor((b-a+1)*x);
  return v;
}

long Random::binomial(long n, double p)
{ 
  if(n <= 0 || p < 0 || p > 1) 
    SSF_THROW("invalid arguments: n=" << n << ", p=" << p);
  long y = 0;
  for(long i=0; i<n; i++) y += bernoulli(p);
  return y;
}

long Random::geometric(double p)
{
  if(p <= 0 || p >= 1) 
    SSF_THROW("invalid argument: p=" << p);
  long result = 1+(long)(log(1.0-(*this)())/log(1.0-p));
  return result;
}

long Random::pascal(long n, double p)
{ 
  if(n <= 0 || p <= 0 || p >= 1) 
    SSF_THROW("invalid arguments: n=" << n << ", p=" << p);
  long y = 0;
  for(long i=0; i<n; i++) y += geometric(p);
  return y;
}

long Random::poisson(double m)
{ 
  if(m <= 0) SSF_THROW("invalid argument: m=" << m);
  if(m < 30.0) {
    long y = 0;
    double t = exp(m);
    do {
      y++;
      t *= (*this)();
    } while(t >= 1.0);
    y--;
    return y;
  } else {
    // This is the rejection method from "The Computer Generation of
    // Poisson Random Variables", by A. C. Atkinson, Journal of the
    // Royal Statistical Society Series C (Applied Statistics)
    // Vol. 28, No. 1. (1979), pages 29-35.
    double c = 0.767 - 3.36/m;
    double beta = PI/sqrt(3.0*m);
    double alpha = beta*m;
    double k = log(c) - m - log(beta);
    for(;;) {
      double u = (*this)();
      double x = (alpha - log((1.0 - u)/u))/beta;
      int n = (int) floor(x + 0.5);
      if (n < 0) continue;
      double v = (*this)();
      double y = alpha - beta*x;
      double temp = 1.0 + exp(y);
      double lhs = y + log(v/(temp*temp));
      double rhs = k + n*log(m) - logfactorial(n);
      if (lhs <= rhs) return n;
    }
  }
}

void Random::permute(long n, long* array)
{
  if(n <= 0 || !array) 
    SSF_THROW("invalid arguments: n=" << n << ", array=" << array);
  int i;
  for(i=0; i<n; i++) array[i] = i;
  for(i=0; i<n-1; i++) {
    long j = equilikely(i, n-1);
    if(i != j) {
      long tmp = array[i];
      array[i] = array[j];
      array[j] = tmp;
    }
  }
}

// copied from SPRNG for generating a random seed from the machine time
int Random::make_new_seed() 
{
  time_t tp;
  struct tm *temp;
  unsigned int temp2, temp3;
  static unsigned int temp4 = 0xe0e1;
  
  time(&tp);
  temp = localtime(&tp);
  temp2 = (temp->tm_sec<<26)+(temp->tm_min<<20)+(temp->tm_hour<<15)+
    (temp->tm_mday<<10)+(temp->tm_mon<<6);
  temp3 = (temp->tm_year<<13)+(temp->tm_wday<<10)+(temp->tm_yday<<1)+
    temp->tm_isdst;
  temp2 ^= clock()^temp3;
  temp4 = (temp4*0xeeee)%0xffff;
  temp2 ^= temp4<<16;
  temp4 = (temp4*0xaeee)%0xffff;
  temp2 ^= temp4;
  temp2 &= 0x7fffffff;
  return temp2;
}

double Random::logfactorial(int n)
{
  if(n < 0) { SSF_THROW("invalid argument: n=" << n); }  
  if(n > 254) {
    double x = n + 1;
    return (x - 0.5)*log(x) - x + 0.5*log(2*PI) + 1.0/(12.0*x);
  } else {
    double lf[] = {
      0.000000000000000,
      0.000000000000000,
      0.693147180559945,
      1.791759469228055,
      3.178053830347946,
      4.787491742782046,
      6.579251212010101,
      8.525161361065415,
      10.604602902745251,
      12.801827480081469,
      15.104412573075516,
      17.502307845873887,
      19.987214495661885,
      22.552163853123421,
      25.191221182738683,
      27.899271383840894,
      30.671860106080675,
      33.505073450136891,
      36.395445208033053,
      39.339884187199495,
      42.335616460753485,
      45.380138898476908,
      48.471181351835227,
      51.606675567764377,
      54.784729398112319,
      58.003605222980518,
      61.261701761002001,
      64.557538627006323,
      67.889743137181526,
      71.257038967168000,
      74.658236348830158,
      78.092223553315307,
      81.557959456115029,
      85.054467017581516,
      88.580827542197682,
      92.136175603687079,
      95.719694542143202,
      99.330612454787428,
      102.968198614513810,
      106.631760260643450,
      110.320639714757390,
      114.034211781461690,
      117.771881399745060,
      121.533081515438640,
      125.317271149356880,
      129.123933639127240,
      132.952575035616290,
      136.802722637326350,
      140.673923648234250,
      144.565743946344900,
      148.477766951773020,
      152.409592584497350,
      156.360836303078800,
      160.331128216630930,
      164.320112263195170,
      168.327445448427650,
      172.352797139162820,
      176.395848406997370,
      180.456291417543780,
      184.533828861449510,
      188.628173423671600,
      192.739047287844900,
      196.866181672889980,
      201.009316399281570,
      205.168199482641200,
      209.342586752536820,
      213.532241494563270,
      217.736934113954250,
      221.956441819130360,
      226.190548323727570,
      230.439043565776930,
      234.701723442818260,
      238.978389561834350,
      243.268849002982730,
      247.572914096186910,
      251.890402209723190,
      256.221135550009480,
      260.564940971863220,
      264.921649798552780,
      269.291097651019810,
      273.673124285693690,
      278.067573440366120,
      282.474292687630400,
      286.893133295426990,
      291.323950094270290,
      295.766601350760600,
      300.220948647014100,
      304.686856765668720,
      309.164193580146900,
      313.652829949878990,
      318.152639620209300,
      322.663499126726210,
      327.185287703775200,
      331.717887196928470,
      336.261181979198450,
      340.815058870798960,
      345.379407062266860,
      349.954118040770250,
      354.539085519440790,
      359.134205369575340,
      363.739375555563470,
      368.354496072404690,
      372.979468885689020,
      377.614197873918670,
      382.258588773060010,
      386.912549123217560,
      391.575988217329610,
      396.248817051791490,
      400.930948278915760,
      405.622296161144900,
      410.322776526937280,
      415.032306728249580,
      419.750805599544780,
      424.478193418257090,
      429.214391866651570,
      433.959323995014870,
      438.712914186121170,
      443.475088120918940,
      448.245772745384610,
      453.024896238496130,
      457.812387981278110,
      462.608178526874890,
      467.412199571608080,
      472.224383926980520,
      477.044665492585580,
      481.872979229887900,
      486.709261136839360,
      491.553448223298010,
      496.405478487217580,
      501.265290891579240,
      506.132825342034830,
      511.008022665236070,
      515.890824587822520,
      520.781173716044240,
      525.679013515995050,
      530.584288294433580,
      535.496943180169520,
      540.416924105997740,
      545.344177791154950,
      550.278651724285620,
      555.220294146894960,
      560.169054037273100,
      565.124881094874350,
      570.087725725134190,
      575.057539024710200,
      580.034272767130800,
      585.017879388839220,
      590.008311975617860,
      595.005524249382010,
      600.009470555327430,
      605.020105849423770,
      610.037385686238740,
      615.061266207084940,
      620.091704128477430,
      625.128656730891070,
      630.172081847810200,
      635.221937855059760,
      640.278183660408100,
      645.340778693435030,
      650.409682895655240,
      655.484856710889060,
      660.566261075873510,
      665.653857411105950,
      670.747607611912710,
      675.847474039736880,
      680.953419513637530,
      686.065407301994010,
      691.183401114410800,
      696.307365093814040,
      701.437263808737160,
      706.573062245787470,
      711.714725802289990,
      716.862220279103440,
      722.015511873601330,
      727.174567172815840,
      732.339353146739310,
      737.509837141777440,
      742.685986874351220,
      747.867770424643370,
      753.055156230484160,
      758.248113081374300,
      763.446610112640200,
      768.650616799717000,
      773.860102952558460,
      779.075038710167410,
      784.295394535245690,
      789.521141208958970,
      794.752249825813460,
      799.988691788643450,
      805.230438803703120,
      810.477462875863580,
      815.729736303910160,
      820.987231675937890,
      826.249921864842800,
      831.517780023906310,
      836.790779582469900,
      842.068894241700490,
      847.352097970438420,
      852.640365001133090,
      857.933669825857460,
      863.231987192405430,
      868.535292100464630,
      873.843559797865740,
      879.156765776907600,
      884.474885770751830,
      889.797895749890240,
      895.125771918679900,
      900.458490711945270,
      905.796028791646340,
      911.138363043611210,
      916.485470574328820,
      921.837328707804890,
      927.193914982476710,
      932.555207148186240,
      937.921183163208070,
      943.291821191335660,
      948.667099599019820,
      954.046996952560450,
      959.431492015349480,
      964.820563745165940,
      970.214191291518320,
      975.612353993036210,
      981.015031374908400,
      986.422203146368590,
      991.833849198223450,
      997.249949600427840,
      1002.670484599700300,
      1008.095434617181700,
      1013.524780246136200,
      1018.958502249690200,
      1024.396581558613400,
      1029.838999269135500,
      1035.285736640801600,
      1040.736775094367400,
      1046.192096209724900,
      1051.651681723869200,
      1057.115513528895000,
      1062.583573670030100,
      1068.055844343701400,
      1073.532307895632800,
      1079.012946818975000,
      1084.497743752465600,
      1089.986681478622400,
      1095.479742921962700,
      1100.976911147256000,
      1106.478169357800900,
      1111.983500893733000,
      1117.492889230361000,
      1123.006317976526100,
      1128.523770872990800,
      1134.045231790853000,
      1139.570684729984800,
      1145.100113817496100,
      1150.633503306223700,
      1156.170837573242400,
    };
    return lf[n];
  }
}

}; /*namespace minissf*/


/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
