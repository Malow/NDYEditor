/*
Include file for all samplers, rasterizer states, and blend states and constant buffers.

*/


#ifndef STDAFX_FX
#define STDAFX_FX

//------------------------------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------------------------------
#define PI		float(3.14159265358979323846f)

#define WHITE	float4(1.0f, 1.0f, 1.0f, 1.0f)
#define BLACK	float4(0.0f, 0.0f, 0.0f, 1.0f)
#define RED		float4(1.0f, 0.0f, 0.0f, 1.0f)
#define GREEN	float4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE	float4(0.0f, 0.0f, 1.0f, 1.0f)
#define YELLOW	float4(1.0f, 1.0f, 0.0f, 1.0f)
#define CYAN	float4(0.0f, 1.0f, 1.0f, 1.0f)
#define MAGENTA float4(1.0f, 0.0f, 1.0f, 1.0f)

#define OBJECT_TYPE_TERRAIN float(1.0f)
#define OBJECT_TYPE_BILLBOARD float(2.0f)

//------------------------------------------------------------------------------------------------------
//	Own structures
//------------------------------------------------------------------------------------------------------
struct Light
{
	float4 LightPosition;
	float4 LightColor;
	float LightIntensity;
	matrix LightViewProj;
};

struct Cascade
{
	matrix viewProj;
};

struct SunLight
{
	float4 Direction;
	float4 LightColor;
	float LightIntensity;
};


//------------------------------------------------------------------------------------------------------
//	Constant buffers
//------------------------------------------------------------------------------------------------------
matrix CameraVP;
float4 CameraPosition;

float NrOfLights;//**tillman**
Light lights[10];//**tillman**
float4 SceneAmbientLight;//**tillman**

bool UseSun;//**tillman**
SunLight sun; //**tillman**

float NrOfCascades; //Deprecated**tillman**
float4 CascadeLevels; //Deprecated**tillman**
Cascade cascades[10];

float timerMillis;

uint windowWidth;
uint windowHeight;
float NearClip;
float FarClip;

//------------------------------------------------------------------------------------------------------
//	State structures
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
//	SamplersStates
//------------------------------------------------------------------------------------------------------
//**TILLMAN ev todo: MaxLOD for all samplers**
SamplerState LinearWrapSampler
{
	Filter = MIN_MAG_MIP_LINEAR; 
	AddressU = Wrap;
	AddressV = Wrap;
};
SamplerState LinearClampSampler
{
	Filter = MIN_MAG_MIP_LINEAR; 
	AddressU = Clamp;
	AddressV = Clamp;
};
SamplerState PointWrapSampler
{
	Filter = MIN_MAG_MIP_POINT; 
	AddressU = Wrap;
	AddressV = Wrap;
};
SamplerState PointBorderSampler
{
	Filter = MIN_MAG_MIP_POINT; 
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 1e5f);
};
SamplerState PointClampSampler
{
	Filter = MIN_MAG_MIP_POINT; 
	AddressU = Clamp;
	AddressV = Clamp;
};
SamplerState AnisotropicClampSampler 
{
	Filter = ANISOTROPIC;
	AddressU = Clamp;
	AddressV = Clamp;
	AddressW = Clamp;
	MaxAnisotropy = 4;
	ComparisonFunc = ALWAYS;
	MaxLOD = 0.0f;
	MinLOD = 0.0f;
};
SamplerState AnisotropicWrapSampler 
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
	MipLODBias = 0.0f;//Unused
	MaxAnisotropy = 16;
	ComparisonFunc = ALWAYS; 
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //Unused
	MinLOD = 0.0f;
	MaxLOD = 16.0f; //Max
};
//------------------------------------------------------------------------------------------------------
//	RasterizerStates
//------------------------------------------------------------------------------------------------------
RasterizerState BackCulling
{
	CullMode = Back;
};
RasterizerState FrontCulling
{
	CullMode = Front;
};
RasterizerState SolidFrontCulling
{
	FillMode = Solid;
	CullMode = Front;
};
RasterizerState NoCulling
{
	CullMode = None;
};

//------------------------------------------------------------------------------------------------------
//	RasterizerStates
//------------------------------------------------------------------------------------------------------
BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

//------------------------------------------------------------------------------------------------------
//	DepthStencilStates
//------------------------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS;
};

#endif