// SceneManager.cpp

// Header include
#include "SceneManager.h"

// File includes
#include "Engine/Scene.h"

#include "BaseClasses/GameObject.h"

#include "Components/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"

void DDM::SceneManager::EndProgram()
{
    m_pScenes.clear();
    m_ActiveScene = nullptr;
}

std::shared_ptr<DDM::Scene> DDM::SceneManager::CreateScene(const std::string& name)
{
    const auto& scene = std::shared_ptr<DDM::Scene>(new Scene(name));
    m_pScenes.push_back(scene);

    return scene;
}

void DDM::SceneManager::DeleteScene(const std::string& name)
{
    auto scene = GetScene(name);

    if (scene != nullptr)
    {
        m_pScenes.erase(std::remove(m_pScenes.begin(), m_pScenes.end(), scene), m_pScenes.end());
    }
}

std::shared_ptr<DDM::Scene> DDM::SceneManager::GetScene(const std::string& name)
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

void DDM::SceneManager::NextScene()
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

void DDM::SceneManager::PreviousScene()
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

void DDM::SceneManager::SetActiveScene(std::shared_ptr<Scene> scene)
{
    m_NextActiveScene = scene;
}

void DDM::SceneManager::SetActiveScene(const std::string& name)
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

std::shared_ptr<DDM::Scene> DDM::SceneManager::GetActiveScene()
{
    return m_ActiveScene;
}

void DDM::SceneManager::StartFrame()
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

void DDM::SceneManager::EarlyUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->EarlyUpdate();
    }
}

void DDM::SceneManager::Update()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Update();
    }
}

void DDM::SceneManager::FixedUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->FixedUpdate();
    }
}

void DDM::SceneManager::LateUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->LateUpdate();
    }
}

void DDM::SceneManager::PostUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->PostUpdate();
    }
}

void DDM::SceneManager::OnGui()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->OngGUI();
    }
}

void DDM::SceneManager::RenderSkybox()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->RenderSkyBox();
    }
}

void DDM::SceneManager::RenderDepth() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->RenderDepth();
    }
}

void DDM::SceneManager::Render() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Render();
    }
}

void DDM::SceneManager::RenderTransparancy() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->RenderTransparancy();
    }
}

const std::shared_ptr<DDM::CameraComponent> DDM::SceneManager::GetCamera() const
{
    if (m_ActiveScene != nullptr)
    {
        return m_ActiveScene->GetCamera();
    }
    else if (m_NextActiveScene != nullptr)
    {
        return m_NextActiveScene->GetCamera();
    }

    return nullptr;
}

const std::shared_ptr<DDM::DirectionalLightComponent> DDM::SceneManager::GetGlobalLight() const
{
    if (m_ActiveScene != nullptr)
    {
        return m_ActiveScene->GetLight();
    }
    else if (m_NextActiveScene != nullptr)
    {
        return m_NextActiveScene->GetLight();
    }

    return std::shared_ptr<DirectionalLightComponent>();
}
