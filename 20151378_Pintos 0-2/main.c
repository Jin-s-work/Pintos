#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include"list.h"
#include"round.h"
#include"hash.h"
#include"round.h"
#include"debug.h"
#include"limits.h"
#include"hex_dump.h"

typedef struct list_item{
	struct list_elem elem;
	int data;
}list_item;
typedef struct LIST{
	struct list* List;
	char name[20];
}LIST;
typedef struct BITMAP{
	struct bitmap* Bitmap;
	char name[20]; 
}BITMAP;
typedef struct hash_item{
	struct hash_elem elem;
	int data;
}hash_item;
typedef struct HASH{
	struct hash Hash;
	char name[20];
}HASH;

void list_swap(struct list_elem *a, struct list_elem *b);
void list_shuffle(struct list *list);
unsigned hash_int_2(int i);
struct bitmap *bitmap_expand(struct bitmap *bitmap, int size);
unsigned hfunc(const struct hash_elem *e, void *aux);
bool h_less(const struct hash_elem *a, const struct hash_elem *b, void *aux);
bool l_less(const struct list_elem *a, const struct list_elem *b, void *aux);
void hashsp(struct hash_elem *h, void *aux);
void hashtre(struct hash_elem *h, void *aux);

struct LIST L[10];
struct BITMAP B[10];
struct HASH H[10];

int Lindex = 0; 
int Hindex = 0;
int Bindex = 0;
int ItisList;
int ItisHash;
int ItisBit;
int i,j,q;
int s1, s2, s3;
struct list_item *tempitem;
struct list_elem *tempelem;
struct hash_item *temphash;
struct hash_elem *temphelem;
int main(){

char str[300];
char str1[100], str2[100], str3[100];
	
	while (1){
	str[0] = '\0';
	str1[0] = '\0';
	str2[0] = '\0';
	str3[0] = '\0';	

	fgets(str, 300, stdin);
	sscanf(str, "%s ", str1);
		if (strcmp(str1, "quit") == 0){
			ItisList = 0;
			ItisHash = 0;
			ItisBit = 0;
			break;
}
		else if (strcmp(str1, "create")==0){
			sscanf(str, "%s %s %s", str1, str2, str3);
			
			if (strcmp(str2, "list")==0){
				L[Lindex].List = (struct list*)malloc(sizeof(struct list));
				list_init(L[Lindex].List);
				strcpy(L[Lindex].name, str3);
				Lindex++;
				ItisList = 1;			
			}	
			else if (strcmp(str2, "hashtable") ==0){
		
				hash_init(&(H[Hindex].Hash),hfunc, h_less, NULL);
				strcpy(H[Hindex].name, str3);
				Hindex++;
				ItisHash = 1;
			}
			else if (strcmp(str2, "bitmap")==0){
				int n;
				sscanf(str, "%s %s %s %d" , str1, str2, str3, &n);
				B[Bindex].Bitmap = bitmap_create(n);
				strcpy(B[Bindex].name, str3);
				Bindex++;
				ItisBit = 1;
			}
		}	
		else if (strcmp(str1, "dumpdata") ==0){
		
			int x;
			sscanf(str, "%s %s", str1, str2);
			
			if(ItisList ==1){
				for(i = 0; i<10; i++){
				if(strcmp(L[i].name , str2) ==0){
					q = i;
					break;
				}
				}
				struct list_elem *a;
				a = (struct list_elem*)malloc(sizeof(struct list_elem));
				for (a = list_begin(L[q].List) ; a!=list_end(L[q].List); a = list_next(a)){
				printf("%d ", list_entry(a, struct  list_item, elem)->data);
				}	
				printf("\n");
			}
			else if(ItisHash ==1){
				struct hash_iterator hi;
				for (i=0; i<10; i++){
				if(strcmp(H[i].name, str2) ==0){
					q = i;
					break;	
				}
				}
				hash_first(&hi, &H[q].Hash);
				while(1){
					if(hash_next(&hi) == 0) break;
					printf("%d ", hash_entry(hi.elem, struct hash_item, elem)->data);	
				}
				printf("\n");
			}
			
			else if(ItisBit == 1){
			
				for(i=0; i<10; i++){
				if(strcmp(B[i].name, str2) == 0){
					q = i;
					break;
					}	
				}
				x = bitmap_size(B[q].Bitmap);
				for (i=0; i<x; i++){
					if(bitmap_test(B[q].Bitmap, i) == false)
						printf("0");
					else if(bitmap_test(B[q].Bitmap, i) == true) 
						printf("1");
				}
				printf("\n");
			}
		}
		else if(strcmp(str1, "delete") == 0){
		
			sscanf(str, "%s %s", str1, str2);
			if(ItisList == 1){
				for(i=0; i<10; i++){
				if(strcmp(L[i].name, str2) == 0){
					q = i;
					break;
				}
				}
				for(i = q ; i<Lindex-1; i++){
				strcpy(L[i].name, L[i+1].name);
				L[i].List = L[i+1].List;
				}
				strcpy(L[Lindex-1].name, "");
				list_init(L[Lindex-1].List);
				Lindex--;
			}

			else if(ItisHash == 1){
				for(i=0; i<10; i++){
				if(strcmp(H[i].name,str2) == 0){
					q = i;
					break;
				} 
				}
				for(i=q; i<Hindex-1; i++){
				strcpy(H[i].name, H[i+1].name);
				H[i].Hash = H[i+1].Hash;
				}	
				strcpy(H[Hindex-1].name, "");
				hash_init(&H[Hindex-1].Hash,hfunc,h_less,NULL);
				Hindex--; 
			}	
			else if(ItisBit == 1){
				for(i=0; i<10; i++){
				if(strcmp(B[i].name, str2) ==0){
					q=i;
					break;
				}
				}
				for(i=q; i<Bindex-1; i++){
				strcpy(B[i].name, B[i+1].name);
				B[i].Bitmap = B[i+1].Bitmap;
				}
				strcpy(B[Bindex-1].name, "");
				Bindex--;
			}
			}	
		else if(strcmp(str1, "list_insert") ==0) {
			
			int n1, n2;
			sscanf(str, "%s %s %d %d", str1, str2, &n1, &n2); 
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2)==0){
				q = i;
				break;
			}
			}
		
			tempitem = (struct list_item*)malloc(sizeof(struct list_item));
			
			if(n1 == 0){
			list_push_front(L[q].List, &(tempitem->elem));
			tempitem->data = n2;
			}
			else {
				tempitem->data = n2;
				tempelem = list_begin(L[q].List);
				for(i=0; i < n1 ; i++){
					tempelem = list_next(tempelem);}
				if (tempelem == NULL)
				list_push_back(L[q].List, &(tempitem->elem));
				else
				list_insert(tempelem, &(tempitem->elem));	
			}
		
			
		}

		else if (strcmp(str1, "list_splice") == 0){
		struct list_elem *sp1;
		struct list_elem *sp2;
		struct list_elem *sp3;
		int u,v;
		sscanf(str, "%s %s %d %s %d %d", str1, str2, &s1, str3, &s2, &s3);
		for(i=0; i<10; i++){
		if (strcmp(L[i].name, str2) == 0){
			u = i;
			break;
		}
		}
		for(i=0; i<10; i++){
		if (strcmp(L[i].name, str3) == 0){
			v = i;
			break;
		}
		}
		sp1 = list_begin(L[u].List);
		for(i = 0; i<s1 ; i++)
			sp1 = list_next(sp1);
		sp2 = list_begin(L[v].List);
		for(i = 0; i<s2; i++)
			sp2 = list_next(sp2);
		sp3 = list_begin(L[v].List);
		for(i = 0; i<s3; i++)
			sp3 = list_next(sp3);

		list_splice(sp1, sp2, sp3);
		}

		else if (strcmp(str1, "list_push_front") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i = 0; i<10; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			tempitem = (struct list_item*)malloc(sizeof(struct list_item));
			tempitem->data = s1;
			list_push_front(L[q].List, &tempitem->elem);
		}
		
		else if (strcmp(str1, "list_push_back") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i = 0; i<10; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			tempitem = (struct list_item*)malloc(sizeof(struct list_item));
			tempitem->data = s1;
			list_push_back(L[q].List, &tempitem->elem);
		}
		else if (strcmp(str1, "list_remove") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i = 0; i<10; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			tempelem = list_begin(L[q].List);
			for(i = 0; i<s1; i++){
			tempelem = list_next(tempelem);
			}
			list_remove(tempelem);
		}
		else if (strcmp(str1, "list_pop_front") == 0){
			sscanf(str, "%s %s" , str1, str2);
			for (i=0; i<10; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			list_pop_front(L[q].List);
		}
		else if (strcmp(str1, "list_pop_back") == 0){
			sscanf(str, "%s %s" , str1, str2);
			for (i=0 ; i<10 ; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			list_pop_back(L[q].List);
		}
		else if (strcmp(str1, "list_front") == 0){
			sscanf(str, "%s %s", str1, str2);
			for( i = 0; i<10; i++){
			if (strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			tempelem = list_front(L[q].List);
			printf("%d\n", list_entry(tempelem, struct list_item, elem)->data);			
		}
		else if (strcmp(str1, "list_back") == 0){
			sscanf(str, "%s %s", str1, str2);
			for (i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			tempelem = list_back(L[q].List);
			printf("%d\n", list_entry(tempelem, struct list_item, elem)->data);
		}
		else if (strcmp(str1, "list_size") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			printf("%d\n", list_size(L[q].List));
		}
		else if (strcmp(str1, "list_empty") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(list_empty(L[q].List) == true)
			printf("true\n");
			else printf("false\n");
		}
		else if (strcmp(str1, "list_reverse") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			list_reverse(L[q].List);
		}
		else if (strcmp(str1, "list_sort") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			list_sort(L[q].List,l_less, NULL);
		}
		else if (strcmp(str1, "list_swap") == 0){
			sscanf(str, "%s %s %d %d", str1, str2, &s1, &s2);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			struct list_elem* listelem1;
			struct list_elem* listelem2;
			listelem1 = list_begin(L[q].List);
			for(i = 0; i<s1; i++)
			listelem1 = list_next(listelem1);
			
			listelem2 = list_begin(L[q].List);
			for(i=0; i<s2; i++)
			listelem2 = list_next(listelem2);
		
			list_swap(listelem1, listelem2);
		}
		else if (strcmp(str1, "list_insert_ordered") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			tempitem = (struct list_ltem*)malloc(sizeof(struct list_item));
			tempitem->data = s1;
			list_insert_ordered(L[q].List, &tempitem->elem, l_less, NULL);
			
		}
		else if (strcmp(str1, "list_unique") == 0){
			sscanf(str, "%s %s %s", str1, str2, str3);
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(str3 == '\0')
				list_unique(L[q].List, NULL, l_less, NULL);
			else{
			int w;
			for(i=0; i<10; i++){
			if(strcmp(L[i].name, str3) == 0){
				w = i;
				break;
			}
			}
			list_unique(L[q].List, L[w].List, l_less, NULL);
			}
		}
		else if (strcmp(str1, "list_max") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i = 0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			tempelem = list_max(L[q].List, l_less, NULL);
			printf("%d\n", list_entry(tempelem, struct list_item, elem)->data);

		}
		else if (strcmp(str1, "list_min") == 0){
			sscanf(str, "%s %s", str1, str2);
			for (i=0; i<10; i++){
			if(strcmp(L[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			tempelem = list_min(L[q].List, l_less, NULL);
			printf("%d\n", list_entry(tempelem, struct list_item, elem)->data);
		}
		
		else if (strcmp(str1, "hash_replace") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for (i=0; i<10; i++){
			if (strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			temphash = (struct hash_item*)malloc(sizeof(struct hash_item));
			temphash->data = s1;
			hash_replace(&H[q].Hash, &temphash->elem);
		}
        else if (strcmp(str1, "hash_insert") == 0){
            sscanf(str, "%s %s %d", str1, str2, &s1);
            for (i=0; i<10; i++){
            if(strcmp(H[i].name, str2) == 0){
                q = i;
                break;
            }
            }
            temphash = (struct hash_item*)malloc(sizeof(struct hash_item));
            temphash->data = s1;
            hash_insert(&H[q].Hash, &temphash->elem);
        }
		else if (strcmp(str1, "hash_find") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10; i++){
			if (strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			temphash = (struct hash_item*)malloc(sizeof(struct hash_item));
			temphash->data = s1;
			if (hash_find(&H[q].Hash, &(temphash->elem)) != NULL)
				printf("%d\n", temphash->data);	
		}
		else if (strcmp(str1, "hash_delete") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10 ; i++){
			if (strcmp(H[i].name,str2) == 0){
				q = i;
				break;
			}
			}	
			temphash = (struct hash_item*)malloc(sizeof(struct hash_item));
			temphash->data = s1;
			hash_delete(&H[q].Hash, &(temphash->elem));
		}
		else if (strcmp(str1, "hash_clear") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0 ; i<10; i++){
			if(strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}	
			hash_clear(&H[q].Hash, NULL);
		}
		else if (strcmp(str1, "hash_size") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			printf("%d\n", hash_size(&H[q].Hash));
		}
		else if (strcmp(str1, "hash_empty") == 0){
			sscanf(str, "%s %s", str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(hash_empty(&H[q].Hash))
				printf("true\n");
			else printf("false\n");
		}
		else if (strcmp(str1, "hash_apply") == 0){
			sscanf(str, "%s %s %s", str1, str2, str3);
			for(i=0; i<10; i++){
			if(strcmp(H[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(strcmp(str3, "square") == 0){
				hash_apply(&H[q].Hash, hashsp);}
			
			else if(strcmp(str3, "triple") == 0){
				hash_apply(&H[q].Hash, hashtre);}
		}
		else if (strcmp(str1, "bitmap_size") == 0){
			sscanf(str, "%s %s" , str1, str2);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			printf("%d\n", bitmap_size(B[q].Bitmap));
		}	
		else if (strcmp(str1, "bitmap_set") == 0){
			sscanf(str, "%s %s %d %s", str1, str2, &s1, str3);
			for (i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(strcmp(str3, "true") == 0)
				bitmap_set(B[q].Bitmap, s1, 1);
			else
				bitmap_set(B[q].Bitmap, s1, 0);

		}
		else if (strcmp(str1, "bitmap_mark") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for( i = 0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bitmap_mark(B[q].Bitmap, s1);
		}
		else if (strcmp(str1, "bitmap_reset") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bitmap_reset(B[q].Bitmap, s1);
		}
		else if (strcmp(str1, "bitmap_flip") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i = 0; i<10 ; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bitmap_flip(B[q].Bitmap, s1);
		}
		else if (strcmp(str1, "bitmap_test") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(bitmap_test(B[q].Bitmap, s1) == true)
			printf("true\n");
			else printf("false\n");
		}
		else if (strcmp(str1, "bitmap_set_all") == 0){
			sscanf(str, "%s %s %s", str1, str2, str3);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(strcmp(str3, "true") == 0)
			bitmap_set_all(B[q].Bitmap, true);
			else 
			bitmap_set_all(B[q].Bitmap, false);
		}
		else if (strcmp(str1, "bitmap_set_multiple") == 0){
			sscanf(str, "%s %s %d %d %s", str1, str2, &s1, &s2, str3);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(strcmp(str3, "true") == 0)
			bitmap_set_multiple(B[q].Bitmap, s1, s2, true);
			else
			bitmap_set_multiple(B[q].Bitmap, s1, s2, false);
		}
		else if (strcmp(str1, "bitmap_count") == 0){
			sscanf(str, "%s %s %d %d %s", str1, str2, &s1, &s2, str3);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(strcmp(str3, "true") == 0)
			printf("%d\n", bitmap_count(B[q].Bitmap, s1, s2, true));
			else
			printf("%d\n", bitmap_count(B[q].Bitmap, s1, s2, false));
		}
		else if (strcmp(str1, "bitmap_contains") == 0){
			sscanf(str, "%s %s %d %d %s", str1, str2 , &s1, &s2, str3);
			for(i = 0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bool v;
			if(strcmp(str3, "true") == 0)
			v = true;
			else v = false;
			if(bitmap_contains(B[q].Bitmap, s1, s2, v) == true)
			printf("true\n");
			else if (bitmap_contains(B[q].Bitmap, s1, s2, v) == false)
			printf("false\n");
		}
		else if (strcmp(str1, "bitmap_any") == 0){
			sscanf(str, "%s %s %d %d", str1, str2, &s1, &s2);
			for(i=0 ; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;	
				break;
			}
			}
			if(bitmap_any(B[q].Bitmap, s1, s2) == true)
			printf("true\n");
			else printf("false\n");
		}
		else if (strcmp(str1, "bitmap_none") == 0){
			sscanf(str, "%s %s %d %d", str1, str2, &s1, &s2);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			if(bitmap_none(B[q].Bitmap, s1, s2) == true)
			printf("true\n");
			else printf("false\n");
		}
		else if (strcmp(str1, "bitmap_all") == 0){
                      sscanf(str, "%s %s %d %d", str1, str2, &s1, &s2);
                         for(i=0; i<10; i++){
                         if(strcmp(B[i].name, str2) == 0){
                                 q = i;
                                 break;                         }
                         }
                         if(bitmap_all(B[q].Bitmap, s1, s2) == true)
                         printf("true\n");
                         else printf("false\n");
		}
		else if (strcmp(str1, "bitmap_scan") == 0){
			sscanf(str, "%s %s %d %d %s", str1, str2, &s1, &s2, str3);
			for(i = 0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bool b;
			if(strcmp(str3, "true") == 0)
				b = true;
			else b = false;
			printf("%u\n", bitmap_scan(B[q].Bitmap, s1, s2, b));
		}
		else if(strcmp(str1, "bitmap_expand") == 0){
			sscanf(str, "%s %s %d", str1, str2, &s1);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
		struct bitmap *bittemp;
		bittemp = bitmap_expand(B[q].Bitmap, s1);
		B[q].Bitmap = bittemp;
		}

		else if (strcmp(str1, "bitmap_scan_and_flip") == 0){
			sscanf(str, "%s %s %d %d %s", str1, str2, &s1, &s2, str3);
			for(i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q = i;
				break;
			}
			}
			bool b1;
			if(strcmp(str3, "true") == 0)	
			b1 = true;
			else b1 = false;
			printf("%u\n", bitmap_scan_and_flip(B[q].Bitmap, s1, s2, b1));
		}
		else if (strcmp(str1, "bitmap_dump") == 0){
			sscanf(str, "%s %s", str1, str2);
			for (i=0; i<10; i++){
			if(strcmp(B[i].name, str2) == 0){
				q= i;
				break;
			}
			}
			bitmap_dump(B[q].Bitmap);
		}
	}

return 0;
}
void list_swap(struct list_elem *a, struct list_elem *b){
         int temp1,temp2;
         temp1 = list_entry(a, struct list_item, elem)->data;
         temp2 = list_entry(b, struct list_item, elem)->data;
         list_entry(a, struct list_item, elem)->data = temp2;
         list_entry(b, struct list_item, elem)->data = temp1;
}
 
void list_shuffle(struct list *list){
	int size;
	srand(time(NULL));
	struct list_elem* e1;
	struct list_elem* e2;
	
	size = list_size(list);
	for (i = 0; i<=9; i++){
		e1 = list_begin(list);
		for(j=0; j<rand()%size; j++){
			e1 = list_next(e1);
		}
		e2 = list_begin(list);
		for(j=0; j<rand()%size; j++){
			e2 = list_next(e2);
		}
	}
	
	list_swap(e1, e2);
}

unsigned hash_int_2(int i){
	return ((7*i)+3)%4;
}

struct bitmap *bitmap_expand(struct bitmap *bitmap, int size){
	struct bitmap *temp;
	int z;
	z = size + bitmap_size(bitmap);
//	temp =(struct bitmap*)malloc(sizeof *temp);
//	temp->bits = malloc(byte_cnt(z));
//	z = size + bitmap_size(bitmap);
	temp = bitmap_create(z);
	
	for(j =0; j<bitmap_size(bitmap); j++){
		if(bitmap_test(bitmap, j) == true)
			bitmap_mark(temp, j);
		
	}
	if (temp == NULL) return NULL;	
	return temp;
}

unsigned hfunc(const struct hash_elem *e, void *aux){
	return hash_int(hash_entry(e, struct hash_item, elem)->data);
}
bool h_less(const struct hash_elem *a, const struct hash_elem *b, void *aux){
	struct hash_item *temp1, *temp2;
	temp1 = hash_entry(a,struct hash_item, elem);
	temp2 = hash_entry(b, struct hash_item, elem);
	if (temp1->data < temp2->data)
		return true;
	else return false;
} 

bool l_less(const struct list_elem *a, const struct list_elem *b, void *aux){
	int x,y;
	x = list_entry(a, struct list_item, elem)->data;
	y = list_entry(b, struct list_item, elem)->data;
	if(x<y) return true;
	else return false;
}
void hashsp(struct hash_elem* h, void *aux){
	int s;
	s=hash_entry(h, struct hash_item, elem)->data;
	s = s*s;
	hash_entry(h, struct hash_item, elem)->data = s;
}
void hashtre(struct hash_elem* h, void *aux){
	int s;
	s = hash_entry(h, struct hash_item , elem)->data;
	s = s*s*s;
	hash_entry(h,struct hash_item, elem)->data = s;
}
