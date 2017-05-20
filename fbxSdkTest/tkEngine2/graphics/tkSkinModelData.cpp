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
		/*FbxManager& fbxManager = Engine().GetFbxManager();
		//インポーターを作成。
		FbxImporter* importer = FbxImporter::Create(&fbxManager, "Importer");
		if (importer == nullptr) {
			//インポーターの作成に失敗した。
			return false;
		}
		//シーンクラスを作成。
		FbxScene* scene = FbxScene::Create(&fbxManager, "Scene");
		if (scene == nullptr) {
			//シーンの作成に失敗した。
			return false;
		}
		//FBXの読み込み。
		importer->Initialize(filePath);
		//読み込んだFBXファイルからシーン情報を取り出す。
		importer->Import(scene);

		//シーンを解析。
		Analyze(scene->GetRootNode());

		//読み込みが終わったらもういらないから破棄する。
		importer->Destroy();
		scene->Destroy();*/
		return true;
	}
}

