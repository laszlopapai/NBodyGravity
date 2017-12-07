#include "CameraController.h"
#include "App.h"

using namespace Cefied::Game;

CameraController::CameraController(Transformation& cameraTransformation) :
	m_cameraTransformation(cameraTransformation),
	m_stepSize(6.0f),
	m_stepFactor(1)
{ }

bool CameraController::initialize()
{
	return true;
}

void CameraController::update(const GameTime& gameTime)
{
	IOHandler& io = App::getIOHandle();

	XMVECTOR rotation = XMQuaternionIdentity();
	XMVECTOR step     = XMVectorZero();
	
	if (io.isButtonDown(4)) {
		step += XMVectorSet(gameTime.deltaTime * m_stepSize * io.getAxisPosX(), 0, gameTime.deltaTime * m_stepSize * -io.getAxisPosY(), 0);
		m_roll += gameTime.deltaTime * m_stepSize * .1f * -io.getAxisPosZ();
		m_pitch += gameTime.deltaTime * m_stepSize * .1f * -io.getAxisRotZ();
	}

	if (io.isKeyDown(DIK_LSHIFT)) {
		m_stepFactor = 5;
	}
	else {
		m_stepFactor = 1;
	}

	if (io.isKeyDown(DIK_W))
	{
		step += XMVectorSet(0, 0, -gameTime.deltaTime * m_stepSize *m_stepFactor, 0);
	}
	if (io.isKeyDown(DIK_A))
	{
		step += XMVectorSet(-gameTime.deltaTime * m_stepSize *m_stepFactor, 0, 0, 0);
	}
	if (io.isKeyDown(DIK_S))
	{
		step += XMVectorSet(0, 0, gameTime.deltaTime * m_stepSize *m_stepFactor, 0);
	}
	if (io.isKeyDown(DIK_D))
	{
		step += XMVectorSet(gameTime.deltaTime * m_stepSize *m_stepFactor, 0, 0, 0);
	}

	DIMOUSESTATE& mouse = io.getMouseState();
	if ((mouse.rgbButtons[1] & 0x80 || io.isKeyDown(DIK_SPACE)))
	{
		int left, top, width, height;
		App::getWindow().getPosition(left, top);
		App::getWindow().getSize(width, height);
		POINT cur;
		GetCursorPos(&cur);

		if (cur.x > left && cur.x < left + width && cur.y > top && cur.y < top + height) {
			
			m_roll += -mouse.lY * 0.003f;
			m_pitch += -mouse.lX * 0.003f;

			SetCursorPos(
				width / 2 + left,
				height / 2 + top);
		}
	}

	m_cameraTransformation.setRotation(XMQuaternionRotationRollPitchYaw(m_roll, m_pitch, 0));
	m_cameraTransformation.move(XMVector3Rotate(step, m_cameraTransformation.getRotation()));
}
