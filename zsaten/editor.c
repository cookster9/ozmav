#include "globals.h"
#include <stdio.h>
#include <limits.h>
#include <windows.h>

void outputFile() {
	char cwd[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, cwd);
	dbgprintf(0, 0, "Output sent to: %s", strcat(cwd, "\\output.txt"));
	FILE* f = fopen("output.txt","w");
	//int written = fwrite(TempBones, sizeof(tempActorBone), sizeof(TempBones), f);
	for (int i = 0; i < vCurrentActor.frameTotal + 1; i++) {		//not totally sure why we need the plus 1 but we do
		fprintf(f, "%04x\n", (TempBones[i][0].X)%65536);
		fprintf(f, "%04x\n", TempBones[i][0].Y % 65536);
		fprintf(f, "%04x\n", TempBones[i][0].Z % 65536);
		for (int k = 0; k < 21; k++) {
			fprintf(f, "%04x\n", TempBones[i][k].RX %65536);
			fprintf(f, "%04x\n", TempBones[i][k].RY %65536);
			fprintf(f, "%04x\n", TempBones[i][k].RZ %65536);
		}
		//dropping this here for now - I don't think original zSaten tried to grab the eye/face data. I'll have to add that later
		fprintf(f, "%04x\n", 0);
	}
	fclose(f);

}
void readFileData() {

	unsigned int BoneOffset = vCurrentActor.offsetBoneSetup[vCurrentActor.boneSetupCurrent];

	int Seg = (BoneOffset >> 24) & 0xFF;
	BoneOffset &= 0xFFFFFF;

	int BoneCount = RAM[Seg].Data[(BoneOffset)+4];
	
	memset(TempBones, 0, sizeof(actorBone) * BoneCount);

	//get current animation into a temp array
	unsigned int AnimationOffset = vCurrentActor.offsetAnims[vCurrentActor.animCurrent];
	for (int frame = 0; frame <= vCurrentActor.frameTotal; frame++) {
		int rot_offset = AnimationOffset & 0xFFFFFF;
		rot_offset += (frame * (BoneCount * 6 + 8));
		int AniSeg = AnimationOffset >> 24;

		TempBones[frame][0].X = Read16(RAM[AniSeg].Data, rot_offset);
		rot_offset += 2;
		TempBones[frame][0].Y = Read16(RAM[AniSeg].Data, rot_offset);
		rot_offset += 2;
		TempBones[frame][0].Z = Read16(RAM[AniSeg].Data, rot_offset);
		rot_offset += 2;

		for (int i = 0; i < BoneCount; i++) {
			TempBones[frame][i].RX = Read16(RAM[AniSeg].Data, rot_offset);
			rot_offset += 2;
			TempBones[frame][i].RY = Read16(RAM[AniSeg].Data, rot_offset);
			rot_offset += 2;
			TempBones[frame][i].RZ = Read16(RAM[AniSeg].Data, rot_offset);
			rot_offset += 2;
		}
	}
}

//all bones - probably not useful after all
void animCopyForward() {
	for(int frame = vCurrentActor.frameCurrent + 1; frame <= vCurrentActor.frameTotal; frame++){
		TempBones[frame][0].X = TempBones[vCurrentActor.frameCurrent][0].X;
		TempBones[frame][0].Y = TempBones[vCurrentActor.frameCurrent][0].Y;
		TempBones[frame][0].Z = TempBones[vCurrentActor.frameCurrent][0].Z;
		for (int i = 0; i < 21; i++) {
			TempBones[frame][i].RX = TempBones[vCurrentActor.frameCurrent][i].RX;
			TempBones[frame][i].RY = TempBones[vCurrentActor.frameCurrent][i].RY;
			TempBones[frame][i].RZ = TempBones[vCurrentActor.frameCurrent][i].RZ;
		}
	}
}

//copy forward rotoation/position of current bone
void animCopyCurrent(){
	for (int frame = vCurrentActor.frameCurrent + 1; frame <= vCurrentActor.frameTotal; frame++) {
		TempBones[frame][0].X = TempBones[vCurrentActor.frameCurrent][0].X;
		TempBones[frame][0].Y = TempBones[vCurrentActor.frameCurrent][0].Y;
		TempBones[frame][0].Z = TempBones[vCurrentActor.frameCurrent][0].Z;
		TempBones[frame][vCurrentActor.boneCurrent].RX = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RX;
		TempBones[frame][vCurrentActor.boneCurrent].RY = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RY;
		TempBones[frame][vCurrentActor.boneCurrent].RZ = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RZ;
	}

}

void animCopyCurrentBack() {
	for (int frame = vCurrentActor.frameCurrent - 1; frame >= 0; frame--) {
		TempBones[frame][0].X = TempBones[vCurrentActor.frameCurrent][0].X;
		TempBones[frame][0].Y = TempBones[vCurrentActor.frameCurrent][0].Y;
		TempBones[frame][0].Z = TempBones[vCurrentActor.frameCurrent][0].Z;
		TempBones[frame][vCurrentActor.boneCurrent].RX = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RX;
		TempBones[frame][vCurrentActor.boneCurrent].RY = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RY;
		TempBones[frame][vCurrentActor.boneCurrent].RZ = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RZ;
	}
}