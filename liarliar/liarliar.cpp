#include <stdio.h>
#include <vector>
#include <map>
#include <queue>
#include "member.h"

void printAdjacentLists(vector<Member> members)
{
	for(int i = 0 ; i < (int)members.size() ; ++i)
	{
		printf("%s d=%d: ", members[i].name.c_str(), members[i].distance);
		
		for(set<int>::iterator it = members[i].adjacent.begin() ; it != members[i].adjacent.end() ; ++it)
		{
			if(it != members[i].adjacent.begin())
				printf(" -> "); 
			printf("%d", *it);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	const int MAX_READ_LINE_CHAR = 255;
	const int MAX_READ_NAME_CHAR = 127;
	char line[MAX_READ_LINE_CHAR+1];
	char name[MAX_READ_NAME_CHAR+1];

	map<string,int> name_to_index_map;
	map<string,int>::iterator member_iter;
	vector<Member> members;

	FILE *fp = fopen(argv[1], "r");
	if(fp == 0)
		return 0;
	
	// get total member numbers
	int numMembers;
	if(fgets(line, MAX_READ_LINE_CHAR, fp) == 0)
		return 0;
	sscanf(line, "%d", &numMembers);
	//printf("Total Members = %d\n", numMembers);
	members.reserve(numMembers);

	// 1st pass: 
	// store each member in member list
	int numAdjacents;
	for(int i = 0 ; i < numMembers ; ++i)
	{
		if(fgets(line, MAX_READ_LINE_CHAR, fp) == 0)
			return 0;
		sscanf(line, "%s %d", name, &numAdjacents);
		name_to_index_map.insert(make_pair(name, i));
		Member newMember;
		newMember.name = name;
		members.push_back(newMember);
		for(int j = 0 ; j < numAdjacents ; ++j)
		{
			fgets(line, MAX_READ_LINE_CHAR, fp);
		}
	}

	fseek(fp, 0, SEEK_SET);
	// skip the first line
	fgets(line, MAX_READ_LINE_CHAR, fp);
	// 2nd pass:
	// make complete adjacent lists for each member
	for(int i = 0 ; i < (int)members.size() ; ++i)
	{
		fgets(line, MAX_READ_LINE_CHAR, fp);
		sscanf(line, "%s %d", name, &numAdjacents);
		for(int j = 0 ; j < numAdjacents ; ++j)
		{
			fgets(line, MAX_READ_NAME_CHAR, fp);
			sscanf(line, "%s", name);
			member_iter = name_to_index_map.find(name);
			members[i].adjacent.insert(member_iter->second);
			members[member_iter->second].adjacent.insert(i);
		}
	}

	// 3rd pass: BFS
	queue<int> waitingQueue;
	members[0].color = Member::GRAY;
	members[0].distance = 0;
	members[0].parent = -1;
	waitingQueue.push(0);
	while(waitingQueue.size() > 0)
	{
		int now = waitingQueue.front();
		waitingQueue.pop();
		for(set<int>::iterator it = members[now].adjacent.begin() ; it != members[now].adjacent.end() ; ++it)
		{
			if(members[*it].color == Member::WHITE)
			{
				members[*it].color = Member::GRAY;
				members[*it].distance = members[now].distance + 1;
				members[*it].parent = now;
				waitingQueue.push(*it);
			}
		}
		members[now].color = Member::BLACK;
	}

	//printAdjacentLists(members);
	int numA = 0;
	for(int i = 0 ; i < (int)members.size() ; ++i)
	{
		if(members[i].distance % 2 == 0)
			numA++;
	}

	if(numA > numMembers - numA)
		printf("%d %d\n", numA, numMembers - numA);
	else
		printf("%d %d\n", numMembers - numA, numA);
	//name_to_index_map.clear();
	fclose(fp);

	return 0;
}
