#include <vector>

#define M_PI 3.1415926
#define M2_PI    2. * M_PI
#define RAD      360. / (2. * M_PI)
#define DEG      (2. * M_PI)/360.
#define EARTHRADIUS 6371000.	/* appx. for most ellipsoids or projections */
#define UNDEF    0.		/* undefined value for terrain aspect */
#define UNDEFZ   -9999.		/* internal undefined value for NULL */
#define SKIP    "1"
#define BIG      1.e20
#define IBIG     32767
#define EPS      1.e-4
#define LINKE    "3.0"
#define ALB      "0.2"
#define STEP     "0.5"
#define BSKY	  1.0
#define DSKY	  1.0
#define DIST      0.8

#define AMAX1(arg1, arg2) ((arg1) >= (arg2) ? (arg1) : (arg2))
#define AMIN1(arg1, arg2) ((arg1) <= (arg2) ? (arg1) : (arg2))
#define DISTANCE1(x1, x2, y1, y2) (sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)))

const double deg2rad = M_PI / 180.;
const double rad2deg = 180. / M_PI;

#define DEGREEINMETERS 111120.
//#define linke 3.0
//#define cbh BSKY
//#define cdh DSKY

double cbh = BSKY;//clear-sky index for beam component
double cdh = DSKY;//clear-sky index for diffuse component
#define alb 0.2
#define pi 3.14159265358979323846
#define earthRadiusKm 6371000.0
// This function converts decimal degrees to radians
double deg2rad2(double deg) {
	return (deg * pi / 180);
};

//  This function converts radians to decimal degrees
double rad2deg2(double rad) {
	return (rad * 180 / pi);
};

typedef struct/* __align__(16)*/
{
public:
	bool tien;
	double day;
	double linke;
	double length, c, declin, step;
	double elev, slope, aspect;
	double lum_C11, lum_C13, lum_C22, lum_C31, lum_C33, lum_Lx, lum_Ly, lum_Lz;
	double lum_p, sunrise_time, sunset_time, h0, tanh0, A0, angle;
	double longitude, latitude, lum_time, declination;
	double sinlat, coslat, sindecl, cosdecl;
	double longit_l, latid_l, cos_u, cos_v, sin_u, sin_v;
	double sin_phi_l, tan_lam_l, lum_C31_l, lum_C33_l;
	double beam_e, diff_e, refl_e, bh, dh, rr, insol_t;
	double coslatsq;
	bool* shadowInfo;
	SolarRadiation* parts;
}TempVariables;

void com_par(TempVariables& tmpval);
void com_par_const(TempVariables& tmpval);
double lumcline2(TempVariables& tmpval);
void joules2(TempVariables& tmpval,const bool& isInstaneous,const double& assignedTime);
double com_sol_const(double no_of_day);
double com_declin(double);
double brad(double sh,TempVariables& tmpval);
//double drad(double sh,TempVariables& tmpval);
double drad_isotropic(double sh,TempVariables& tmpval);

// This function converts decimal degrees to radians
double deg2rad2(double deg);

//  This function converts radians to decimal degrees
double rad2deg2(double rad);

double distanceEarth(double Lat1, double Long1, double Lat2, double Long2);

bool COLLECT_SUN_VECTOR = false;
std::vector<SunVector> SunVectors;
std::vector<SolarRadiation> ClearSkyRads;
SolarRadiation PreviousSolarRad;