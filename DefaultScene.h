#pragma once

#include <Interfaces/IScene.h>
#include <string>
#include <unordered_map>
#include <Interfaces/IDrawable.h>
#include <Scene/Camera.h>
#include <Tools/Macros.h>

namespace Cefied
{
	namespace Game
	{
		class DefaultScene : public IScene
		{
		public:
			explicit DefaultScene(ICamera * camera)
				: m_mainCamera(camera)
			{ }

			bool initialize() override;
			
			bool addCamera(std::string cameraName, ICamera * camera) override;
			ICamera * getCamera(std::string cameraName) override;

			bool setMainCamera(std::string cameraName) override;
			bool setChosenCamera(std::string cameraName) override;

			const ICamera* getMainCamera() override;
			const ICamera* getChosenCamera() override;
			
			bool addController(std::string name, IObject * controller);

			bool addDrawableObject(std::string name, IDrawable * drawable);

			void update(const GameTime & gameTime) override;
			void draw(const GameTime & gameTime) override;

			~DefaultScene() {
				
				for (auto it = m_controllers.begin(); it != m_controllers.end(); ++it)
					SafeDelete(it->second);

				for (auto it = m_drawables.begin(); it != m_drawables.end(); ++it)
					SafeDelete(it->second);

				for (auto it = m_cameras.begin(); it != m_cameras.end(); ++it)
					SafeDelete(it->second);
			}

		private:
			void finalDrawPass(const GameTime & gameTime);

		private:
			std::unordered_map<std::string, IObject*>   m_controllers;
			std::unordered_map<std::string, IDrawable*> m_drawables;
			std::unordered_map<std::string, ICamera*>   m_cameras;
//			std::unordered_map<std::string, ILight*>    m_lights;

			ICamera *m_mainCamera;
		};
	}
}
