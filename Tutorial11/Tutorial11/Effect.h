#pragma once

//エフェクト
class Effect
{
public:
	Effect();
	~Effect();
	//ロード。
	void Load(const char* shaderFilePath);
	void BeginRender(ID3D11DeviceContext* deiceContext);
private:
	ID3D11VertexShader* m_vs = NULL;	//頂点シェーダー。
	ID3D11PixelShader* m_ps = NULL;	//ピクセルシェーダー。
	ID3DBlob* m_vsBlobOut = NULL;
	ID3DBlob* m_psBlobOut = NULL;
	ID3D11InputLayout* m_inputLayout = NULL;	//入力レイアウト。

};

