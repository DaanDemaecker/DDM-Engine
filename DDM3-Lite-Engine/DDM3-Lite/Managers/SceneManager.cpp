// SceneManager.cpp

// Header include
#include "SceneManager.h"

// File includes
#include "Engine/Scene.h"

#include "BaseClasses/GameObject.h"

#include "Components/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"

void DDM3::SceneManager::EndProgram()
{
    m_pScenes.clear();
    m_ActiveScene = nullptr;
}

std::shared_ptr<DDM3::Scene> DDM3::SceneManager::CreateScene(const std::string& name)
{
    const auto& scene = std::shared_ptr<DDM3::Scene>(new Scene(name));
    m_pScenes.push_back(scene);

    return scene;
}

void DDM3::SceneManager::DeleteScene(const std::string& name)
{
    auto scene = GetScene(name);

    if (scene != nullptr)
    {
        m_pScenes.erase(std::remove(m_pScenes.begin(), m_pScenes.end(), scene), m_pScenes.end());
    }
}

std::shared_ptr<DDM3::Scene> DDM3::SceneManager::GetScene(const std::string& name)
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

void DDM3::SceneManager::NextScene()
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

void DDM3::SceneManager::PreviousScene()
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

void DDM3::SceneManager::SetActiveScene(std::shared_ptr<Scene> scene)
{
    m_NextActiveScene = scene;
}

void DDM3::SceneManager::SetActiveScene(const std::string& name)
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

std::shared_ptr<DDM3::Scene> DDM3::SceneManager::GetActiveScene()
{
    return m_ActiveScene;
}

void DDM3::SceneManager::StartFrame()
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

void DDM3::SceneManager::EarlyUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->EarlyUpdate();
    }
}

void DDM3::SceneManager::Update()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Update();
    }
}

void DDM3::SceneManager::FixedUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->FixedUpdate();
    }
}

void DDM3::SceneManager::LateUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->LateUpdate();
    }
}

void DDM3::SceneManager::PostUpdate()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->PostUpdate();
    }
}

void DDM3::SceneManager::OnGui()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->OngGUI();
    }
}

void DDM3::SceneManager::PrepareRender()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->PrepareRender();
    }
}

void DDM3::SceneManager::RenderSkybox()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->RenderSkyBox();
    }
}

void DDM3::SceneManager::RenderDepth() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->RenderDepth();
    }
}

void DDM3::SceneManager::Render() const
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Render();
    }
}

void DDM3::SceneManager::Cleanup()
{
    if (m_ActiveScene != nullptr)
    {
        m_ActiveScene->Cleanup();
    }
}

const std::shared_ptr<DDM3::CameraComponent> DDM3::SceneManager::GetCamera() const
{
    if (m_ActiveScene != nullptr)
    {
        return m_ActiveScene->GetCamera();
    }
    else if (m_NextActiveScene != nullptr)
    {
        return m_NextActiveScene->GetCamera();
    }

    return std::shared_ptr<CameraComponent>();
}

const std::shared_ptr<DDM3::DirectionalLightComponent> DDM3::SceneManager::GetGlobalLight() const
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
