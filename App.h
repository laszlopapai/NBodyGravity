#pragma once

#include <MainComponents/Renderer.h>
#include <MainComponents/Window.h>
#include <MainComponents/IOHandler.h>
#include <MainComponents/SceneManager.h>
#include <Tools/GameTimer.h>
#include <Tools/ConsoleFPS.h>

using namespace Cefied::Game;

class App
{
public:
	App();

	static App & get() {
		static App * instance = new App;
		return *instance;
	}

	static void unLoad() {
		delete &get();
	}

	static Window & getWindow() {
		return  get().m_window;
	}

	static Renderer & getRenderer() {
		return  get().m_renderer;
	}

	static IOHandler & getIOHandle() {
		return  get().m_ioHandler;
	}

	static SceneManager & getSceneManager() {
		return  get().m_sceneManager;
	}

	bool initialize();

	int run();


private:
	Window        m_window;
	Renderer      m_renderer;
	IOHandler     m_ioHandler;
	SceneManager  m_sceneManager;
	GameTimer     m_gameTimer;
	ConsoleFPS    m_consoleFPS;
};
