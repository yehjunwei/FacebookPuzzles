#include <stdio.h>
#include <vector>
#include <cstring>
#include <cstdlib>

using namespace std;

const int WORD_SIZE = 178691;
const int MAX_LENGTH_SENTENCE = 8192;
const int MAX_LENGTH_PATH = 256;
const int MAX_LENGTH_WORD = 32; // assume most lengthy legal word = 32
const int MIN_WORD_LENGTH = 2;
const int MAX_WORD_LENGTH = 15;
const int CAPS_SHIFT = 32;
const int INSERT_COST = 1;
const int DELETE_COST = 1;
const int REPLACE_COST = 1;
// linux path
const char DICTIONARY_PATH[MAX_LENGTH_PATH] = "twl06.txt";
//const char INPUT_PATH[MAX_LENGTH_PATH] = "/var/tmp/input";

// prototypes
bool pre_process_dictionary(char *dictionary_path, char *dictionary[MAX_LENGTH_WORD], short dictionary_length[]);
void init_dictionary_index(unsigned int dictionary_index_sort_by_length[]);
int editDistance(char *x, char *y, int lengthX, int lengthY, int **c);
int edit_distance_word(char *word, int target_length, char *dictionary[MAX_LENGTH_WORD], unsigned int dictionary_index_sort_by_length[], int count_length[]);
void countingsort_dictionary_by_length(short dictionary_length[], unsigned int dictionary_index_sort_by_length[], int count_length[]);

int main(int argc, const char *argv[])
{
	// initialize dictionary
	char **dictionary = new char* [WORD_SIZE];
	for(int i = 0 ; i < WORD_SIZE ; ++i)
	{
		dictionary[i] = new char [MAX_LENGTH_WORD];
	}
	short *dictionary_length = new short [WORD_SIZE];
	unsigned int *dictionary_index_sort_by_length = new unsigned int [WORD_SIZE];

	char dictionary_path[MAX_LENGTH_PATH] = "\0";
    strcpy(dictionary_path, DICTIONARY_PATH);

	if(!pre_process_dictionary(dictionary_path, dictionary, dictionary_length))
		return 0;

	init_dictionary_index(dictionary_index_sort_by_length);
	int count_length[MAX_LENGTH_WORD] = {0};
	countingsort_dictionary_by_length(dictionary_length, dictionary_index_sort_by_length, count_length);

	// open for input file
	FILE *fp = 0;
	if(argc > 1)
		fp = fopen(argv[1], "r");
	else
		return 0;

	if(fp == 0)
		return 0;

	char line[MAX_LENGTH_SENTENCE];
	fgets(line, MAX_LENGTH_SENTENCE, fp);
	char word[MAX_LENGTH_WORD];
	int i_sentence = 0;
	int index = 0;
	int total_distance = 0;
	int min_distance = 0;
	while (line[i_sentence] != '\0') 
	{
		while(line[i_sentence] != ' ' && line[i_sentence] != '\n')
		{
			word[index] = line[i_sentence];
			i_sentence++;
			index++;
		}
		word[index] = '\0';
        int word_length = (int)strlen(word);
        min_distance = word_length;
        int range = 0;
        if(word_length <= MAX_WORD_LENGTH && word_length >= MIN_WORD_LENGTH)
        {
            range = (word_length - MIN_WORD_LENGTH < MAX_WORD_LENGTH - word_length)?word_length - MIN_WORD_LENGTH : MAX_WORD_LENGTH - word_length;
        }
        else if(word_length < MIN_WORD_LENGTH)
        {
            // this word has only 1 letter
            range = MAX_WORD_LENGTH - word_length;
        }
        else
        {
            // this word is more than 15 letters
            range = word_length - MIN_WORD_LENGTH;
        }
        
        for(int j = 0 ; j <= range ; ++j)
        {
            if(min_distance == j)
                break;
            int d = edit_distance_word(word, word_length+j, dictionary, dictionary_index_sort_by_length, count_length);
            if(d < range)
                range = d;
            if(d < min_distance)
                min_distance = d;
            if(j != 0)
            {
                if(min_distance == j)
                    break;
                d = edit_distance_word(word, word_length-j, dictionary, dictionary_index_sort_by_length, count_length);     
                if(d < range)
                    range = d;
                if(d < min_distance)
                    min_distance = d;
            }
        }
        //printf("d of %s is %d\n", word, min_distance);
		total_distance += min_distance;
		index = 0;
		i_sentence++;
	}
	printf("%d\n", total_distance);
	// clean up
	for(int i = 0 ; i < WORD_SIZE ; ++i)
		delete [] dictionary[i];
	delete [] dictionary;
	delete [] dictionary_length;
	delete [] dictionary_index_sort_by_length;

	return 0;
}

bool pre_process_dictionary(char *dictionary_path, char *dictionary[MAX_LENGTH_WORD], short dictionary_length[])
{
	FILE *fp = 0;
	fp = fopen(dictionary_path, "r");
	if(fp == 0)
		return false;

	char line[MAX_LENGTH_WORD];
	int i = 0;
	while(fgets(line, MAX_LENGTH_WORD, fp) != 0) // EOF or error will return null pointer
	{
		char word[MAX_LENGTH_WORD];
		sscanf(line, "%s", word);
		// copy each word to dictionary array
		strcpy(dictionary[i], word);
		// keep length in dictionary_length
		dictionary_length[i++] = strlen(word);
	}
	fclose(fp);
	return true;
}

void init_dictionary_index(unsigned int dictionary_index_sort_by_length[]) 
{
	for(int i = 0 ; i < WORD_SIZE ; ++i) 
	{
		dictionary_index_sort_by_length[i] = i;
	}
}

void countingsort_dictionary_by_length(short dictionary_length[], unsigned int dictionary_index_sort_by_length[], int count_length[])
{
	for(int i = 0 ; i < WORD_SIZE ; ++i)
	{
		count_length[dictionary_length[i]]++;
	}
	for(int i = 1 ; i < MAX_LENGTH_WORD ; ++i)
	{
		if(count_length[i] != 0)
			count_length[i] += count_length[i-1];
	}
	unsigned int *copy_dictionary_index_sort_by_length = new unsigned int[WORD_SIZE];
	for(int i = 0 ; i < WORD_SIZE ; ++i)
	{
		short word_length = dictionary_length[i];
		int index = count_length[word_length-1];
		copy_dictionary_index_sort_by_length[index] = dictionary_index_sort_by_length[i];
		count_length[word_length-1]++;
	}
	for(int i = 0 ; i < WORD_SIZE ; ++i)
	{
		dictionary_index_sort_by_length[i] = copy_dictionary_index_sort_by_length[i];
	}
	delete [] copy_dictionary_index_sort_by_length;
}

int edit_distance_word(char *word, int target_length, char *dictionary[MAX_LENGTH_WORD], unsigned int dictionary_index_sort_by_length[], int count_length[])
{
	int lengthX = (int)strlen(word);
	int length_shift = target_length - MIN_WORD_LENGTH;
    int target_min_distance = abs(target_length - lengthX);
    
	int **c = new int* [lengthX+1];
	for(int i = 0 ; i < lengthX+1 ; ++i)
	{
		c[i] = new int [target_length+1];
	}

	int start_index = count_length[length_shift];
	int end_index = count_length[length_shift+1]-1;
	int min_distance = target_length;
	for(int i = start_index ; i < end_index ; ++i)
	{
		int index = dictionary_index_sort_by_length[i];        
		int distance = editDistance(word, dictionary[index], lengthX, target_length, c);
		if(distance < min_distance)
		{
            min_distance = distance;
            if(distance == target_min_distance)
                break;
        }
	}

	for(int i = 0 ; i < lengthX+1 ; ++i)
		delete [] c[i];
	delete [] c;
	return min_distance;
}

int editDistance(char *x, char *y, int lengthX, int lengthY, int **c)
{
	int i, j;
	for(i = 0 ; i < lengthX+1 ; ++i)
	{
		for(j = 0 ; j < lengthY+1 ; ++j)
		{
			c[i][j] = 0;
			if(i == 0) // initial c[0][j]
			{
				c[i][j] = j*INSERT_COST;
			}
			if(j == 0) // initial c[i][0]
			{
				c[i][j] = i*DELETE_COST;
			}
		}
	}
    
    for(j = 1 ; j < lengthY+1 ; ++j)
    {
        for(i = 1 ; i < lengthX+1 ; ++i)
        {
			if(x[i-1] == y[j-1]+CAPS_SHIFT)
			{
				c[i][j] = c[i-1][j-1];
			}
			else
			{
				int rep = c[i-1][j-1]+REPLACE_COST;
				int del = c[i-1][j]+DELETE_COST;
				int ins = c[i][j-1]+INSERT_COST;
				c[i][j] = min(min(rep, del), ins);
			}
        }
    }
    
	int distance = c[lengthX][lengthY];
	return distance;
}
