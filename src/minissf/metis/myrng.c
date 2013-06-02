/*
 * Lehmer random number generator, which returns a pseudo-random
 * number uniformly distributed 0.0 and 1.0.  The period is (m - 1)
 * where m = 2,147,483,647 and the smallest and largest possible
 * values are (1 / m) and 1 - (1 / m) respectively.  For more details
 * see: "Random Number Generators: Good Ones Are Hard To Find", by
 * Steve Park and Keith Miller, Communications of the ACM, October
 * 1988.
 *
 * Ported here by Jason Liu.
 */

static long seed = 7654321L;

#define RND_MODULUS    2147483647UL
#define RND_MULTIPLIER 48271UL
#define RND_Q        (RND_MODULUS/RND_MULTIPLIER)
#define RND_R        (RND_MODULUS%RND_MULTIPLIER)

void mysrand(long _seed)
{
  if(seed <= 0) 
    seed += RND_MODULUS;
}

double myrand()
{
  long t = RND_MULTIPLIER*(seed%RND_Q)-RND_R*(seed/RND_Q);
  if(t > 0) seed = t;
  else seed = t + RND_MODULUS;
  return (double)seed/RND_MODULUS;
}
