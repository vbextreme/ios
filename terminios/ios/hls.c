#include "main.h"

#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
    #define g16b(d) (*((const UINT16 *) (d)))
#endif

#if !defined (g16b)
    #define g16b(d) ((((UINT32)(((const BYTE *)(d))[1])) << 8) + (UINT32)(((const BYTE *)(d))[0]) )
#endif

UINT32 fast_hash(const char * data, INT32 len)
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

UINT32 hls_hash(const CHAR* val, INT32 len)
{
    return fast_hash(val,len) % HALIAS_MAX;
}

VOID ali_init(HALIAS* ha)
{
	int i;
    for (i = 0; i < HALIAS_MAX; ++i)
        ha->map[i] = NULL;
}

ALIAS* ali_find(HALIAS* ha, const CHAR* name)
{
	UINT32 h = hls_hash(name,strlen(name));
	ALIAS* f = ha->map[h];
	for (; f; f = f->next)
		if ( !strcmp(f->name,name) ) return f;
	return f;
}

BOOL ali_add(HALIAS* ha, const CHAR* name, const INT32 v)
{
	if ( ali_find(ha,name) ) return FALSE;
	
	ALIAS* a = malloc(sizeof(ALIAS));
	strcpy(a->name,name);
	a->val = v;
	
	UINT32 h = hls_hash(name,strlen(name));
	a->next = ha->map[h];
	ha->map[h] = a;
	
	return TRUE;
}

BOOL ali_remove(HALIAS* ha, const CHAR* name)
{
	UINT32 h = hls_hash(name,strlen(name));
	ALIAS* f = ha->map[h];
	
	if ( !f ) return FALSE;
	
	if ( !strcmp(f->name,name) )
	{
		ha->map[h] = ha->map[h]->next;
		free(f);
		return TRUE;
	}
	
	for ( ; f->next; f = f->next)
	{
		if ( !strcmp(f->next->name,name) )
		{
			ALIAS* r = f->next;
			f->next = f->next->next;
			free(r);
			return TRUE;
		}
	}
	return FALSE;
}

VOID ali_free(HALIAS* ha)
{
	ALIAS* m;
	INT32 i;
	
	for ( i = 0; i < HALIAS_MAX; ++i)
	{
		while ( ha->map[i] )
		{
			m = ha->map[i]->next;
			free(ha->map[i]);
			ha->map[i] = m;
		}
	}
}

VOID ali_print( HALIAS* ha)
{
	ALIAS* m;
	INT32 i;
	
	for ( i = 0; i < HALIAS_MAX; ++i)
	{
		for ( m = ha->map[i]; m; m = m->next )
			printf("%s = 0x%X\n", m->name, m->val); 
	}
}
