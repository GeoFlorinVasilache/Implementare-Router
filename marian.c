#include "queue.h"
#include "lib.h"
#include "protocols.h"
#include <string.h>
#include <arpa/inet.h>

typedef struct triee
{
    void* element;
    struct triee *stanga;
    struct triee* dreapta;

}*copac;

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

int main(int argc, char *argv[])
{
	char buf[MAX_PACKET_LEN];

	// Do not modify this line
	init(argc - 2, argv + 2);

	struct route_table_entry *rtable=malloc(sizeof(struct route_table_entry )*100000);
	int nr =read_rtable(argv[1],rtable);
	copac TRIE=malloc(sizeof (struct triee));
    TRIE->element=NULL;
    TRIE->dreapta=NULL;
    TRIE->stanga=NULL;

	for(int i=0;i<nr;i++)
        introducereInArbore(&rtable[i],TRIE);


	struct arp_entry *arp_table=malloc(sizeof(struct arp_entry*)*10);
    char * fisier1=malloc(20*1);
    strcpy(fisier1,"arp_table.txt");
    int n=parse_arp_table(fisier1,arp_table);

	while (1) {

		int interface;
		size_t len;

		interface = recv_from_any_link(buf, &len);
		DIE(interface < 0, "recv_from_any_links");

		struct ether_header *eth_hdr = (struct ether_header *) buf;
		struct iphdr *ip_hdr = (struct iphdr *)(buf + sizeof(struct ether_header));
		/* Note that packets received are in network order,
		any header field which has more than 1 byte will need to be conerted to
		host order. For example, ntohs(eth_hdr->ether_type). The oposite is needed when
		sending a packet on the link, */
		uint8_t *mac=malloc(6*1);
		get_interface_mac(interface,mac);


		FILE * fp;
		fp = fopen ("file.txt", "wr+");
		
		fprintf(fp,"%d.%d.%d.%d\n\n\n",(rtable[1].next_hop >> 24) & 0xFF,(rtable[1].next_hop >> 16) & 0xFF,(rtable[1].next_hop >> 8) & 0xFF,rtable[1].next_hop & 0xFF);


		char * el1=malloc(4*1);
		el1=get_interface_ip(interface);


		fprintf(fp,"%s\n",el1);


		uint32_t *el=malloc(4*1);
		*el=ntohl(ip_hdr->daddr);

		fprintf(fp,"%ld\n",sizeof(eth_hdr->ether_dhost));


		fprintf(fp,"%d.%d.%d.%d\n",(*el >> 24) & 0xFF,(*el >> 16) & 0xFF,(*el >> 8) & 0xFF,*el & 0xFF);
		fprintf(fp,"%02x:%02x:%02x:%02x:%02x:%02x\n",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));

		//if(strcmp(((char*)(mac)),((char*)(eth_hdr->ether_dhost)))==0){
		if(memcmp(mac,eth_hdr->ether_dhost,sizeof(eth_hdr->ether_dhost))==0){
			if(ntohs(eth_hdr->ether_type)==0x0800)
				if(memcmp(get_interface_ip(interface),el,sizeof(ip_hdr->daddr))==0)
					break;//aici va fi in cazul in care ruterul primeste icmp de echo request replay
				else{
					uint16_t suma=(ntohs)(ip_hdr->check);
					ip_hdr->check=0;
					uint16_t sumanou=checksum((uint16_t*)ip_hdr,sizeof(*ip_hdr));
					if(suma==sumanou)
						{
						if((ip_hdr->ttl==1)||(ip_hdr->ttl==0))
							continue;//trebuie sa mai trimit un icmp inapoi
						ip_hdr->ttl--;
						ip_hdr->check=htons(checksum((uint16_t*)ip_hdr,sizeof(*ip_hdr)));
						fprintf(fp,"%d %d %d\n",suma ,sumanou,ip_hdr->check);	
						struct route_table_entry * dfg=calloc(sizeof(struct route_table_entry),1);
						intoarceElem(*el,TRIE,&dfg);
						memcpy(eth_hdr->ether_shost,eth_hdr->ether_dhost,6);
						uint8_t * newmac=malloc(sizeof(uint8_t)*6);

						for(int i=0;i<n;i++){
							if(dfg->next_hop == ntohl(arp_table[i].ip)){
								memcpy(newmac,arp_table[i].mac,6);
							}
						}
						memcpy(eth_hdr->ether_dhost,newmac,6);
						
						
						send_to_link(dfg->interface,buf,len);

						}
					else
						continue;
					}
			else
				if(ntohs(eth_hdr->ether_type)==0x0806)
					fprintf(fp,"%d\n",10);
		} 
		else
			continue;
		fclose(fp);
	}
}
