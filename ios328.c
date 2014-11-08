#include "prvios.h"

#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
    #define g16b(d) (*((const UINT16 *) (d)))
#endif

#if !defined (g16b)
    #define g16b(d) ((((UINT32)(((const BYTE *)(d))[1])) << 8) + (UINT32)(((const BYTE *)(d))[0]) )
#endif


/// linee massime: maxline * 4096
#define LINE_SIZE 256
#define CODE_MAX 1048576
#define LHS_NAME_MAX 80
#define LHS_REP_MAX 256
#define HASH_MAX 256

//step1 parse preprocessor

//parse include 
//start include
//parse define
//Fill struct cmd parse


typedef enum {TK_CMD,TK_DEF,TK_LBL,TK_REG} TKTYPE;

typedef struct _LHS
{
	TKTYPE t;
	CHAR n[LHS_NAME_MAX];
	CHAR r[LHS_REP_MAX];
	UINT32 a;
	struct _LHS* next;
}LHS;

typedef struct _IAL
{
	LHS* tk[HASH_MAX];
}IAL;


UINT32 fast_hash(char * data, INT32 len)
{
    UINT32 hash = len;
    UINT32 tmp;
    INT32 rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--)
    {
        hash  += g16b(data);
        tmp    = (g16b(data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2 * sizeof(INT16);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += g16b (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += g16b(data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

UINT32 _lhs_hash(CHAR* val, INT32 len)
{
    return fast_hash(val,len) % HASH_MAX;
}

LHS* _lhs_find(IAL* a, UINT32 h, CHAR* n)
{	
	LHS* f;
	for (f = a->tk[h]; f; f = f->next)
		if ( !strcmp(f->n,n) ) return f;
	return NULL;
}

BOOL _lhs_add(IAL* a, TKTYPE tt,CHAR* n, CHAR* r, UINT32 adr)
{
	UINT32 h = _lhs_hash(n,strlen(n));
	if ( _lhs_find(a, h, n) ) return FALSE;
	
	LHS* t = malloc(sizeof(LHS));
	t->t = tt;
	strcpy(t->n,n);
	if ( r ) strcpy(t->r,r);
	t->a = adr;
	
	t->next = a->tk[h];
	a->tk[h] = t;
	return TRUE;
}

BOOL _lhs_remove(IAL* a, CHAR* n)
{
	UINT32 h = _lhs_hash(n,strlen(n));
	
	LHS* f = a->tk[h];
	
	if ( !f ) return FALSE;
	if ( !strcmp(f->n,n) )
	{
		a->tk[h] = f->next;
		free(f);
		return TRUE;
	}
	
	LHS* r;
	for (; f->next; f = f->next)
	{
		if ( !strcmp(f->next->n,n) )
		{
			r = f->next;
			f->next = r->next;
			free(r);
		}
	}
	return FALSE;
}

VOID _ial_init(IAL* a)
{
    int i;
    for (i = 0; i < HASH_MAX; ++i)
    {
        a->tk[i] = NULL;
    }
    
    _lhs_add(a,TK_CMD,"sleep",NULL,CMD_SLEEP);
    _lhs_add(a,TK_CMD,"opz",NULL,CMD_OPZ);
    _lhs_add(a,TK_CMD,"maxto",NULL,CMD_MAXTO);
    _lhs_add(a,TK_CMD,"pindir",NULL,CMD_PIN_DIR);
    _lhs_add(a,TK_CMD,"pinwrite",NULL,CMD_PIN_WRITE);
    _lhs_add(a,TK_CMD,"pinread",NULL,CMD_PIN_READ);
    _lhs_add(a,TK_CMD,"pwmfq",NULL,CMD_PWM_FQ_SET);
    _lhs_add(a,TK_CMD,"pwmdt",NULL,CMD_PWM_DT_SET);
    _lhs_add(a,TK_CMD,"pwmtout",NULL,CMD_PWM_TOUT_SET);
    _lhs_add(a,TK_CMD,"pwmenable",NULL,CMD_PWM_ENABLE);
    _lhs_add(a,TK_CMD,"pwmdisable",NULL,CMD_PWM_DISABLE);
    _lhs_add(a,TK_CMD,"preset",NULL,CMD_PRE_SET);
    _lhs_add(a,TK_CMD,"pwmauto",NULL,CMD_PWM_AUTO);
    _lhs_add(a,TK_CMD,"nop",NULL,CMD_NOP);
    _lhs_add(a,TK_CMD,"oppc",NULL,CMD_OPPC);
    _lhs_add(a,TK_CMD,"rr",NULL,CMD_RR);
    _lhs_add(a,TK_CMD,"ri",NULL,CMD_RI);
    _lhs_add(a,TK_CMD,"rm",NULL,CMD_RM);
    _lhs_add(a,TK_CMD,"mr",NULL,CMD_MR);
    _lhs_add(a,TK_CMD,"add",NULL,CMD_ADD);
    _lhs_add(a,TK_CMD,"dif",NULL,CMD_DIF);
    _lhs_add(a,TK_CMD,"mul",NULL,CMD_MUL);
    _lhs_add(a,TK_CMD,"div",NULL,CMD_DIV);
    _lhs_add(a,TK_CMD,"shl",NULL,CMD_SHL);
    _lhs_add(a,TK_CMD,"shr",NULL,CMD_SHR);
	_lhs_add(a,TK_CMD,"or",NULL,CMD_OR);
	_lhs_add(a,TK_CMD,"and",NULL,CMD_AND);
	_lhs_add(a,TK_CMD,"xor",NULL,CMD_XOR);
	_lhs_add(a,TK_CMD,"not",NULL,CMD_NOT);
	_lhs_add(a,TK_CMD,"jmp",NULL,CMD_JMP);
	_lhs_add(a,TK_CMD,"jmi",NULL,CMD_JMI);
	_lhs_add(a,TK_CMD,"jma",NULL,CMD_JMA);
	_lhs_add(a,TK_CMD,"jeq",NULL,CMD_JEQ);
	_lhs_add(a,TK_CMD,"jan",NULL,CMD_JAN);
	_lhs_add(a,TK_CMD,"jor",NULL,CMD_JOR);
	_lhs_add(a,TK_CMD,"jno",NULL,CMD_JNO);
	_lhs_add(a,TK_CMD,"push",NULL,CMD_PUSH);
	_lhs_add(a,TK_CMD,"pop",NULL,CMD_POP);
	_lhs_add(a,TK_CMD,"call",NULL,CMD_CALL);
	_lhs_add(a,TK_CMD,"ret",NULL,CMD_RET);
	_lhs_add(a,TK_CMD,"pulsed",NULL,CMD_PULSED_OUT);
	_lhs_add(a,TK_CMD,"pulseu",NULL,CMD_PULSEU_OUT);
	_lhs_add(a,TK_CMD,"pulsef",NULL,CMD_PULSEF_IN);
	_lhs_add(a,TK_CMD,"pulsec",NULL,CMD_PULSEC_IN);
	_lhs_add(a,TK_CMD,"mdelay",NULL,CMD_MDELAY);
	_lhs_add(a,TK_CMD,"udelay",NULL,CMD_UDELAY);
	
	_lhs_add(a,TK_REG,"%0",NULL,0x00);
	_lhs_add(a,TK_REG,"%1",NULL,0x01);
	_lhs_add(a,TK_REG,"%2",NULL,0x02);
	_lhs_add(a,TK_REG,"%3",NULL,0x03);
	_lhs_add(a,TK_REG,"%4",NULL,0x04);
	_lhs_add(a,TK_REG,"%5",NULL,0x05);
	_lhs_add(a,TK_REG,"%6",NULL,0x06);
	_lhs_add(a,TK_REG,"%7",NULL,0x07);
	_lhs_add(a,TK_REG,"%8",NULL,0x08);
	_lhs_add(a,TK_REG,"%9",NULL,0x09);
	_lhs_add(a,TK_REG,"%A",NULL,0x0A);
	_lhs_add(a,TK_REG,"%B",NULL,0x0B);
	_lhs_add(a,TK_REG,"%C",NULL,0x0C);
	_lhs_add(a,TK_REG,"%D",NULL,0x0D);
	_lhs_add(a,TK_REG,"%E",NULL,0x0E);
	///_lhs_add(a,TK_CMD,"",NULL,CMD_);

    
}

VOID _ial_free(IAL* a)
{
	LHS* r;
	int i;
    for (i = 0; i < HASH_MAX; ++i)
    {
        for (r = a->tk[i]; r; r = a->tk[i])
        {
			a->tk[i] = a->tk[i]->next;
			free(r);
		}
    }
}



VOID _prs_space(CHAR* d, CHAR* s)
{	
	while( *s )
	{	
		while( *s && ( *s == ' ' || *s == '\t') ) ++s;
		
		while ( *s && !( *s == ' ' || *s == '\t' || *s == '\n') ) *d++ = *s++;

		if ( *s ) *d++ = *s++;
	}
	*d = '\0';
}

VOID _prs_fit(CHAR* d, CHAR* s)
{
	BOOL stnl = TRUE;
	
	for(; *s; ++s)
	{
		if ( stnl && *s == '\n' ) continue;
		if ( stnl && *s != '\n' ) stnl = FALSE;
		if ( !stnl && *s == '\n' ) stnl = TRUE;
		*d++ = *s;
	}
	*d = '\0';
}

VOID _prs_comment(CHAR* d, CHAR* s)
{	
	for(; *s; ++s)
	{
		if ( *s == ';' )
		{
			while ( *s && *s != '\n' ) ++s;	
		}
			
		*d++ = *s;
	}
	*d = '\0';
}

CHAR* _prs_argstring(CHAR* d, CHAR* s)
{
	while ( *s && *s != '\"' ) ++s;
	if ( *s != '"' ) return NULL;
	++s;
	if ( !*s ) return NULL;
	
	while ( *s && *s != '\"' ) *d++ = *s++;
	if ( *s != '"' ) return NULL;
	*d = '\0';
	return s + 1;
}

CHAR* _prs_token(CHAR* d, CHAR* s)
{
	while ( *s && (*s != ' ' && *s != ',' && *s != ':' )) *d++ = *s++;
	*d = '\0';
	return s;
}

CHAR* _prs_loadfile(CHAR* d,CHAR* fname)
{
	CHAR* md = d;
	FILE* f = fopen(fname,"r");
		if ( !f ) return NULL;
	
	while ( (*d = fgetc(f)) > 0 ) ++d;
	if ( *d )
	{
		if ( *(d-1) )
			*d = '\0';
		else
			--d;
	}
	
	fclose(f);
	
	CHAR ps0[CODE_MAX];
	
	_prs_space(ps0,md);
	_prs_comment(md,ps0);
	
	return d;
}

INT32 _prs_include(CHAR* d, CHAR* s)
{	
	CHAR fi[LINE_SIZE];
	
	for(; *s; ++s)
	{
		if ( *s == '#' )
		{
			if ( !strncmp(s,"#include",8) )
			{
				s += 8;
				if ( !(s = _prs_argstring(fi,s)) ) return IAL_ERR_STRING;
				while ( *s && *s != '\n' ) ++s;
				if ( !(d = _prs_loadfile(d,fi)) ) return IAL_ERR_FILE;
			}
		}
		*d++ = *s;
	}
	*d = '\0';
	return 0;
}

INT32 _prs_pre(IAL* a, CHAR* d, CHAR* s)
{	
	CHAR n[LHS_NAME_MAX];
	CHAR r[LHS_REP_MAX];
	CHAR* pr;
	BOOL skl = FALSE;
	BOOL ifd = FALSE;
	
	for(; *s; ++s)
	{
		if ( *s == '#' )
		{
			if ( !skl && !strncmp(s,"#define",7) )
			{
				s += 7;
				if (*s != ' ' || !*s) return IAL_ERR_DEFINE;
				++s;
				if (!*s) return IAL_ERR_DEFINE;
				
				s = _prs_token(n,s);
				pr = r;
				while ( *s && *s != '\n' )  *pr++ = *s++;
				*pr = '\0';
				
				if ( !_lhs_add(a,TK_DEF,n,r,0) ) return IAL_ERR_DEFINE;
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#ifdef",6) )
			{
				if (ifd) return IAL_ERR_MULTIPLEIFDEF;
				ifd = TRUE;
				s += 6;
				if (*s != ' ' || !*s) return IAL_ERR_IFDEF;
				++s;
				if (!*s) return IAL_ERR_IFDEF;
				
				s = _prs_token(n,s);
				
				while ( *s && *s != '\n' )  ++s;
				
				if ( !_lhs_find(a,_lhs_hash(n,strlen(n)),n) ) skl = TRUE;
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#ifndef",7) )
			{
				if (ifd) return IAL_ERR_MULTIPLEIFDEF;
				ifd = TRUE;
				s += 7;
				if (*s != ' ' || !*s) return IAL_ERR_IFDEF;
				++s;
				if (!*s) return IAL_ERR_IFDEF;
				
				s = _prs_token(n,s);
				
				while ( *s && *s != '\n' )  ++s;
				
				if ( _lhs_find(a,_lhs_hash(n,strlen(n)),n) ) skl = TRUE;
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#elif",5) )
			{
				if (!ifd) return IAL_ERR_IFDEF;
				s += 5;
				if (*s != ' ' || !*s) return IAL_ERR_IFDEF;
				++s;
				if (!*s) return IAL_ERR_IFDEF;
				
				s = _prs_token(n,s);
				
				while ( *s && *s != '\n' )  ++s;
				
				if ( !_lhs_find(a,_lhs_hash(n,strlen(n)),n) ) 
					skl = TRUE;
				else
					skl = FALSE;
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#elnif",6) )
			{
				if (!ifd) return IAL_ERR_IFDEF;
				s += 6;
				if (*s != ' ' || !*s) return IAL_ERR_IFDEF;
				++s;
				if (!*s) return IAL_ERR_IFDEF;
				
				s = _prs_token(n,s);
				
				while ( *s && *s != '\n' )  ++s;
				
				if ( !_lhs_find(a,_lhs_hash(n,strlen(n)),n) ) 
					skl = FALSE;
				else
					skl = TRUE;
					
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#else",6) )
			{
				if (!ifd) return IAL_ERR_IFDEF;
				s += 6;
				while ( *s && *s != '\n' )  ++s;
				skl = !skl;
				if ( !*s ) break;
			}
			else if ( !skl && !strncmp(s,"#endif",6) )
			{
				if (!ifd) return IAL_ERR_IFDEF;
				ifd = FALSE;
				skl = FALSE;
				s += 6;
				while ( *s && *s != '\n' )  ++s;
				if ( !*s ) break;
			}
		}
		
		if ( !skl ) *d++ = *s;
	}
	*d = '\0';
	return 0;
}

INT32 _prs_prerep(IAL* a, CHAR* d, CHAR* s)
{
	CHAR tk[LHS_NAME_MAX];
	CHAR* ptk;
	LHS* f;
	INT32 ret = 0;
	
	while ( *(s = _prs_token(tk,s)) )
	{
		f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
		
		if ( f && f->t == TK_DEF)
		{
			ptk = f->r;
			ret = 1;
		}
		else
			ptk = tk;
			
		while ( *ptk ) *d++ = *ptk++;
		*d++ = *s++;
	}
	*d = '\0';
	return ret;
}

INT32 _prs_ibc(IAL* a,CHAR* ibc, CHAR* s)
{
	CHAR tk[LHS_NAME_MAX];
	CHAR* ptk;
	LHS* f;
	
	while ( *(s = _prs_token(tk,s)) )
	{
		printf("TOKEN:%s:",tk);
		f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
		if (f) 
			puts("exist");
		else
			puts("not exist");
		
		if ( f && ( f->t == TK_CMD ) )
			sprintf(tk,"%d",f->a);
		
		ptk = tk;
		while ( *ptk ) *ibc++ = *ptk++;
		*ibc++ = *s++;
	}
	*ibc = '\0';
	return 0;
}

INT32 _prs_lbl(IAL* a, UINT32 offset, CHAR* d, CHAR* s)
{
	CHAR tk[LHS_NAME_MAX];
	CHAR* ptk;
	
	while ( *(s = _prs_token(tk,s)) )
	{
		if ( *s != ':' ) 
		{
			ptk = tk;
			while ( *ptk ) *d++ = *ptk++;
		}
		else
		{
			if ( !_lhs_add(a,TK_LBL,tk,NULL,offset) ) return IAL_ERR_MAKE;
			++s;
		}
		
		while ( *s != '\n') *d++ = *s++;
		*d++ = *s++;
		offset +=4 ;
	}
	*d = '\0';
	return 0;
}

BOOL _prs_hp(CHAR* s)
{
	while ( *s && *s != '\n' )
		if ( *s != ' ' ) return TRUE;
	return FALSE;
}

INT32 _prs_rv(BOOL* isreg,CHAR* s)
{
	INT32 ret;
	
	if ( *s == '%')
	{
		*isreg = TRUE;
		ret = atoi(s+1);
		if (ret > 15) return -1;
	}
	else
	{
		*isreg = FALSE;
		ret = atoi(s);
	}
	return ret;
}

INT32 _prs_bc(IAL* a, UINT32* offset, BYTE* bc, CHAR* s)
{
	CHAR tk[LHS_NAME_MAX];
	UINT32 conv;
	UINT32 icode;
	LHS* f;
	BOOL isr;
	
	while ( *(s = _prs_token(tk,s)) )
	{
		f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
		if ( f ) return IAL_ERR_MAKE;
		
		icode = atoi(tk);
		*bc++ = icode;
		switch (icode)
		{
			/// cmd
			case CMD_SLEEP:
			case CMD_NOP:
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
				
				*bc++ = 0;
				*bc++ = 0;
				*bc++ = 0;
			break;
			
			///cmd Lint,Hint
			case CMD_OPPC:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr ) return IAL_ERR_MAKE;
				*bc++ = (BYTE) conv;
				*bc++ = (BYTE)( conv >> 8 ) ;
				*bc++ = 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd {byte,reg},isreg
			case CMD_OPZ:
			case CMD_PWM_ENABLE:
			case CMD_PWM_DISABLE:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr && conv < 0 ) return IAL_ERR_MAKE;
				*bc++ = (BYTE)conv;
				*bc++ = ( isr ) ? 1 : 0;
				*bc++ = 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd (Lint,reg},Hint,isreg
			case CMD_MAXTO:
			    if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr )
				{
					if ( conv < 0 ) return IAL_ERR_MAKE;
					*bc++ = (BYTE) conv;
					*bc++ = 0;
					*bc++ = 1;
				}
				else
				{
					*bc++ = (BYTE) conv;
					*bc++ = (BYTE)( conv >> 8 ) ;
					*bc++ = 0;
				}
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd (byte,reg},{byte,reg},isreg
			case CMD_PIN_DIR:
			case CMD_PIN_WRITE:
			case CMD_PIN_READ:
			case CMD_PWM_DT_SET:
			case CMD_PRE_SET:
			case CMD_PWM_AUTO:
			case CMD_PULSEF_IN:
			case CMD_PULSEC_IN:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( isr && conv < 0 ) return IAL_ERR_MAKE;
				*bc++ = (BYTE) conv;
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr && conv < 0 ) return IAL_ERR_MAKE;
				*bc++ = (BYTE) conv;
				
				*bc++ = ( isr ) ? 1 : 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd (byte,reg + 100},{Lint,reg},Hint
			case CMD_PWM_FQ_SET:
			case CMD_PWM_TOUT_SET:
			case CMD_PULSED_OUT:
			case CMD_PULSEU_OUT:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( isr && conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)( (isr) ? conv + 100 : conv );
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr && conv < 0 ) return IAL_ERR_MAKE;
				
				if ( isr )
				{
					*bc++ = (BYTE) conv;
					*bc++ = 0;
				}
				else
				{
					*bc++ = (BYTE) conv;
					*bc++ = (BYTE) (conv >> 8);
				}
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd {reg,label}
			case CMD_JMP:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
				if ( f )
				{
					--bc;
					*bc++ = CMD_OPPC;
					*bc++ = (BYTE) f->a;
					*bc++ = (BYTE) (f->a >> 8);
				}
				else
				{
					conv = _prs_rv(&isr,tk);
					if ( !isr ) return IAL_ERR_MAKE;
					if ( conv < 0 ) return IAL_ERR_MAKE;
					*bc++ = (BYTE)conv;
					*bc++ = 0;
				}
				*bc++ = 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd reg
			case CMD_RET:
			case CMD_MDELAY:
			case CMD_UDELAY:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				*bc++ = 0;
				*bc++ = 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd reg,reg
			case CMD_RR:
			case CMD_RM:
			case CMD_MR:
			case CMD_NOT:
			case CMD_PUSH:
			case CMD_POP:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				*bc++ = 0;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd reg,Lint,Hint
			case CMD_RI:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( isr ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				*bc++ =(BYTE)(conv >> 8);
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd reg,reg,reg
			case CMD_ADD:
			case CMD_DIF:
			case CMD_MUL:
			case CMD_DIV:
			case CMD_SHL:
			case CMD_SHR:
			case CMD_OR:
			case CMD_AND:
			case CMD_XOR:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd {reg,label},reg,reg
			case CMD_JMI:
			case CMD_JMA:
			case CMD_JEQ:
			case CMD_JAN:
			case CMD_JOR:
			case CMD_JNO:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
				if ( f )
				{
					--bc;
					*bc++ = CMD_RI;
					*bc++ = 0;
					*bc++ = (BYTE) f->a;
					*bc++ = (BYTE) (f->a >> 8);
					*bc++ = icode;
					*bc++ = 0;
				}
				else
				{
					conv = _prs_rv(&isr,tk);
					if ( !isr ) return IAL_ERR_MAKE;
					if ( conv < 0 ) return IAL_ERR_MAKE;
					*bc++ = (BYTE)conv;
				}
				
				++s;
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ = (BYTE)conv;
				
				if ( icode == CMD_JNO )
				{
					*bc++ = 0;
				}
				else
				{
					++s;
					if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
					s = _prs_token(tk,s);
					conv = _prs_rv(&isr,tk);
					if ( !isr ) return IAL_ERR_MAKE;
					if ( conv < 0 ) return IAL_ERR_MAKE;
					*bc++ = (BYTE)conv;
				}
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
			/// cmd reg,label
			case CMD_CALL:
				if ( !_prs_hp(s) ) return IAL_ERR_MAKE;
				s = _prs_token(tk,s);
				
				conv = _prs_rv(&isr,tk);
				if ( !isr ) return IAL_ERR_MAKE;
				if ( conv < 0 ) return IAL_ERR_MAKE;
				*bc++ =(BYTE)conv;
				
				++s;
				s = _prs_token(tk,s);
				f = _lhs_find(a,_lhs_hash(tk,strlen(tk)),tk);
				if ( !f ) return IAL_ERR_MAKE;
				*bc++ = (BYTE) f->a;
				*bc++ = (BYTE) (f->a >> 8);
				
				++s;
				if ( _prs_hp(s) ) return IAL_ERR_MAKE;
			break;
			
		}
		
		while ( *s++ != '\n');
		*offset += 4;
	}
	
	return 0;
}

void dbg_make(CHAR* d,CHAR* s)
{
	FILE* f = fopen(d,"w");
		fprintf(f,"%s",s);
	fclose(f);
}

INT32 i328_make(UINT32* offset, BYTE* bytecode, CHAR* code)
{
	CHAR ps0[CODE_MAX];
	CHAR ps1[CODE_MAX];
	IAL ial;
	INT32 ret;
	
	_ial_init(&ial);
	
	dbg_make("./dbg/source",code);
	
	_prs_space(ps0,code);
	
	dbg_make("./dbg/nospace",ps0);
	
	_prs_comment(ps1,ps0);
	
	dbg_make("./dbg/nocomment",ps1);
	
	if ( (ret = _prs_include(ps0,ps1)) ) return ret;
	
	dbg_make("./dbg/include",ps0);
	
	if ( (ret = _prs_pre(&ial,ps1,ps0)) ) return ret;
	
	dbg_make("./dbg/preprocessor0",ps1);
	
	while ( _prs_prerep(&ial,ps0,ps1) )
		strcpy(ps0,ps1);
	
	dbg_make("./dbg/preprocessor1",ps0);
	
	if ( (ret = _prs_ibc(&ial,ps1,ps0)) ) return ret;
	
	dbg_make("./dbg/ibc",ps0);
	
	_prs_fit(ps1,ps0);
	
	dbg_make("./dbg/fit",ps0);
	
	if ( (ret = _prs_lbl(&ial,*offset,ps1,ps0)) ) return ret;
	
	dbg_make("./dbg/label",ps1);
	
	if ( (ret = _prs_bc(&ial,offset,bytecode,ps1)) ) return ret;
	
	printf("end make\n");
	
	if ( *offset > IAL_BYTECODE_MAX ) return IAL_ERR_MAKE;
	
	return 0;
}

_H328* _328_open(CHAR* m)
{
	_H328* h = malloc(sizeof(_H328));
	
	h->stwr = 0;
	return h;
}

INT32 _328_write(_H328* h,const BYTE* v, UINT32 sz)
{
	if ( h->stwr == IOS_IOCTL_328_PRE_SET )
	{
		if ( sz != 2 ) return 0;
		ard_send(CMD_PWM_AUTO,*v,*(v+1),0);
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_DIR )
	{
		if ( sz != 2 ) return 0;
		ard_send(CMD_PIN_WRITE,*v,*(v+1),0);
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_MEM_STA )
	{
		for (; sz; --sz, ++v)
		{
			ard_send(CMD_MEM_WRITE,*v,0,0);
		}
	}
	
	return 0;
}

INT32 _328_read(_H328* h,BYTE* v, UINT32 sz)
{
	BYTE cmd[4];
	UINT32* i = (UINT32*) v;
	
	if ( h->stwr == IOS_IOCTL_328_DIR )
	{
		if ( sz != 4 ) return 0;
		ard_send(CMD_PIN_READ,*i,0,0);
		ard_recv(cmd);
		if ( cmd[0] != CMD_PIN_READ || cmd[1] != *i ) return 0;
		if ( *i > 13 )
			*i = cmd[2] | ((UINT32)(cmd[3]) << 8);
		else
			*i = cmd[2];
		return 1;
	}
	else if ( h->stwr == IOS_IOCTL_328_MEM_STA )
	{
		for (; sz; --sz, ++v)
		{
			ard_send(CMD_MEM_READ,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_MEM_READ ) return 0;
			*v = cmd[1];
		}
	}
	
	return 0;
}

INT32 _328_ioctl(_H328* h, INT32 req, VOID* val)
{
	BYTE cmd[4];
	BYTE* b;
	INT32* i;
	
	switch (req)
	{
		case IOS_IOCTL_328_ECHO:
		case IOS_IOCTL_328_WAKE:
			ard_send(CMD_ECHO,1,2,3);
			ard_recv(cmd);
			if ( cmd[0] != CMD_ECHO || cmd[1] != 1 || cmd[2] != 2 || cmd[3] != 3 ) return IOS_ERR_IOCTL;
		break;
		
		case IOS_IOCTL_328_SLEEP:
			ard_send(CMD_SLEEP,0,0,0);
		break;
		
		case IOS_IOCTL_328_OPZ:
			b = (BYTE*) val;
			ard_send(CMD_OPZ,*b,0,0);
		break;
		
		case IOS_IOCTL_328_MAXTO:
			i = (INT32*)val;
			ard_send(CMD_MAXTO,(BYTE)(*i),(BYTE)(*i >> 8),0);
		break;
		
		case IOS_IOCTL_328_PRE_SET:
			b = (BYTE*)val;
			ard_send(CMD_PRE_SET,*b,*(b+1),0);
			h->stwr = IOS_IOCTL_328_PRE_SET;
		break;
		
		case IOS_IOCTL_328_MEM_STA:
			i = (INT32*)val;
			ard_send(CMD_MEM_STA,(BYTE)(*i),(BYTE)(*i >> 8),0);
			h->stwr = IOS_IOCTL_328_MEM_STA;
		break;
		
		case IOS_IOCTL_328_NOP:
			ard_send(CMD_NOP,0,0,0);
		break;
		
		case IOS_IOCTL_328_OPPC:
			i = (INT32*)val;
			ard_send(CMD_OPPC,(BYTE)(*i),(BYTE)(*i >> 8),0);
		break;
		
		case IOS_IOCTL_328_DIR:
			b = (BYTE*)val;
			ard_send(CMD_PIN_DIR,*b,*(b+1),0);
			h->stwr = IOS_IOCTL_328_DIR;
		break;
		
		case IOS_IOCTL_328_OVDT:
			ard_send(CMD_OVDT,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_OVDT ) return IOS_ERR_IOCTL;
			i = (INT32*) val;
			*i = 0;
			*i = cmd[1] | ((INT32)(cmd[2]) << 8) | ((INT32)(cmd[3]) << 16);
		break;
		
		case IOS_IOCTL_328_FREERAM:
			ard_send(CMD_FREERAM,0,0,0);
			ard_recv(cmd);
			if ( cmd[0] != CMD_FREERAM ) return IOS_ERR_IOCTL;
			i = (INT32*) val;
			*i = 0;
			*i = cmd[1] | ((INT32)(cmd[2]) << 8);
		break;
			
	}
	
	return 0;
}

VOID _328_close(_H328* h)
{
	free(h);
}
