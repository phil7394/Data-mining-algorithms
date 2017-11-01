#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define minsupport 0.02

int k = 0;

typedef struct market_basket_data
{
	int sizeOftransaction;
	int *transaction;
}market_basket_data_t;

typedef struct candidateItem_set{
	int size;
	int supportCount;
	int *candidateItem;
} candidateItem_set_t;

typedef struct frequentItem_set
{
	int size;
	int *frequentItem;
	int supportCount;
}frequentItem_set_t;

typedef struct node{
	int key;
	int candidate_size;
	int *candidateItem;
	int supportCount;
	int validBit;
	struct node *next;
}node_t;

typedef struct hashTable
{
	int block_count;
	node_t *head;
} hash_t;

hash_t *hash_table;


market_basket_data_t *market_data;
int numOfItems, numOfTransactions;
double minsupportCount;

int gen_candidate(candidateItem_set_t *Ck, frequentItem_set_t *Fk_1, int k, int Fk_size);
void initializeFrequent_kminus1(frequentItem_set_t *F1, frequentItem_set_t *Fk_1, int numOfTransactions, int Fk_size);
void gen_hashTree(candidateItem_set_t *Ck, int k, int Ck_size);

node_t *createNode(int key, int candidate_size, int *candidateItem);
void insertToHash( int key, int candidate_size, int *candidateItem, int k);
int parseHashTable(int numOfTransactions, int k);
int LCSLength(int *array1 , int *array2, int size1, int size2);
void gen_frequent(frequentItem_set_t *Fk, int Fk_size, int k);
void copy_Fk_to_Fkminus1(frequentItem_set_t *Fk, frequentItem_set_t *Fk_1 , int Fk_size, int k);


int main(int argc, char const *argv[])
{
	FILE *fp, *frequent;
	fp = fopen("market_output.txt", "r");	
	frequent = fopen("frequentItemSetData.txt","w");
	
	fscanf(fp,"%d ",&numOfItems);
	fscanf(fp,"%d",&numOfTransactions);
	numOfItems += 1;
	printf("#items : %d \t #transactions : %d\n",numOfItems,numOfTransactions);

	market_data = (market_basket_data_t*) malloc (sizeof(market_basket_data_t) * numOfTransactions * (numOfItems*sizeof(int)));

	int k_counter = 1;
	int i = 0, size, j, data,l;
	while(fscanf(fp,"%d ", &size) != EOF){
			
		market_data[i].sizeOftransaction = size;
		market_data[i].transaction = (int *) malloc (sizeof(int) * size);
		for(j = 0 ; j < size ; j++){
			fscanf(fp,"%d ",&data);
			market_data[i].transaction[j] = data;
		}
		i++;
	}
	fclose(fp);
	candidateItem_set_t *C1;
	C1 = (candidateItem_set_t*) malloc (sizeof(candidateItem_set_t)*numOfItems*sizeof(int) * k);
	for(i = 0 ; i < numOfItems; i++){
		C1[i].supportCount = 0;
		C1[i].size = k;
		C1[i].candidateItem = (int *) malloc (sizeof(int) * k);
		C1[i].candidateItem[0] = i;
	}

	for(j = 0 ; j < numOfTransactions; j++){
		size = market_data[j].sizeOftransaction;
		for( l = 0 ; l < size ; l++){
			data = market_data[j].transaction[l];
			C1[data].supportCount += 1;
		}
	}	
	
		minsupportCount = minsupport*numOfTransactions;
	frequentItem_set_t *F1;
	F1 = (frequentItem_set_t*) malloc (sizeof(frequentItem_set_t) *sizeof(int) *k * numOfTransactions);
	for(i = 0 ; i < numOfItems; i++){
		F1[i].supportCount = 0;
		F1[i].size = k;
		F1[i].frequentItem = (int*) malloc (sizeof(int)*k);
		F1[i].frequentItem[0] = -1;
	}
	int s;
	double support;
	for(i = 0 ; i < numOfItems ; i++){
		s = C1[i].supportCount;
		support = (double)s/(double)numOfTransactions;
		if(support > minsupport){
			F1[i].supportCount = s;
			F1[i].frequentItem[0] = C1[i].candidateItem[0];
		}
	}
	printf("minimum support Count : %lf \n", minsupport*numOfTransactions);
	int Fk_size = 0;
	for(i = 0 ; i < numOfItems; i++){
		data = F1[i].frequentItem[0];
		if( data != -1){
			Fk_size++;
		}
	}
	printf("frequent %d-itemset size  : %d\n",k_counter++, Fk_size);

	frequentItem_set_t *Fk_1;
	candidateItem_set_t *Ck;
	frequentItem_set_t *Fk;

	Fk_1 = (frequentItem_set_t*) malloc (sizeof(frequentItem_set_t)* sizeof(int)  * Fk_size);

	initializeFrequent_kminus1( F1, Fk_1, numOfTransactions, Fk_size);

	for(i = 0 ; i < Fk_size; i++){
		data = Fk_1[i].frequentItem[0];
		if( data != -1){
			fprintf(frequent, "1 %d\t\t%d\n", data, Fk_1[i].supportCount );
		}
	}

	for(k = 2; Fk_size != 0; k++){
		int Ck_size = Fk_size*(Fk_size-1)/2;

		
		Ck = (candidateItem_set_t*) malloc (sizeof(candidateItem_set_t)* (sizeof(int)*k ) * Ck_size);
		
		int Fk_newSize = 0, Ck_newSize = 0;
		Ck_newSize =  gen_candidate(Ck, Fk_1, k, Fk_size);

		hash_table = NULL;
		hash_table = (hash_t *) malloc (sizeof(hash_t) * k);
		for(i = 0 ; i < k ; i++){
			hash_table[i].block_count = 0;
			hash_table[i].head = NULL;
		}
		gen_hashTree(Ck, k, Ck_newSize);
		

		Fk_newSize = parseHashTable(numOfTransactions, k);
															/*displayHashTable(k);*/
		Fk = (frequentItem_set_t*) malloc (sizeof(frequentItem_set_t)*(sizeof(int) * k) * Fk_newSize);
		gen_frequent( Fk, Fk_newSize, k);
		int iter, jter;
		for(iter = 0 ; iter < Fk_newSize; iter++){
				fprintf(frequent, "%d ",k);
				for(jter = 0 ; jter < Fk[iter].size; jter++){
					fprintf(frequent, "%d ",Fk[iter].frequentItem[jter]);
				}
				fprintf(frequent, "\t\t%d\n",Fk[iter].supportCount );

		}

		Fk_1 = (frequentItem_set_t*) malloc (sizeof(frequentItem_set_t)*(sizeof(int) * k) * Fk_newSize);
		// rewrite Fk_1 with Fk
		Fk_size = Fk_newSize;
	}
	
	fclose(frequent);
	return 0;
}

int gen_candidate(candidateItem_set_t *Ck, frequentItem_set_t *Fk_1, int k, int Fk_size){
	int Ck_size = Fk_size*(Fk_size - 1)/2;
	int i, j;
	for(i = 0 ; i < Ck_size; i++){
		Ck[i].supportCount = 0;
		Ck[i].size = k;
		Ck[i].candidateItem = (int*) malloc (sizeof(int)* k);
		for ( j = 0; j < k; j++)
		{
			Ck[i].candidateItem[j] = -1;
		}
	}
	int Ck_newSize = 0;
	int bit = 0;
	int n = 0, p = 0 , m = 0;
	for(i = 0 ; i <Fk_size ; i++){
		for( j = i+1 ; j < Fk_size ; j++){
			bit = 1;
			for(m = 0 ; m < k-2 ; m++){
				if(Fk_1[i].frequentItem[m] != Fk_1[j].frequentItem[m])
					bit = 0;
			}
			if(bit){
				for(p = 0; p < k-1; p++){
					Ck[n].candidateItem[p] = Fk_1[i].frequentItem[p];
				}
				Ck[n].candidateItem[p] = Fk_1[j].frequentItem[p-1];
				Ck_newSize++;
				n++;
			}
			
		}
	}
	return Ck_newSize;
}

void gen_frequent(frequentItem_set_t *Fk, int Fk_size, int k){
	node_t *temp;
	int i,m,j = 0;

	for(i = 0 ; i < Fk_size; i++){
		Fk[i].supportCount = 0;
		Fk[i].size = k;
		Fk[i].frequentItem = (int*) malloc (sizeof(int)* k);
		for ( j = 0; j < k; j++)
		{
			Fk[i].frequentItem[j] = -1;
		}
	}
	j = 0;
	for(i = 0 ; i < k; i++){
		if(hash_table[i].block_count == 0)
			continue;
		temp = hash_table[i].head;
		if(!temp)
			continue;
		while(temp != NULL){
			if(temp->validBit == 1){
				Fk[j].size = temp-> candidate_size;
				Fk[j].frequentItem = (int *) malloc (sizeof(int) * Fk[j].size);
				for(m = 0 ; m < Fk[j].size; m++){
					Fk[j].frequentItem[m] =  temp->candidateItem[m];
				}
				Fk[j].supportCount = temp -> supportCount;
				j++;
			}
			temp = temp->next;
		}
	}
}

void copy_Fk_to_Fkminus1(frequentItem_set_t *Fk, frequentItem_set_t *Fk_1 , int Fk_size, int k){
	int i, j, data;
	j = 0;
	for(i = 0 ; i < Fk_size; i++){
		Fk_1[i].size = Fk[i].size;
		Fk_1[i].frequentItem = (int *) malloc (sizeof(int) * k);
		for(j = 0 ; j < Fk_1[i].size; j++){
			Fk_1[i].frequentItem[j] = Fk[i].frequentItem[j];
		}
		Fk_1[i].supportCount = Fk[i].supportCount;
	}
}

void initializeFrequent_kminus1(frequentItem_set_t *F1, frequentItem_set_t *Fk_1, int numOfTransactions, int Fk_size){
	int i,j = 0,data;
	int k = 1;
	for(i = 0 ; i < Fk_size; i++){
		Fk_1[i].supportCount = 0;
		Fk_1[i].size = k;
		Fk_1[i].frequentItem = (int*) malloc (sizeof(int)* k);
		Fk_1[i].frequentItem[0] = -1;
	}
	for(i = 0 ; i <= numOfTransactions; i++){
		if(j >= Fk_size)
			break;
		data = F1[i].frequentItem[0];
		if( data != -1){
			Fk_1[j].size = F1[i].size;
			Fk_1[j].supportCount = F1[i].supportCount;
			Fk_1[j].frequentItem[0] = F1[i].frequentItem[0];
			j++;
		}	
	}

	return;
}


node_t *createNode(int key, int candidate_size, int *candidateItem){
	node_t *newNode;
	newNode = (node_t *) malloc (sizeof(node_t) * sizeof(int) * candidate_size );
	newNode->key = key;
	newNode -> candidate_size = candidate_size;
	newNode -> supportCount = 0;
	newNode -> candidateItem = (int *) malloc (sizeof(int)*candidate_size);
	int i;
	for(i = 0 ; i < candidate_size; i++){
		newNode -> candidateItem[i] = candidateItem[i];
	}
	newNode -> next = NULL;
	return newNode;
}

void insertToHash(int key, int candidate_size, int *candidateItem, int k){
	int hash_index = key % k;

	node_t *newNode = createNode(key, candidate_size, candidateItem);
	node_t *temp;
	if( !(hash_table[hash_index].head)  ){
		hash_table[hash_index].head = newNode;
		hash_table[hash_index].block_count = 1;
			
		return;
	}
	hash_table[hash_index].block_count++;
	newNode -> next = (hash_table[hash_index].head);

	hash_table[hash_index].head = newNode;
	hash_table[hash_index].block_count += 1;
	return;

}
void gen_hashTree(candidateItem_set_t *Ck, int k, int Ck_size){
	int i;
	for(i = 0 ; i < Ck_size ; i++){
		
		insertToHash(Ck[i].candidateItem[0], Ck[i].size, Ck[i].candidateItem, k);

	}
}

int parseHashTable(int numOfTransactions, int k){
	int Fk_newSize = 0;
	int i,j,m;
	int seq_size;
	int size, key, hash_index, len;
	node_t *temp;
	int *temp_transaction = (int *) malloc (sizeof(int) * numOfItems);
	for(i = 0 ; i < numOfTransactions ; i++){
		size = market_data[i].sizeOftransaction;
		node_t *temp;
		for( j = 0 ; j < size - k + 1 ; j++){
			key = market_data[i].transaction[j];
			for(m = j+1; m < size ; m++){
				temp_transaction[m-j-1] = market_data[i].transaction[m];
			}
			hash_index = key % k;
			temp = hash_table[hash_index].head;
			if(temp == NULL)
				continue;
			while(temp  != NULL){
				if(key == temp->candidateItem[0]){
					// get seq size here!!
					if(seq_size == k-1)
						temp->supportCount++;
					if(temp->supportCount > minsupportCount){
						temp->validBit = 1;
					}
				}
				temp = temp->next;
			}
		}
	}
	for(i = 0 ; i < k; i++){
		if(hash_table[i].block_count == 0)
			continue;
		temp = hash_table[i].head;
		if(!temp)
			continue;
		while(temp != NULL){
			if(temp -> validBit == 1)
				Fk_newSize += 1;
			temp = temp->next;
		}
	}
	return Fk_newSize;
}
