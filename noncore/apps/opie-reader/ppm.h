#include "utypes.h"

#ifndef PPM_H

#define PPM_H

#include "arith.h"

/* pour le calcul �ventuel de quelques statistiques */
/* #define STAT */

/* mise en mode debogage */
/* #define DEBUG */

/* nombre de noeuds maximum: peut �tre modifi�, mais doit �tre tel que:
 * HASH_SIZE+NODE_NBMAX<65530
 */
//#define NODE_NBMAX 40000

/*
 * taille des buffers d'entr�e/sortie
 */
#define BUFSIZE 1024

/*
 * Taille de la table de hachage
 */
#define HASH_SIZE 16384

/* ordre maximale de pr�diction utilis� (Phi) */
#define ORDER_MAX 4

/* fr�quence maximale d'un symbole */
#define RENORM_FREQSYM 250

/* fr�quence maximale pour la somme des fr�quences des symboles d'un contexte */
#define RENORM_FREQTOT 15000

/* nombre de couples symbole,fr�quence dans les structures de noeuds
 * ajust� pour faire au total 16 octets */
#define NODE_SFNB 7
#define HDR_SFNB 2

/* nombre de symboles � coder sans compter les symboles sp�ciaux */
#define SYM_NB 256

/* nombre de symboles sp�ciaux: pour extension si besoin de signalisation */
#define SYM_SPECIAL_NB 1

/* code associ� au symbole ESCAPE (jamais cod� de fa�on explicite) */
#define SYM_ESCAPE 256

/* code de fin de fichier */
#define SYM_EOF 256

/* valeur NULL pour les pointeurs stock�s dans des USHORT */
#define NIL 0xFFFF

/* codage de NIL utilis� pour retrouver l'adresse dans la table de hachage
 * d'un contexte en regardant seulement CTXHDR.hash_next */
#define HASH_ADDRESS (65530-HASH_SIZE)


/* stockage d'un couple symbole, fr�quence */
typedef struct {
  UCHAR sym;   /* num�ro du symbole */
  UCHAR freq;  /* fr�quence du symbole */
} SYMFREQ;


/* header pour g�rer un contexte */
typedef struct {
  USHORT ctx_next;        /* contexte suivant (moins utilis�) */
  USHORT ctx_prev;        /* contexte pr�c�dent (plus utilis�) */
  USHORT hash_next;       /* contexte suivant dans une entr�e de la table
			   * de hachage */
  UCHAR order;            /* ordre du contexte */
  UCHAR sym[ORDER_MAX];   /* symboles constituant le contexte */
  UCHAR sf_max;           /* nombre de symboles-1 dans la liste L */
  union  {
    SYMFREQ sf[HDR_SFNB];   /* s'il y a moins de HDR_SFNB symboles dans
			     * le contexte, on les stocke directement ici
			     */
    struct {
      USHORT freq_tot;     /* sinon on stocke la fr�quence totale (c) */
      USHORT sf_next;      /* et un pointeur sur le premier noeud
			    * constituant la liste L des symboles associ�s
			    * au contexte */
    } l;
  } sf;
} CTXHDR;

/* structure pour g�rer NODE_SFNB �l�ments de la liste des symboles associ�s
 * � un contexte */
typedef struct {
  SYMFREQ sf[NODE_SFNB];  /* les couples symbole, fr�quence */
  USHORT sf_next;         /* pointeur sur l'�ventuel bloc suivant */
} CTXSYMFREQ;

/*
 * structure de base pour la gestion de la m�moire: le noeud
 */

typedef union _NODE  {
  CTXHDR hdr;             /* si le noeud contient un header de contexte */
  CTXSYMFREQ sf;          /* si le noeud contient une partie de la liste L */
  USHORT free_next;       /* si le noeud est vide: pointeur sur un noeud vide
			   * suivant */
} NODE;

class ppm_worker
{

/* gestion des noeuds */
NODE *node_heap;           /* heap contenant tous les noeuds */
UINT node_free_nb;         /* nombre de noeuds vides */
UINT node_free_first;      /* premier noeud de la liste des noeuds vides */
UINT node_free_last;       /* dernier noeud de la liste des noeuds vides */

/* gestion de la liste des contextes les plus utilis�s */
USHORT *hash_table;        /* table de hachage pour rechercher un contexte */
UINT ctx_first;            /* premier contexte: le plus utilis� */
UINT ctx_last;             /* dernier contexte: le moins utilis� */
UINT ctx_nb;               /* nombre de contextes */

/* donn�es caract�risant le contexte courant */
UCHAR sym_context[ORDER_MAX+1];    /* symboles pr�c�dants le symbole en cours
				    * de codage */
int sym_hash[ORDER_MAX+1];         /* index dans la table de hachage
				    * correspondants aux diff�rentes longueurs
				    * de contextes
				    */

/* syst�me d'exclusion des caract�res */
UCHAR sym_excl[SYM_NB];            /* tableau pour l'exclusion */
UCHAR sym_excl_code;               /* code courant pour l'exclusion */


/* d�claration des fonctions de base */

/* noeuds */
void Node_Free(UINT p);
UINT Node_Alloc(void);

/* contextes */
void Context_DeleteLast(void);
void Context_MoveFirst(UINT c);
void Context_New(int sym,int order);
void Context_NewSym(int sym,UINT c);
UINT Context_Search(int order);
void Context_Renorm(UINT ctx);
void Hash_Update(int sym);
 void Sym_ExcludeReset(void);

/* codage */
/*
void Encode_NewSym(int sym);
int Encode_NoExclude(int sym,UINT ctx);
int Decode_Exclude(UINT ctx);
int PPM_Decode(void);
*/

/* d�codage */
int Decode_NewSym(void);
int Decode_NoExclude(UINT ctx);
int Decode_Exclude(UINT ctx);

#ifdef STAT
void PrintStat(void);
#endif
 public:
  ArithClass arith;
  int PPM_Init(unsigned short);
  void PPM_End(void);
  int PPM_Decode(void);
};

#endif

