/*!
 * @brief	���m�N�����R���s���[�g�V�F�[�_�[�B
 */



//���̓e�N�X�`���B
Texture2D inTexture : register( t0 );
//�o�̓e�N�X�`���B
RWStructuredBuffer<uint>	outputBuffer : register(u0);

/*!
 * @brief	float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
 */
uint PackedFloat4ToRGBA32( float4 In )
{
	return (uint)(In.a * 255) << 24 
			| (uint)(In.b * 255) << 16 
			| (uint)(In.g * 255) << 8 
			| (uint)(In.r * 255);
}
float4 tex2D(uint2 uv)
{
	return inTexture.Load(uint3( uv.x, uv.y, 0));
}
float4 FxaaPixelShader( 
	uint3 pos, 
    float4 fxaaConsoleRcpFrameOpt,
    float4 fxaaConsoleRcpFrameOpt2,
    float fxaaQualityEdgeThreshold,
    float fxaaQualityEdgeThresholdMin,
    float fxaaConsoleEdgeSharpness,
    float fxaaConsoleEdgeThreshold,
    float fxaaConsoleEdgeThresholdMin
)
{
	//�A �����Ŏw�肵�Ă���͎̂��g�̃e�N�Z���ƁA
	//   �ׂ̃e�N�Z���Ƃ̉��d�a���t�F�b�`���邽�߂̂��́B
	//   �Ȃ̂ŃR���s���[�g�V�F�[�_�[�ł�1�e�N�Z���ׂ��t�F�b�`����
	//   49�s�ڂ���̃t�F�b�`�ŉ��d�a������Ă���B
	//�ߖT4�e�N�Z�����t�F�b�`�B
	uint4 nTex = float4( 
		pos.x + 1, 
		pos.y + 1,
		pos.x - 1, 
		pos.y - 1
	);
	float4 rgbyM = tex2D(pos.xy);
	float lumaNw = (tex2D(nTex.xy).y + rgbyM.y) * 0.5f;
	float lumaSw = (tex2D(nTex.xw).y + rgbyM.y) * 0.5f;
	float lumaNe = (tex2D(nTex.zy).y + rgbyM.y) * 0.5f;
	float lumaSe = (tex2D(nTex.zw).y + rgbyM.y) * 0.5f;
	
	
	float lumaM = rgbyM.y;
	
	
	//�ߖT�e�N�Z���̋P�x�̍��𒲂ׂ�B
	float lumaMaxNwSw = max(lumaNw, lumaSw);
    lumaNe += 1.0/384.0;
    float lumaMinNwSw = min(lumaNw, lumaSw);
/*--------------------------------------------------------------------------*/
    float lumaMaxNeSe = max(lumaNe, lumaSe);
    float lumaMinNeSe = min(lumaNe, lumaSe);
/*--------------------------------------------------------------------------*/
    float lumaMax = max(lumaMaxNeSe, lumaMaxNwSw);
    float lumaMin = min(lumaMinNeSe, lumaMinNwSw);
/*--------------------------------------------------------------------------*/
    float lumaMaxScaled = lumaMax * fxaaConsoleEdgeThreshold;
/*--------------------------------------------------------------------------*/
    float lumaMinM = min(lumaMin, lumaM);
    float lumaMaxScaledClamped = max(fxaaConsoleEdgeThresholdMin, lumaMaxScaled);
    float lumaMaxM = max(lumaMax, lumaM);
    float dirSwMinusNe = lumaSw - lumaNe;
    float lumaMaxSubMinM = lumaMaxM - lumaMinM;
    float dirSeMinusNw = lumaSe - lumaNw;
    if(lumaMaxSubMinM < lumaMaxScaledClamped){
		//�P�x�̍����������l�ȉ��������̂ŁA���̃s�N�Z���̓A���`�������Ȃ��B
		 return rgbyM;
	}
    
/*--------------------------------------------------------------------------*/
	//�P�x�̍��𗘗p���āA�M�U�M�U���������Ă���\���̍����e�N�Z�����t�F�b�`����B
    float2 dir;
    dir.x = dirSwMinusNe + dirSeMinusNw;
    dir.y = dirSwMinusNe - dirSeMinusNw;
   
/*--------------------------------------------------------------------------*/
    float2 dir1 = normalize(dir.xy);
   
    float4 rgbyN1 = tex2D(pos.xy - dir1 * fxaaConsoleRcpFrameOpt.zw);
    float4 rgbyP1 = tex2D(pos.xy + dir1 * fxaaConsoleRcpFrameOpt.zw);
/*--------------------------------------------------------------------------*/
    float dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * fxaaConsoleEdgeSharpness;
    float2 dir2 = clamp(dir1.xy / dirAbsMinTimesC, -2.0, 2.0);
/*--------------------------------------------------------------------------*/
    float4 rgbyN2 = tex2D(pos.xy - dir2 * fxaaConsoleRcpFrameOpt2.zw);
    float4 rgbyP2 = tex2D(pos.xy + dir2 * fxaaConsoleRcpFrameOpt2.zw);
    
/*--------------------------------------------------------------------------*/
	//�u�����h�u�����h�B
    float4 rgbyA = rgbyN1 + rgbyP1;
    float4 rgbyB = ((rgbyN2 + rgbyP2) * 0.25) + (rgbyA * 0.25);
/*--------------------------------------------------------------------------*/
    int twoTap = (rgbyB.y < lumaMin) || (rgbyB.y > lumaMax);
    
    if(twoTap){
		//�܂��P�x�̍����傫���̂ŁA�ēx�u�����h�B
		rgbyB.xyz = rgbyA.xyz * 0.5;
	}
    return rgbyB;
    
}
/*!
 * @brief	���C���֐��B
 */
[numthreads(2, 2, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID)
{
	//�@�s�N�Z���V�F�[�_�[�̎���zw�ŋ��߂Ă���̂͂P�e�N�Z���ׂ��t�F�b�`���邽�߂�UV���W�B
	//  �R���s���[�g�V�F�[�_�[�Ńe�N�X�`�������[�h����Ƃ��́A�e�N�Z�����W���g�����߁A�����ł�1���w�肷��B
	//float4 rcpFrame = float4(0.0f, 0.0f, 1.0f/g_sceneTexSize.x, 1.0f/g_sceneTexSize.y);
	float4 rcpFrame = float4(0.0f, 0.0f, 1.0f, 1.0f);	
	float4 color = FxaaPixelShader( 
		DTid,
        rcpFrame,							// float4 fxaaConsoleRcpFrameOpt,
        rcpFrame,							// float4 fxaaConsoleRcpFrameOpt2,
        0.166f,								// FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,							// FxaaFloat fxaaQualityEdgeThresholdMin,
        1.0f,								// FxaaFloat fxaaConsoleEdgeSharpness,
        0.4f,								// FxaaFloat fxaaConsoleEdgeThreshold,
        0.0833f								// FxaaFloat fxaaConsoleEdgeThresholdMin,
	);
	//float4��RGBA32�t�H�[�}�b�g�ɕϊ��B
	outputBuffer[DTid.x + DTid.y * 256] = PackedFloat4ToRGBA32(color);
}

