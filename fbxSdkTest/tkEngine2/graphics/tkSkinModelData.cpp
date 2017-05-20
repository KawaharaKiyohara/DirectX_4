/*!
 *@brief	�X�L�����f���f�[�^�B
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
		//�C���|�[�^�[���쐬�B
		FbxImporter* importer = FbxImporter::Create(&fbxManager, "Importer");
		if (importer == nullptr) {
			//�C���|�[�^�[�̍쐬�Ɏ��s�����B
			return false;
		}
		//�V�[���N���X���쐬�B
		FbxScene* scene = FbxScene::Create(&fbxManager, "Scene");
		if (scene == nullptr) {
			//�V�[���̍쐬�Ɏ��s�����B
			return false;
		}
		//FBX�̓ǂݍ��݁B
		importer->Initialize(filePath);
		//�ǂݍ���FBX�t�@�C������V�[���������o���B
		importer->Import(scene);

		//�V�[������́B
		Analyze(scene->GetRootNode());

		//�ǂݍ��݂��I��������������Ȃ�����j������B
		importer->Destroy();
		scene->Destroy();*/
		return true;
	}
}

