#pragma once
#include <stddef.h>
// Note: All these "len" means element numbers of the array.

struct WaveTab {
	size_t Tab2TLen; // 2 turpled arg length.
	float * Tab2T;
	struct AttnTab * Attn;
};

struct AttnTab {
	float * Attntab;
	size_t TabLen;
	float * Tab2T;
	size_t Tab2TLen;
}; // WaveTab 里面的 AttnTab 是 时变频域
// WaveArgs 里面的是包络

struct LoudnessLoopTab { // 3 turpled: tick start, tick stop, factor
	float * Tab;
	size_t Len;	
};

struct WaveArgs {
	float Loudnessfac;
	float TrackGlobalLoudnessFac;
	float freq;
	struct AttnTab Attn;
	size_t total_len;
	size_t wrote_len;
	size_t pending_len; // len 指数组长度, 或者采样数量
	char skipflag;
};

void CalcLoudnessAttnTab(struct AttnTab * Tab);
void WriteWave(float * Buffer,struct WaveTab * wt,struct WaveArgs * arg,size_t len);
