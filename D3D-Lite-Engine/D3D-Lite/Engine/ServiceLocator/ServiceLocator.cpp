// ServiceLocator.cpp
#include "ServiceLocator.h"

std::unique_ptr<DDM3::DefaultModelLoader> DDM3::ServiceLocator::m_pDefaultModelLoaderInstance{ std::make_unique<DDM3::DefaultModelLoader>() };
std::unique_ptr<DDM3::ModelLoader> DDM3::ServiceLocator::m_pModelLoaderInstance{ nullptr };

DDM3::ModelLoader& DDM3::ServiceLocator::GetModelLoader()
{
    return m_pModelLoaderInstance.get() == nullptr ? *m_pDefaultModelLoaderInstance.get() : *m_pModelLoaderInstance.get();
}

void DDM3::ServiceLocator::RegisterModelLoader(std::unique_ptr<ModelLoader> modelLoader)
{
    if (modelLoader != nullptr)
    {
        m_pModelLoaderInstance = std::move(modelLoader);
    }
}
