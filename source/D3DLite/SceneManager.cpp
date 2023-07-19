#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

#include "Model.h"

void D3D::SceneManager::EndProgram()
{
    m_pScenes.clear();
    m_ActiveScene = nullptr;
}

std::shared_ptr<D3D::Scene> D3D::SceneManager::CreateScene(const std::string& name)
{
    const auto& scene = std::shared_ptr<D3D::Scene>(new Scene(name));
    m_pScenes.push_back(scene);

    return scene;
}

void D3D::SceneManager::DeleteScene(const std::string& name)
{
    auto scene = GetScene(name);

    if (scene != nullptr)
    {
        m_pScenes.erase(std::remove(m_pScenes.begin(), m_pScenes.end(), scene), m_pScenes.end());
    }
}

std::shared_ptr<D3D::Scene> D3D::SceneManager::GetScene(const std::string& name)
{
    for (auto& scene : m_pScenes)
    {
        if (scene->GetName() == name)
        {
            return scene;
        }
    }
    return nullptr;
}

void D3D::SceneManager::NextScene()
{
    int currentSceneIndex{ -1 };

    for (int i{}; i < static_cast<int>(m_pScenes.size()); ++i)
    {
        if (m_pScenes[i] == m_ActiveScene)
        {
            currentSceneIndex = i;
        }
    }

    if (currentSceneIndex >= 0)
    {
        int nextActiveScene = (currentSceneIndex + 1) % m_pScenes.size();
        SetActiveScene(m_pScenes[nextActiveScene]);
    }
}

void D3D::SceneManager::PreviousScene()
{
    int currentSceneIndex{ -1 };

    for (int i{}; i < static_cast<int>(m_pScenes.size()); ++i)
    {
        if (m_pScenes[i] == m_ActiveScene)
        {
            currentSceneIndex = i;
        }
    }

    if (currentSceneIndex >= 0)
    {
        int nextActiveScene = (currentSceneIndex - 1);

        if (nextActiveScene < 0)
            nextActiveScene = static_cast<int>(m_pScenes.size() - 1);

        SetActiveScene(m_pScenes[nextActiveScene]);
    }
}

void D3D::SceneManager::SetActiveScene(std::shared_ptr<Scene> scene)
{
    m_NextActiveScene = scene;
}

void D3D::SceneManager::SetActiveScene(const std::string& name)
{
    for (auto& scene : m_pScenes)
    {
        if (scene->GetName() == name)
        {
            SetActiveScene(scene);
            return;
        }
    }
}

void D3D::SceneManager::StartFrame()
{
    if (m_NextActiveScene != nullptr)
    {
        if (m_ActiveScene != nullptr)
        {
            m_ActiveScene->OnSceneUnload();
        }
        m_ActiveScene = m_NextActiveScene;
        m_ActiveScene->OnSceneLoad();
        m_NextActiveScene = nullptr;
    }


    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->StartFrame();
    }
}

void D3D::SceneManager::Update()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Update();
    }
}

void D3D::SceneManager::FixedUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->FixedUpdate();
    }
}

void D3D::SceneManager::LateUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->LateUpdate();
    }
}

void D3D::SceneManager::OnGui()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->OngGUI();
    }
}

void D3D::SceneManager::Render() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Render();
    }
}

void D3D::SceneManager::Cleanup()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Cleanup();
    }
}
