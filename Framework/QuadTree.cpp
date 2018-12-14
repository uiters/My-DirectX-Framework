﻿#include "stdafx.h"
#include "QuadTree.h"
#include "Graphic.h"
#include "Rigidbody.h"
#include "GameManager.h"
#include "Collider.h"

using namespace Framework;

CQuadTree::CQuadTree(Vector2 size)
{
	m_id = 0;
	m_level = 0;
	m_bounds = Rect(Vector2(0,0), size);
}

CQuadTree::CQuadTree(int id, int level, Rect bounds)
{
	m_id = id;
	m_level = level;
	m_bounds = bounds;
}

void CQuadTree::Update(DWORD dt)
{
}

void CQuadTree::Render()
{
	CGraphic::GetInstance()->DrawRectangle(m_bounds, D3DCOLOR_XRGB(255, 0, 0));
	if(m_pNodes[0])
		for (CQuadTree* node : m_pNodes)
		{
			node->Render();
		}
}

void CQuadTree::clearDynamicObject()
{
	//Remove dynamic gameObjects
	auto tmp = m_pObjects;
	for (CGameObject* game_object : tmp)
	{
		if (!game_object->GetComponent<CRigidbody>()->GetIsKinematic()) 
			m_pObjects.remove(game_object);
	}

	int nodesCanDelete = 0;

	if (m_pNodes[0]) {
		for (int i = 0; i < 4; i++)
		{
			m_pNodes[i]->clearDynamicObject();

			if(!m_pNodes[i]->m_pObjects.size() && !m_pNodes[i]->m_pNodes[0])
			{
				nodesCanDelete++;
			}
		}
	}

	if(nodesCanDelete == 4)
		for (int i = 0; i < 4; i++)
			SAFE_DELETE(m_pNodes[i]);
}

void CQuadTree::split()
{
	const int subWidth = m_bounds.Size().x / 2;
	const int subHeight = m_bounds.Size().y / 2;
	const int x = m_bounds.left;
	const int y = m_bounds.top;

	m_pNodes[0] = new CQuadTree(m_id * 10 + 1, m_level + 1, Rect(Vector2(x + subWidth, y), Vector2(subWidth, subHeight)));
	m_pNodes[1] = new CQuadTree(m_id * 10 + 2, m_level + 1, Rect(Vector2(x, y), Vector2(subWidth, subHeight)));
	m_pNodes[2] = new CQuadTree(m_id * 10 + 3, m_level + 1, Rect(Vector2(x, y + subHeight), Vector2(subWidth, subHeight)));
	m_pNodes[3] = new CQuadTree(m_id * 10 + 4, m_level + 1, Rect(Vector2(x + subWidth, y + subHeight), Vector2(subWidth, subHeight)));
}

bool CQuadTree::remove(CGameObject* gameObject)
{
	if(!CGameManager::GetInstance()->IsRunning())
	{
		if (const int size = m_pObjects.size())
		{
			m_pObjects.remove(gameObject);

			if (size != m_pObjects.size())
				return true;
		}

		if(m_pNodes[0])
		{
			int i = 0;
			while (i < 4 && !m_pNodes[i]->remove(gameObject))
				i++;
			
			return i != 4;
		}
	}

	return false;
}

int CQuadTree::getQuadrant(Rect rectangle) const
{
	int index = -1;
	const float verticalMidpoint = m_bounds.left + static_cast<float>(m_bounds.Size().x / 2);
	const float horizontalMidpoint = m_bounds.top + static_cast<float>(m_bounds.Size().y / 2);

	const bool topQuadrant = rectangle.bottom < horizontalMidpoint;
	const bool bottomQuadrant = rectangle.top > horizontalMidpoint;

	if (rectangle.right < verticalMidpoint) {
		if (topQuadrant) index = 1;
		else if (bottomQuadrant) index = 2;
	}
	else if (rectangle.left > verticalMidpoint)
	{
		if (topQuadrant) index = 0;
		else if (bottomQuadrant) index = 3;
	}

	return index;
}

tinyxml2::XMLElement* CQuadTree::ToXmlElement(tinyxml2::XMLDocument &doc) const
{
	tinyxml2::XMLElement *result = doc.NewElement("Node");

	result->SetAttribute("id", m_id);
	result->SetAttribute("x", m_bounds.left);
	result->SetAttribute("y", m_bounds.top);
	result->SetAttribute("width", m_bounds.Size().x);
	result->SetAttribute("height", m_bounds.Size().y);

	if (m_pObjects.size()) {
		auto gameObjects = doc.NewElement("GameObjects");
		for (CGameObject* const game_object : m_pObjects)
		{
			const auto transform = game_object->GetComponent<CTransform>();
			const auto collider = game_object->GetComponent<CBoxCollider>();

			const Vector2 pos = transform->Get_Position();
			const Vector2 scale = transform->Get_Scale();
			const Vector2 anchor = collider->GetAnchor();
			const Vector2 size = collider->GetSize();

			auto gameObject = doc.NewElement("GameObjectID");
			gameObject->SetAttribute("id", static_cast<int>(game_object->GetID()));
			gameObject->SetAttribute("name", game_object->GetName().c_str());
			gameObject->SetAttribute("posX", pos.x);
			gameObject->SetAttribute("posY", pos.y);
			gameObject->SetAttribute("scaleX", scale.x);
			gameObject->SetAttribute("scaleY", scale.y);
			gameObject->SetAttribute("anchorX", anchor.x);
			gameObject->SetAttribute("anchorY", anchor.y);
			gameObject->SetAttribute("sizeX", size.x);
			gameObject->SetAttribute("sizeY", size.y);
			gameObjects->InsertEndChild(gameObject);
		}
		result->InsertFirstChild(gameObjects);
	}

	if(m_pNodes[0])
	{
		auto pNodes = doc.NewElement("ChildNodes");
		for (CQuadTree* const node : m_pNodes)
		{
			pNodes->InsertEndChild(node->ToXmlElement(doc));
		}
		result->InsertEndChild(pNodes);
	}

	return result;
}

void CQuadTree::SaveToXml(const char* xmlPath)
{
	FILE* file;
	fopen_s(&file, xmlPath, "wb");

	tinyxml2::XMLDocument doc;

	auto pRoot = doc.NewElement("QuadTree");
	doc.InsertFirstChild(pRoot);

	pRoot->InsertFirstChild(ToXmlElement(doc));

	doc.SaveFile(file);
	fclose(file);
}

void CQuadTree::LoadFromXml(tinyxml2::XMLElement *node)
{
	this->m_id = node->IntAttribute("id");
	this->m_bounds = Bound(Vector2(node->IntAttribute("y"), node->IntAttribute("x")),
	                 Vector2(node->IntAttribute("width"), node->IntAttribute("height")));
	if(auto gameObjects = node->FirstChildElement("GameObjects"))
	{
		tinyxml2::XMLElement * gameObjectXML = gameObjects->FirstChildElement("GameObjectID");
		CScene *scene = CGameManager::GetInstance()->GetCurrentScene();
		while (gameObjectXML != nullptr)
		{
			auto gameObject = scene->FindGameObject(gameObjectXML->IntAttribute("id", -1));

			if (gameObject) {
				gameObject->GetComponent<CTransform>()
					->Set_Position(Vector2(gameObjectXML->IntAttribute("posX", 0), gameObjectXML->IntAttribute("posY", 0)))
					->Set_Scale(Vector2(gameObjectXML->IntAttribute("scaleX", 1), gameObjectXML->IntAttribute("scaleY", 1)));

				gameObject->GetComponent<CBoxCollider>()
					->SetSize(Vector2(gameObjectXML->IntAttribute("sizeX", 0), gameObjectXML->IntAttribute("sizeY", 0)))
					->SetAnchor(Vector2(gameObjectXML->IntAttribute("anchorX", 0.5), gameObjectXML->IntAttribute("anchorY", 0.5)));

				this->m_pObjects.push_back(gameObject);
			}
			gameObjectXML = gameObjectXML->NextSiblingElement("GameObjectID");
		}

	}

	if (auto childNodes = node->FirstChildElement("ChildNodes"))
	{
		split();
		tinyxml2::XMLElement * childNode = childNodes->FirstChildElement("Node");
		int index = 0;
		while (childNode)
		{
			m_pNodes[index++]->LoadFromXml(childNode);
			childNode = childNode->NextSiblingElement("Node");
		}

	}
}

int CQuadTree::getFitId(Rect rectangle) const
{
	int id = m_id;

	// If this node is split 
	if(m_pNodes[0]) 
	{
		// If having a appropriate quadrant for this rectangle
		const int index = getQuadrant(rectangle);
		if(index != -1)
		{
			id = m_pNodes[index]->getFitId(rectangle);
		}
	}

	// Else this node does not have sub node or no sub node can fit with this rectangle
	return id;
}

void CQuadTree::insert(CGameObject *gameObject)
{
	//Get bound of Game Object
	const Rect bound = gameObject->GetComponent<CCollider>()->GetBoundGlobal();

	// Check if this node has sub node, get quadrant and recursive insert game object 
	// into appropriate sub node, then break out of this function
	if(m_pNodes[0])
	{
		const int index = getQuadrant(bound);
		if(index!=-1)
		{
			m_pNodes[index]->insert(gameObject);			
			return;
		}
	}

	// Add game object to list object of this node
	m_pObjects.push_back(gameObject);

	// Check if node has more than max objects and level lower than max level, 
	// split this node and add to appropriate sub node
	if(m_pObjects.size() > MAX_QUAD_TREE_OBJECTS && m_level < MAX_QUAD_TREE_LEVEL)
	{
		if(!m_pNodes[0])
		{
			split();
		}

		// To store game object to remove after that
		std::list<CGameObject*> removedObjects = {}; 

		for (CGameObject* object : m_pObjects)
		{
			const int index = getQuadrant(object->GetComponent<CCollider>()->GetBoundGlobal());
			if(index!=-1)
			{
				m_pNodes[index]->insert(object);
				removedObjects.push_back(object);
			}
		}
		for (CGameObject* object : removedObjects)
		{
			m_pObjects.remove(object);
		}
	}
}

std::list<CGameObject*> CQuadTree::query(Rect rectangle)
{
	std::list<CGameObject*> result = {};

	// Check if it overlap with this node, then get all objects of this node to result;
	/*if (!m_bounds.intersect(rectangle))
		return result;*/

	for (CGameObject* object : m_pObjects)
	{
		result.push_back(object);
	}

	//Check if this node is split, get all objects of sub node intersecting with this rectangle
	const int index = getQuadrant(rectangle);
	if(index != -1 && m_pNodes[0]) // If this rectangle is cover by 1 sub node, just get objects of this node
	{
		auto tmp = m_pNodes[index]->query(rectangle);
		for (CGameObject* object : tmp)
		{
			result.push_back(object);
		}
	}
	else if(m_pNodes[0])// Else check all of sub node
	{
		for (CQuadTree* node : m_pNodes)
		{
			auto tmp = node->query(rectangle);
			for (CGameObject* object : tmp)
			{
				result.push_back(object);
			}
		}
	}

	return result;
}
