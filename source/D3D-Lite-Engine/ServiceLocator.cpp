// ServiceLocator.cpp
#include "ServiceLocator.h"

std::unique_ptr<D3D::DefaultModelLoader> D3D::ServiceLocator::m_pDefaultModelLoaderInstance{ std::make_unique<D3D::DefaultModelLoader>() };
std::unique_ptr<D3D::ModelLoader> D3D::ServiceLocator::m_pModelLoaderInstance{ nullptr };

D3D::ModelLoader& D3D::ServiceLocator::GetModelLoader()
{
    return m_pModelLoaderInstance.get() == nullptr ? *m_pDefaultModelLoaderInstance.get() : *m_pModelLoaderInstance.get();
}

void D3D::ServiceLocator::RegisterModelLoader(std::unique_ptr<ModelLoader> modelLoader)
{
    if (modelLoader != nullptr)
    {
        m_pModelLoaderInstance = std::move(modelLoader);
    }
}
