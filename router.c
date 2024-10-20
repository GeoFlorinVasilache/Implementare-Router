#include "queue.h"
#include "lib.h"
#include "protocols.h"
#include "list.h"
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
    //int c=0;
    copac p=Trie;
    copac el;
    //int sw=0;
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


    //creare lista de arp
    list arplista=cons(NULL,NULL);

    //coada de mesaje netrimise
    queue coada=queue_create();

    //structura pentru mesaje
    struct continut {
        char buffer[1600];
        int lungime;
    };
    //typedef struct continut * interior;






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


		char * el1=get_interface_ip(interface);


		fprintf(fp,"%s\n",el1);


		uint32_t *el=malloc(4*1);
		*el=ntohl(ip_hdr->daddr);

		fprintf(fp,"%ld\n",sizeof(eth_hdr->ether_dhost));


		fprintf(fp,"%d.%d.%d.%d\n",(*el >> 24) & 0xFF,(*el >> 16) & 0xFF,(*el >> 8) & 0xFF,*el & 0xFF);
		fprintf(fp,"%02x:%02x:%02x:%02x:%02x:%02x\n",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5));

        uint8_t* brodcast=malloc(6*1);
        for(int i=0;i<6;i++){
            brodcast[i]=0xFF;
        }
        uint32_t adresadest=0;
        char *p=strtok(el1,".");
        uint32_t gh=24;
        while (p!=NULL)
        {
            uint32_t x =atoi(p);
            adresadest=adresadest+(x<<gh);
            gh=gh-8;
            p=strtok(NULL,".");
        }
        

		//if(strcmp(((char*)(mac)),((char*)(eth_hdr->ether_dhost)))==0){
		if((memcmp(mac,eth_hdr->ether_dhost,sizeof(eth_hdr->ether_dhost))==0) || (memcmp(brodcast,eth_hdr->ether_dhost,sizeof(eth_hdr->ether_dhost))==0)){
			if(ntohs(eth_hdr->ether_type)==0x0800)
				{//if(memcmp(htonl(adresadest),el,sizeof(ip_hdr->daddr))==0)
                if(adresadest == *el)
					{
                        //break;//aici va fi in cazul in care ruterul primeste icmp de echo request replay
                        char noubuf[1600];
                        struct ether_header * nouheader=malloc(sizeof(struct ether_header)*1);
                        struct iphdr* nouip=malloc(sizeof(struct iphdr)*1);
                        struct icmphdr* nouicmp=malloc(sizeof(struct icmphdr)*1);
                        memcpy(nouheader->ether_dhost,eth_hdr->ether_shost,6);
                        memcpy(nouheader->ether_shost,eth_hdr->ether_dhost,6);
                        nouheader->ether_type=htons(0x0800);

                        nouip->ihl=ip_hdr->ihl;
                        nouip->version=ip_hdr->version;
                        nouip->tos=ip_hdr->tos;
                        nouip->tot_len=htons(sizeof(struct iphdr)+sizeof(struct icmphdr));
                        nouip->id=ip_hdr->id;
                        nouip->frag_off=ip_hdr->frag_off;
                        nouip->ttl=64;
                        nouip->protocol=1;
                        nouip->daddr=ip_hdr->saddr;
                        nouip->saddr=ip_hdr->daddr;
                        nouip->check=0;
                        nouip->check=htons(checksum((uint16_t *)nouip,sizeof(struct iphdr)));

                        nouicmp->type=0;
                        nouicmp->code=0;
                        nouicmp->checksum=htons(checksum((uint16_t *)nouicmp,sizeof(struct icmphdr)));

                        memcpy(noubuf,nouheader,sizeof(struct ether_header));
                        memcpy(noubuf+sizeof(struct ether_header),nouip,sizeof(struct iphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr),nouicmp,sizeof(struct icmphdr));

                        send_to_link(interface,noubuf,sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr));                      

                    }
				else{
					uint16_t suma=(ntohs)(ip_hdr->check);
					ip_hdr->check=0;
					uint16_t sumanou=checksum((uint16_t*)ip_hdr,sizeof(*ip_hdr));
					if(suma==sumanou)
						{
						if((ip_hdr->ttl==1)||(ip_hdr->ttl==0))
							{
                        char noubuf[1600];
                        struct ether_header * nouheader=malloc(sizeof(struct ether_header)*1);
                        struct iphdr* nouip=malloc(sizeof(struct iphdr)*1);
                        struct icmphdr* nouicmp=malloc(sizeof(struct icmphdr)*1);
                        memcpy(nouheader->ether_dhost,eth_hdr->ether_shost,6);
                        memcpy(nouheader->ether_shost,eth_hdr->ether_dhost,6);
                        nouheader->ether_type=htons(0x0800);

                        nouip->ihl=ip_hdr->ihl;
                        nouip->version=ip_hdr->version;
                        nouip->tos=ip_hdr->tos;
                        nouip->tot_len=htons(2*sizeof(struct iphdr)+sizeof(struct icmphdr)+8);
                        //nouip->tot_len=htons(sizeof(struct iphdr)+sizeof(struct icmphdr));
                        nouip->id=ip_hdr->id;
                        nouip->frag_off=ip_hdr->frag_off;
                        nouip->ttl=64;
                        nouip->protocol=1;
                        nouip->daddr=ip_hdr->saddr;
                        nouip->saddr=ip_hdr->daddr;
                        nouip->check=0;
                        nouip->check=htons(checksum((uint16_t *)nouip,sizeof(struct iphdr)));

                        nouicmp->type=11;
                        nouicmp->code=0;
                        nouicmp->checksum=0;
                        //nouicmp->checksum=htons(checksum((uint16_t *)nouicmp,sizeof(struct icmphdr)));
                        char bufferajutator[1600];
                        memcpy(bufferajutator,nouicmp,sizeof(struct icmphdr));
                        memcpy(bufferajutator+sizeof(struct icmphdr),ip_hdr,sizeof(struct iphdr));
                        memcpy(bufferajutator+sizeof(struct icmphdr)+sizeof(struct iphdr),buf+sizeof(struct ether_header)+sizeof(struct iphdr),8);
                        nouicmp->checksum=htons(checksum((uint16_t *)bufferajutator,sizeof(struct icmphdr)+sizeof(struct iphdr)+8));

                        memcpy(noubuf,nouheader,sizeof(struct ether_header));
                        memcpy(noubuf+sizeof(struct ether_header),nouip,sizeof(struct iphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr),nouicmp,sizeof(struct icmphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr),ip_hdr,sizeof(struct iphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr)+sizeof(struct iphdr),buf+sizeof(struct ether_header)+sizeof(struct iphdr),8);

                        send_to_link(interface,noubuf,sizeof(struct ether_header)+2*sizeof(struct iphdr)+sizeof(struct icmphdr)+8); 
                        //send_to_link(interface,noubuf,sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr));
                        continue; 
                            }
						ip_hdr->ttl--;
						ip_hdr->check=htons(checksum((uint16_t*)ip_hdr,sizeof(*ip_hdr)));
						//printf("%d %d %d\n",suma ,sumanou,ip_hdr->check);	
						struct route_table_entry * dfg=calloc(sizeof(struct route_table_entry),1);
						intoarceElem(*el,TRIE,&dfg);

                        if(*((int*)(dfg))==0){

                        char noubuf[1600];
                        struct ether_header * nouheader=malloc(sizeof(struct ether_header)*1);
                        struct iphdr* nouip=malloc(sizeof(struct iphdr)*1);
                        struct icmphdr* nouicmp=malloc(sizeof(struct icmphdr)*1);
                        memcpy(nouheader->ether_dhost,eth_hdr->ether_shost,6);
                        memcpy(nouheader->ether_shost,eth_hdr->ether_dhost,6);
                        nouheader->ether_type=htons(0x0800);

                        nouip->ihl=ip_hdr->ihl;
                        nouip->version=ip_hdr->version;
                        nouip->tos=ip_hdr->tos;
                        nouip->tot_len=htons(2*sizeof(struct iphdr)+sizeof(struct icmphdr)+8);
                        //nouip->tot_len=htons(sizeof(struct iphdr)+sizeof(struct icmphdr));
                        nouip->id=ip_hdr->id;
                        nouip->frag_off=ip_hdr->frag_off;
                        nouip->ttl=64;
                        nouip->protocol=1;
                        nouip->daddr=ip_hdr->saddr;
                        nouip->saddr=ip_hdr->daddr;
                        nouip->check=0;
                        nouip->check=htons(checksum((uint16_t *)nouip,sizeof(struct iphdr)));

                        nouicmp->type=3;
                        nouicmp->code=0;
                        nouicmp->checksum=0;
                        //nouicmp->checksum=htons(checksum((uint16_t *)nouicmp,sizeof(struct icmphdr)));
                        char bufferajutator[1600];
                        memcpy(bufferajutator,nouicmp,sizeof(struct icmphdr));
                        memcpy(bufferajutator+sizeof(struct icmphdr),ip_hdr,sizeof(struct iphdr));
                        memcpy(bufferajutator+sizeof(struct icmphdr)+sizeof(struct iphdr),buf+sizeof(struct ether_header)+sizeof(struct iphdr),8);
                        nouicmp->checksum=htons(checksum((uint16_t *)bufferajutator,sizeof(struct icmphdr)+sizeof(struct iphdr)+8));

                        memcpy(noubuf,nouheader,sizeof(struct ether_header));
                        memcpy(noubuf+sizeof(struct ether_header),nouip,sizeof(struct iphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr),nouicmp,sizeof(struct icmphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr),ip_hdr,sizeof(struct iphdr));
                        memcpy(noubuf+sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr)+sizeof(struct iphdr),buf+sizeof(struct ether_header)+sizeof(struct iphdr),8);

                        send_to_link(interface,noubuf,sizeof(struct ether_header)+2*sizeof(struct iphdr)+sizeof(struct icmphdr)+8); 
                        //send_to_link(interface,noubuf,sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct icmphdr));
                        continue;                            
                        }

                        uint8_t * newmac=malloc(sizeof(uint8_t)*6);
                        list p =arplista;
                        int sw=0;
                        while ((sw==0)&&(p!=NULL))
                        {
                            if(p->element==NULL)
                                break;
                            struct arp_entry * supli=(struct arp_entry * )(p->element);
                            if(dfg->next_hop==supli->ip)
                                {
                                    memcpy(newmac,supli->mac,6);
                                    sw=1;}

                            p= p->next;
                        }
                        if(sw==1){
                            memcpy(eth_hdr->ether_shost,eth_hdr->ether_dhost,6);
                            memcpy(eth_hdr->ether_dhost,newmac,6);
                            send_to_link(dfg->interface,buf,len);
                        }
                        else{
                            struct continut *real=malloc(sizeof(struct continut)*1);
                            memcpy(real->buffer,buf,len);
                            real->lungime=len;
                            queue_enq(coada,(void*)real);
                            char sir[1600];


                            struct ether_header *undedauarp =malloc(sizeof(struct ether_header)*1);
                            struct arp_header *cerarp=malloc(sizeof(struct arp_header)*1);

                            uint8_t * adresaeth=malloc(6*1);
                            get_interface_mac(dfg->interface,adresaeth);

                            memcpy(undedauarp->ether_shost,adresaeth,6);
                            memcpy(undedauarp->ether_dhost,brodcast,6);
                            undedauarp->ether_type=htons(0x0806);

                            cerarp->op=htons(1);
                            cerarp->htype=htons(1);
                            cerarp->ptype=htons(0x0800);
                            cerarp->hlen=6;
                            cerarp->plen=4;

                            // uint8_t * adresaeth=malloc(6*1);
                            // get_interface_mac(dfg->interface,adresaeth);
                            memcpy(cerarp->sha,adresaeth,6);

                            char * el10=get_interface_ip(dfg->interface);

                            uint32_t adre=0;
                            char *p1=strtok(el10,".");
                            uint32_t gh1=24;
                            while (p1!=NULL)
                            {
                            uint32_t x =atoi(p1);
                            adre=adre+(x<<gh1);
                            gh1=gh1-8;
                            p1=strtok(NULL,".");
                            }

                            cerarp->spa=htonl(adre);
                            cerarp->tpa=htonl(dfg->next_hop);
                            memset(cerarp->tha,0,6);

                            memcpy(sir,undedauarp,sizeof(struct ether_header));
                            memcpy(sir+sizeof(struct ether_header),cerarp,sizeof(struct arp_header));

                            send_to_link(dfg->interface,sir,sizeof(struct ether_header)+sizeof(struct arp_header));

                        }
                        
						// memcpy(eth_hdr->ether_shost,eth_hdr->ether_dhost,6);
						// uint8_t * newmac=malloc(sizeof(uint8_t)*6);

						// for(int i=0;i<n;i++){
						// 	if(dfg->next_hop == ntohl(arp_table[i].ip)){
						// 		memcpy(newmac,arp_table[i].mac,6);
						// 	}
						// }
						// memcpy(eth_hdr->ether_dhost,newmac,6);
						
						
						// send_to_link(dfg->interface,buf,len);

						}
					else
						continue;
					}}
			else
				if(ntohs(eth_hdr->ether_type)==0x0806){
                    struct arp_header *arp = (struct arp_header *)(buf + sizeof(struct ether_header));
                    if(ntohs(arp->op)==1){
                        if(htonl(adresadest) == arp->tpa){
                            arp->op=htons(2);
                            memcpy(arp->tha,arp->sha,6);
                            memcpy(arp->sha,mac,6);
                            uint32_t ko= arp->spa;
                            arp->spa=arp->tpa;
                            arp->tpa=ko;
                            struct route_table_entry * dfg5=calloc(sizeof(struct route_table_entry),1);
						    intoarceElem(ntohs(arp->tpa),TRIE,&dfg5);
                            
                            memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost,6);
                            memcpy(eth_hdr->ether_shost,mac ,6);

                            send_to_link(interface,buf,len); 
                        }
                        // else{
                        //     memcpy(eth_hdr->ether_shost,mac,6);
                        //     struct route_table_entry * dfg6=calloc(sizeof(struct route_table_entry),1);
						//     intoarceElem(ntohs(arp->tpa),TRIE,&dfg6);
                        //     memcpy(arp->sha,mac,6);
                        //     send_to_link(dfg6->interface,buf,len);    
                        // }   
                    }
                    else {
                    // printf("ceva2\n");

                    if(ntohs(arp->op)==2){
                        if(htonl(adresadest) == arp->tpa){
                            struct arp_entry * arp_element=malloc(sizeof(struct arp_entry)*1);
                            arp_element->ip=ntohl(arp->spa);
                            memcpy(arp_element->mac,arp->sha,6);
                            arplista=cons((void*)arp_element,arplista);
                            if(queue_empty(coada))
                                continue;
                            //list p = coada->head;
                            while (1)
                            {
                                if(queue_empty(coada))
                                    break;
                                struct continut * geo=(struct continut*)(coada->head->element);
                                
                                struct ether_header *ethrezerva = (struct ether_header *) (geo->buffer);
                                struct iphdr *iprezerva = (struct iphdr *)(geo->buffer + sizeof(struct ether_header));
                                struct route_table_entry * dfg20=calloc(sizeof(struct route_table_entry),1);
						        intoarceElem(ntohl(iprezerva->daddr),TRIE,&dfg20);


                                if(dfg20->next_hop==arp_element->ip)
                                {
                                    struct continut * geo=(struct continut*)queue_deq(coada);
                                    //struct iphdr *iprezerva = (struct iphdr *)(geo->buffer + sizeof(struct ether_header));
                                    memcpy(ethrezerva->ether_shost,ethrezerva->ether_dhost,6);
                                    memcpy(ethrezerva->ether_dhost,arp_element->mac,6);
                                    struct route_table_entry * dfg10=calloc(sizeof(struct route_table_entry),1);
						            intoarceElem(ntohl(iprezerva->daddr),TRIE,&dfg10);

                                    send_to_link(dfg10->interface,geo->buffer,geo->lungime);

                                }
                                else{
                                    break;
                                }

                            }                           

                        }
                        else{
                            memcpy(eth_hdr->ether_shost,mac,6);
                            struct route_table_entry * dfg11=calloc(sizeof(struct route_table_entry),1);
						    intoarceElem(ntohs(arp->tpa),TRIE,&dfg11);
                            memcpy(arp->sha,mac,6);
                            send_to_link(dfg11->interface,buf,len); 

                        }


                    }}
                }
					//fprintf(fp,"%d\n",10);
		} 
		else
			continue;
		fclose(fp);
	}
}

