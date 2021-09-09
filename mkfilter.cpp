
#include <string.h>
#include "stdafx.h"
#include <math.h>
#include "mkfilter.h"
#define global
#define unless(x)   if(!(x))
#define until(x)    while(!(x))

#define VERSION	    "4.6"
#undef	PI
#define PI	    3.14159265358979323846  /* Microsoft C++ does not define M_PI ! */
#define TWOPI	    (2.0 * PI)
#define EPS	    1e-10
#define MAXORDER    10
#define MAXPZ	    512	    /* .ge. 2*MAXORDER, to allow for doubling of poles in BP filter;
			       high values needed for FIR filters */
#define MAXSTRING   256

typedef void (*proc)();
typedef unsigned int uint;

char ffile[2048];
extern "C"
  { double atof(const char*);
    int atoi(char*);
    void exit(int);
  };

extern char *progname;
extern void readdata(char*, double&, int&, double*, int&, double*);

inline double sqr(double x)	    { return x*x;			       }
//inline bool seq(char *s1, char *s2) { return strcmp(s1,s2) == 0;	       }
inline bool onebit(uint m)	    { return (m != 0) && ((m & m-1) == 0);     }

inline double asinh(double x)
  { /* Microsoft C++ does not define */
    return log(x + sqrt(1.0 + sqr(x)));
  }

inline double fix(double x)
  { /* nearest integer */
    return (x >= 0.0) ? floor(0.5+x) : -floor(0.5-x);
  }


#define opt_be 0x00001	/* -Be		Bessel characteristic	       */
#define opt_bu 0x00002	/* -Bu		Butterworth characteristic     */
#define opt_ch 0x00004	/* -Ch		Chebyshev characteristic       */
#define opt_re 0x00008	/* -Re		Resonator		       */
#define opt_pi 0x00010	/* -Pi		proportional-integral	       */

#define opt_lp 0x00020	/* -Lp		lowpass			       */
#define opt_hp 0x00040	/* -Hp		highpass		       */
#define opt_bp 0x00080	/* -Bp		bandpass		       */
#define opt_bs 0x00100	/* -Bs		bandstop		       */
#define opt_ap 0x00200	/* -Ap		allpass			       */

#define opt_a  0x00400	/* -a		alpha value		       */
#define opt_l  0x00800	/* -l		just list filter parameters    */
#define opt_o  0x01000	/* -o		order of filter		       */
#define opt_p  0x02000	/* -p		specified poles only	       */
#define opt_w  0x04000	/* -w		don't pre-warp		       */
#define opt_z  0x08000	/* -z		use matched z-transform	       */
#define opt_Z  0x10000	/* -Z		additional zero		       */
#define opt_f  0x20000	/* -f		save on file		       */
#define opt_s  0x40000	/* -Z		output in machinereadableformat		       */



struct c_complex
  { double re, im;
  };

struct complex
  { double re, im;
    complex(double r, double i = 0.0) { re = r; im = i; }
    complex() { }					/* uninitialized complex */
    complex(c_complex z) { re = z.re; im = z.im; }	/* init from denotation */
  };

extern complex csqrt(complex), cexp(complex), expj(double);	    /* from complex.C */
extern complex evaluate(complex[], int, complex[], int, complex);   /* from complex.C */

inline double hypot(complex z) { return ::hypot(z.im, z.re); }
inline double atan2(complex z) { return ::atan2(z.im, z.re); }

inline complex cconj(complex z)
  { z.im = -z.im;
    return z;
  }

inline complex operator * (double a, complex z)
  { z.re *= a; z.im *= a;
    return z;
  }

inline complex operator / (complex z, double a)
  { z.re /= a; z.im /= a;
    return z;
  }

inline void operator /= (complex &z, double a)
  { z = z / a;
  }

extern complex operator * (complex, complex);
extern complex operator / (complex, complex);

inline complex operator + (complex z1, complex z2)
  { z1.re += z2.re;
    z1.im += z2.im;
    return z1;
  }

inline complex operator - (complex z1, complex z2)
  { z1.re -= z2.re;
    z1.im -= z2.im;
    return z1;
  }

inline complex operator - (complex z)
  { return 0.0 - z;
  }

inline bool operator == (complex z1, complex z2)
  { return (z1.re == z2.re) && (z1.im == z2.im);
  }

inline complex sqr(complex z)
  { return z*z;
  }



static complex eval(complex[], int, complex);
static double Xsqrt(double);


global complex evaluate(complex topco[], int nz, complex botco[], int np, complex z)
  { /* evaluate response, substituting for z */
    return eval(topco, nz, z) / eval(botco, np, z);
  }

static complex eval(complex coeffs[], int npz, complex z)
  { /* evaluate polynomial in z, substituting for z */
    complex sum = complex(0.0);
    for (int i = npz; i >= 0; i--) sum = (sum * z) + coeffs[i];
    return sum;
  }

global complex csqrt(complex x)
  { double r = hypot(x);
    complex z = complex(Xsqrt(0.5 * (r + x.re)),
			Xsqrt(0.5 * (r - x.re)));
    if (x.im < 0.0) z.im = -z.im;
    return z;
  }

static double Xsqrt(double x)
  { /* because of deficiencies in hypot on Sparc, it's possible for arg of Xsqrt to be small and -ve,
       which logically it can't be (since r >= |x.re|).	 Take it as 0. */
    return (x >= 0.0) ? sqrt(x) : 0.0;
  }

global complex cexp(complex z)
  { return exp(z.re) * expj(z.im);
  }

global complex expj(double theta)
  { return complex(cos(theta), sin(theta));
  }

global complex operator * (complex z1, complex z2)
  { return complex(z1.re*z2.re - z1.im*z2.im,
		   z1.re*z2.im + z1.im*z2.re);
  }

global complex operator / (complex z1, complex z2)
  { double mag = (z2.re * z2.re) + (z2.im * z2.im);
    return complex (((z1.re * z2.re) + (z1.im * z2.im)) / mag,
		    ((z1.im * z2.re) - (z1.re * z2.im)) / mag);
  }



struct pzrep
  { complex poles[MAXPZ], zeros[MAXPZ];
    int numpoles, numzeros;
  };

static pzrep splane, zplane;
static int order;
static double raw_alpha1, raw_alpha2, raw_alphaz;
static complex dc_gain, fc_gain, hf_gain;
static uint options;
static double warped_alpha1, warped_alpha2, chebrip, qfactor;
static bool infq;
static uint polemask;
static double xcoeffs[MAXPZ+1], ycoeffs[MAXPZ+1];

static c_complex bessel_poles[] =
  { /* table produced by /usr/fisher/bessel --	N.B. only one member of each C.Conj. pair is listed */
    { -1.00000000000e+00, 0.00000000000e+00}, { -1.10160133059e+00, 6.36009824757e-01},
    { -1.32267579991e+00, 0.00000000000e+00}, { -1.04740916101e+00, 9.99264436281e-01},
    { -1.37006783055e+00, 4.10249717494e-01}, { -9.95208764350e-01, 1.25710573945e+00},
    { -1.50231627145e+00, 0.00000000000e+00}, { -1.38087732586e+00, 7.17909587627e-01},
    { -9.57676548563e-01, 1.47112432073e+00}, { -1.57149040362e+00, 3.20896374221e-01},
    { -1.38185809760e+00, 9.71471890712e-01}, { -9.30656522947e-01, 1.66186326894e+00},
    { -1.68436817927e+00, 0.00000000000e+00}, { -1.61203876622e+00, 5.89244506931e-01},
    { -1.37890321680e+00, 1.19156677780e+00}, { -9.09867780623e-01, 1.83645135304e+00},
    { -1.75740840040e+00, 2.72867575103e-01}, { -1.63693941813e+00, 8.22795625139e-01},
    { -1.37384121764e+00, 1.38835657588e+00}, { -8.92869718847e-01, 1.99832584364e+00},
    { -1.85660050123e+00, 0.00000000000e+00}, { -1.80717053496e+00, 5.12383730575e-01},
    { -1.65239648458e+00, 1.03138956698e+00}, { -1.36758830979e+00, 1.56773371224e+00},
    { -8.78399276161e-01, 2.14980052431e+00}, { -1.92761969145e+00, 2.41623471082e-01},
    { -1.84219624443e+00, 7.27257597722e-01}, { -1.66181024140e+00, 1.22110021857e+00},
    { -1.36069227838e+00, 1.73350574267e+00}, { -8.65756901707e-01, 2.29260483098e+00},
  };

static void readcmdline(char*[]);
static uint decodeoptions(char*), optbit(char);
static double getfarg(char*);
static int getiarg(char*);
static void usage(), checkoptions(), opterror(char*, int = 0, int = 0), setdefaults();
static void compute_s(), choosepole(complex), prewarp(), normalize(), compute_z_blt();
static complex blt(complex);
static void compute_z_mzt();
static void compute_notch(), compute_apres();
static complex reflectg(complex);
static void compute_bpres(), add_extra_zero();
static void expandpoly(), expand(complex[], int, complex[]), multin(complex, int, complex[]);
static void printresults(char*[]), printcmdline(char*[]), printfilter(), printgain(char*, complex);
static void printcoeffs(char*, int, double[]);
static void printrat_s(), printrat_z(), printpz(complex*, int), printrecurrence(), prcomplex(complex);


static void setdefaults()
  { unless (options & opt_p) polemask = ~0; /* use all poles */
    unless (options & (opt_bp | opt_bs)) raw_alpha2 = raw_alpha1;
  }

static void compute_s() /* compute S-plane poles for prototype LP filter */
  { splane.numpoles = 0;
    if (options & opt_be)
      { /* Bessel filter */
  int p = (order*order)/4; /* ptr into table */
  if (order & 1) choosepole(bessel_poles[p++]);
  for (int i = 0; i < order/2; i++)
    { choosepole(bessel_poles[p]);
      choosepole(cconj(bessel_poles[p]));
      p++;
    }
      }
    if (options & (opt_bu | opt_ch))
      { /* Butterworth filter */
  for (int i = 0; i < 2*order; i++)
    { double theta = (order & 1) ? (i*PI) / order : ((i+0.5)*PI) / order;
      choosepole(expj(theta));
    }
      }
    if (options & opt_ch)
      { /* modify for Chebyshev (p. 136 DeFatta et al.) */
  if (chebrip >= 0.0)
    { 
      exit(1);
    }
  double rip = pow(10.0, -chebrip / 10.0);
  double eps = sqrt(rip - 1.0);
  double y = asinh(1.0 / eps) / (double) order;
  if (y <= 0.0)
    { 
      exit(1);
    }
  for (int i = 0; i < splane.numpoles; i++)
    { splane.poles[i].re *= sinh(y);
      splane.poles[i].im *= cosh(y);
    }
      }
  }

static void choosepole(complex z)
  { if (z.re < 0.0)
      { if (polemask & 1) splane.poles[splane.numpoles++] = z;
  polemask >>= 1;
      }
  }

static void prewarp()
  { /* for bilinear transform, perform pre-warp on alpha values */
    if (options & (opt_w | opt_z))
      { warped_alpha1 = raw_alpha1;
  warped_alpha2 = raw_alpha2;
      }
    else
      { warped_alpha1 = tan(PI * raw_alpha1) / PI;
  warped_alpha2 = tan(PI * raw_alpha2) / PI;
      }
  }

static void normalize()		/* called for trad, not for -Re or -Pi */
  { double w1 = TWOPI * warped_alpha1;
    double w2 = TWOPI * warped_alpha2;
    /* transform prototype into appropriate filter type (lp/hp/bp/bs) */
    switch (options & (opt_lp | opt_hp | opt_bp| opt_bs))
      { case opt_lp:
    { for (int i = 0; i < splane.numpoles; i++) splane.poles[i] = splane.poles[i] * w1;
      splane.numzeros = 0;
      break;
    }

  case opt_hp:
    { int i;
      for (i=0; i < splane.numpoles; i++) splane.poles[i] = w1 / splane.poles[i];
      for (i=0; i < splane.numpoles; i++) splane.zeros[i] = 0.0;	 /* also N zeros at (0,0) */
      splane.numzeros = splane.numpoles;
      break;
    }

  case opt_bp:
    { double w0 = sqrt(w1*w2), bw = w2-w1; int i;
      for (i=0; i < splane.numpoles; i++)
        { complex hba = 0.5 * (splane.poles[i] * bw);
    complex temp = csqrt(1.0 - sqr(w0 / hba));
    splane.poles[i] = hba * (1.0 + temp);
    splane.poles[splane.numpoles+i] = hba * (1.0 - temp);
        }
      for (i=0; i < splane.numpoles; i++) splane.zeros[i] = 0.0;	 /* also N zeros at (0,0) */
      splane.numzeros = splane.numpoles;
      splane.numpoles *= 2;
      break;
    }

  case opt_bs:
    { double w0 = sqrt(w1*w2), bw = w2-w1; int i;
      for (i=0; i < splane.numpoles; i++)
        { complex hba = 0.5 * (bw / splane.poles[i]);
    complex temp = csqrt(1.0 - sqr(w0 / hba));
    splane.poles[i] = hba * (1.0 + temp);
    splane.poles[splane.numpoles+i] = hba * (1.0 - temp);
        }
      for (i=0; i < splane.numpoles; i++)	   /* also 2N zeros at (0, +-w0) */
        { splane.zeros[i] = complex(0.0, +w0);
    splane.zeros[splane.numpoles+i] = complex(0.0, -w0);
        }
      splane.numpoles *= 2;
      splane.numzeros = splane.numpoles;
      break;
    }
      }
  }

static void compute_z_blt() /* given S-plane poles & zeros, compute Z-plane poles & zeros, by bilinear transform */
  { int i;
    zplane.numpoles = splane.numpoles;
    zplane.numzeros = splane.numzeros;
    for (i=0; i < zplane.numpoles; i++) zplane.poles[i] = blt(splane.poles[i]);
    for (i=0; i < zplane.numzeros; i++) zplane.zeros[i] = blt(splane.zeros[i]);
    while (zplane.numzeros < zplane.numpoles) zplane.zeros[zplane.numzeros++] = -1.0;
  }

static complex blt(complex pz)
  { return (2.0 + pz) / (2.0 - pz);
  }

static void compute_z_mzt() /* given S-plane poles & zeros, compute Z-plane poles & zeros, by matched z-transform */
  { int i;
    zplane.numpoles = splane.numpoles;
    zplane.numzeros = splane.numzeros;
    for (i=0; i < zplane.numpoles; i++) zplane.poles[i] = cexp(splane.poles[i]);
    for (i=0; i < zplane.numzeros; i++) zplane.zeros[i] = cexp(splane.zeros[i]);
  }

static void compute_notch()
  { /* compute Z-plane pole & zero positions for bandstop resonator (notch filter) */
    compute_bpres();		/* iterate to place poles */
    double theta = TWOPI * raw_alpha1;
    complex zz = expj(theta);	/* place zeros exactly */
    zplane.zeros[0] = zz; zplane.zeros[1] = cconj(zz);
  }
static complex reflect(complex z)
  { double r = hypot(z);
    return z / sqr(r);
  }

static void compute_apres()
  { /* compute Z-plane pole & zero positions for allpass resonator */
    compute_bpres();		/* iterate to place poles */
    zplane.zeros[0] = reflect(zplane.poles[0]);
    zplane.zeros[1] = reflect(zplane.poles[1]);
  }


static void compute_bpres()
  { /* compute Z-plane pole & zero positions for bandpass resonator */
    zplane.numpoles = zplane.numzeros = 2;
    zplane.zeros[0] = 1.0; zplane.zeros[1] = -1.0;
    double theta = TWOPI * raw_alpha1; /* where we want the peak to be */
    if (infq)
      { /* oscillator */
  complex zp = expj(theta);
  zplane.poles[0] = zp; zplane.poles[1] = cconj(zp);
      }
    else
      { /* must iterate to find exact pole positions */
  complex topcoeffs[MAXPZ+1]; expand(zplane.zeros, zplane.numzeros, topcoeffs);
  double r = exp(-theta / (2.0 * qfactor));
  double thm = theta, th1 = 0.0, th2 = PI;
  bool cvg = false;
  for (int i=0; i < 50 && !cvg; i++)
    { complex zp = r * expj(thm);
      zplane.poles[0] = zp; zplane.poles[1] = cconj(zp);
      complex botcoeffs[MAXPZ+1]; expand(zplane.poles, zplane.numpoles, botcoeffs);
      complex g = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, expj(theta));
      double phi = g.im / g.re; /* approx to atan2 */
      if (phi > 0.0) th2 = thm; else th1 = thm;
      if (fabs(phi) < EPS) cvg = true;
      thm = 0.5 * (th1+th2);
    }
  unless (cvg) ;
      }
  }

static void add_extra_zero()
  { if (zplane.numzeros+2 > MAXPZ)
      { ;
  exit(1);
      }
    double theta = TWOPI * raw_alphaz;
    complex zz = expj(theta);
    zplane.zeros[zplane.numzeros++] = zz;
    zplane.zeros[zplane.numzeros++] = cconj(zz);
    while (zplane.numpoles < zplane.numzeros) zplane.poles[zplane.numpoles++] = 0.0;	 /* ensure causality */
  }

static void expandpoly() /* given Z-plane poles & zeros, compute top & bot polynomials in Z, and then recurrence relation */
  { complex topcoeffs[MAXPZ+1], botcoeffs[MAXPZ+1]; int i;
    expand(zplane.zeros, zplane.numzeros, topcoeffs);
    expand(zplane.poles, zplane.numpoles, botcoeffs);
    dc_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, 1.0);
    double theta = TWOPI * 0.5 * (raw_alpha1 + raw_alpha2); /* "jwT" for centre freq. */
    fc_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, expj(theta));
    hf_gain = evaluate(topcoeffs, zplane.numzeros, botcoeffs, zplane.numpoles, -1.0);
    for (i = 0; i <= zplane.numzeros; i++) xcoeffs[i] = +(topcoeffs[i].re / botcoeffs[zplane.numpoles].re);
    for (i = 0; i <= zplane.numpoles; i++) ycoeffs[i] = -(botcoeffs[i].re / botcoeffs[zplane.numpoles].re);
  }

static void expand(complex pz[], int npz, complex coeffs[])
  { /* compute product of poles or zeros as a polynomial of z */
    int i;
    coeffs[0] = 1.0;
    for (i=0; i < npz; i++) coeffs[i+1] = 0.0;
    for (i=0; i < npz; i++) multin(pz[i], npz, coeffs);
    /* check computed coeffs of z^k are all real */
    for (i=0; i < npz+1; i++)
      { if (fabs(coeffs[i].im) > EPS)
    { ;
      exit(1);
    }
      }
  }

static void multin(complex w, int npz, complex coeffs[])
  { /* multiply factor (z-w) into coeffs */
    complex nw = -w;
    for (int i = npz; i >= 1; i--) coeffs[i] = (nw * coeffs[i]) + coeffs[i-1];
    coeffs[0] = nw * coeffs[0];
  }



static bool optsok;


int CalculateFilter(double tau, double clkFREQ, double *Gain, double *ZerosI, double *ZerosR, int *nZeros, double *PolesI, double *PolesR, int *nPoles)
{
	
	int i;


	options = order = polemask = 0;
	uint m = 0;
	m |= opt_bu;
	m |= opt_lp;
	m |= opt_a;
	m |= opt_o;
	m |= opt_s;
	options |= m;

	order = 1;
	raw_alpha1 = 1.0/(PI *2.0 * tau * 10e-6 * clkFREQ);								//AAAAAAALFA!!!!
	raw_alpha2 = raw_alpha1;

	
	
	
    setdefaults();
    if (options & opt_re)
      { if (options & opt_bp) compute_bpres();	   /* bandpass resonator	 */
  if (options & opt_bs) compute_notch();	   /* bandstop resonator (notch) */
  if (options & opt_ap) compute_apres();	   /* allpass resonator		 */
      }
    else
      { if (options & opt_pi)
    { prewarp();
      splane.poles[0] = 0.0;
      splane.zeros[0] = -TWOPI * warped_alpha1;
      splane.numpoles = splane.numzeros = 1;
    }
  else
    { compute_s();
      prewarp();
      normalize();
    }
  if (options & opt_z) compute_z_mzt(); else compute_z_blt();
      }
    if (options & opt_Z) add_extra_zero();
    expandpoly();


		*Gain = hypot(dc_gain);
		*nZeros = zplane.numzeros;
		*nPoles = zplane.numpoles;
			for (i = 0;i<zplane.numzeros;i++)
			{
				ZerosI[i] = zplane.zeros [i].im;
				ZerosR[i] = zplane.zeros [i].re;
			}
			
			for (i = 0;i<zplane.numpoles;i++)
			{
				PolesR[i] = zplane.poles  [i].re;
				PolesI[i] = zplane.poles  [i].im;
			}

	return 0;
}