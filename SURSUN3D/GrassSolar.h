
#ifndef  GrassSolar_H
#define  GrassSolar_H

#include <math.h>
#include <vector>
#include <osg/Vec3>
#include <osg/Matrix>
#include <map>
struct Stats
{
	float fmin;
	float fmax;
	float fmean;
	float std;
	float sum;
	float validnum;
	Stats()
	{

		fmin = FLT_MAX;
		fmax = -FLT_MAX;
		fmean = 0;
		std = 0;
		sum = 0;
		validnum = 0;
	}
};

//structure of solar vector
typedef struct 
{ 
	float time;
	float azimuth;//azimuth angle
	float alt; //elevation angle
	//float x,y,z; //position
}SunVector;

struct SolarTime
{
	int hour;
	int minute;
	int second;
	SolarTime()
	{
		hour = 6;
		minute = 0;
		second = 0;
	}
	SolarTime(int h,int m,int s)
		:hour(h),minute(m),second(s)
	{
	}
	double toDecimalHour()
	{
		return hour + minute / 60.0 + second / 3600.0;
	}

};


//parameters for r.sun calculation
struct SolarParam
{
	float linke;//turbidity factor
	float bsky;//scale factor for the beam component
	float dsky;//scale factor for the diffuse component
	float lon;//longitude
	float lat;//latitude
	float elev;//elevation
	float slope;//slope in degrees
	float aspect;//aspect in degrees
	float time_step;//time resolution in hours
	int day;//range from 1 to 366
	bool* shadowInfo;//an array of shadow masks corresponding to the number of solar vectors in a day
	bool isShadowed;//a single shadow mask will be used if 'shadowInfo' is null
	bool isInstantaneous;//apply instantaneous calculation mode
	SolarTime time;//decimal time 
	SolarParam()
	{
		shadowInfo = NULL;
		isShadowed = false;
		isInstantaneous = false;//time-integrated calculation mode as default
		elev = 0;
		slope = 0;
		aspect = 0;
		linke = 3.0;
		bsky = 1;
		dsky = 1;
		time_step = 1;
		day = 1;
	}

};

//structure of solar radiation result
struct SolarRadiation
{
	float global;//global component
	float beam;//beam component
	float diffuse;//diffuse component
	float reflected;//reflected component
public:
	SolarRadiation()
	{
		global = -9999;
		beam = -9999;
		diffuse = -9999;
		reflected =-9999;
	}
	void Zero()
	{
		global = 0;
		beam = 0;
		diffuse = 0;
		reflected =0;
	}

	SolarRadiation operator+(const SolarRadiation& rad)
	{
		SolarRadiation newrad;
		newrad.beam=rad.beam+this->beam;
		newrad.global=rad.global+this->global;
		newrad.diffuse=rad.diffuse+this->diffuse;
		newrad.reflected=rad.reflected+this->reflected;
		return newrad;
	}
	//SolarRadiation operator*(const SolarRadiation& rad,const float& val)
	//{
	//	SolarRadiation newrad;
	//	newrad.beam=rad.beam*val;
	//	newrad.global=rad.global*val;
	//	newrad.diffuse=rad.diffuse*val;
	//	newrad.reflected=rad.reflected*val;
	//	return newrad;
	//}
	SolarRadiation operator*(const SolarRadiation& rad)
	{
		SolarRadiation newrad;
		newrad.beam=rad.beam*this->beam;
		newrad.global=rad.global*this->global;
		newrad.diffuse=rad.diffuse*this->diffuse;
		newrad.reflected=rad.reflected*this->reflected;
		return newrad;
	}
	SolarRadiation operator*(const float& val)
	{
		SolarRadiation newrad;
		newrad.beam=beam*val;
		newrad.global=global*val;
		newrad.diffuse=diffuse*val;
		newrad.reflected=reflected*val;
		return newrad;
	}

	SolarRadiation operator/(const float& val)
	{
		SolarRadiation newrad;
		newrad.beam=beam/val;
		newrad.global=global/val;
		newrad.diffuse=diffuse/val;
		newrad.reflected=reflected/val;
		return newrad;
	}

	//SolarRadiation operator/(const SolarRadiation& rad)
	//{
	//	SolarRadiation newrad;
	//	newrad.beam=rad.beam/this->beam;
	//	newrad.global=rad.global/this->global;
	//	newrad.diffuse=rad.diffuse/this->diffuse;
	//	newrad.reflected=rad.reflected/this->reflected;
	//	return newrad;
	//}
	//SolarRadiation operator/(const SolarRadiation& rad,const float& val)
	//{
	//	SolarRadiation newrad;
	//	newrad.beam=rad.beam/val;
	//	newrad.global=rad.global/val;
	//	newrad.diffuse=rad.diffuse/val;
	//	newrad.reflected=rad.reflected/val;
	//	return newrad;
	//}

};
//perform actual calculation
SolarRadiation calculateSolarRadiation(SolarParam solar_param);
//obtain the array of solar vectors for a specified day
std::vector<SunVector> getSunVectors(SolarParam sparam);

#endif