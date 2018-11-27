#version 150 core

out vec4 fragment;
uniform float time;
uniform vec2 mousePos;
uniform vec2 windowSize;
uniform sampler2D image;

void main()
{
    // ��f����}�E�X�ʒu�܂ł̋���
	float phase = length(mousePos.xy - gl_FragCoord.xy);
    // ��f����}�E�X�ʒu�֌������P�ʃx�N�g���i����1�̃x�N�g���j
	vec2 diff = normalize(mousePos.xy - gl_FragCoord.xy);
	// �g�嗦�𐳋K���z�ɏ]���Čv�Z�i�}�E�X�ʒu�ɋ߂��قǑ傫���g�傳���悤�Ɂj
	float mod = exp(-phase / 100.0) * phase;
	// �\������e�N�X�`���̉�f������
	vec2 texc;
	texc.x = (gl_FragCoord.x + mod * diff.x) / windowSize.x;
	texc.y = (gl_FragCoord.y + mod * diff.y) / windowSize.y;
	// �e�N�X�`���}�b�s���O
	fragment = texture(image, texc.xy);
}
