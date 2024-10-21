#include "BHV_Node.h"
#include "Scene.h"

bool BVH_Node::box_compare(const Sphere& a, const Sphere& b, int axis_index)
{
	auto a_axis_interval = a.BoundingBox.axis_interval(axis_index);
	auto b_axis_interval = b.BoundingBox.axis_interval(axis_index);
	return a_axis_interval.Min < b_axis_interval.Min;
}

void BVH_Node::RayCast(const Ray& ray, HitDetection& transientHit, const Scene* scene)
{
	if (m_boundingObjectIndex >= 0)
	{
		RayCastHit hit = scene->TestHitSphere(ray, m_boundingObjectIndex, transientHit.CurrentHitInterval);
		if (hit.ObjectIndex != -1)
		{
			if (transientHit.CurrentHitInterval.Max > hit.HitDistance)
			{
				transientHit.CurrentHitInterval.Max = hit.HitDistance;
				transientHit.HitObject = hit.ObjectIndex;
				return;
			}
		}
	}

	if (!m_bbox.hit(ray, transientHit.CurrentHitInterval))
	{
		return;
	}

	if (m_left)
	{
		m_left->RayCast(ray, transientHit, scene);
	}
	if (m_right) 
	{
		m_right->RayCast(ray, transientHit, scene);
	}
}
