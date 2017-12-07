#include <_blendStates.fx>
#include <_rasterizerStates.fx>
#include <_depthStencilStates.fx>
#include <_samplerStates.fx>


cbuffer Matrices
{
    matrix view;
    matrix projection;    
};

cbuffer ComputeParams
{    
    float3 massCenter;
    float deltaTime;
    float g;
    float size;
    float resistance;
};

Texture2D Base;


struct Particle
{
    float4 position : SV_POSITION;
    float4 velocity : COLOR0;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

Buffer<float> drawableParticles;


VS_OUT VS(Particle part, uint i : SV_VertexID)
{
    VS_OUT Out;
    Particle p;
    p.position.x = drawableParticles[i * 6 + 0];
    p.position.y = drawableParticles[i * 6 + 1];
    p.position.z = drawableParticles[i * 6 + 2];
    p.position.w = 0;
    
    p.velocity.x = drawableParticles[i * 6 + 3];
    p.velocity.y = drawableParticles[i * 6 + 4];
    p.velocity.z = drawableParticles[i * 6 + 5];
    p.velocity.w = 0;


    Out.position = part.position;

    Out.color = float4(clamp(float3(part.velocity.xyz), float3(.2,.2,.2), float3(1, 1, 1)), 1.0f);
	//Out.color = float4(1,1,1,1);

	return Out;
}

//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

[maxvertexcount(4)]
void GS(point VS_OUT sprite[1], inout TriangleStream<GS_OUT> triStream)
{
	GS_OUT v;
	v.color = sprite[0].color;

	float hStep = size/2;
	float wStep = size/2;

    float4 viewPos = mul(sprite[0].position, view);

    v.pos = viewPos + float4(-wStep, -hStep, 0, 0);
	v.pos = mul( v.pos, projection );
	v.tex = float2(0,0);
	triStream.Append(v);

    v.pos = viewPos + float4(-wStep, hStep, 0, 0);
	v.pos = mul( v.pos, projection );
	v.tex = float2(0,1);
	triStream.Append(v);

    v.pos = viewPos + float4(wStep, -hStep, 0, 0);
	v.pos = mul( v.pos, projection );
	v.tex = float2(1,0);
	triStream.Append(v);

    v.pos = viewPos + float4(wStep, hStep, 0, 0);
	v.pos = mul( v.pos, projection );
	v.tex = float2(1,1);
	triStream.Append(v);

}
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( GS_OUT In ) : SV_TARGET
{
    return In.color * Base.Sample(FilterAnisotropic, In.tex);
}


BlendState particularBlender
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = one;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
};

//--------------------------------------------------------------------------------------
technique11 Render
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetRasterizerState(defaultRasterizer);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        SetDepthStencilState(noDepthTestCompositor, 0);
		SetBlendState(particularBlender, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}




RWBuffer<float> computableParticles;
RWBuffer<float> particlesZOrder;
#define groupSize 64

[numthreads(groupSize, 1, 1)]
void csUpdateParticles(uint tid : SV_GroupIndex, uint3 groupIdx : SV_GroupID)
{
    uint allThreadId = groupIdx.x * groupSize + tid;
    uint particleId = allThreadId * 6;

    uint d;
    computableParticles.GetDimensions(d);
    if (particleId < d)
    {
        Particle pIn;
        pIn.position.x = computableParticles[particleId + 0];
        pIn.position.y = computableParticles[particleId + 1];
        pIn.position.z = computableParticles[particleId + 2];
        pIn.position.w = 1;
    
        pIn.velocity.x = computableParticles[particleId + 3];
        pIn.velocity.y = computableParticles[particleId + 4];
        pIn.velocity.z = computableParticles[particleId + 5];
        pIn.velocity.w = 1;


        float forceMagnitude = 0;
        float3 d = massCenter - (float3)pIn.position;

        float distSquare = pow(d.x, 2) + pow(d.y, 2) + pow(d.z, 2);
        if (distSquare >= 200)
        {
            forceMagnitude = g / distSquare;
        }

        float3 force = d * forceMagnitude;

        pIn.velocity *= exp(-deltaTime * resistance);
        pIn.velocity += float4(force, 0) * deltaTime;
        pIn.position += pIn.velocity * deltaTime;
        float4 pos = mul(pIn.position, view);
        particlesZOrder[allThreadId] = pos.z;
        

        computableParticles[particleId + 0] = pIn.position.x;
        computableParticles[particleId + 1] = pIn.position.y;
        computableParticles[particleId + 2] = pIn.position.z;
         
        computableParticles[particleId + 3] = pIn.velocity.x;
        computableParticles[particleId + 4] = pIn.velocity.y;
        computableParticles[particleId + 5] = pIn.velocity.z;
    }
}

void swap(int id)
{
    float tmp;
    tmp = particlesZOrder[id];
    particlesZOrder[id] = particlesZOrder[id + 1];
    particlesZOrder[id + 1] = tmp;
    

    tmp = computableParticles[id * 6 + 0];
    computableParticles[id * 6 + 0] = computableParticles[(id + 1) * 6 + 0];
    computableParticles[(id + 1) * 6 + 0] = tmp;

    tmp = computableParticles[id * 6 + 1];
    computableParticles[id * 6 + 1] = computableParticles[(id + 1) * 6 + 1];
    computableParticles[(id + 1) * 6 + 1] = tmp;

    tmp = computableParticles[id * 6 + 2];
    computableParticles[id * 6 + 2] = computableParticles[(id + 1) * 6 + 2];
    computableParticles[(id + 1) * 6 + 2] = tmp;

    tmp = computableParticles[id * 6 + 3];
    computableParticles[id * 6 + 3] = computableParticles[(id + 1) * 6 + 3];
    computableParticles[(id + 1) * 6 + 3] = tmp;

    tmp = computableParticles[id * 6 + 4];
    computableParticles[id * 6 + 4] = computableParticles[(id + 1) * 6 + 4];
    computableParticles[(id + 1) * 6 + 4] = tmp;

    tmp = computableParticles[id * 6 + 5];
    computableParticles[id * 6 + 5] = computableParticles[(id + 1) * 6 + 5];
    computableParticles[(id + 1) * 6 + 5] = tmp;
}


#define sortGroupSize 32
[numthreads(sortGroupSize, 1, 1)]
void csOrderParticles(uint tid : SV_GroupIndex, uint3 groupIdx : SV_GroupID)
{
    uint allThreadId = groupIdx.x * sortGroupSize + tid;
    uint swapId = allThreadId * 2;
    uint d;
    particlesZOrder.GetDimensions(d);
    if (swapId < d)
    {
        if (particlesZOrder[swapId] < particlesZOrder[swapId + 1])
        {
            swap(swapId);
        }

        if ((d % 2 == 1 || swapId < d - 2) && particlesZOrder[swapId + 1] < particlesZOrder[swapId + 2])
        {
            swap(swapId + 1);
        }
    }
}

technique11 Update
{
	pass P0 {
        SetComputeShader(CompileShader(cs_5_0, csUpdateParticles()));
    }
	pass P1 {
        SetComputeShader(CompileShader(cs_5_0, csOrderParticles()));
	}
}