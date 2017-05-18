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
	bool CSkinModelData::Analyze(FbxNode* node)
	{
		FbxMesh* mesh = node->GetMesh();

		if (mesh != nullptr) {
			//���b�V�������B
			//mesh->
		}
		for (int i = 0; i < node->GetChildCount(); i++) {
			Analyze(node->GetChild(i));
		}
	}
	bool CSkinModelData::Load(const char* filePath)
	{
		FbxManager& fbxManager = Engine().GetFbxManager();
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
		scene->Destroy();
		return true;
	}
}

