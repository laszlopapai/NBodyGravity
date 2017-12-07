#pragma once

#include <Interfaces/IObject.h>
#include <Scene/Transformation.h>

namespace Cefied
{
	namespace Game
	{
		class CameraController : public IObject
		{
		public:
			explicit CameraController(Transformation & cameraTransformation);

			bool initialize() override;

			void update(const GameTime & gameTime) override;

		private:
			Transformation & m_cameraTransformation;
			float m_stepSize;
			int   m_stepFactor;

			float m_roll = 0;
			float m_pitch = 0;
		};
	}
}
