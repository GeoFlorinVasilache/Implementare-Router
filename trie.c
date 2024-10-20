#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <asm/byteorder.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct triee
{
    void* element;
    struct triee *stanga;
    struct triee* dreapta;

}*copac;

struct arp_entry {
    uint32_t ip;
    uint8_t mac[6];
};


struct route_table_entry {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
 } __attribute__((packed));


static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}
int read_rtable(const char *path, struct route_table_entry *rtable)
{
	FILE *fp = fopen(path, "r");
	int j = 0, i;
	char *p, line[64];
	
	while (fgets(line, sizeof(line), fp) != NULL) {
		p = strtok(line, " .");
		i = 0;
		while (p != NULL) {
			if (i < 4)
				*(((unsigned char *)&rtable[j].prefix)  + 3-(i % 4)) = (unsigned char)atoi(p);

			if (i >= 4 && i < 8)
				*(((unsigned char *)&rtable[j].next_hop)  + 3-(i % 4)) = atoi(p);

			if (i >= 8 && i < 12)
				*(((unsigned char *)&rtable[j].mask)  + 3-(i % 4)) = atoi(p);

			if (i == 12)
				rtable[j].interface = atoi(p);
			p = strtok(NULL, " .");
			i++;
		}
		j++;
	}
	return j;
}

int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;

	return (a << 4) | b;
}

int hwaddr_aton(const char *txt, uint8_t *addr)
{
	int i;
	for (i = 0; i < 6; i++) {
		int a, b;
		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

void adaugaElem(void *elem,copac Trie,int x){
    copac el = malloc(sizeof(struct triee )*1);
    el->element=elem;
    el->dreapta=NULL;
    el->stanga=NULL;

    int sw=0;
    copac p;
    for( p =Trie;p!=NULL;){
        if(x==0)
            {
            if(p->stanga==NULL)
                {
                    sw=1;
                    break;}
            else
                p=p->stanga;}
        if(x==1){
            {
                if(p->dreapta==NULL)
                    {
                        sw=2;
                        break;
                    }
                else
                    p=p->dreapta;
            }
        }           
    }
    if(sw==1)
    {
        //printf("%u\n",*((int*)elem));
        p->stanga=el;}
    else{
        //printf("%u\n",*((int*)elem));
        p->dreapta=el;}


}

//192.6.1.1
//1100 0000 0000 0110 0000 0001 0000 0001
//3221618945
//192.6.1.0
//1100 0000 0000 0110 0000 0001 0000 0000
//3221618944

//192.254.44.0
//1100 0000 1111 1110 0010 1100 0000 0000
//3237882880
//192.254.45.0 
//1100 0000 1111 1110 0010 1101 0000 0000
//3237883136

//192.254.44.2
//1100 0000 1111 1110 0010 1100 0000 0010
//3237882882

//192.254.44.3
//192.254.44.4

//1100 0000 1111 1110 0010 1100 0000 0011
//3237882883
//1100 0000 1111 1110 0010 1100 0000 0100
//3237882884
int introducereInArbore(struct route_table_entry *mama,copac Trie){
    int x;
    int c=0;
    copac p=Trie;
    copac el;
    int sw=0;
    int nr=0;
    for(int i=31;i>=0;i--)
        {
            x=((1 << i) & (mama->mask));
            if(x!=0)
                nr++;
        }
    for(int i=31;i>(32-nr);i--){
        x=((1 << i) & (mama->prefix));
        // printf("%u\n",x);
            el = malloc(sizeof(struct triee )*1);
            el->element=calloc(1, sizeof(int));
            el->dreapta=NULL;
            el->stanga=NULL;
        if(x==0)
            {
                if(p->stanga==NULL){
                // el = malloc(sizeof(struct triee )*1);
                // el->element=&c;
                // el->dreapta=NULL;
                // el->stanga=NULL;
                p->stanga=el;}
                p=p->stanga;
            }
        else
            {
                if(p->dreapta==NULL){
                // el = malloc(sizeof(struct triee )*1);
                // el->element=&c;
                // el->dreapta=NULL;
                // el->stanga=NULL;
                p->dreapta=el;}
                p=p->dreapta;
            }
    }
    if(((1 << (32-nr)) & (mama->prefix))==0)
        {

            if(p->stanga==NULL){
            el = malloc(sizeof(struct triee )*1);
            el->element=mama;
            el->dreapta=NULL;
            el->stanga=NULL;
            p->stanga=el;}
            p=p->stanga;
        }
    else
        {
            if(p->dreapta==NULL){
            el = malloc(sizeof(struct triee )*1);
            el->element=mama;
            el->dreapta=NULL;
            el->stanga=NULL;
            p->dreapta=el;}
            p=p->dreapta;
        }
    return nr;
}

int numara(copac Tie){
    int y=0;
    copac p=Tie;
    while(p!=NULL){
        if(p->dreapta!=NULL)
            {
                y++;
                p=p->dreapta;}
        if(p->stanga!=NULL){
                y++;
                p=p->stanga;
            }
        if(p->dreapta==NULL && p->stanga==NULL)
                break;     

    }
    return y;
}
//1100 0000 1111 1110 0010 1100 0000 0000
//1100 0000 1111 1110 0010 1101 0000 0000
void intoarceElem(uint32_t prefix,copac Trie,struct route_table_entry **tyu){
    copac p=Trie;
    int x=0;
    int ge =0;


    for(int i=31;i>=0;i--){
        if(p==NULL)
            break;
        x=(1 << i) & prefix;
        //printf("%u\n",x);
        if(x==0)
            {  
                if(p->stanga!=NULL){
                    p=p->stanga;
                    ge=*((int*)(p->element));
                    if(ge != 0)
                        *tyu=(struct route_table_entry*)(p->element);

                }
                else
                    p=p->stanga;    
            }
        if(x!=0){
            if(p->dreapta!=NULL){
                    p=p->dreapta;
                    ge=*((int *)(p->element));
                    if(ge != 0)
                        *tyu=(struct route_table_entry*)(p->element);

                }
                else
                    p=p->dreapta; 
        }
    }
}

int parse_arp_table(char *path, struct arp_entry *arp_table)
{
	FILE *f;
	fprintf(stderr, "Parsing ARP table\n");
	f = fopen(path, "r");
	//DIE(f == NULL, "Failed to open %s", path);
	char line[100];
	int i = 0;
	for(i = 0; fgets(line, sizeof(line), f); i++) {
		char ip_str[50], mac_str[50];
		sscanf(line, "%s %s", ip_str, mac_str);
		fprintf(stderr, "IP: %s MAC: %s\n", ip_str, mac_str);
		arp_table[i].ip = inet_addr(ip_str);
		int rc = hwaddr_aton(mac_str, arp_table[i].mac);
		//DIE(rc < 0, "invalid MAC");
	}
	fclose(f);
	fprintf(stderr, "Done parsing ARP table.\n");
	return i;
}

//1100 0000 0000 0000 0000 0001 0000 0010
int main(){
    copac TRIE=malloc(sizeof (struct triee));
    TRIE->element=NULL;
    TRIE->dreapta=NULL;
    TRIE->stanga=NULL;

    char * fisier=malloc(12*1);
    strcpy(fisier,"rtable1.txt");
    uint32_t prefix=3232235522;
    uint32_t prefix1=3221292032;
    struct route_table_entry *rtable=malloc(sizeof(struct route_table_entry )*100000);
	int nr =read_rtable(fisier,rtable);

    struct arp_entry *arp_table=malloc(sizeof(struct arp_entry*)*10);
    char * fisier1=malloc(20*1);
    strcpy(fisier1,"arp_table.txt");
    int n=parse_arp_table(fisier1,arp_table);

    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",*arp_table[0].mac,*(arp_table[0].mac+1),*(arp_table[0].mac+2),*(arp_table[0].mac+3),*(arp_table[0].mac+4),*(arp_table[0].mac+5));

    //printf("%d\n",introducereInArbore(&rtable[2034],TRIE));
    //struct route_table_entry *este= (struct route_table_entry*)(TRIE->dreapta->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->dreapta->stanga->element);
    //printf("%u\n",este->prefix);

    struct route_table_entry * dfg=calloc(sizeof(struct route_table_entry),1);
    for(int i=0;i<nr;i++)
        introducereInArbore(&rtable[i],TRIE);
    
    // struct route_table_entry *este= (struct route_table_entry*)(TRIE->dreapta->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->dreapta->stanga->element);
    // printf("%u\n",este->prefix);
    // struct route_table_entry *sq=intoarceElem(prefix,TRIE);
    // if(sq!=NULL)
    //     printf("%u\n",sq->prefix);

    // printf("%u\n",rtable[2034].prefix);

    // sq=intoarceElem(prefix1,TRIE);
    // if(sq!=NULL)
    //     printf("%u\n",sq->prefix);

    // printf("%u\n",rtable[0].prefix);

    //3221292032

    intoarceElem(prefix,TRIE,&dfg);

    printf("%u %d\n",dfg->next_hop,dfg->interface);
    // sq=intoarceElem(prefix3,TRIE);
    // if(sq!=NULL)
    //     printf("%u\n",sq->prefix);
    //     else
    //     printf("nu este in arbore!\n");


    // uint32_t *prefix=malloc(sizeof(uint32_t)*1);
    // *prefix=3237882880;
    // uint32_t* prefix2=malloc(sizeof(uint32_t)*1);
    // *prefix2=3237883136;
    // uint32_t * prefix3=malloc(sizeof(uint32_t)*1);
    // *prefix3=3237882882;
    // uint32_t *prefix4=malloc(sizeof(uint32_t)*1);
    // *prefix4=3237882883;
    // uint32_t* prefix5=malloc(sizeof(uint32_t)*1);
    // *prefix5=3237882884;
    // uint32_t* prefix0=malloc(sizeof(uint32_t)*1);
    // *prefix0=3237882881;
    // //int sw3=introducereInArbore(prefix3,TRIE);
    // int sw1=introducereInArbore(prefix,TRIE);
    // int sw2=introducereInArbore(prefix2,TRIE);
    // introducereInArbore(prefix4,TRIE);
    // //uint32_t mamama=1;
    // //introducereInArbore(mamama,TRIE);
    // printf("%u %u %u %u %u %u\n", intoarceElem(prefix2, TRIE), intoarceElem(prefix, TRIE), intoarceElem(prefix3, TRIE), intoarceElem(prefix4, TRIE), intoarceElem(prefix5, TRIE),intoarceElem(prefix0, TRIE));

    //printf("%u\n",numara(TRIE));
    //1100 0000 1111 1110 0010 1100 0000 0000
    //printf("%u\n",*((int*)(TRIE->dreapta->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->dreapta->dreapta->dreapta->dreapta->dreapta->dreapta->dreapta->stanga->stanga->stanga->dreapta->stanga->dreapta->dreapta->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->stanga->element)));
    //intoarceElem(prefix,TRIE);
    

    return 0;
}