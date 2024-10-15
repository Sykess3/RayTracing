#pragma once

#include "Walnut/Image.h"
#include "Walnut/Layer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Engine.h"
#include "Interval.h"

#include <memory>

#include "imgui.h"


class Viewport : public Walnut::Layer
{
private:
public:

	static Viewport& Get();

	Viewport(std::shared_ptr<Engine> inEngine);
	virtual ~Viewport() override;

	virtual void OnUpdate(float ts) override;

	virtual void OnUIRender() override;

	std::shared_ptr<Engine> m_Engine;

	glm::vec2 Size() const { return glm::vec2((float)Width, (float)Height); }

	glm::vec4 GetClippedPosition(const glm::vec2& windowPos);

	bool IsWithin(const glm::vec2& windowPos);

	uint32_t Width;
	uint32_t Height;

	glm::vec2 Min;
	glm::vec2 Max;
	glm::vec2 ContentPosition;
};