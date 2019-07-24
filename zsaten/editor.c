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

void animCopyCurrentOne() {
	TempBones[vCurrentActor.frameCurrent + 1][0].X = TempBones[vCurrentActor.frameCurrent][0].X;
	TempBones[vCurrentActor.frameCurrent + 1][0].Y = TempBones[vCurrentActor.frameCurrent][0].Y;
	TempBones[vCurrentActor.frameCurrent + 1][0].Z = TempBones[vCurrentActor.frameCurrent][0].Z;
	TempBones[vCurrentActor.frameCurrent + 1][vCurrentActor.boneCurrent].RX = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RX;
	TempBones[vCurrentActor.frameCurrent + 1][vCurrentActor.boneCurrent].RY = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RY;
	TempBones[vCurrentActor.frameCurrent + 1][vCurrentActor.boneCurrent].RZ = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RZ;
}

void animCopyCurrentBackOne() {
	TempBones[vCurrentActor.frameCurrent - 1][0].X = TempBones[vCurrentActor.frameCurrent][0].X;
	TempBones[vCurrentActor.frameCurrent - 1][0].Y = TempBones[vCurrentActor.frameCurrent][0].Y;
	TempBones[vCurrentActor.frameCurrent - 1][0].Z = TempBones[vCurrentActor.frameCurrent][0].Z;
	TempBones[vCurrentActor.frameCurrent - 1][vCurrentActor.boneCurrent].RX = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RX;
	TempBones[vCurrentActor.frameCurrent - 1][vCurrentActor.boneCurrent].RY = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RY;
	TempBones[vCurrentActor.frameCurrent - 1][vCurrentActor.boneCurrent].RZ = TempBones[vCurrentActor.frameCurrent][vCurrentActor.boneCurrent].RZ;
}

void animCalculateKeyframes() {
	dbgprintf(0, 0, "executing function??");
	/*
	loop through all frames
	if #keyframes =< 1 do nothing
	if #keyframes > 1
	Find shortest distance between all axes. absolute value of them subtracted
	Divide by number of frames distance
	For number of frames distance, add the quotient to the first frame, save it off, continue
	*/

	int keyFrameTotal = 0;
	int rotDist, addDist, frameDist, anyDist;
	int transDist;
	int firstKeyFrame = -1;
	int secondKeyFrame = -1;

	for (int i = 0; i < vCurrentActor.frameTotal; i++) {
		if (TempBones[i][0].isKeyFrame) keyFrameTotal++;
	}

	if (keyFrameTotal > 1) {
		for (int i = 0; i < vCurrentActor.frameTotal; i++) {
			if (firstKeyFrame == -1) {
				if (TempBones[i][0].isKeyFrame) firstKeyFrame = i;
			}

			else if (secondKeyFrame == -1) {
				if (TempBones[i][0].isKeyFrame) {
					secondKeyFrame = i;
					//found both so...
					frameDist = secondKeyFrame - firstKeyFrame;

					//there's probably a cleverer way to loop through all the axes but I'm not going to try to figure it out
					//might have to completely rewrite the actor struct

					//X
					//unsigned shorts go 0-65535
					transDist = getDist(TempBones[secondKeyFrame][0].X,TempBones[firstKeyFrame][0].X);
					addDist = transDist / (frameDist);
					for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
						TempBones[k][0].X = TempBones[firstKeyFrame][0].X + (addDist * (k - firstKeyFrame));
					}

					for (int b = 0; b < 20; b++) {
						rotDist = getDist(TempBones[secondKeyFrame][b].RX,TempBones[firstKeyFrame][b].RX);
						addDist = rotDist / (frameDist);
						for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
							TempBones[k][b].RX = TempBones[firstKeyFrame][b].RX + (addDist * (k - firstKeyFrame));
						}
					}

					//Y
					transDist = getDist(TempBones[secondKeyFrame][0].Y, TempBones[firstKeyFrame][0].Y);
					addDist = transDist / (frameDist);
					for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
						TempBones[k][0].Y = TempBones[firstKeyFrame][0].Y + (addDist * (k - firstKeyFrame));
					}
					for (int b = 0; b < 20; b++) {
						rotDist = getDist(TempBones[secondKeyFrame][b].RY, TempBones[firstKeyFrame][b].RY);
						addDist = rotDist / (frameDist);
						for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
							TempBones[k][b].RY = TempBones[firstKeyFrame][b].RY + (addDist * (k - firstKeyFrame));
						}
					}

					//Z
					transDist = getDist(TempBones[secondKeyFrame][0].Z, TempBones[firstKeyFrame][0].Z);
					addDist = transDist / (frameDist);
					for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
						TempBones[k][0].Z = TempBones[firstKeyFrame][0].Z + (addDist * (k - firstKeyFrame));
					}
					for (int b = 0; b < 20; b++) {
						rotDist = getDist(TempBones[secondKeyFrame][b].RZ, TempBones[firstKeyFrame][b].RZ);
						addDist = rotDist / (frameDist);
						for (int k = firstKeyFrame + 1; k < secondKeyFrame; k++) {
							TempBones[k][b].RZ = TempBones[firstKeyFrame][b].RZ + (addDist * (k - firstKeyFrame));
						}
					}

					//clean up for next loop
					firstKeyFrame = secondKeyFrame;
					secondKeyFrame = -1;
				}
			}
		}
	}
}

//there must be a better/more readable way... but my brain hurts
int getDist(unsigned short end, unsigned short start) {
	int dist;
	int midPoint = 65536 / 2;
	dist = end - start;
	if (abs(dist) < midPoint) return dist;
	else if (dist < 0) return dist + 65536;
	else return (dist - 65536);
}