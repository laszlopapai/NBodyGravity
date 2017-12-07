#include "App.h"
#include <winerror.h>
#include <ctime>
#include <Tools/Macros.h>


#include "NBodyGravity.h"
#include "DefaultScene.h"
#include "CameraController.h"

App::App() :
	m_window("NBodyGravity", 1280, 720),
	m_renderer(true, &m_window)
{ }

bool App::initialize()
{
	InitCheck(m_window.initialize(),
		"Window initialization failure.");
	
	m_renderer.setHWND(m_window.getHWND());

	InitCheck(m_renderer.initialize(),
		"Renderer initialization failure.");

	InitCheck(m_sceneManager.initialize(),
		"Scene manager initialization failure.");
	
	m_ioHandler.setHWND(m_window.getHWND());

	InitCheck(m_ioHandler.initialize(),
		"IO handler initialization failure.");


	auto camera = new Camera(m_window);

	auto scene = new DefaultScene(camera);
	m_sceneManager.addScene("Default", scene);

	InitCheck(scene->initialize(),
		"Scene initialization failed.");	
	{
		const auto camController = new CameraController(camera->getTransformation());
		scene->addController("CameraController", camController);
		
		auto object = new NBodyGravity;
		scene->addDrawableObject("gravity", object);

		InitCheck(object->initialize(),
			"Scene initialization failed.");
	}

	m_sceneManager.loadScene("Default");

	return true;
}

int App::run()
{
	MSG msg;
	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Initial times
	GameTime gameTime = m_gameTimer.startNew();

	while (msg.message != WM_QUIT)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_gameTimer.update(gameTime);
		m_consoleFPS.update(gameTime);

		m_ioHandler.getState();

		if (m_ioHandler.isKeyPressed(DIK_ESCAPE))
			PostMessage(m_window.getHWND(), WM_DESTROY, 0, 0);
		
		// Generate new frame
		//m_renderer.beginScene(.4f, .6f, .9f, 1);
		m_renderer.beginScene(.0f, .0f, .0f, 1);
		
		if (IScene *scene = m_sceneManager.getCurrentScene())
		{
			scene->update(gameTime);
			scene->draw(gameTime);
		}

		m_renderer.endScene();
	}

	return 0;
}



int main(int argc, char *argv[])
{
	int result = 0;

	if (App::get().initialize()) {
		result = App::get().run();
	}
	else {
		result = 1;
	}

	App::unLoad();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	if (_CrtDumpMemoryLeaks() || result)
		std::cin.ignore();

	return result;
}
