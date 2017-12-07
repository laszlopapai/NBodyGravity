#pragma once

#include <DirectXMath.h>
#include <Interfaces/IDrawable.h>
#include <d3dx11effect.h>
#include <Tools/GameTimer.h>
#include "Interfaces/IMaterial.h"

using namespace Cefied::Game;
using namespace DirectX;

struct Particle
{
	XMFLOAT3 position;
	XMFLOAT3 velocity;
};

class NBodyGravity : public IDrawable
{
public:
	NBodyGravity();
	bool initialize() override;
	void update(const GameTime& gameTime) override;
	void draw(const GameTime& gameTime) override;
	virtual ~NBodyGravity();

private:
	IMaterial *m_material { nullptr };
	ID3DX11Effect *m_effect{ nullptr };
	ID3DX11EffectTechnique *m_renderTech { nullptr };
	ID3DX11EffectTechnique *m_updateTech{ nullptr };

	ID3D11Buffer *m_zOrderbuffer{ nullptr };
	ID3D11UnorderedAccessView *m_zOrderBufferUav{ nullptr };

	ID3D11Buffer *m_particleBuffer{ nullptr };
	ID3D11UnorderedAccessView *m_particleBufferUav{ nullptr };

	ID3D11InputLayout *m_vertexLayout{ nullptr };
	ID3D11ShaderResourceView *m_particleTexture{ nullptr };

	ID3DX11EffectMatrixVariable *m_view{ nullptr }, *m_proj{ nullptr };

	XMVECTOR m_centerOfMass;
};

