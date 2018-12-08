// Testbed.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include "noise_suppression.h"


void splitFloat32(float *inFrame, int frames, float *lowFrame, float *highFrame)
{
	for (int i = 0; i < frames; i++) {
		lowFrame[i] = inFrame[(i << 1)];
		highFrame[i] = inFrame[(i << 1) + 1];
	}
}

void combineFloat32(float *lowFrame, float *highFrame, int frames, float *outFrame)
{
	for (int i = 0; i < frames; i++) {
		outFrame[2 * i] = lowFrame[i];
		outFrame[2 * i + 1] = highFrame[i];
	}
}


int main()
{
	// insert code here...
	float *spframe = NULL;
	float *outframe = NULL;
	float *inFrame[2] = { 0 };
	float *outFrame[2] = { 0 };
	float *lowBand = NULL;
	float *highBand = NULL;
	float *lowBand_out = NULL;
	float *highBand_out = NULL;
	size_t num_bands = 2;
	size_t sampleRate = 32000;
	int filter_state1[6] = { 0 };
	int filter_state2[6] = { 0 };

	spframe = (float*)malloc(sizeof(float) * sampleRate / 100);//10ms
	if (!spframe) {
		return 0;
	}
	memset(spframe, 0, sizeof(float) * sampleRate / 100);

	outframe = (float*)malloc(sizeof(float) * sampleRate / 100);
	if (!outframe) {
		return 0;
	}
	memset(outframe, 0, sizeof(float) * sampleRate / 100);

	lowBand = (float*)malloc(sizeof(float) * sampleRate / 100);
	if (!lowBand) {
		return 0;
	}
	memset(lowBand, 0, sizeof(float) * sampleRate / 100);

	highBand = (float*)malloc(sizeof(float) * sampleRate / 100);
	if (!highBand) {
		return 0;
	}
	memset(highBand, 0, sizeof(float) * sampleRate / 100);

	lowBand_out = (float*)malloc(sizeof(float) * sampleRate / 100);
	if (!lowBand_out) {
		return 0;
	}
	memset(lowBand_out, 0, sizeof(float) * sampleRate / 100);

	highBand_out = (float*)malloc(sizeof(float) * sampleRate / 100);
	if (!highBand_out) {
		return 0;
	}
	memset(highBand_out, 0, sizeof(float) * sampleRate / 100);

	//读取pcm数据，带噪声
	FILE *iFile = fopen("E:/resource/ns/output_float1.pcm", "rb");
	if (!iFile) {
		return 0;
	}

	FILE *oFile = fopen("E:/resource/ns/dst1.pcm", "wb");
	if (!oFile) {
		return 0;
	}

	NsHandle *ns = WebRtcNs_Create();
	if (!ns) {
		return 0;
	}

	int res = WebRtcNs_Init(ns, sampleRate);
	if (res) {
		return 0;
	}

	res = WebRtcNs_set_policy(ns, 1);
	if (res) {
		return 0;
	}

	fread(spframe, sizeof(float), sampleRate / 100, iFile);
	WebRtcNs_Analyze(ns, spframe);

	while (fread(spframe, sizeof(float), sampleRate / 100, iFile)) {
		inFrame[0] = lowBand;
		inFrame[1] = highBand;

		outFrame[0] = lowBand_out;
		outFrame[1] = highBand_out;

		//WebRtcSpl_AnalysisQMF(spframe,sampleRate / 100,lowBand,highBand,filter_state1,filter_state2);

		splitFloat32(spframe, sampleRate / 200, lowBand, highBand);

		WebRtcNs_Analyze(ns, lowBand);
		WebRtcNs_Process(ns, inFrame, num_bands, outFrame);

		//            memcpy(outframe, spframe, sizeof(float) * num_bands);

		combineFloat32(lowBand_out, highBand_out, sampleRate / 200, outframe);
		//WebRtcSpl_SynthesisQMF(lowBand_out,highBand_out,sampleRate / 100,outframe,filter_state1,filter_state2);
		fwrite(outframe, sizeof(float), sampleRate / 100, oFile);
	}


	WebRtcNs_Free(ns);

	fclose(iFile);
	fclose(oFile);

	free(spframe);
	free(outframe);
	printf("Hello world\n");
    return 0;
}

