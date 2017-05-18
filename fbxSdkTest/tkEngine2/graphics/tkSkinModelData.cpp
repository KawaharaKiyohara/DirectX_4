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
	bool CSkinModelData::Analyze(FbxNode* node)
	{
		FbxMesh* mesh = node->GetMesh();

		if (mesh != nullptr) {
			//メッシュ発見。
			//mesh->
		}
		for (int i = 0; i < node->GetChildCount(); i++) {
			Analyze(node->GetChild(i));
		}
	}
	bool CSkinModelData::Load(const char* filePath)
	{
		FbxManager& fbxManager = Engine().GetFbxManager();
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
		scene->Destroy();
		return true;
	}
}

