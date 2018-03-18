#include "mt19937.h"
#include "sort.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <vector>	// �������Ļ���64λlib�޷����ɣ����������

//extern "C" {
typedef struct {
	char Enabled;	// 0 - 1
	char Slot;	// 0 - 5
	char Team;	// 0 - 2
	char Rank;	// 0 - 5
	int Frame;
	float FramePos;
	float FrameRate;	// 0, 0.9, 1.0, 1.1
} PlayerList;

// ��������
void __stdcall gmSortRank(int UserCount, int Frame, PlayerList* UsersIn, PlayerList* UsersOut);

// �������
int __stdcall gmGetRank(int UserCount, int MySlot, int TotalFrame, PlayerList* UsersIn, PlayerList* UsersOut, bool AllowSameRank);

// ��õ���ID
int __stdcall gmGetRandomItemID(bool IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int MusicLevel);

// ��ù������ߵ�Ŀ��Slot
int __stdcall gmGetAttackSlot(bool IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int ItemID);
//}

// ���߸�������
static char itemcoef[66][18] = {
// 0-5: 6�˾ָ��ʼӳ�:
	{9,0,9,0,3,0,9,0,0,4,9,9,9,0,3,3,3,0},	// /70
	{3,3,6,2,9,5,6,3,3,3,6,0,6,3,3,3,3,3},
	{3,7,3,6,9,9,6,3,3,6,3,0,6,6,0,0,0,0},
	{2,9,3,6,9,9,6,6,6,6,1,0,1,6,0,0,0,0},
	{3,3,0,3,0,3,6,3,3,9,1,0,6,6,6,6,6,6},
	{3,3,0,3,0,3,6,6,6,9,1,0,6,0,6,6,6,6},
// 6-10: 5�˾ָ��ʼӳ�: ȡ6�˾�1,3,4,5,6��
	{9,0,9,0,3,0,9,0,0,4,9,9,9,0,3,3,3,0},
	{3,7,3,6,9,9,6,3,3,6,3,0,6,6,0,0,0,0},
	{2,9,3,6,9,9,6,6,6,6,1,0,1,6,0,0,0,0},
	{3,3,0,3,0,3,6,3,3,9,1,0,6,6,6,6,6,6},
	{3,3,0,3,0,3,6,6,6,9,1,0,6,0,6,6,6,6},
// 11-14: 4�˾ָ��ʼӳ�: ȡ6�˾�1,4,5,6��
	{9,0,9,0,3,0,9,0,0,4,9,9,9,0,3,3,3,0},
	{2,9,3,6,9,9,6,6,6,6,1,0,1,6,0,0,0,0},
	{3,3,0,3,0,3,6,3,3,9,1,0,6,6,6,6,6,6},
	{3,3,0,3,0,3,6,6,6,9,1,0,6,0,6,6,6,6},
// 15-17: 3�˾ָ��ʼӳ�: ȡ6�˾�1,4,6��
	{9,0,9,0,3,0,9,0,0,4,9,9,9,0,3,3,3,0},
	{2,9,3,6,9,9,6,6,6,6,1,0,1,6,0,0,0,0},
	{3,3,0,3,0,3,6,6,6,9,1,0,6,0,6,6,6,6},
// 1-2�˾�û�и��ʼӳ�

// 18-25: �����Ѷȸ��ʼӳ�:
/* 0����û�мӳ� */
/* 0.5-1.0 */	{-9,9,3,9,9,9,2,6,6,3,1,0,1,6,3,3,3,6},
/* 1.5-2.0 */	{-9,8,3,9,9,9,3,5,5,4,3,1,1,5,3,3,3,5},
/* 2.5-3.0 */	{4,6,4,6,3,6,3,3,3,6,3,1,3,5,3,3,3,5},
/* 3.5-4.0 */	{5,5,4,4,3,6,3,3,3,6,3,3,3,3,5,5,3,3},
/* 4.5-5.0 */	{5,3,4,3,6,3,3,3,3,6,1,3,3,3,6,6,6,3},
/* 5.5-6.0 */	{5,1,1,1,6,3,3,2,3,9,1,6,9,1,6,6,6,1},
/* 6.5-7.0 */	{7,1,1,1,9,3,3,1,1,9,1,6,9,1,6,6,4,1},
/* 7.5+ */	{9,1,1,1,9,2,3,1,1,9,1,9,6,1,6,6,3,1},

// 26-35: ��ǰһ��������ʼӳ�:
	{0,0,0,0,0,0,0,-8,-7,0,0,0,0,0,0,0,0,0},
	{0,0,-1,0,1,0,0,-7,-6,1,0,0,0,1,0,0,0,0},
	{0,0,-2,0,2,0,0,-6,-5,2,0,0,0,2,0,0,0,0},
	{0,0,-3,0,3,1,0,-5,-4,3,0,0,0,3,0,0,0,0},
	{0,0,-4,0,4,2,-1,-4,-3,4,0,0,0,4,1,1,1,1},
	{0,0,-5,0,5,3,-2,-3,-2,5,0,0,0,3,2,2,2,2},
	{0,0,-6,0,6,4,-3,-2,-1,6,0,0,0,2,3,3,3,3},
	{0,0,-7,0,7,5,-4,-1,1,7,0,0,0,1,4,4,4,4},
	{0,0,-8,0,8,6,-5,1,2,8,0,0,0,0,5,5,5,5},
	{0,0,-9,0,9,7,-6,2,3,9,0,0,0,0,6,6,6,6},

// 36-45: ���һ��������ʼӳ�:
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,-1,-1,0,0,0,0,0,1,0,0,0,0,0,0},
	{0,0,2,0,-2,-2,0,0,0,0,0,2,0,0,0,0,0,0},
	{0,0,3,0,-3,-3,0,0,0,0,0,3,0,0,0,0,0,0},
	{0,0,4,0,-4,-4,0,0,0,0,0,4,0,0,0,0,0,0},
	{0,0,3,0,-5,-5,0,0,0,0,0,5,0,0,0,0,0,0},
	{0,0,2,0,-6,-6,0,0,0,0,0,6,0,0,0,0,0,0},
	{0,0,1,0,-7,-7,0,0,0,0,0,7,0,0,0,0,0,0},
	{0,0,0,0,-8,-8,0,0,0,0,0,8,0,0,0,0,0,0},
	{0,0,0,0,-9,-9,0,0,0,0,0,9,0,0,0,0,0,0},

// 46-55: ���һ��������ʼӳ�:
	{0,0,0,0,0,0,0,-8,-7,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,-7,-6,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,-6,-5,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,-5,-4,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,-4,-3,0,0,0,0,0,1,1,1,1},
	{0,0,0,0,0,0,0,-3,-2,0,0,0,0,0,2,2,2,2},
	{0,0,0,0,0,0,0,-2,-1,0,0,0,0,0,3,3,3,3},
	{0,0,0,0,0,0,0,-1,1,0,0,0,0,0,4,4,4,4},
	{0,0,0,0,0,0,0,1,2,0,0,0,0,0,5,5,5,5},
	{0,0,0,0,0,0,0,2,3,0,0,0,0,0,6,6,6,6},

// 56-65: �����һ��������ʼӳ�:
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,-2,-2,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,-3,-3,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,-4,-4,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,-5,-5,0,0,0,0,0,0,0,0,0,0,0,0},
};

//�������࿪��:�����Զ�ر�;
static bool itemon[19][18] = {
// 0-5: ����
/* 1-2�� */	{1,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,0},
		{1,1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,0},
/* 3-6�� */	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
// 6-7: ģʽ
/* �Ŷ� */	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
/* ��ս */	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
// 8-16: �ܵ��ֶ�
/* ��1�� */	{1,1,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0},
/* ��2�� */	{1,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,0},
/* ��3�� */	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
/* ��4-7�� */	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
/* ��8�� */	{1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1},
/* ��9�� */	{0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1},
// 17-18: ����
/* ��һ�� */	{1,0,1,0,1,0,1,0,0,1,1,1,1,0,0,0,0,0},
/* ���һ�� */	{1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1},
};

static int __stdcall RankCompareProc(const void* a, const void* b) {
	PlayerList *aa = (PlayerList*)a, *bb = (PlayerList*)b;
	int r = aa->Enabled - bb->Enabled;
	if (r)
		return -r;
	else
		return -(int)(aa->FramePos - bb->FramePos);
	// �Ӵ�С����Ӧ�üӸ�����
}

// ��������
void __stdcall gmSortRank(int UserCount, int Frame, PlayerList* UsersIn, PlayerList* UsersOut) {
	if (UserCount <= 0) return;
	memmove(UsersOut, UsersIn, 6*sizeof(PlayerList));
	int i;
	for (i=0; i<6; i++)
		if (UsersOut[i].Enabled) {
			UsersOut[i].FramePos += (Frame - UsersOut[i].Frame) * UsersOut[i].FrameRate;
			UsersOut[i].Frame = Frame;
		}
	MergeSort(UsersOut, 6, sizeof(PlayerList), &RankCompareProc);
	UsersOut[0].Rank = 0;
	for (i=1; i<UserCount; i++) {
		if (UsersOut[i].FramePos == UsersOut[i-1].FramePos)
			UsersOut[i].Rank = UsersOut[i-1].Rank;
		else
			UsersOut[i].Rank = i;
	}
}

static int GetIndexBySlot(int UserCount, int MySlot, PlayerList* Users) {
	int i;
	for (i=0; i<UserCount; i++) {
		if (Users[i].Slot == MySlot) {
			return i;
		}
	}
	return -1;
}

// �������
int __stdcall gmGetRank(int UserCount, int MySlot, int TotalFrame, PlayerList* UsersIn, PlayerList* UsersOut, bool AllowSameRank) {
	int i;
	if (UserCount <= 1) return 0;

	PlayerList u[6];
	i = GetIndexBySlot(6, MySlot, UsersIn);
	gmSortRank(UserCount, UsersIn[i].Frame, UsersIn, u);
	if (UsersOut != NULL) {
		memmove(UsersOut, u, 6*sizeof(PlayerList));
	}
	int myindex = GetIndexBySlot(UserCount, MySlot, u);
	if (AllowSameRank) {
		return u[myindex].Rank;
	} else {
		return myindex;
	}
}

// ��õ���ID
int __stdcall gmGetRandomItemID(bool IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int MusicLevel) {
	int i;
	if (UserCount <= 0) return 5;

	PlayerList u[6];
	i = GetIndexBySlot(6, MySlot, Users);
	gmSortRank(UserCount, Users[i].Frame, Users, u);
	int myindex = GetIndexBySlot(UserCount, MySlot, u);
	char myrank = u[myindex].Rank;
	//std::vector<char> p;	// ����������ż���
	//std::vector<char> s;	// ������ż���
	int p[8], s[5]; int pc=0, sc=0;

	if (UserCount == 6)
		//p.push_back(myrank);
		p[pc++] = myrank;
	else if (UserCount == 5)
		//p.push_back(myrank + 6);
		p[pc++] = myrank + 6;
	else if (UserCount == 4)
		//p.push_back(myrank + 11);
		p[pc++] = myrank + 11;
	else if (UserCount == 3)
		//p.push_back(myrank + 15);
		p[pc++] = myrank + 15;
	
	if (MusicLevel > 0) {
		int m = (MusicLevel - 1) / 2;
		if (m > 7) m = 7;
		//p.push_back(m + 18);
		p[pc++] = m + 18;
	}
	
	int d, n;

	// ������ǰ��һ���ľ���
	if (myindex == 0) i = 0; else i = myindex - 1;
	d = (int)(u[i].FramePos - u[myindex].FramePos);
	if (d <= 0) {
		n = 0;
	} else {
		n = d / 15 + 1;
		if (n > 9) n = 9;
	}
	//p.push_back(n + 26);
	p[pc++] = n + 26;
	
	// ���������һ���ľ���
	if (myindex == UserCount - 1) i = UserCount - 1; else i = myindex + 1;
	d = (int)(u[myindex].FramePos - u[i].FramePos);
	if (d <= 0) {
		n = 0;
	} else {
		n = d / 15 + 1;
		if (n > 9) n = 9;
	}
	//p.push_back(n + 36);
	p[pc++] = n + 36;
	
	// �������һ���ľ���
	i = 0;
	d = (int)(u[i].FramePos - u[myindex].FramePos);
	if (d <= 0) {
		n = 0;
	} else {
		n = d / 15 + 1;
		if (n > 9) n = 9;
	}
	//p.push_back(n + 46);
	p[pc++] = n + 46;
	
	// ���������һ���ľ���
	i = UserCount - 1;
	d = (int)(u[myindex].FramePos - u[i].FramePos);
	if (d <= 0) {
		n = 0;
	} else {
		n = d / 15 + 1;
		if (n > 9) n = 9;
	}
	//p.push_back(n + 56);
	p[pc++] = n + 56;
	
	// �������࿪��
	//s.push_back(UserCount - 1);
	s[sc++] = UserCount - 1;
	//if (IsTeamMode) s.push_back(6); else s.push_back(7);
	if (IsTeamMode) s[sc++] = 6; else s[sc++] = 7;
	n = (int)(9 * u[myindex].FramePos / TotalFrame);
	//s.push_back(n + 8);
	s[sc++] = n + 8;
	//if (myrank == 0) s.push_back(17);
	if (myrank == 0) s[sc++] = 17;
	//if (myrank == UserCount - 1) s.push_back(18);
	if (myrank == UserCount - 1) s[sc++] = 18;
	
	// ���ɸ�������
	bool itemison[18] = {1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1};
	int itemprob[18] = {0};
	int j;
	
	//for (i=0; i<p.size(); i++) {
	for (i=0; i<pc; i++) {
		for (j=0; j<18; j++) {
			itemprob[j] += itemcoef[p[i]][j];
		}
	}
	//for (i=0; i<s.size(); i++) {
	for (i=0; i<sc; i++) {
		for (j=0; j<18; j++) {
			itemison[j] &= itemon[s[i]][j];
		}
	}
	
	//std::vector<char> pp;
	char pp[250]; int ppc=0;	// �����Ǹ������������241��
	for (i=0; i<18; i++) {
		if (itemison[i] && itemprob[i]>0) {
			//pp.insert(pp.end(), itemprob[i], i+1);
			for (j=0; j<itemprob[i]; j++)
				pp[ppc++] = i+1;
		}
	}
	
	// ��ȡ����ID
	//i = MT64_Rand() % pp.size();
	i = MT64_Rand() % ppc;
	return pp[i];
}

// ��ù������ߵ�Ŀ��Slot
int __stdcall gmGetAttackSlot(bool IsTeamMode, int UserCount, int MySlot, int TotalFrame, PlayerList* Users, int ItemID) {
	int i;
	char myteam = 0;
	int myrank = 0;
	if (UserCount <= 1) return 6;

	PlayerList u[6];
	i = GetIndexBySlot(6, MySlot, Users);
	gmSortRank(UserCount, Users[i].Frame, Users, u);
	myrank = GetIndexBySlot(UserCount, MySlot, u);	// �ҹ���ģ������ҵ�����ʱ�����Ʋ���
	myteam = u[myrank].Team;

	switch (ItemID) {
	case 6:		// �ڹ꣬��һ���������������ѡһ��
		if (u[0].Slot != MySlot)
			i = 0;
		else
			i = 1;
		goto checkbingliek_single;
	case 2:		// �����Է�ǰ�����壬�����������ѡһ��
	case 4:		// �������Է�ǰ�����壬�����������ѡһ��
		// ��ǰ��ĵ�һ���Է����
		for (i=myrank-1; i>=0; i--) {
			if (IsTeamMode && u[i].Team == myteam)
				continue;
			else
				goto checkbingliek_team;
		}
		// �Һ���ĵ�һ���Է����
		for (i=myrank+1; i<UserCount; i++) {
			if (IsTeamMode && u[i].Team == myteam)
				continue;
			else
				goto checkbingliek_team;
		}
		// �����Լ��ˡ�����
		//if (myrank - 1 >= 0)
		//	return u[myrank - 1].Slot;
		//if (myrank + 1 < UserCount)
		//	return u[myrank + 1].Slot;
		// ֻ��һ���ˡ�����
		return 6;
	case 14:	// ը�����Է��������
	default:	// ���幥������
	{
		//std::vector<char> opponent;
		char opponent[6]; int oc=0;
		for (i=0; i<UserCount; i++) {
			if (u[i].Slot != MySlot && (!IsTeamMode || IsTeamMode && u[i].Team != myteam))
				//opponent.push_back(u[i].Slot);
				opponent[oc++] = u[i].Slot;
		}
		//if (opponent.size() == 0) {
		if (oc == 0) {
			return 6;
		} else {
			//i = MT64_Rand() % opponent.size();
			i = MT64_Rand() % oc;
			return opponent[i];
		}
	}
	}
	bool SameTeam = false;
checkbingliek_team:
	SameTeam = true;
	goto checkbingliek;
checkbingliek_single:
	SameTeam = false;
checkbingliek: {
		int r = i;
		char opponent[6]; int oc=0;
		for (i=0; i<UserCount; i++) {
			if (u[i].Rank == u[r].Rank) {
				if (!SameTeam || SameTeam && u[i].Team == u[r].Team)
					opponent[oc++] = u[i].Slot;
			}
		}
		i = MT64_Rand() % oc;
		return opponent[i];
	}
}

