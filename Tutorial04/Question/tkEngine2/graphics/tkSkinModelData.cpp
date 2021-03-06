/*!
 *@brief	スキンモデルデータ。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/graphics/tkSkinModelData.h"
#include "tkEngine2/tkEngine.h"

namespace tkEngine2{
	/*!
	*@brief
	*  スキンモデルエフェクト。
	*/
	class CSkinModelEffect : public DirectX::IEffect {
		CShader m_vsShader;
		CShader m_psShader;
		ID3D11ShaderResourceView* diffuseTex = nullptr;
	public:
		CSkinModelEffect()
		{
			m_vsShader.Load("Assets/shader/model.fx", "VSMainSkin", CShader::EnType::VS);
			m_psShader.Load("Assets/shader/model.fx", "PSMain", CShader::EnType::PS);
		}
		void __cdecl Apply(ID3D11DeviceContext* deviceContext) override
		{
			deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
			deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
			deviceContext->PSSetShaderResources(0, 1, &diffuseTex);
			
		}

		void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
		{
			*pShaderByteCode = m_vsShader.GetByteCode();
			*pByteCodeLength = m_vsShader.GetByteCodeSize();
		}
		void SetDiffuseTexture(ID3D11ShaderResourceView* tex) 
		{
			diffuseTex = tex;
		}
	};
	/*!
	 *@brief
	 *  エフェクトファクトリ。
	 */
	class CSkinModelEffectFactory : public DirectX::EffectFactory {
	public:
		CSkinModelEffectFactory(ID3D11Device* device) :
			EffectFactory(device) {}
		std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(const EffectInfo& info,  ID3D11DeviceContext* deviceContext)override
		{
			std::shared_ptr<CSkinModelEffect> effect = std::make_shared<CSkinModelEffect>();
			if (info.diffuseTexture && *info.diffuseTexture)
			{
				ID3D11ShaderResourceView* texSRV;
				DirectX::EffectFactory::CreateTexture(info.diffuseTexture, deviceContext, &texSRV);
				effect->SetDiffuseTexture(texSRV);
			}
			return effect;
			return DirectX::EffectFactory::CreateEffect(info, deviceContext);
		}

		void __cdecl CreateTexture( const wchar_t* name,  ID3D11DeviceContext* deviceContext,  ID3D11ShaderResourceView** textureView) override
		{
			return DirectX::EffectFactory::CreateTexture(name, deviceContext, textureView);
		}
	};
	CSkinModelData::CSkinModelData()
	{
	}
	CSkinModelData::~CSkinModelData()
	{
	}
	
	bool CSkinModelData::Load(const wchar_t* filePath)
	{
		CSkinModelEffectFactory effectFactory(Engine().GetD3DDevice());
		m_modelDx = DirectX::Model::CreateFromCMO(Engine().GetD3DDevice(), filePath, effectFactory, false);
		return true;
	}
}

