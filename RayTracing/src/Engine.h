#pragma once

#include "Walnut/Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"


class Engine
{
public:

	Engine(std::shared_ptr<Camera> inCamera, std::shared_ptr<Scene> inScene, std::shared_ptr<Renderer> inRenderer)
		: m_Camera(inCamera),
		m_Scene(inScene),
		m_Renderer(inRenderer),
		m_LastRenderTime(0.0f)

	{

	}

	void Render(float viewportHeight, float viewportWidth)
	{
		Walnut::Timer timer;

		m_Renderer->OnResize(viewportWidth, viewportHeight);
		m_Camera->OnResize(viewportWidth, viewportHeight);
		m_Renderer->Render(*m_Scene, *m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

public:
	std::shared_ptr<Renderer> m_Renderer;
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Scene> m_Scene;
	float m_LastRenderTime;
};

