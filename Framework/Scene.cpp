﻿#include "stdafx.h"
#include "Scene.h"
#include "GameManager.h"
#include "Camera.h"
#include "Rigidbody.h"

using namespace Framework;

CScene::CScene(std::string name, Vector2 quadTreeSize)
{
	if (!this->Init(name, quadTreeSize))
		delete this;
}

bool CScene::Init(std::string name, Vector2 quadTreeSize)
{
	if (!InitMainCamera()) return false;

	struct stat buffer;
	if(stat("QuadTree.xml", &buffer) == 0)
	{
		m_loadedQuadTree = true;
	}

	m_pQuadTree = new CQuadTree(quadTreeSize);
	m_Name = name;

	return true;
}

bool CScene::InitMainCamera()
{
	bool result = false;
	do
	{
		m_pMainCamera = new CGameObject("Main Camera");
		m_pMainCamera->AddComponent<CCamera>();
		//TODO Check Init Camera
		result = true;
	} while (false);

	return result;
}

bool CScene::Release()
{
	bool result = false;
	do {
		for (auto gameObject : GetAllGameObjects())
		{
			SAFE_DELETE(gameObject)
		}

		m_pMainCamera->SetScene(nullptr);
		SAFE_DELETE(m_pMainCamera);

		result = true;
	} while (false);

	return result;
}

std::set<CGameObject*> CScene::GetListGameObject() const
{
	return m_gameObjectList;
}

std::set<CGameObject*> CScene::GetListDynamicGameObject() const
{
	return m_dynamicObjectList;
}

std::set<CGameObject*> CScene::GetListStaticGameObject() const
{
	return m_staticObjectList;
}

std::set<CGameObject*> CScene::GetListHalfStaticGameObject() const
{
	return m_halfStaticObjectList;
}

std::set<CGameObject*> CScene::GetAllGameObjects() const
{
	std::set<CGameObject*> result = {};
	result.insert(m_dynamicObjectList.begin(), m_dynamicObjectList.end());
	result.insert(m_staticObjectList.begin(), m_staticObjectList.end());
	result.insert(m_gameObjectList.begin(), m_gameObjectList.end());
	result.insert(m_halfStaticObjectList.begin(), m_halfStaticObjectList.end());

	return result;
}

std::set<CGameObject*> CScene::GetRenderGameObjects() const
{
	std::set<CGameObject*> result = {};
	result.insert(m_gameObjectList.begin(), m_gameObjectList.end());
	result.insert(m_dynamicObjectList.begin(), m_dynamicObjectList.end());
	auto quadTreeList = m_pQuadTree->query(Rect(m_pMainCamera->GetComponent<CTransform>()->Get_Position(), { 512, 512 }, { 0.5,0.5 }));
	result.insert(quadTreeList.begin(), quadTreeList.end());
	return result;
}

std::set<CGameObject*> CScene::GetUpdateGameObjects() const
{
	std::set<CGameObject*> result = {};
	result.insert(m_gameObjectList.begin(), m_gameObjectList.end());
	result.insert(m_dynamicObjectList.begin(), m_dynamicObjectList.end());
	result.insert(m_halfStaticObjectList.begin(), m_halfStaticObjectList.end());
	for (CGameObject* const object : m_staticObjectList)
	{
		if (object->GetComponent<CRigidbody>()->GetNeedUpdate())
			result.insert(object);
	}
	return result;
}

bool CScene::Destroy(CScene* scene)
{
	const auto result = scene->Release();
	SAFE_DELETE(scene);

	return result;
}

void CScene::Update(DWORD dt)
{
	CInput::GetInstance()->Update();

	for (CGameObject* pGameObject : GetUpdateGameObjects())
	{
		if (pGameObject&&pGameObject->GetIsActive())
				pGameObject->Update(dt);
	}

	if (m_pMainCamera) {
		if (m_pMainCamera->GetIsActive())
			m_pMainCamera->Update(dt);

		CGraphic::GetInstance()->SetTransform(m_pMainCamera->GetComponent<CCamera>()->GetOrthographicMatrix(),
			m_pMainCamera->GetComponent<CCamera>()->GetIdentityMatrix(),
			m_pMainCamera->GetComponent<CCamera>()->GetViewMatrix());
	}
}

void CScene::Render()
{
	/*std::set<CGameObject*> listRender = m_dynamicObjectList;
	listRender.insert(reinterpret_cast<CGameObject*>(m_pQuadTree));*/
	CGraphic::GetInstance()->Render(this);
}

bool CScene::AddGameObject(CGameObject* gameObject)
{
	if (!gameObject) return false;

	m_gameObjectList.insert(gameObject);
	gameObject->SetScene(this);
	AddColliderObject(gameObject);
	return true;
}

void CScene::RemoveGameObject(CGameObject* gameObject)
{
	m_dynamicObjectList.erase(gameObject);
	m_gameObjectList.erase(gameObject);
	m_halfStaticObjectList.erase(gameObject);
	m_staticObjectList.erase(gameObject);
}

CGameObject* CScene::FindGameObject(std::string name)
{
	for (CGameObject* const game_object : m_gameObjectList)
	{
		if (game_object->GetName() == name)
			return game_object;
	}

	return nullptr;
}

CGameObject* CScene::FindGameObject(DWORD id)
{
	for (CGameObject* const game_object : GetAllGameObjects())
	{
		if (game_object->GetID() == id)
			return game_object;
	}

	return nullptr;
}

void CScene::AddColliderObject(CGameObject* gameObject)
{
	if (gameObject->GetComponent<CCollider>())
	{
		const auto rigidBody = gameObject->GetComponent<CRigidbody>();
		if (rigidBody->GetIsKinematic())
		{
			m_staticObjectList.insert(gameObject);
			m_dynamicObjectList.erase(gameObject);
			m_gameObjectList.erase(gameObject);
			m_halfStaticObjectList.erase(gameObject);

			if (!m_loadedQuadTree) {
				m_pQuadTree->insert_s(gameObject);
			}
		}
		else if (rigidBody->GetLimitedArea() != Rect(0, 0, 0, 0))
		{
			m_staticObjectList.erase(gameObject);
			m_dynamicObjectList.erase(gameObject);
			m_gameObjectList.erase(gameObject);
			m_halfStaticObjectList.insert(gameObject);

			if (!m_loadedQuadTree) {
				m_pQuadTree->insert_s(gameObject);
			}
		}
		else
		{
			m_dynamicObjectList.insert(gameObject);
			m_staticObjectList.erase(gameObject);
			m_gameObjectList.erase(gameObject);
			m_halfStaticObjectList.erase(gameObject);

			if (!m_loadedQuadTree)
				m_pQuadTree->remove(gameObject);
		}
	}
}

void CScene::SaveQuadTree() const
{
	m_pQuadTree->SaveToXml("QuadTree.xml");
}

void CScene::LoadQuadTree() const
{
	if (m_loadedQuadTree) 
	{
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile("QuadTree.xml") == tinyxml2::XML_SUCCESS)
		{
			tinyxml2::XMLNode* pRoot = doc.FirstChild();

			tinyxml2::XMLElement * pNodeRoot = pRoot->FirstChildElement("Node");

			m_pQuadTree->LoadFromXml(pNodeRoot);
		}
	}
	else SaveQuadTree();
}
