#pragma once

#include "Walnut/Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Renderer.h"
#include "AssetManager.h"
#include <fstream>


class Engine
{
public:

	Engine(std::shared_ptr<Camera> inCamera, std::shared_ptr<Scene> inScene, std::shared_ptr<Renderer> inRenderer, std::shared_ptr<AssetManager> inAssetManager)
		: m_Camera(inCamera),
		m_Scene(inScene),
		m_Renderer(inRenderer),
		m_AssetManager(inAssetManager),
		m_LastRenderTime(0.0f)

	{

	}
	~Engine() 
	{
		std::ofstream file("Saved/Profile/number.txt");
		float result = m_AccumulatedRenderTime / (float)m_ProfileFrameCount;
		file << result;
		file.close();
	}

	void Render(float viewportHeight, float viewportWidth)
	{
		m_ProfileFrameCount++;
		Walnut::Timer timer;

		m_Renderer->OnResize(viewportWidth, viewportHeight);
		m_Camera->OnResize(viewportWidth, viewportHeight);
		m_Renderer->Render(*m_Scene, *m_Camera);
		Renderer* t = m_Renderer.get() ;

		m_LastRenderTime = timer.ElapsedMillis();
		m_AccumulatedRenderTime += m_LastRenderTime;
	}

public:
	std::shared_ptr<Renderer> m_Renderer;
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Scene> m_Scene;
	std::shared_ptr<AssetManager> m_AssetManager;
	float m_LastRenderTime;
	float m_AccumulatedRenderTime = 0;
	int m_ProfileFrameCount = 0;
};

