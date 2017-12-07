#include "DefaultScene.h"

using namespace Cefied::Game;

bool DefaultScene::initialize()
{
	addCamera("MainCamera", m_mainCamera);
	InitCheck(m_mainCamera->initialize(),
		"Camera initialization failed.");

	Transformation & cameraTransform = m_mainCamera->getTransformation();

	cameraTransform.move(XMVectorSet(0, 0, 500, 0));

	return true;
}

bool DefaultScene::addCamera(std::string cameraName, ICamera* camera)
{
	InitCheck(m_cameras.find(cameraName) == m_cameras.end(),
		"Already exists a camera with the same name.");

	m_cameras[cameraName] = camera;

	return true;
}

bool DefaultScene::addController(std::string name, IObject * controller) {
	InitCheck(m_controllers.find(name) == m_controllers.end(),
		"Already exists a drawable with the same name.");

	m_controllers[name] = controller;
	return true;
}

bool DefaultScene::addDrawableObject(std::string name, IDrawable * drawable) {
	InitCheck(m_drawables.find(name) == m_drawables.end(),
		"Already exists a drawable with the same name.");

	m_drawables[name] = drawable;
	return true;
}

ICamera* DefaultScene::getCamera(std::string cameraName)
{
	return nullptr;
}

bool DefaultScene::setMainCamera(std::string cameraName)
{
	return true;
}

bool DefaultScene::setChosenCamera(std::string cameraName)
{
	return true;
}

const ICamera* DefaultScene::getMainCamera()
{
	return m_mainCamera;
}

const ICamera* DefaultScene::getChosenCamera()
{
	return nullptr;
}

void DefaultScene::update(const GameTime & gameTime)
{
	for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
	{
		it->second->update(gameTime);
	}

	for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it)
	{
		it->second->update(gameTime);
	}

	for (auto it = m_drawables.begin(); it != m_drawables.end(); ++it)
	{
		it->second->update(gameTime);
	}
}

void DefaultScene::draw(const GameTime & gameTime)
{
	// TODO: shadowDrawPass(gameTime);
	// TODO: reflectDrawPass(gameTime);
	finalDrawPass(gameTime);
}

void DefaultScene::finalDrawPass(const GameTime & gameTime)
{
	for (auto it = m_drawables.begin(); it != m_drawables.end(); ++it)
	{
		it->second->draw(gameTime);
	}
}
