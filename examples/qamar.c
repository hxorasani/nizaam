/* qamar: 
 * 
 * */

#include "qamar.h"

#define XATAA 0

int curcolor = 0, angle = 180, circlesize = 2, extent = 30, cursor = 0,
	ex0 = 0, vy0 = 0,
	ex1 = 50, vy1 = 50,
	ex2 = 100, vy2 = 100, affect = 0;
kaatib ktb;
struct timespec waqt;

#define		PI	3.1415926535897932384626433832795
#define		RAD	(PI/180.0)
#define		SMALL_FLOAT	(1e-12)

/*static double Julian(int year,int month,double day) {
    // Returns the number of julian days for the specified day.
    
    int a,b=0,c,e;
    if (month < 3) {
	year--;
	month += 12;
    }
    if (year > 1582 || (year == 1582 && month>10) ||
	(year == 1582 && month==10 && day > 15)) {
	a=year/100;
	b=2-a+a/4;
    }
    c = 365.25*year;
    e = 30.6001*(month+1);
    return b+c+e+day+1720994.5;
}
static double sun_position(double j) {
    double n,x,e,l,dl,v;
    //double m2;
    int i;

    n=360/365.2422*j;
    i=n/360;
    n=n-i*360.0;
    x=n-3.762863;
    if (x<0) x += 360;
    x *= RAD;
    e=x;
    do {
	dl=e-.016718*sin(e)-x;
	e=e-dl/(1-.016718*cos(e));
    } while (fabs(dl)>=SMALL_FLOAT);
    v=360/PI*atan(1.01686011182*tan(e/2));
    l=v+282.596403;
    i=l/360;
    l=l-i*360.0;
    return l;
}
static double moon_position(double j, double ls) {
    double ms,l,mm,n,ev,sms,ae,ec;//,z,x;//,lm,bm,ec;
    
    //double d;
    //double ds, as, dm;
    int i;
    
    // ls = sun_position(j)
    ms = 0.985647332099*j - 3.762863;
    if (ms < 0) ms += 360.0;
    l = 13.176396*j + 64.975464;
    i = l/360;
    l = l - i*360.0;
    if (l < 0) l += 360.0;
    mm = l-0.1114041*j-349.383063;
    i = mm/360;
    mm -= i*360.0;
    n = 151.950429 - 0.0529539*j;
    i = n/360;
    n -= i*360.0;
    ev = 1.2739*sin((2*(l-ls)-mm)*RAD);
    sms = sin(ms*RAD);
    ae = 0.1858*sms;
    mm += ev-ae- 0.37*sms;
    ec = 6.2886*sin(mm*RAD);
    l += ev+ec-ae+ 0.214*sin(2*mm*RAD);
    l= 0.6583*sin(2*(l-ls)*RAD)+l;
    return l;
}
static double moon_phase2(int year, int month, int day, double hour, int *ip) {
	// Calculates more accurately than Moon_phase , the phase of the moon at
	// the given epoch.
	// returns the moon phase as a real number (0-1)
      

    double j= Julian(year,month,(double)day+hour/24.0)-2444238.5;
    double ls = sun_position(j);
    double lm = moon_position(j, ls);

    double t = lm - ls;
    if (t < 0) t += 360;
    *ip = (int)((t + 22.5)/45) & 0x7;
    return (1.0 - cos((lm - ls)*RAD))/2;
}*/

void hfizmalaf() {
	matn_nazaf(&ktb.mtn);
	kaatib_ilaa_matn(&ktb);
	malaf mlf;
	mlf.length = ktb.mtn.length;
	mlf.barzax = (unsigned char *) ktb.mtn.barzax;
	malaf_hfiz(&mlf, "./data/test.matn");
}
int handler(waaqi3ah *w) {
	if (w) {
		if (XATAA) amr_tb3_waaqi3ah(w);
		
		if (w->ism == MUDEER && w->miftaah == LOWHAH) {
			char yes = 1;
			mafateeh m = { 0 };
			str2mafateeh(w->qadrstr, &m);
			if (m.state) {
				if (m.key == KEY_1) affect = 0;
				if (m.key == KEY_2) affect = 1;
				if (m.key == KEY_3) affect = 2;
				if (m.key == KEY_UP && extent < 120) {
//					extent += 5;
					if (affect == 0) vy0 -= 5;
					if (affect == 1) vy1 -= 5;
					if (affect == 2) vy2 -= 5;
				}
				if (m.key == KEY_DOWN && extent > 10) {
//					extent -= 5;
					if (affect == 0) vy0 += 5;
					if (affect == 1) vy1 += 5;
					if (affect == 2) vy2 += 5;
				}
				if (m.key == KEY_LEFT/* && circlesize < 9*/) {
//					circlesize += 1;
					if (affect == 0) ex0 -= 5;
					if (affect == 1) ex1 -= 5;
					if (affect == 2) ex2 -= 5;
				}
				if (m.key == KEY_RIGHT/* && circlesize > 2*/) {
//					circlesize -= 1;
					if (affect == 0) ex0 += 5;
					if (affect == 1) ex1 += 5;
					if (affect == 2) ex2 += 5;
				}
				
				if (m.ctrl && m.key == KEY_EQUAL && m.state) {
					if (ktb.g.s < 6) { ktb.g.s += .5; yes = 1; }
				} else
				if (m.ctrl && m.key == KEY_MINUS && m.state) {
					if (ktb.g.s > 1.5) { ktb.g.s -= .5; yes = 1; }
				} else
				if (m.ctrl && m.key == KEY_O && m.state == 1) {
					yes = 1;
				} else
				if (m.ctrl && m.key == KEY_S && m.state == 1) {
					hfizmalaf();
				} else
				if (m.ctrl && m.key == KEY_A && m.state == 1 && m.shift) {
					yes = kaatib_ghayr_muntaxab_kul(&ktb);
				} else
				if (m.ctrl && m.key == KEY_A && m.state == 1 && !m.shift) {
					yes = kaatib_muntaxab_kul(&ktb);
				} else
				if (m.ctrl && m.key == KEY_B && m.state == 1) {
					yes = kaatib_wazan(&ktb);
				} else
				if (m.key == KEY_LEFT) {
					if (m.shift) yes = kaatib_muntaxab(&ktb);
					yes = kaatib_left(&ktb);
				} else
				if (m.key == KEY_RIGHT) {
					if (m.shift) yes = kaatib_muntaxab(&ktb);
					yes = kaatib_right(&ktb);
				} else
				if (m.key == KEY_UP) {
					yes = kaatib_up(&ktb);
				} else
				if (m.key == KEY_DOWN) {
					yes = kaatib_down(&ktb);
				} else
				if (m.key == KEY_ENTER) {
					kaatib_adaaf(&ktb, '\n'); yes = 1;
				} else
				if (m.key == KEY_TAB) {
					kaatib_adaaf(&ktb, '\t'); yes = 1;
				} else
				if (m.key == KEY_SPACE) {
					kaatib_adaaf(&ktb, ' '); yes = 1;
				} else
				if (m.key == KEY_DELETE) {
					kaatib_hadhaf(&ktb, 1); yes = 1;
				} else
				if (m.key == KEY_BACKSPACE) {
					kaatib_hadhaf(&ktb, 0); yes = 1;
				} else {
					int len = strlen(m.huroof);
					if (len) {
						kaatib_adaaf(&ktb, m.huroof[0]);
						yes = 1;
					}
				}
				
				if (yes) {
					WJHH.mshr.mutadarrar = 1;
					WJHH.raees.mutadarrar = 1;
				}
			}
		}
	}

	return 0;
}
int main(int argc, char **argv) {
	composer t = {
		.ism = "qamar",
		.xitaab = "apps/kaatibmatn.js",
	};
	composer_init(t);

    composer_tamaam();

    return EXIT_SUCCESS;
}
