﻿#pragma once
#include "Component.h"
#include "PhysicObserver.h"
#include "Physic.h"

namespace Framework {
	class CMonoBehavior : public CComponent, public CPhysicObserver
	{

	private:
		CMonoBehavior() = default;
	public:
		CMonoBehavior(const CMonoBehavior &monoBehavior) : CComponent(monoBehavior) {}
		CMonoBehavior(CGameObject* gameObject) : CComponent(gameObject) {}
		virtual ~CMonoBehavior() = default;

		void Update(DWORD dt) override {}
		void Render() override {}
		CMonoBehavior* Clone() override = 0;
	};
}
