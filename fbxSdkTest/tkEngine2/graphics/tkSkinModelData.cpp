/*!
 *@brief	スキンモデルデータ。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkSkinModelData.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	CSkinModelData::CSkinModelData()
	{
	}
	CSkinModelData::~CSkinModelData()
	{
	}
	
	bool CSkinModelData::Load(const wchar_t* filePath)
	{
		DirectX::EffectFactory effectFactory(Engine().GetD3DDevice());
		m_modelDx = DirectX::Model::CreateFromCMO(Engine().GetD3DDevice(), filePath, effectFactory, false);
		return true;
	}
}

