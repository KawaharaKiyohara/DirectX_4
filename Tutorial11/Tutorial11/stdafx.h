#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d11.h>

const int NUM_THREAD = 2;
extern ID3D11Device* g_pd3dDevice ;								//D3D11�f�o�C�X�B
extern ID3D11DeviceContext* g_pd3dDiferredContext[NUM_THREAD];	//DiferredContext�R���e�L�X�g�B
extern ID3D11DeviceContext* g_pd3dImmidiateContext;				//�����R���e�L�X�g�B

