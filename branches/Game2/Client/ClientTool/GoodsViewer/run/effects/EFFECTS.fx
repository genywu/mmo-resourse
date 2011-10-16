struct VS_INPUT
{
	float4 position:POSITION;
	#ifdef USENORMAL
	float3 normal:NORMAL;
	#endif
	#ifdef USEUV
	float2 uv:TEXCOORD;
	#endif
	#ifdef USEMATINDEX
	int boneindex:BLENDINDICES;
	#endif
	
	#ifdef USECOLOR
	float4 color:COLOR;
	#endif
	
};
struct VS_OUTPUT
{
	float4 position:POSITION;
	vector color:COLOR;
	float2 uv:TEXCOORD;
};

#ifdef HWMAXMAT
#ifdef ANIMUSESCAL
matrix WVPMatrix[HWMAXMAT/2];
matrix WVPMatrixN[HWMAXMAT/2];
#else
matrix WVPMatrix[HWMAXMAT];
#endif
#endif

#ifndef HWMAXMAT
#ifdef SWMAXMAT
#ifdef ANIMUSESCAL
matrix WVPMatrix[SWMAXMAT];
matrix WVPMatrixN[SWMAXMAT];
#else
matrix WVPMatrix[SWMAXMAT];
#endif
#endif
#endif

#ifdef ENVIROMENT
matrix MatrixForUse;
#endif


#ifndef ENVIROMENT
#ifdef UVANIMI
matrix MatrixForUse;
#endif
#endif

#ifndef ENVIROMENT
#ifndef UVANIMI
#ifdef TEXTUREANIMI
matrix MatrixForUse;
#endif
#endif
#endif


#ifdef USELIGHT
vector LightDir;
vector LightColor;
vector MultiColor;
#endif


matrix VPMatrix;


#ifdef TEXPROJECTED
matrix TPVPMatrix;
#endif

vector basecolor;


VS_OUTPUT main(VS_INPUT	input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	#ifdef USEMATINDEX
	output.position = mul(input.position,WVPMatrix[input.boneindex]);
	output.position = mul(output.position,VPMatrix);
	#ifdef USENORMAL
	#ifdef ANIMUSESCAL
	input.normal    = mul(input.normal,WVPMatrixN[input.boneindex]);
	normalize(input.normal);
	#else
	input.normal    = mul(input.normal,WVPMatrix[input.boneindex]);
	#endif
	#endif
	#else
	output.position	= mul(input.position,WVPMatrix[0]);
	output.position = mul(output.position,VPMatrix);
	#ifdef USENORMAL
	#ifdef ANIMUSESCAL
	input.normal    = mul(input.normal,WVPMatrixN[0]);
	normalize(input.normal);
	#else
	input.normal    = mul(input.normal,WVPMatrix[0]);
	#endif
	#endif
	#endif
	
	
	
	#ifdef USELIGHT
	output.color = saturate(basecolor + float4((LightColor * 2 * saturate(-dot(LightDir.xyz,input.normal))).xyz,0)) * MultiColor;
	#endif
	
	#ifndef USELIGHT
	#ifdef USECOLOR
	output.color = input.color;
	#else
	output.color = basecolor;
	#endif
	#endif
	
	
	#ifdef USEUV
	
	#ifdef ENVIROMENT
	output.uv = normalize(mul(input.normal,MatrixForUse)) * 0.5 + 0.5; 
	#endif
	
	#ifndef ENVIROMENT
	#ifdef UVANIMI
	output.uv = mul(float4(input.uv,0,1),MatrixForUse);
	#endif
	#endif
	
	#ifndef ENVIROMENT
	#ifndef UVANIMI
	output.uv = input.uv;
	#endif
	#endif
	
	#endif
	
	
	
	#ifdef TEXPROJECTED
	output.uv = mul(input.position,TPVPMatrix);
	#endif
	
	return output;
};
technique T0
{
	pass P0
	{
		vertexShader = compile vs_1_1 main();
	}
}