#include "NBodyGravity.h"
#include "App.h"
#include <Tools/DDSTextureLoader.h>
#include <Tools/Macros.h>
#include <d3dcompiler.h>
#include "Scene/Material.h"


const float defG = 16.6738480f;
const float defResistance = 0.0035f;
const float defSize = 2;

float G = defG;
float Resistance = defResistance;
float Size = defSize;

int particleCount = 500000;

NBodyGravity::NBodyGravity() :
	m_centerOfMass(XMVectorSet(0, 0, 0, 0))
{ }

bool NBodyGravity::initialize()
{
	App::getRenderer().createMaterial(L"Shaders/NBodyGravity.fx", &m_material);

	if (m_material->isInstanceOf<Material>()) {
		m_effect = static_cast<Material*>(m_material)->getEffect();
	}

	// Obtain the technique
	m_renderTech = m_effect->GetTechniqueByName("Render");
	m_updateTech = m_effect->GetTechniqueByName("Update");

	InitCheck(m_renderTech->IsValid() && m_updateTech->IsValid(),
		"Technique initialization failed.");

	// Setup matrices	
	m_view = m_effect->GetVariableByName("view")->AsMatrix();
	m_proj = m_effect->GetVariableByName("projection")->AsMatrix();

	// Setup variables
	m_effect->GetVariableByName("size")->AsScalar()->SetFloat(Size);
	m_effect->GetVariableByName("g")->AsScalar()->SetFloat(G);
	m_effect->GetVariableByName("resistance")->AsScalar()->SetFloat(Resistance);

	// Setup Textures
	HRCheck(CreateDDSTextureFromFile(App::getRenderer().getDevice(), L"Media/particle.dds", nullptr, &m_particleTexture), "Failed to load thexture: Particle DDS");
	m_effect->GetVariableByName("Base")->AsShaderResource()->SetResource(m_particleTexture);

	Particle *particles = new Particle[particleCount];
	Particle tmp;
	tmp.position.z = 0.5f;
	tmp.velocity.z = 1;
	for (int i = 0; i < particleCount; ++i)
	{
		tmp.position.x = (float)rand() / RAND_MAX * 2.0f - 1.0f;
		tmp.position.y = (float)rand() / RAND_MAX * 2.0f - 1.0f;
		tmp.position.z = (float)rand() / RAND_MAX * 2.0f - 1.0f;
		tmp.velocity.x = (float)(rand() % 2);
		tmp.velocity.y = (float)(rand() % 2);
		tmp.velocity.z = (float)(rand() % 2);

		tmp.position.x *= 300.0f;
		tmp.position.y *= 300.0f;
		tmp.position.z *= 300.0f;

		particles[i] = tmp;
	}


	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_DESC PassDesc;
	HRCheck(m_renderTech->GetPassByIndex(0)->GetDesc(&PassDesc), "RenderTech Desc Fail");

	HRCheck(App::getRenderer().getDevice()->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &m_vertexLayout), "Input layout fail.");

	// Set the input layout
	App::getRenderer().getImmediateContext()->IASetInputLayout(m_vertexLayout);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Particle) * particleCount;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = NULL;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = particles;

	HRCheck(App::getRenderer().getDevice()->CreateBuffer(&bd, &InitData, &m_particleBuffer),
		"Particle buffer creation fail.");

	bd.ByteWidth = sizeof(FLOAT) * particleCount;
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	HRCheck(App::getRenderer().getDevice()->CreateBuffer(&bd, NULL, &m_zOrderbuffer),
		"ZOrder buffer creation fail.");


	D3D11_UNORDERED_ACCESS_VIEW_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	rd.Format = DXGI_FORMAT_R32_FLOAT;
	rd.Buffer.FirstElement = 0;
	rd.Buffer.NumElements = sizeof(Particle) / sizeof(FLOAT) * particleCount;

	HRCheck(App::getRenderer().getDevice()->CreateUnorderedAccessView(m_particleBuffer, &rd, &m_particleBufferUav),
		"Particle buffer Unorderd Access View creation fail.");


	rd.Buffer.NumElements = particleCount;

	HRCheck(App::getRenderer().getDevice()->CreateUnorderedAccessView(m_zOrderbuffer, &rd, &m_zOrderBufferUav),
		"ZOrder buffer Unorderd Access View creation fail.");


	SafeDeleteArr(particles);

	return true;
}

void NBodyGravity::update(const GameTime & gameTime)
{
	static int time = 20;
	static bool zState = false;
	IOHandler & io = App::getIOHandle();
	const auto camera = App::getSceneManager().getCurrentScene()->getMainCamera();


	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(App::getWindow().getHWND(), &pos);

	const int mx = pos.x;
	const int my = pos.y;

	if (io.getMouseState().rgbButtons[2] & 0x80) {
		m_effect->GetVariableByName("g")->AsScalar()->SetFloat(-G);
	}
	else {
		m_effect->GetVariableByName("g")->AsScalar()->SetFloat(G);
	}

	if (io.isKeyPressed(DIK_NUMPAD0)) {
		time = 0;
	}
	if (io.isKeyPressed(DIK_NUMPAD1)) {
		time = 30;
	}

	if (io.getMouseState().rgbButtons[0] & 0x80)
	{
		XMVECTOR n;
		XMVECTOR f;

		camera->unProject(mx, my, 0.0f, n);
		camera->unProject(mx, my, 1.0f, f);
		const auto cursorRay = XMVector3Normalize(f - n);

		const auto distance = XMVector3Length(m_centerOfMass - camera->getPosition());

		m_centerOfMass = cursorRay * distance + camera->getPosition();
	}

	// Setup variables
	m_view->SetMatrix((float*)&camera->getView());
	m_proj->SetMatrix((float*)&camera->getProjection());


	m_effect->GetVariableByName("massCenter")->AsVector()->SetFloatVector((float*)&m_centerOfMass);
	m_effect->GetVariableByName("deltaTime")->AsScalar()->SetFloat(gameTime.deltaTime * (float)time);

	m_effect->GetVariableByName("computableParticles")
		->AsUnorderedAccessView()
		->SetUnorderedAccessView(m_particleBufferUav);
	m_effect->GetVariableByName("particlesZOrder")
		->AsUnorderedAccessView()
		->SetUnorderedAccessView(m_zOrderBufferUav);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_updateTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_updateTech->GetPassByIndex(p)->Apply(0, App::getRenderer().getImmediateContext());
		App::getRenderer().getImmediateContext()->Dispatch((UINT)ceilf(particleCount / 64.0f), 1, 1);
	}

	ID3D11UnorderedAccessView* uav[1];
	uav[0] = 0;
	App::getRenderer().getImmediateContext()->CSSetUnorderedAccessViews(0, 1, uav, 0);
}

void NBodyGravity::draw(const GameTime & gameTime)
{
	UINT stride = sizeof(Particle);
	UINT offset = 0;

	D3DX11_TECHNIQUE_DESC techDesc;
	m_renderTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_renderTech->GetPassByIndex(p)->Apply(0, App::getRenderer().getImmediateContext());

		App::getRenderer().getImmediateContext()->IASetVertexBuffers(0, 1, &m_particleBuffer, &stride, &offset);
		App::getRenderer().getImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		App::getRenderer().getImmediateContext()->IASetInputLayout(m_vertexLayout);
		App::getRenderer().getImmediateContext()->Draw(particleCount, 0);
	}

	ID3D11Buffer* buffer[1];
	buffer[0] = 0;
	App::getRenderer().getImmediateContext()->IASetVertexBuffers(0, 1, buffer, &stride, &offset);
}


NBodyGravity::~NBodyGravity()
{
	SafeDelete(m_material);
	SafeRelease(m_particleBuffer);
	SafeRelease(m_particleBufferUav);
	SafeRelease(m_particleTexture);
	SafeRelease(m_vertexLayout);
}
